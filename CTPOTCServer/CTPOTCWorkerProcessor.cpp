/***********************************************************************
 * Module:  CTPOTCWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2015年9月2日 14:47:00
 * Purpose: Implementation of the class CTPOTCWorkerProcessor
 ***********************************************************************/

#include "CTPOTCWorkerProcessor.h"
#include "../CTPServer/CTPConstant.h"

#include "../common/Attribute_Key.h"

#include "../message/DefMessageID.h"
#include "../message/SysParam.h"
#include "../message/message_macro.h"
#include "../message/AppContext.h"
#include "../dataobject/EnumTypes.h"
#include "../pricingengine/PricingUtility.h"

#include "../databaseop/OTCOrderDAO.h"
#include "../databaseop/ContractDAO.h"

#include "../bizutility/ContractCache.h"

#include "../ordermanager/OrderSeqGen.h"
 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPOTCWorkerProcessor::CTPOTCWorkerProcessor(const std::map<std::string, std::string>& configMap)
 // Purpose:    Implementation of CTPOTCWorkerProcessor::CTPOTCWorkerProcessor()
 // Parameters:
 // - frontserver
 // Return:     
 ////////////////////////////////////////////////////////////////////////

CTPOTCWorkerProcessor::CTPOTCWorkerProcessor(const std::map<std::string, std::string>& configMap,
	IServerContext* pServerCtx,
	const std::shared_ptr<CTPOTCTradeProcessor>& otcTradeProcessorPtr) :
	CTPMarketDataProcessor(configMap),
	_ctpOtcTradeProcessorPtr(otcTradeProcessorPtr)
{

}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCWorkerProcessor::~CTPOTCWorkerProcessor()
// Purpose:    Implementation of CTPOTCWorkerProcessor::~CTPOTCWorkerProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPOTCWorkerProcessor::~CTPOTCWorkerProcessor()
{
	// TODO : implement
}

void CTPOTCWorkerProcessor::setSession(IMessageSession_WkPtr msgSession_wk_ptr)
{
	CTPMarketDataProcessor::setSession(msgSession_wk_ptr);
	((CTPOTCTradeProcessor*)GetOTCTradeProcessor())->setSession(msgSession_wk_ptr);
}

bool CTPOTCWorkerProcessor::OnSessionClosing(void)
{
	return GetOTCTradeProcessor()->Dispose();
}

void CTPOTCWorkerProcessor::Initialize(void)
{
	auto productType = GetProductType();
	if (auto vectPtr = ContractDAO::FindContractByProductType(productType))
	{
		for (auto& contract : *vectPtr)
			ContractCache::Get(productType).Add(contract);
	}

	CTPMarketDataProcessor::Initialize();
	((CTPOTCTradeProcessor*)GetOTCTradeProcessor())->Initialize();

	LoginSystemUserIfNeed();
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCWorkerProcessor::LoginIfNeed()
// Purpose:    Implementation of CTPOTCWorkerProcessor::LoginIfNeed()
// Return:     void
////////////////////////////////////////////////////////////////////////

int CTPOTCWorkerProcessor::LoginSystemUserIfNeed(void)
{
	int ret = 0;

	if (!_isLogged)
	{
		CThostFtdcReqUserLoginField req{};
		std::strcpy(req.BrokerID, SysParam::Get(CTP_MD_BROKERID).data());
		std::strcpy(req.UserID, SysParam::Get(CTP_MD_USERID).data());
		std::strcpy(req.Password, SysParam::Get(CTP_MD_PASSWORD).data());
		ret = ((CTPRawAPI*)getRawAPI())->MdAPI->ReqUserLogin(&req, AppContext::GenNextSeq());
	}

	if (ret == 0)
		ret = ((CTPOTCTradeProcessor*)GetOTCTradeProcessor())->LoginSystemUserIfNeed();

	return ret;
}

int CTPOTCWorkerProcessor::RefreshStrategy(const StrategyContractDO& strategyDO)
{
	int ret = OTCWorkerProcessor::RefreshStrategy(strategyDO);

	if (ret > 0)
	{
		auto instPtr = std::make_unique<char*[]>(ret);
		if (strategyDO.IsOTC())
		{
			int i = 0;
			for (auto& contract : strategyDO.PricingContracts)
			{
				instPtr[i++] = const_cast<char*>(contract.InstrumentID().data());
			}
		}
		else
		{
			instPtr[0] = const_cast<char*>(strategyDO.InstrumentID().data());
		}
		ret = _rawAPI->MdAPI->SubscribeMarketData(instPtr.get(), ret);
	}

	return ret;
}

void CTPOTCWorkerProcessor::RegisterLoggedSession(IMessageSession * pMessageSession)
{
	((CTPOTCTradeProcessor*)GetOTCTradeProcessor())->RegisterLoggedSession(pMessageSession);
}

ProductType CTPOTCWorkerProcessor::GetProductType()
{
	return ProductType::PRODUCT_OTC;
}

OTCTradeProcessor * CTPOTCWorkerProcessor::GetOTCTradeProcessor()
{
	return _ctpOtcTradeProcessorPtr.get();
}


//CTP APIs

void CTPOTCWorkerProcessor::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	auto mdMap = GetOTCTradeProcessor()->GetPricingContext()->GetMarketDataMap();
	auto it = mdMap->find(pDepthMarketData->InstrumentID);
	if (it != mdMap->end())
	{
		auto& mdo = it->second;
		if (mdo.BidPrice != pDepthMarketData->BidPrice1 ||
			mdo.AskPrice != pDepthMarketData->AskPrice1 ||
			mdo.BidVolume != pDepthMarketData->BidVolume1 ||
			mdo.AskVolume != pDepthMarketData->AskVolume1)
		{
			mdo.BidPrice = pDepthMarketData->BidPrice1;
			mdo.AskPrice = pDepthMarketData->AskPrice1;
			mdo.BidVolume = pDepthMarketData->BidVolume1;
			mdo.AskVolume = pDepthMarketData->AskVolume1;

			//mdo.PreClosePrice = pDepthMarketData->PreClosePrice;
			//mdo.OpenPrice = pDepthMarketData->OpenPrice;
			//mdo.HighestPrice = pDepthMarketData->HighestPrice;
			//mdo.LowestPrice = pDepthMarketData->LowestPrice;
			//mdo.Volume = pDepthMarketData->Volume;
			//mdo.Turnover = pDepthMarketData->Turnover;
			//mdo.UpperLimitPrice = pDepthMarketData->UpperLimitPrice;
			//mdo.LowerLimitPrice = pDepthMarketData->LowerLimitPrice;
			//mdo.PreSettlementPrice = pDepthMarketData->PreSettlementPrice;
			//mdo.SettlementPrice = pDepthMarketData->SettlementPrice;

			// Start to trigger pricing
			TriggerUpdating(mdo);
		}
	}
}

void CTPOTCWorkerProcessor::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
}

void CTPOTCWorkerProcessor::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
}


