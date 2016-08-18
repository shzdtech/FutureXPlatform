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


const char* ConfigBasedCreator::DEFAULT_CFG_PATH = "instance";

////////////////////////////////////////////////////////////////////////
// Name:       ConfigBasedCreator::CreateInstance(const char* moduleUUID, const char* modulePath, const char* classUUID)
// Purpose:    Implementation of ConfigBasedCreator::CreateInstance()
// Parameters:
// - moduleUUID
// - modulePath
// - classUUID
// Return:     std::shared_ptr<void>
////////////////////////////////////////////////////////////////////////

std::shared_ptr<void> ConfigBasedCreator::CreateInstance(const char* moduleUUID, const char* modulePath, const char* classUUID)
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
// Name:       FactoryInitializer::CreateFactoryFromCfg(const char* factoryUUID, const char* configFile, void** factoryInstance)
// Purpose:    Implementation of FactoryInitializer::CreateFactoryFromCfg()
// Parameters:
// - factoryUUID
// - configFile
// - factoryInstance
// Return:     bool
////////////////////////////////////////////////////////////////////////

std::shared_ptr<void> ConfigBasedCreator::CreateInstance(const char* configFile, const char* sectionPath) {
	std::shared_ptr<void> instancePtr;
	if (auto cfgReader = AbstractConfigReaderFactory::OpenConfigReader(configFile)) {
		std::map<std::string, std::string> facMap;
		if (cfgReader->getMap(sectionPath, facMap)) {
			instancePtr = CreateInstance(facMap["module.uuid"].data(), 
				facMap["module.path"].data(), facMap["class.uuid"].data());
		}
	}

	return instancePtr;
}

////////////////////////////////////////////////////////////////////////
// Name:       FactoryInitializer::ConfigBasedCreator(const char* factoryUUID)
// Purpose:    Implementation of ConfigBasedCreator::CreateFactoryFromCfg()
// Parameters:
// - factoryUUID
// - factoryInstance
// Return:     bool
////////////////////////////////////////////////////////////////////////

std::shared_ptr<void> ConfigBasedCreator::CreateInstance(const char* factoryUUID) {
	return CreateInstance(factoryUUID, DEFAULT_CFG_PATH);
}