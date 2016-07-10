/***********************************************************************
 * Module:  MicroFurtureSystem.cpp
 * Author:  milk
 * Modified: 2015年8月6日 0:31:35
 * Purpose: Implementation of the class MicroFurtureSystem
 ***********************************************************************/

#include "MicroFurtureSystem.h"
#include "../utility/LiteLogger.h"
#include "../utility/TUtil.h"
#include "../utility/stringutility.h"
#include "../configuration/AbstractConfigReaderFactory.h"
#include "../dynamicloader/ConfigBasedCreator.h"
#include "../message/SysParam.h"

#include "../databaseop/SysParamsDAO.h"
#include "../databaseop/AbstractConnectionManager.h"
#include "../dataobject/AbstractDataSerializerFactory.h"

void MicroFurtureSystem::InitLogger(const char* logPath)
{
	InitLogger(std::string(logPath));
}

void MicroFurtureSystem::InitLogger(const std::string& logPath)
{
	LiteLogger::InitLogger(logPath);

	LiteLogger::Info("Log path: " + logPath + '\n');
}

const std::string & MicroFurtureSystem::GetLogPath(void)
{
	return LiteLogger::LogPath();
}

////////////////////////////////////////////////////////////////////////
// Name:       MicroFurtureSystem::Load(const std::string& config)
// Purpose:    Implementation of MicroFurtureSystem::Load()
// Parameters:
// - config
// Return:     void
////////////////////////////////////////////////////////////////////////

bool MicroFurtureSystem::Load(const std::string& config)
{
	bool ret = false;
	
	if (auto cfgReader = AbstractConfigReaderFactory::OpenConfigReader(config)) {
		LiteLogger::Info("Initializing system (ver. " 
			+ cfgReader->getValue("system.version")  + ")...\n");

		LiteLogger::Info("  Loading System Config: " + config + '\n');

		std::vector<std::string> section_vec;
		// Initialize Databases
		cfgReader->getVector("system.databases.configs", section_vec);
		for (auto& dbsection : section_vec)
		{
			std::map<std::string, std::string> dbCfgMap;
			if (cfgReader->getMap("system.databases." + dbsection, dbCfgMap) > 0)
				AbstractConnectionManager::DefaultInstance()->LoadDbConfig(dbCfgMap);
			
			LiteLogger::Info("  Loading database: " + dbsection + " (" + dbCfgMap["key"] + ")...\n");
			if(auto pConnMgr = AbstractConnectionManager::DefaultInstance()->
				FindConnectionManager(dbCfgMap["key"]))
			{
				pConnMgr->Initialize();
				LiteLogger::Info("    " + std::to_string(pConnMgr->DBConfig().DB_POOL_SIZE)
					+ " connections have been created in pool.\n");
			}
		}

		// Initialize SysParam
		std::map<std::string, std::string> cfgMap;
		cfgReader->getMap("system.sysparam", cfgMap);
		SysParam::Merge(cfgMap);
		auto it = cfgMap.find("mergedbparam");
		if (it != cfgMap.end())
		{
			auto sysparamMap = SysParamsDAO::FindSysParams("%");

			if (stringutility::compare(it->second.data(), "override") == 0)
				SysParam::Update(*sysparamMap);
			else
				SysParam::Merge(*sysparamMap);
		}

		// Initialize Serializers
		LiteLogger::Info("  Initializing DataSerializer...\n");
		section_vec.clear();
		cfgReader->getVector("system.serializers.modules", section_vec);
		for (auto& cfg : section_vec)
		{
			cfgMap.clear();
			cfgReader->getMap("system.serializers." + cfg, cfgMap);

			MessageSerializerConfig msgSlzCfg;
			msgSlzCfg.MODULE_UUID = cfgMap["module.uuid"];
			msgSlzCfg.MODULE_PATH = cfgMap["module.path"];
			msgSlzCfg.CLASS_UUID = cfgMap["class.uuid"];

			AbstractDataSerializerFactory::MessageSerializerConfigs.push_back(std::move(msgSlzCfg));
		}

		AbstractDataSerializerFactory::Instance();

		LiteLogger::Info("  DataSerializers have initialized.\n");

		// Initialize Services
		std::string serve_cfg = cfgReader->getValue("system.service.config");
		if (cfgReader = AbstractConfigReaderFactory::OpenConfigReader(serve_cfg)) {
			LiteLogger::Info("  Loading Service Config: " + serve_cfg + '\n');
			std::vector<std::string> sections;
			cfgReader->getVector("service.servercfg", sections);

			ret = true;
			for (auto& sec : sections) {
				bool initserver = false;
				// Initialize Handler Factory
				cfgMap.clear();
				cfgReader->getMap(sec, cfgMap);
				std::string facCfg = cfgMap["factory.config"];
				std::string facCfgSec = cfgMap["factory.config.section"];
				if (auto msgfac = ConfigBasedCreator::CreateInstance(facCfg, facCfgSec)) {
					auto msgsvcfactory = std::static_pointer_cast<IMessageServiceFactory>(msgfac);
					if (msgsvcfactory->Load(facCfg, facCfgSec)) {

						std::string svrUUID = cfgMap["server.module.uuid"];
						std::string svrModule = cfgMap["server.module.path"];
						std::string svrClass = cfgMap["server.class.uuid"];
						std::string svrCfg = cfgMap["server.config"];
						std::string svrCfgSec = cfgMap["server.config.section"];
						std::string svruri = cfgMap["server.uri"];
						
						// Initialize Server
						if (auto srvPtr = ConfigBasedCreator::CreateInstance(svrUUID, svrModule, svrClass)) {
							auto server = std::static_pointer_cast<IMessageServer>(srvPtr);

							server->RegisterServiceFactory(msgsvcfactory);
							
							if (server->Initialize(svruri, svrCfg)) {
								this->_servers.push_back(server);
								initserver = true;
								LiteLogger::Info("  Server " + server->getUri() + " initialized.\n");
							}
							else
							{
								LiteLogger::Error("  Server " + server->getUri() + " failed to initialize.\n");
							}
						}
						else
						{
							LiteLogger::Error("  Failed to create server: " + svrUUID + '\n');
						}
					}
				}
				else
				{
					LiteLogger::Error("  Failed to create service factory: " + facCfgSec 
						+ " from " + facCfg + '\n');
				}

				ret = ret && initserver;
			}
		}
	}
	if (ret)
		LiteLogger::Info("Succeed to initialize system.\n");
	else
		LiteLogger::Info("Failed to initialize system.\n");

	return ret;
}

bool MicroFurtureSystem::Load(const char* config)
{
	return Load(std::string(config));
}

////////////////////////////////////////////////////////////////////////
// Name:       MicroFurtureSystem::Run()
// Purpose:    Implementation of MicroFurtureSystem::Run()
// Return:     void
////////////////////////////////////////////////////////////////////////

bool MicroFurtureSystem::Start(void)
{
	bool ret = _running;

	if (!_running)
	{
		LiteLogger::Info("Starting servers:\n");
		int i = 0;
		for (auto& svr : _servers) {
			if (svr->Start())
			{
				i++;
				LiteLogger::Info("  " + svr->getUri() + " has started.\n");
			}
			else
			{
				LiteLogger::Error("  " + svr->getUri()+ " failed to start!\n");
			}
		}
		LiteLogger::Info(std::to_string(i) + " servers started running.\n");

		_running = true;

		ret = i == _servers.size();
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       MicroFurtureSystem::Stop()
// Purpose:    Implementation of MicroFurtureSystem::Stop()
// Return:     void
////////////////////////////////////////////////////////////////////////

bool MicroFurtureSystem::Stop(void)
{
	bool ret;

	LiteLogger::Info("Stopping servers:\n");
	int i = 0;
	for (auto& svr : _servers) {
		if (svr->Stop())
		{
			i++;
			LiteLogger::Info("  " + svr->getUri() + " has stopped.\n");
		}
		else
		{
			LiteLogger::Error("  " + svr->getUri() + " failed to stop!\n");
		}
		svr->Stop();
	}

	_running = false;

	ret = i == _servers.size();

	LiteLogger::Info(std::to_string(i) + " servers have stopped.\n");

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       MicroFurtureSystem::MicroFurtureSystem()
// Purpose:    Implementation of MicroFurtureSystem::MicroFurtureSystem()
// Return:     
////////////////////////////////////////////////////////////////////////

MicroFurtureSystem::MicroFurtureSystem()
{
	_running = false;
}

////////////////////////////////////////////////////////////////////////
// Name:       MicroFurtureSystem::~MicroFurtureSystem()
// Purpose:    Implementation of MicroFurtureSystem::~MicroFurtureSystem()
// Return:     
////////////////////////////////////////////////////////////////////////

MicroFurtureSystem::~MicroFurtureSystem()
{
	if (_running)
	{
		Stop();
	}
}

////////////////////////////////////////////////////////////////////////
// Name:       MicroFurtureSystem::IsRunning()
// Purpose:    Implementation of MicroFurtureSystem::IsRunning()
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool MicroFurtureSystem::IsRunning(void)
{
	return _running;
}
