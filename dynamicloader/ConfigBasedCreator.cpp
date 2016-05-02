/***********************************************************************
 * Module:  FactoryInitializer.cpp
 * Author:  milk
 * Modified: 2014年10月1日 10:30:15
 * Purpose: Implementation of the class FactoryInitializer
 ***********************************************************************/

#include <memory>

#include "ConfigBasedCreator.h"
#include "AbstractDynamicLoader.h"
#include "../configuration/AbstractConfigReaderFactory.h"


std::string ConfigBasedCreator::DEFAULT_CFG_PATH = "instance";

////////////////////////////////////////////////////////////////////////
// Name:       ConfigBasedCreator::CreateInstance(const std::string& moduleUUID, const std::string& modulePath, const std::string& classUUID)
// Purpose:    Implementation of ConfigBasedCreator::CreateInstance()
// Parameters:
// - moduleUUID
// - modulePath
// - classUUID
// Return:     std::shared_ptr<void>
////////////////////////////////////////////////////////////////////////

std::shared_ptr<void> ConfigBasedCreator::CreateInstance(const std::string& moduleUUID, const std::string& modulePath, const std::string& classUUID)
{
	std::shared_ptr<void> instancePtr;
	AbstractDynamicLoader* adl = AbstractDynamicLoader::Instance();
	void* handle = adl->LoadModule(moduleUUID, modulePath);
	if (handle) {
		void* instance;
		if (adl->CreateInstance(handle, classUUID, &instance))
			instancePtr.reset(instance);
	}

	return instancePtr;
}


////////////////////////////////////////////////////////////////////////
// Name:       FactoryInitializer::CreateFactoryFromCfg(const std::string& factoryUUID, const std::string& configFile, void** factoryInstance)
// Purpose:    Implementation of FactoryInitializer::CreateFactoryFromCfg()
// Parameters:
// - factoryUUID
// - configFile
// - factoryInstance
// Return:     bool
////////////////////////////////////////////////////////////////////////

std::shared_ptr<void> ConfigBasedCreator::CreateInstance(const std::string& configFile, const std::string& sectionPath) {
	std::shared_ptr<void> instancePtr;
	if (auto cfgReader = AbstractConfigReaderFactory::OpenConfigReader(configFile)) {
		std::map<std::string, std::string> facMap;
		if (cfgReader->getMap(sectionPath, facMap)) {
			std::string mPath = facMap["module.path"];
			std::string mUUID = facMap["module.uuid"];
			std::string cUUID = facMap["class.uuid"];
			instancePtr = CreateInstance(mUUID, mPath, cUUID);
		}
	}

	return instancePtr;
}

////////////////////////////////////////////////////////////////////////
// Name:       FactoryInitializer::ConfigBasedCreator(const std::string& factoryUUID)
// Purpose:    Implementation of ConfigBasedCreator::CreateFactoryFromCfg()
// Parameters:
// - factoryUUID
// - factoryInstance
// Return:     bool
////////////////////////////////////////////////////////////////////////

std::shared_ptr<void> ConfigBasedCreator::CreateInstance(const std::string& factoryUUID) {
	return CreateInstance(factoryUUID, DEFAULT_CFG_PATH);
}