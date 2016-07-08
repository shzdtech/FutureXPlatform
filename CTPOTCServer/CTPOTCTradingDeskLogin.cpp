/***********************************************************************
 * Module:  CTPOTCTradingDeskLogin.cpp
 * Author:  milk
 * Modified: 2015年9月3日 13:30:09
 * Purpose: Implementation of the class CTPOTCTradingDeskLogin
 ***********************************************************************/

#include "CTPOTCTradingDeskLogin.h"

#include "../CTPServer/CTPWorkerProcessorID.h"
#include "CTPOTCWorkerProcessor.h"

#include "../CTPServer/CTPUtility.h"
#include "../message/GlobalProcessorRegistry.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCTradingDeskLogin::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCTradingDeskLogin::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCTradingDeskLogin::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto workPrc = std::static_pointer_cast<CTPOTCWorkerProcessor>
		(GlobalProcessorRegistry::FindProcessor(CTPWorkerProcessorID::WORKPROCESSOR_OTC));
	if (workPrc)
	{
		int ret = workPrc->LoginSystemUserIfNeed();
		CTPUtility::HasReturnError(ret);
	}

	return CTPOTCLogin::HandleRequest(reqDO, rawAPI, session);
}