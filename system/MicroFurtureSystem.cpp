/***********************************************************************
 * Module:  MicroFurtureSystem.cpp
 * Author:  milk
 * Modified: 2015年8月6日 0:31:35
 * Purpose: Implementation of the class MicroFurtureSystem
 ***********************************************************************/

#include "MicroFurtureSystem.h"
#include <glog/logging.h>
#include "../utility/TUtil.h"
#include "../configuration/AbstractConfigReaderFactory.h"
#include "../dynamicloader/ConfigBasedCreator.h"
#include "../message/SysParam.h"

#include "../databaseop/SysParamsDAO.h"
#include "../dataobject/AbstractDataSerializerFactory.h"

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
	LOG(INFO) << "Initializing Application:" << std::endl;

	if (auto cfgReader = AbstractConfigReaderFactory::OpenConfigReader(config)) {
		LOG(INFO) << "  Loading System Config: " << config << std::endl;

		// Initialize SysParam
		std::map<std::string, std::string> cfgMap;
		cfgReader->getMap("system.sysparam", cfgMap);
		SysParam::Update(cfgMap);
		if (cfgMap.find("mergedbparam") != cfgMap.end())
		{
			auto sysparamMap = SysParamsDAO::FindSysParams("%");
			SysParam::Update(*sysparamMap);
		}

		// Initialize Serializers
		cfgReader->getMap("system.serializer", cfgMap);
		auto it = cfgMap.find("module.uuid");
		if (it != cfgMap.end())
			AbstractDataSerializerFactory::DefaultMessageSerializerConfig.MODULE_UUID =
			it->second;
		it = cfgMap.find("module.path");
		if (it != cfgMap.end())
			AbstractDataSerializerFactory::DefaultMessageSerializerConfig.MODULE_PATH =
			it->second;

		// Initialize Services
		std::string serve_cfg = cfgReader->getValue("system.service.cfg");
		if (cfgReader = AbstractConfigReaderFactory::OpenConfigReader(serve_cfg)) {
			LOG(INFO) << "  Loading Service Config: " << serve_cfg << std::endl;
			std::vector<std::string> sections;
			cfgReader->getVector("service.servercfg", sections);

			ret = true;
			for (auto& sec : sections) {
				bool initserver = false;

				// Initialize Handler Factory
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

						// Initialize Server
						if (auto srvPtr = ConfigBasedCreator::CreateInstance(svrUUID, svrModule, svrClass)) {
							auto server = std::static_pointer_cast<IMessageServer>(srvPtr);
							server->RegisterServiceFactory(msgsvcfactory);
							std::string svruri = cfgMap["server.uri"];
							if (server->Initialize(svruri, svrCfg)) {
								this->_servers.push_back(server);
								initserver = true;
								LOG(INFO) << "  Server " << server->getUri() << " initialized." << std::endl;
							}
						}
					}
				}

				ret = ret && initserver;
			}
		}
	}
	if (ret)
		LOG(INFO) << "Succeed to initialize system." << std::endl;
	else
		LOG(INFO) << "Failed to initialize system." << std::endl;

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

bool MicroFurtureSystem::Run(void)
{
	bool ret = _running;

	if (!_running)
	{
		LOG(INFO) << "Starting servers:" << std::endl;
		int i = 0;
		for (auto& svr : _servers) {
			if (svr->Start())
			{
				i++;
				LOG(INFO) << "  " << svr->getUri() << " has started." << std::endl;
			}
			else
			{
				LOG(ERROR) << "  " << svr->getUri() << " failed to start!" << std::endl;
			}
		}
		LOG(INFO) << i << " servers started running." << std::endl;

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

	LOG(INFO) << "Stopping servers:" << std::endl;
	int i = 0;
	for (auto& svr : _servers) {
		if (svr->Stop())
		{
			i++;
			LOG(INFO) << "  " << svr->getUri() << " has stopped." << std::endl;
		}
		else
		{
			LOG(ERROR) << "  " << svr->getUri() << " failed to stop!" << std::endl;
		}
		svr->Stop();
	}

	_running = false;

	ret = i == _servers.size();

	LOG(INFO) << i << " servers have stopped." << std::endl;

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
