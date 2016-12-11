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

#include "../message/MessageUtility.h"
#include "../common/BizErrorIDs.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../dataobject/ExchangeDO.h"

#include "../message/BizError.h"
#include "../message/DefMessageID.h"

#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"


#include "CTPUtility.h"
 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPQueryExchange::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPQueryExchange::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryExchange::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
	{
		auto stdo = (MapDO<std::string>*)reqDO.get();
		auto& exchangeid = stdo->TryFind(STR_EXCHANGE_ID, EMPTY_STRING);

		auto& exchangeInfo = pWorkerProc->GetExchangeInfo();
		if (exchangeInfo.empty())
		{
			CThostFtdcQryExchangeField req{};
			int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryExchange(&req, serialId);
			CTPUtility::CheckReturnError(iRet);

			// std::this_thread::sleep_for(CTPProcessor::DefaultQueryTime);

			return nullptr;
		}

		// ThrowNotFoundExceptionIfEmpty(&exchangeInfo);

		if (exchangeid.empty())
		{
			auto endit = exchangeInfo.end();
			for (auto it = exchangeInfo.begin(); it != endit; it++)
			{
				auto exchangeDO_Ptr = std::make_shared<ExchangeDO>(*it);
				exchangeDO_Ptr->HasMore = std::next(it) != endit ;

				pWorkerProc->SendDataObject(session, MSG_ID_QUERY_EXCHANGE, serialId, exchangeDO_Ptr);
			}
		}
		else
		{
			ExchangeDO exchangeDO;
			exchangeDO.ExchangeID = exchangeid;
			auto it = exchangeInfo.find(exchangeDO);
			if (it != exchangeInfo.end())
			{
				auto exchangeDO_Ptr = std::make_shared<ExchangeDO>(*it);
				pWorkerProc->SendDataObject(session, MSG_ID_QUERY_EXCHANGE, serialId, exchangeDO_Ptr);
			}
			else
			{
				throw NotFoundException(exchangeid + " does not exist!");
			}
		}
	}
	else
	{
		CThostFtdcQryExchangeField req{};
		int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryExchange(&req, serialId);
		CTPUtility::CheckReturnError(iRet);
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryExchange::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPQueryExchange::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryExchange::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CTPUtility::CheckNotFound(rawRespParams[0]);
	CTPUtility::CheckError(rawRespParams[1]);

	auto pDO = new ExchangeDO;
	dataobj_ptr ret(pDO);

	pDO->HasMore = !(*((bool*)rawRespParams[3]));

	if (auto pData = (CThostFtdcExchangeField*)rawRespParams[0])
	{
		pDO->ExchangeID = boost::locale::conv::to_utf<char>(pData->ExchangeID, CHARSET_GB2312);
		pDO->Name = boost::locale::conv::to_utf<char>(pData->ExchangeName, CHARSET_GB2312);
		pDO->Property = pData->ExchangeProperty;

		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
		{
			auto& exchangeSet = pWorkerProc->GetExchangeInfo();
			if (exchangeSet.find(*pDO) == exchangeSet.end())
			{
				exchangeSet.insert(*pDO);
			}
		}
	}

	return ret;
}