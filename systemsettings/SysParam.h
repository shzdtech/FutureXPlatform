/***********************************************************************
 * Module:  SysParam.h
 * Author:  milk
 * Modified: 2015年9月2日 22:08:49
 * Purpose: Declaration of the class SysParam
 ***********************************************************************/

#if !defined(__systemsettings_SysParam_h)
#define __systemsettings_SysParam_h
#include <string>
#include <map>
#include "systemsettings_exp.h"

class SYSTEMSETTINGS_CLASS_EXPORT SysParam
{
public:
	static bool Contains(const std::string& key);
	static bool TryGet(const std::string& key, std::string& value);
	static const char* Get(const std::string& key);
	static void Update(const std::string& key, const std::string& value);
	static void Update(const std::map<std::string, std::string>& paramMap);
	static void Merge(const std::map<std::string, std::string>& paramMap);

protected:

private:
};

#endif