/***********************************************************************
 * Module:  UserInfoDO.h
 * Author:  milk
 * Modified: 2014年10月1日 1:41:34
 * Purpose: Declaration of the class UserInfoDO
 ***********************************************************************/

#if !defined(__dataobject_UserInfoDO_h)
#define __dataobject_UserInfoDO_h

#include <set>
#include <string>
#include "dataobjectbase.h"

enum GenderType
{
	GENDER_UNSPECIFIED = 0,
	GENDER_MAN = 1,
	GENDER_WOMAN = 2
};

class UserInfoDO : public dataobjectbase
{
public:
	std::string BrokerId;
	std::string UserId;
	std::string UserName;
	std::string Company;
	std::string Password;
	std::string FirstName;
	std::string LastName;
	std::string Email;
	std::string ContactNum;
	std::string IdentityNum;
	std::string Address;
	std::string ZipCode;
	GenderType Gender;
	int Role;
	int Permission;

	std::set<std::string> Portfolios;

protected:

private:

};

#endif