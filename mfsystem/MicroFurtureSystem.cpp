/***********************************************************************
 * Module:  MicroFurtureSystem.cpp
 * Author:  milk
 * Modified: 2015年8月6日 0:31:35
 * Purpose: Implementation of the class MicroFurtureSystem
 ***********************************************************************/

#include "MicroFurtureSystem.h"
#include "../litelogger/LiteLogger.h"
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
	LiteLogger::Instance().InitLogger(logPath);

	LOG_INFO << "Log path: " << logPath;
}

const std::string & MicroFurtureSystem::GetLogPath(void)
{
	return LiteLogger::Instance().LogPath();
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
		auto ver = cfgReader->getValue("system.version");
		LOG_INFO << "Initializing system (ver. " << ver << ")...";

		LOG_INFO << "  Loading system config: " << config;

		std::vector<std::string> section_vec;
		// Initialize Databases
		cfgReader->getVector("system.databases.configs", section_vec);
		for (auto& dbsection : section_vec)
		{
			std::map<std::string, std::string> dbCfgMap;
			if (cfgReader->getMap("system.databases." + dbsection, dbCfgMap) > 0)
				AbstractConnectionManager::DefaultInstance()->LoadDbConfig(dbCfgMap);

			LOG_INFO << "  Loading database: " << dbsection <<
				" (" << dbCfgMap["key"] << ")...";
			if (auto pConnMgr = AbstractConnectionManager::DefaultInstance()->
				FindConnectionManager(dbCfgMap["key"]))
			{
				pConnMgr->Initialize();
				LOG_INFO << "    " << pConnMgr->DBConfig().DB_POOL_SIZE
					<< " connections have been created in pool.";
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
		LOG_INFO << "  Initializing DataSerializer...";
		std::vector<std::string> serial_vec;
		cfgReader->getVector("system.serializers.modules", serial_vec);
		std::string section("system.serializers.");
		for (auto& cfg : serial_vec)
		{
			cfgReader->getMap(section + cfg, cfgMap);

			MessageSerializerConfig msgSlzCfg;
			msgSlzCfg.MODULE_UUID = cfgMap["module.uuid"];
			msgSlzCfg.MODULE_PATH = cfgMap["module.path"];
			msgSlzCfg.CLASS_UUID = cfgMap["class.uuid"];

			AbstractDataSerializerFactory::AddConfigForInstance(msgSlzCfg);
		}

		AbstractDataSerializerFactory::Instance();

		LOG_INFO << "  DataSerializers have initialized.";

		// Initialize Services
		std::string serve_cfg = cfgReader->getValue("system.service.config");
		if (cfgReader = AbstractConfigReaderFactory::OpenConfigReader(serve_cfg)) {
			LOG_INFO << "  Loading service config: " << serve_cfg;
			std::vector<std::string> sections;
			cfgReader->getVector("service.servercfg", sections);

			ret = true;
			for (auto& sec : sections) {
				bool initserver = false;
				// Initialize Handler Factory
				std::map<std::string, std::string> cfgMap;
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
								LOG_INFO << "  Server " << svruri << " initialized.";
							}
							else
							{
								LOG_ERROR <<
									"  Server: " << svruri << " failed to initialize.";
							}
						}
						else
						{
							LOG_ERROR << "  Failed to create server: " << svrUUID;
						}
					}
				}
				else
				{
					LOG_ERROR << "  Failed to create service factory: " << facCfgSec
						<< " from " << facCfg;
				}

				ret = ret && initserver;
			}
		}
	}
	if (ret)
		LOG_INFO << "Succeed to initialize system.";
	else
		LOG_INFO << "Failed to initialize system.";

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
		LOG_INFO << "Starting servers:";
		int i = 0;
		for (auto& svr : _servers) {
			if (svr->Start())
			{
				i++;
				LOG_INFO << "  " << svr->getUri() << " has started.";
			}
			else
			{
				LOG_ERROR << "  " << svr->getUri() << " failed to start!";
			}
		}
		LOG_INFO << i << " servers started running.";

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

	LOG_INFO << "Stopping servers:";
	int i = 0;
	for (auto& svr : _servers) {
		if (svr->Stop())
		{
			i++;
			LOG_INFO << "  " << svr->getUri() << " has stopped.";
		}
		else
		{
			LOG_ERROR << "  " << svr->getUri() << " failed to stop!";
		}
		svr->Stop();
	}

	_running = false;

	ret = i == _servers.size();

	LOG_INFO << i + " servers have stopped.";

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
