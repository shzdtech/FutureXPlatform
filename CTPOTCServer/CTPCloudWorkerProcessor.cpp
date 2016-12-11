/***********************************************************************
 * Module:  CTPCloudWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2015年9月2日 14:47:00
 * Purpose: Implementation of the class CTPCloudWorkerProcessor
 ***********************************************************************/

#include "CTPCloudWorkerProcessor.h"
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
#include "../bizutility/ExchangeRouterTable.h"

#include "../ordermanager/OrderSeqGen.h"
 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPCloudWorkerProcessor::CTPCloudWorkerProcessor(const std::map<std::string, std::string>& configMap)
 // Purpose:    Implementation of CTPCloudWorkerProcessor::CTPCloudWorkerProcessor()
 // Parameters:
 // - frontserver
 // Return:     
 ////////////////////////////////////////////////////////////////////////

CTPCloudWorkerProcessor::CTPCloudWorkerProcessor(IServerContext* pServerCtx,
	const std::shared_ptr<CTPOTCTradeProcessor>& otcTradeProcessorPtr) :
	OTCWorkerProcessor(otcTradeProcessorPtr->PricingDataContext()),
	_ctpOtcTradeProcessorPtr(otcTradeProcessorPtr)
{
	setServerContext(pServerCtx);
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPCloudWorkerProcessor::~CTPCloudWorkerProcessor()
// Purpose:    Implementation of CTPCloudWorkerProcessor::~CTPCloudWorkerProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPCloudWorkerProcessor::~CTPCloudWorkerProcessor()
{
	LOG_DEBUG << __FUNCTION__;
}

void CTPCloudWorkerProcessor::setMessageSession(const IMessageSession_Ptr& msgSession_ptr)
{
	CTPMarketDataProcessor::setMessageSession(msgSession_ptr);
	((CTPOTCTradeProcessor*)GetOTCTradeProcessor())->setMessageSession(msgSession_ptr);
}

bool CTPCloudWorkerProcessor::OnSessionClosing(void)
{
	return GetOTCTradeProcessor()->Dispose();
}

void CTPCloudWorkerProcessor::Initialize(IServerContext* serverCtx)
{
	OTCWorkerProcessor::Initialize();

	CTPMarketDataProcessor::Initialize(serverCtx);

	LoginSystemUserIfNeed();
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPCloudWorkerProcessor::LoginIfNeed()
// Purpose:    Implementation of CTPCloudWorkerProcessor::LoginIfNeed()
// Return:     void
////////////////////////////////////////////////////////////////////////

int CTPCloudWorkerProcessor::LoginSystemUserIfNeed(void)
{
	int ret = 0;

	if (!_isLogged)
	{
		std::string brokerid;
		CThostFtdcReqUserLoginField req{};
		if (!_serverCtx->getConfigVal(CTP_MD_BROKERID, brokerid))
			brokerid = SysParam::Get(CTP_MD_BROKERID);
		std::strncpy(req.BrokerID, brokerid.data(), sizeof(req.BrokerID));

		std::string usrid;
		if (!_serverCtx->getConfigVal(CTP_MD_USERID, usrid))
			usrid = SysParam::Get(CTP_MD_USERID);
		std::strncpy(req.UserID, usrid.data(), sizeof(req.UserID));

		std::string pwd;
		if (!_serverCtx->getConfigVal(CTP_MD_PASSWORD, pwd))
			pwd = SysParam::Get(CTP_MD_PASSWORD);
		std::strncpy(req.Password, pwd.data(), sizeof(req.Password));

		std::string address;
		ExchangeRouterTable::TryFind(brokerid + ':' + ExchangeRouterTable::TARGET_MD, address);
		InitializeServer(usrid, address);

		ret = ((CTPRawAPI*)getRawAPI())->MdAPI->ReqUserLogin(&req, AppContext::GenNextSeq());
	}

	return ret;
}

int CTPCloudWorkerProcessor::SubscribeMarketData(const ContractKey& contractId)
{
	char* contract[] = { const_cast<char*>(contractId.InstrumentID().data()) };
	return _rawAPI->MdAPI->SubscribeMarketData(contract, 1);
}

void CTPCloudWorkerProcessor::RegisterLoggedSession(const IMessageSession_Ptr& sessionPtr)
{
	((CTPOTCTradeProcessor*)GetOTCTradeProcessor())->RegisterLoggedSession(sessionPtr);
}

ProductType CTPCloudWorkerProcessor::GetContractProductType() const
{
	return ProductType::PRODUCT_OTC;
}

const std::vector<ProductType>& CTPCloudWorkerProcessor::GetStrategyProductTypes() const
{
	static const std::vector<ProductType> productTypes = { ProductType::PRODUCT_OTC };
	return productTypes;
}


OTCTradeProcessor * CTPCloudWorkerProcessor::GetOTCTradeProcessor()
{
	return _ctpOtcTradeProcessorPtr.get();
}


//CTP APIs

void CTPCloudWorkerProcessor::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
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

void CTPCloudWorkerProcessor::OnRspUserLogin(CThostFtdcRspUserLoginField * pRspUserLogin, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
}

void CTPCloudWorkerProcessor::OnRspUserLogout(CThostFtdcUserLogoutField * pUserLogout, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
}

void CTPCloudWorkerProcessor::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
}

void CTPCloudWorkerProcessor::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
}


