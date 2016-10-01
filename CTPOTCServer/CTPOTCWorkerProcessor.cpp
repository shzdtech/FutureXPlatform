/***********************************************************************
 * Module:  CTPOTCWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2015年9月2日 14:47:00
 * Purpose: Implementation of the class CTPOTCWorkerProcessor
 ***********************************************************************/

#include "CTPOTCWorkerProcessor.h"
#include "../litelogger/LiteLogger.h"

#include "../CTPServer/CTPConstant.h"

#include "../common/Attribute_Key.h"

#include "../message/DefMessageID.h"
#include "../message/message_macro.h"
#include "../systemsettings/AppContext.h"
#include "../dataobject/EnumTypes.h"
#include "../pricingengine/PricingUtility.h"

#include "../databaseop/OTCOrderDAO.h"
#include "../databaseop/ContractDAO.h"
#include "../databaseop/StrategyContractDAO.h"
#include "../databaseop/ModelParamsDAO.h"
#include "../pricingengine/PricingAlgorithmManager.h"
#include "../pricingengine/ModelAlgorithmManager.h"

#include "../bizutility/ContractCache.h"
#include "../bizutility/StrategyModelCache.h"

#include "../ordermanager/OrderSeqGen.h"
 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPOTCWorkerProcessor::CTPOTCWorkerProcessor(const std::map<std::string, std::string>& configMap)
 // Purpose:    Implementation of CTPOTCWorkerProcessor::CTPOTCWorkerProcessor()
 // Parameters:
 // - frontserver
 // Return:     
 ////////////////////////////////////////////////////////////////////////

CTPOTCWorkerProcessor::CTPOTCWorkerProcessor(IServerContext* pServerCtx,
	const std::shared_ptr<CTPOTCTradeProcessor>& otcTradeProcessorPtr) :
	OTCWorkerProcessor(otcTradeProcessorPtr->PricingDataContext()),
	_ctpOtcTradeProcessorPtr(otcTradeProcessorPtr)
{
	setServerContext(pServerCtx);
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCWorkerProcessor::~CTPOTCWorkerProcessor()
// Purpose:    Implementation of CTPOTCWorkerProcessor::~CTPOTCWorkerProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPOTCWorkerProcessor::~CTPOTCWorkerProcessor()
{
	LOG_DEBUG << __FUNCTION__;
}

void CTPOTCWorkerProcessor::setSession(const IMessageSession_WkPtr& msgSession_wk_ptr)
{
	CTPMarketDataProcessor::setSession(msgSession_wk_ptr);
	((CTPOTCTradeProcessor*)GetOTCTradeProcessor())->setSession(msgSession_wk_ptr);
}

bool CTPOTCWorkerProcessor::OnSessionClosing(void)
{
	return GetOTCTradeProcessor()->Dispose();
}

void CTPOTCWorkerProcessor::Initialize(IServerContext* serverCtx)
{
	OTCWorkerProcessor::Initialize();

	CTPMarketDataProcessor::Initialize(serverCtx);

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
		std::string value;

		CThostFtdcReqUserLoginField req{};
		if (!_serverCtx->getConfigVal(CTP_MD_BROKERID, value))
			value = SysParam::Get(CTP_MD_BROKERID);
		std::strcpy(req.BrokerID, value.data());

		if (!_serverCtx->getConfigVal(CTP_MD_USERID, value))
			value = SysParam::Get(CTP_MD_USERID);
		std::strcpy(req.UserID, value.data());

		if (!_serverCtx->getConfigVal(CTP_MD_PASSWORD, value))
			value = SysParam::Get(CTP_MD_PASSWORD);
		std::strcpy(req.Password, value.data());

		ret = ((CTPRawAPI*)getRawAPI())->MdAPI->ReqUserLogin(&req, AppContext::GenNextSeq());
	}

	return ret;
}

int CTPOTCWorkerProcessor::SubscribeMarketData(const ContractKey& contractId)
{
	char* contract[] = { const_cast<char*>(contractId.InstrumentID().data()) };
	return _rawAPI->MdAPI->SubscribeMarketData(contract, 1);
}

void CTPOTCWorkerProcessor::RegisterLoggedSession(IMessageSession * pMessageSession)
{
	((CTPOTCTradeProcessor*)GetOTCTradeProcessor())->RegisterLoggedSession(pMessageSession);
}

ProductType CTPOTCWorkerProcessor::GetContractProductType() const
{
	return ProductType::PRODUCT_OTC;
}

const std::vector<ProductType>& CTPOTCWorkerProcessor::GetStrategyProductTypes() const
{
	static const std::vector<ProductType> productTypes = { ProductType::PRODUCT_OTC };
	return productTypes;
}


OTCTradeProcessor * CTPOTCWorkerProcessor::GetOTCTradeProcessor()
{
	return _ctpOtcTradeProcessorPtr.get();
}


//CTP APIs

void CTPOTCWorkerProcessor::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	auto mdMap = PricingDataContext()->GetMarketDataMap();
	auto it = mdMap->find(pDepthMarketData->InstrumentID);
	if (it != mdMap->end())
	{
		auto& mdo = it->second;
		if (mdo.Bid().Price != pDepthMarketData->BidPrice1 ||
			mdo.Ask().Price != pDepthMarketData->AskPrice1 ||
			mdo.Bid().Volume != pDepthMarketData->BidVolume1 ||
			mdo.Ask().Volume != pDepthMarketData->AskVolume1)
		{
			mdo.Bid().Price = pDepthMarketData->BidPrice1;
			mdo.Ask().Price = pDepthMarketData->AskPrice1;
			mdo.Bid().Volume = pDepthMarketData->BidVolume1;
			mdo.Ask().Volume = pDepthMarketData->AskVolume1;

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


