/***********************************************************************
* Module:  OTCUserContextBuilder.cpp
* Author:  milk
* Modified: 2016年2月9日 10:24:06
* Purpose: Implementation of the class OTCUserContextBuilder
***********************************************************************/

#include "OTCUserContextBuilder.h"
#include "TradingDeskContextBuilder.h"

////////////////////////////////////////////////////////////////////////
// Name:       OTCUserContextBuilder::BuildContext(ISession* pSession)
// Purpose:    Implementation of OTCUserContextBuilder::BuildContext()
// Parameters:
// - pSession
// Return:     void
////////////////////////////////////////////////////////////////////////

void OTCUserContextBuilder::BuildContext(const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	auto it = _userContextMap.find(session->getUserInfo()->getRole());

	if (it != _userContextMap.end())
	{
		it->second->BuildContext(msgProcessor, session);
	}
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCUserContextBuilder::OTCUserContextBuilder()
// Purpose:    Implementation of OTCUserContextBuilder::OTCUserContextBuilder()
// Return:     
////////////////////////////////////////////////////////////////////////

OTCUserContextBuilder::OTCUserContextBuilder()
{
	_userContextMap[UserRoleType::ROLE_TRADINGDESK] =
		std::make_shared<TradingDeskContextBuilder>();
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCUserContextBuilder::~OTCUserContextBuilder()
// Purpose:    Implementation of OTCUserContextBuilder::~OTCUserContextBuilder()
// Return:     
////////////////////////////////////////////////////////////////////////

OTCUserContextBuilder::~OTCUserContextBuilder()
{
	// TODO : implement
}