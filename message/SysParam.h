/***********************************************************************
 * Module:  SysParam.h
 * Author:  milk
 * Modified: 2015年9月2日 22:08:49
 * Purpose: Declaration of the class SysParam
 ***********************************************************************/

#if !defined(__message_SysParam_h)
#define __message_SysParam_h
#include <string>
#include <map>
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT SysParam
{
public:
	static bool TryGet(const std::string& key, std::string& value);
	static const std::string& Get(const std::string& key);
	static void Update(std::map<std::string, std::string>& paramMap);

protected:

private:
};

#endif