/***********************************************************************
 * Module:  CTPQueryExchange.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:36:22
 * Purpose: Implementation of the class CTPQueryExchange
 ***********************************************************************/

#include "CTPQueryExchange.h"
#include "CTPRawAPI.h"
#include "CTPTradeWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"

#include "../message/GlobalProcessorRegistry.h"
#include "../common/BizErrorIDs.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../dataobject/ExchangeDO.h"

#include "../message/BizError.h"
#include "../message/DefMessageID.h"

#include "../utility/Encoding.h"
#include "../utility/TUtil.h"


#include "CTPUtility.h"
 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPQueryExchange::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of CTPQueryExchange::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryExchange::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (MapDO<std::string>*)reqDO.get();
	auto& exchangeid = stdo->TryFind(STR_EXCHANGE_ID, EMPTY_STRING);

	CThostFtdcQryExchangeField req{};

	if (auto wkProcPtr = std::static_pointer_cast<CTPTradeWorkerProcessor>
		(GlobalProcessorRegistry::FindProcessor(CTPWorkerProcessorID::TRADE_SHARED_ACCOUNT)))
	{
		auto& exchangeInfo = wkProcPtr->GetExchangeInfo();

		if (exchangeInfo.size() < 1)
		{
			int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryExchange(&req, reqDO->SerialId);
			CTPUtility::CheckReturnError(iRet);

			std::this_thread::sleep_for(std::chrono::seconds(3));
		}

		if (exchangeInfo.size() > 0)
		{
			if (exchangeid != EMPTY_STRING)
			{
				ExchangeDO exchangeDO;
				exchangeDO.ExchangeID = exchangeid;
				auto it = exchangeInfo.find(exchangeDO);
				if (it != exchangeInfo.end())
				{ 
					wkProcPtr->SendDataObject(session, MSG_ID_QUERY_EXCHANGE,
						std::make_shared<ExchangeDO>(*it));
				}
				else
				{
					throw BizError(OBJECT_NOT_FOUND, exchangeid + " does not exist!");
				}
			}
			else
			{
				auto lastit = std::prev(exchangeInfo.end());
				for (auto it = exchangeInfo.begin(); it != exchangeInfo.end(); it++)
				{
					auto exchangeDO_Ptr = std::make_shared<ExchangeDO>(*it);
					exchangeDO_Ptr->SerialId = stdo->SerialId;
					if (it == lastit)
						exchangeDO_Ptr->HasMore = true;
					wkProcPtr->SendDataObject(session, MSG_ID_QUERY_EXCHANGE, exchangeDO_Ptr);
				}
			}
		}
	}
	else
	{
		int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryExchange(&req, reqDO->SerialId);
		CTPUtility::CheckReturnError(iRet);
	}


	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryExchange::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryExchange::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryExchange::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckError(rawRespParams[1]);

	dataobj_ptr ret;

	if (auto pData = (CThostFtdcExchangeField*)rawRespParams[0])
	{
		auto pDO = new ExchangeDO;
		ret.reset(pDO);

		pDO->SerialId = serialId;
		pDO->HasMore = !(*((bool*)rawRespParams[3]));
		pDO->ExchangeID = Encoding::ToUTF8(pData->ExchangeID, CHARSET_GB2312);
		pDO->Name = Encoding::ToUTF8(pData->ExchangeName, CHARSET_GB2312);
		pDO->Property = pData->ExchangeProperty;

		if (auto wkProcPtr = std::static_pointer_cast<CTPTradeWorkerProcessor>
			(GlobalProcessorRegistry::FindProcessor(CTPWorkerProcessorID::TRADE_SHARED_ACCOUNT)))
		{
			auto& exchangeSet = wkProcPtr->GetExchangeInfo();
			if (exchangeSet.find(*pDO) == exchangeSet.end())
			{
				exchangeSet.insert(*pDO);
			}
		}
	}

	return ret;
}