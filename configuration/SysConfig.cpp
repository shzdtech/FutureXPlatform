/***********************************************************************
 * Module:  SysConfig.cpp
 * Author:  milk
 * Modified: 2014年10月3日 14:52:03
 * Purpose: Implementation of the class SysConfig
 ***********************************************************************/

#include "SysConfig.h"
#include "SysConfigImpl.h"

std::once_flag SysConfig::_instance_flag;
SysConfig* SysConfig::_instance = nullptr;
std::string SysConfig::DEFAULT_CONFIG = "system.cfg";

////////////////////////////////////////////////////////////////////////
// Name:       SysConfig::instance()
// Purpose:    Implementation of SysConfig::instance()
// Return:     SysConfig*
////////////////////////////////////////////////////////////////////////

SysConfig* SysConfig::instance(void)
{
    if(!_instance) {
        std::call_once(_instance_flag, []{
            _instance = new SysConfigImpl();
            _instance->Initialize();
        });
    }
    
    return _instance;
}