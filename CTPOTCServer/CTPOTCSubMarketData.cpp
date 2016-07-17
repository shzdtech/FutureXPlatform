/***********************************************************************
 * Module:  CTPOTCSubMarketData.cpp
 * Author:  milk
 * Modified: 2015年8月2日 14:14:39
 * Purpose: Implementation of the class CTPOTCSubMarketData
 ***********************************************************************/

#include "CTPOTCSubMarketData.h"
#include "../CTPServer/tradeapi/ThostFtdcMdApi.h"
#include "../CTPServer/CTPUtility.h"
#include "../common/BizErrorIDs.h"

#include "../common/Attribute_Key.h"
#include "../message/GlobalProcessorRegistry.h"
#include "CTPOTCWorkerProcessor.h"
#include "../CTPServer/CTPWorkerProcessorID.h"

#include <set>

#include "../message/BizError.h"
#include "../message/SysParam.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/UserContractParam.h"
#include "../dataobject/MarketDataDO.h"

#include "../databaseop/BaseContractDAO.h"
#include "../databaseop/ContractDAO.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCSubMarketData::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCSubMarketData::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCSubMarketData::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto otcContractVec = ContractDAO::FindContractByClient(session->getUserInfo()->getName());

	ThrowNotFoundException(otcContractVec);

	auto userContractMap_Ptr =
		std::make_shared < UserContractParamMap >();

	auto ret = std::make_shared<VectorDO<PricingDO>>();
	ret->SerialId = reqDO->SerialId;

	for (auto& it : *otcContractVec)
	{
		UserContractParam ucp(it.ExchangeID(), it.InstrumentID());
		userContractMap_Ptr->emplace(ucp, std::move(ucp));

		PricingDO mdo(it.ExchangeID(), it.InstrumentID());
		mdo.AskPrice = 0;
		mdo.BidPrice = 0;
		ret->push_back(std::move(mdo));
	}

	session->getContext()->setAttribute(STR_KEY_USER_CONTRACTS, userContractMap_Ptr);

	if (auto workPrc = GlobalProcessorRegistry::FindProcessor
		(CTPWorkerProcessorID::WORKPROCESSOR_OTC))
	{
		auto otcworkproc = (std::static_pointer_cast<CTPOTCWorkerProcessor>(workPrc));
		for (auto& it : *ret)
			otcworkproc->RegisterPricingListener(it, (IMessageSession*)session);
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCSubMarketData::HandleResponse(const uint32_t serialId, param_vector rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCSubMarketData::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCSubMarketData::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	if (CTPUtility::HasError(rawRespParams[1]))
	{
		throw BizException("Fail to subscribe OTC market data.");
	}

	dataobj_ptr ret;

	return ret;
}