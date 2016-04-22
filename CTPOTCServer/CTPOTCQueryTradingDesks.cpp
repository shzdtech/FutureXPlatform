/***********************************************************************
 * Module:  CTPOTCTradingDesks.cpp
 * Author:  milk
 * Modified: 2015年8月27日 20:23:20
 * Purpose: Implementation of the class CTPOTCTradingDesks
 ***********************************************************************/

#include "CTPOTCQueryTradingDesks.h"
#include "../databaseop/UserInfoDAO.h"
////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCQueryTradingDesks::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCQueryTradingDesks::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCQueryTradingDesks::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{

	auto userDOVec = UserInfoDAO::FindAllUserByRole(ROLE_TRADINGDESK);

	return userDOVec;
}