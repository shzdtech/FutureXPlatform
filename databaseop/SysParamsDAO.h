/***********************************************************************
 * Module:  SysParamsDAO.h
 * Author:  milk
 * Modified: 2015年8月3日 0:00:08
 * Purpose: Declaration of the class SysParamsDAO
 ***********************************************************************/

#if !defined(__databaseop_SysParamsDAO_h)
#define __databaseop_SysParamsDAO_h

#include <memory>
#include <map>
#include <string>
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS SysParamsDAO
{
public:
   static std::shared_ptr<std::map<std::string,std::string>> FindSysParams(const std::string& key);

protected:
private:

};

#endif