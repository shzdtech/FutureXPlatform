/***********************************************************************
 * Module:  OTCTradingDesks.cpp
 * Author:  milk
 * Modified: 2015年8月27日 20:23:20
 * Purpose: Implementation of the class OTCTradingDesks
 ***********************************************************************/

#include "OTCQueryTradingDesks.h"
#include "../databaseop/UserInfoDAO.h"
////////////////////////////////////////////////////////////////////////
// Name:       OTCQueryTradingDesks::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of OTCQueryTradingDesks::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCQueryTradingDesks::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto userDOVec = UserInfoDAO::FindAllUserByRole(ROLE_TRADINGDESK);

	ThrowNotFoundExceptionIfEmpty(userDOVec);

	return userDOVec;
}