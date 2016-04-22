/***********************************************************************
 * Module:  UserInfoDO.h
 * Author:  milk
 * Modified: 2014年10月1日 1:41:34
 * Purpose: Declaration of the class UserInfoDO
 ***********************************************************************/

#if !defined(__dataobject_UserInfoDO_h)
#define __dataobject_UserInfoDO_h

#include "AbstractDataObj.h"
#include <string>

class UserInfoDO : public AbstractDataObj
{
public:
	std::string BrokerId;
	std::string UserId;
	std::string Password;
	std::string Name;
	std::string Company;
	std::string Email;
	std::string ContactNum;
	int Role;
	int Permission;

protected:

private:

};

#endif