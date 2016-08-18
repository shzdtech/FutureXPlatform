/***********************************************************************
 * Module:  SysConfigImpl.h
 * Author:  milk
 * Modified: 2014年10月3日 14:40:25
 * Purpose: Declaration of the class SysConfigImpl
 ***********************************************************************/

#if !defined(__configuration_SysConfigImpl_h)
#define __configuration_SysConfigImpl_h

#include "SysConfig.h"
#include <map>

class SysConfigImpl : public SysConfig
{
public:
   bool getConfig(const std::string&, std::string& value);

protected:
   void Initialize(void);

private:
   std::map<std::string, std::string> _configMap;


};

#endif