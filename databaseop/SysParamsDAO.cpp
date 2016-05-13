/***********************************************************************
 * Module:  SysParamsDAO.cpp
 * Author:  milk
 * Modified: 2015年8月3日 0:00:08
 * Purpose: Implementation of the class SysParamsDAO
 ***********************************************************************/

#include "SysParamsDAO.h"
#include "ConnectionHelper.h"

////////////////////////////////////////////////////////////////////////
// Name:       SysParamsDAO::FindSysParams(const std::string& key)
// Purpose:    Implementation of SysParamsDAO::FindSysParams()
// Parameters:
// - key
// Return:     std::shared_ptr<std::map<std::string,std::string>>
////////////////////////////////////////////////////////////////////////

std::shared_ptr<std::map<std::string, std::string>> SysParamsDAO::FindSysParams(const std::string& key)
{
	const static std::string sql_findparams(
		"SELECT keycode, val FROM sys_param WHERE keycode LIKE ?");

	std::map<std::string, std::string> sysparams;
	auto session = ConnectionHelper::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findparams));
		prestmt->setString(1, key);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
			sysparams[rs->getString(1)] = rs->getString(2);
	}
	catch (std::exception& ex)
	{
		LOG(ERROR) << __FUNCTION__ << ": " << ex.what();
	}

	auto ret = std::make_shared<std::map<std::string, std::string>>(std::move(sysparams));

	return ret;
}