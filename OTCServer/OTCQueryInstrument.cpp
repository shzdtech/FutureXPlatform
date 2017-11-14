/***********************************************************************
 * Module:  CTPQueryInstrument.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:06:13
 * Purpose: Implementation of the class CTPQueryInstrument
 ***********************************************************************/

#include "OTCQueryInstrument.h"
#include "OTCWorkerProcessor.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../dataobject/TypedefDO.h"

#include "../message/BizError.h"
#include "../message/message_macro.h"
#include "../message/IMessageServiceLocator.h"
#include "../message/MessageUtility.h"

#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"
#include "../utility/stringutility.h"

#include "../bizutility/ContractCache.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPQueryInstrument::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPQueryInstrument::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCQueryInstrument::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto stdo = (StringMapDO<std::string>*)reqDO.get();

	auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);
	auto& exchangeid = stdo->TryFind(STR_EXCHANGE_ID, EMPTY_STRING);
	auto& productid = stdo->TryFind(STR_PRODUCT_ID, EMPTY_STRING);

	VectorDO_Ptr<InstrumentDO> ret;

	if (auto pWorkerProc = MessageUtility::AbstractWorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		if (instrumentid == EMPTY_STRING && exchangeid == EMPTY_STRING && productid == EMPTY_STRING)
			ret = pWorkerProc->GetInstrumentCache().AllInstruments();
		else
			ret = pWorkerProc->GetInstrumentCache().QueryInstrument(instrumentid, exchangeid, productid);
	}

	ret->HasMore = false;
	return ret;
}