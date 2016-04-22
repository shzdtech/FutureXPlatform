/***********************************************************************
* Module:  CTPOTCUserContextBuilder.cpp
* Author:  milk
* Modified: 2016年2月9日 10:24:06
* Purpose: Implementation of the class CTPOTCUserContextBuilder
***********************************************************************/

#include "CTPOTCUserContextBuilder.h"
#include "TradingDeskContextBuilder.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCUserContextBuilder::BuildContext(ISession* pSession)
// Purpose:    Implementation of CTPOTCUserContextBuilder::BuildContext()
// Parameters:
// - pSession
// Return:     void
////////////////////////////////////////////////////////////////////////

void CTPOTCUserContextBuilder::BuildContext(ISession* pSession)
{
	auto it = _userContextMap.find(pSession->getUserInfo()->getRole());

	if (it != _userContextMap.end())
	{
		it->second->BuildContext(pSession);
	}
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCUserContextBuilder::CTPOTCUserContextBuilder()
// Purpose:    Implementation of CTPOTCUserContextBuilder::CTPOTCUserContextBuilder()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPOTCUserContextBuilder::CTPOTCUserContextBuilder()
{
	_userContextMap[UserRoleType::ROLE_TRADINGDESK] =
		std::make_shared<TradingDeskContextBuilder>();
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCUserContextBuilder::~CTPOTCUserContextBuilder()
// Purpose:    Implementation of CTPOTCUserContextBuilder::~CTPOTCUserContextBuilder()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPOTCUserContextBuilder::~CTPOTCUserContextBuilder()
{
	// TODO : implement
}