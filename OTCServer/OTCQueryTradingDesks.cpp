/***********************************************************************
 * Module:  OTCTradingDesks.cpp
 * Author:  milk
 * Modified: 2015年8月27日 20:23:20
 * Purpose: Implementation of the class OTCTradingDesks
 ***********************************************************************/

#include "OTCQueryTradingDesks.h"
#include "../databaseop/UserInfoDAO.h"
////////////////////////////////////////////////////////////////////////
// Name:       OTCQueryTradingDesks::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
// Purpose:    Implementation of OTCQueryTradingDesks::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCQueryTradingDesks::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto userDOVec = UserInfoDAO::FindTradingDesksByAdminId(session->getUserInfo().getUserId());

	ThrowNotFoundExceptionIfEmpty(userDOVec);

	return userDOVec;
}