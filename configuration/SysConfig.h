/***********************************************************************
 * Module:  SysConfig.h
 * Author:  milk
 * Modified: 2014年10月3日 14:52:03
 * Purpose: Declaration of the class SysConfig
 ***********************************************************************/

#if !defined(__configuration_SysConfig_h)
#define __configuration_SysConfig_h
#include "configuration_exp.h"
#include <string>
#include <mutex>

class CONFIGURATION_CLASS_EXPORT SysConfig
{
public:
   virtual bool getConfig(const std::string& config, std::string& value)=0;
   static SysConfig* instance(void);

   static std::string DEFAULT_CONFIG;

protected:
   virtual void Initialize(void)=0;

private:
   static SysConfig* _instance;
   static std::once_flag _instance_flag;

};

#endif