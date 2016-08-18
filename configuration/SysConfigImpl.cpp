/***********************************************************************
 * Module:  SysConfigImpl.cpp
 * Author:  milk
 * Modified: 2014年10月3日 14:40:25
 * Purpose: Implementation of the class SysConfigImpl
 ***********************************************************************/

#include "SysConfigImpl.h"
#include "AbstractConfigReaderFactory.h"

const char * CONFIG_VARIABLE_PATH = "sys.config.variables";

////////////////////////////////////////////////////////////////////////
// Name:       SysConfigImpl::getConfig(const char* config, std::string& value)
// Purpose:    Implementation of SysConfigImpl::getConfig()
// Parameters:
// - config
// - value
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool SysConfigImpl::getConfig(const std::string& config, std::string& value)
{
    bool ret = false;
    auto valueItr = _configMap.find(config);
    if (valueItr != _configMap.end()) {
        value = valueItr->second;
        ret = true;
    }
    
    return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       SysConfigImpl::Initialize()
// Purpose:    Implementation of SysConfigImpl::Initialize()
// Return:     void
////////////////////////////////////////////////////////////////////////

void SysConfigImpl::Initialize(void)
{
    auto configReader = AbstractConfigReaderFactory::CreateConfigReader();
    if (configReader) {
        if (configReader->LoadFromFile(DEFAULT_CONFIG.data()))
            configReader->getMap(CONFIG_VARIABLE_PATH, _configMap);
    }
}