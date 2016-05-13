/***********************************************************************
 * Module:  UserInfoDAO.h
 * Author:  milk
 * Modified: 2015年8月1日 21:28:29
 * Purpose: Declaration of the class UserInfoDAO
 ***********************************************************************/

#if !defined(__databaseop_UserInfoDAO_h)
#define __databaseop_UserInfoDAO_h

#include <memory>
#include "../dataobject/UserInfoDO.h"
#include "../dataobject/TemplateDO.h"
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS UserInfoDAO
{
public:
	static int InsertUser(const UserInfoDO& userDO);

	static std::shared_ptr<UserInfoDO> FindUser(const std::string& userId);

	static VectorDO_Ptr<UserInfoDO> FindAllUserByRole(int role);

protected:
private:

};

#endif