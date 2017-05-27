/***********************************************************************
 * Module:  CTPOTCWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2015年9月2日 14:47:00
 * Purpose: Implementation of the class CTPOTCWorkerProcessor
 ***********************************************************************/

#include "CTPOTCWorkerProcessor.h"

#include "../CTPServer/CTPUtility.h"
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

	std::string brokerid;
	if (!_serverCtx->getConfigVal(CTP_MD_BROKERID, brokerid))
		brokerid = SysParam::Get(CTP_MD_BROKERID);
	_systemUser.setBrokerId(brokerid);

	std::string userid;
	if (!_serverCtx->getConfigVal(CTP_MD_USERID, userid))
		userid = SysParam::Get(CTP_MD_USERID);
	_systemUser.setInvestorId(userid);
	_systemUser.setUserId(userid);

	std::string pwd;
	if (!_serverCtx->getConfigVal(CTP_MD_PASSWORD, pwd))
		pwd = SysParam::Get(CTP_MD_PASSWORD);
	_systemUser.setPassword(pwd);

	std::string address;
	if (!_serverCtx->getConfigVal(CTP_MD_SERVER, address))
	{
		ExchangeRouterTable::TryFind(_systemUser.getBrokerId() + ':' + ExchangeRouterTable::TARGET_MD, address);
	}
	_systemUser.setServer(address);
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

void CTPOTCWorkerProcessor::setMessageSession(const IMessageSession_Ptr& msgSession_ptr)
{
	CTPMarketDataProcessor::setMessageSession(msgSession_ptr);
	((CTPOTCTradeProcessor*)GetOTCTradeProcessor())->setMessageSession(msgSession_ptr);
}

void CTPOTCWorkerProcessor::setServiceLocator(const IMessageServiceLocator_Ptr& svcLct_Ptr)
{
	CTPMarketDataProcessor::setServiceLocator(svcLct_Ptr);
	((CTPOTCTradeProcessor*)GetOTCTradeProcessor())->setServiceLocator(svcLct_Ptr);
}

bool CTPOTCWorkerProcessor::OnSessionClosing(void)
{
	_closing = true;

	if (_initializer.valid())
		_initializer.wait();

	GetOTCTradeProcessor()->Dispose();

	return true;
}

void CTPOTCWorkerProcessor::Initialize(IServerContext* serverCtx)
{
	OTCWorkerProcessor::Initialize();

	CTPMarketDataProcessor::Initialize(serverCtx);

	LoadDataAsync();
}

int CTPOTCWorkerProcessor::LoginSystemUser(void)
{
	CThostFtdcReqUserLoginField req{};
	std::strncpy(req.BrokerID, _systemUser.getBrokerId().data(), sizeof(req.BrokerID));
	std::strncpy(req.UserID, _systemUser.getUserId().data(), sizeof(req.UserID));
	std::strncpy(req.Password, _systemUser.getPassword().data(), sizeof(req.Password));
	return ((CTPRawAPI*)getRawAPI())->MdAPI->ReqUserLogin(&req, AppContext::GenNextSeq());
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCWorkerProcessor::LoginIfNeed()
// Purpose:    Implementation of CTPOTCWorkerProcessor::LoginIfNeed()
// Return:     void
////////////////////////////////////////////////////////////////////////

int CTPOTCWorkerProcessor::LoginSystemUserIfNeed(void)
{
	int ret = 0;

	if (!_isLogged || !_isConnected)
	{
		_isLogged = false;

		std::string address(_systemUser.getServer());
		CreateCTPAPI(_systemUser.getUserId(), address);

		ret = LoginSystemUser();
		if (ret == -1)
		{
			if (ExchangeRouterTable::TryFind(_systemUser.getBrokerId() + ':' + ExchangeRouterTable::TARGET_MD_AM, address))
			{
				CreateCTPAPI(_systemUser.getUserId(), address);
				ret = LoginSystemUser();
			}
		}

		if (ret == 0)
		{
			LOG_INFO << getServerContext()->getServerUri() << ": System user " << _systemUser.getUserId()
				<< " has connected to market data server at: " << address;
		}
		else
		{
			LOG_WARN << getServerContext()->getServerUri() << ": System user " << _systemUser.getUserId()
				<< " cannot connect to market data server at: " << address;
		}
	}

	return ret;
}

int CTPOTCWorkerProcessor::LoadDataAsync(void)
{
	_initializer = std::async(std::launch::async, [this]() {
		auto millsec = std::chrono::milliseconds(500);
		while (!_closing)
		{
			if (!CTPUtility::HasReturnError(LoginSystemUserIfNeed()))
				break;

			for (int cum_ms = 0; cum_ms < RetryInterval && !_closing; cum_ms += millsec.count())
			{
				std::this_thread::sleep_for(millsec);
			}
		}
	});

	return 0;
}


int CTPOTCWorkerProcessor::SubscribeMarketData(const ContractKey& contractId)
{
	char* contract[] = { const_cast<char*>(contractId.InstrumentID().data()) };
	int ret = _rawAPI->MdAPI->SubscribeMarketData(contract, 1);
	return ret;
}

void CTPOTCWorkerProcessor::RegisterLoggedSession(const IMessageSession_Ptr& sessionPtr)
{
	((CTPOTCTradeProcessor*)GetOTCTradeProcessor())->RegisterLoggedSession(sessionPtr);
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

CTPOTCTradeProcessor * CTPOTCWorkerProcessor::GetCTPOTCTradeProcessor()
{
	return _ctpOtcTradeProcessorPtr.get();
}




//CTP APIs

void CTPOTCWorkerProcessor::OnFrontConnected()
{
	_isConnected = true;

	auto loginTime = _systemUser.getLoginTime();
	auto now = std::time(nullptr);
	if (_isLogged && (now - loginTime) > 600)
	{
		LoginSystemUser();
		ResubMarketData();
	}
}


void CTPOTCWorkerProcessor::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	auto mdMap = PricingDataContext()->GetMarketDataMap();
	MarketDataDO mdo;
	if (mdMap->find(pDepthMarketData->InstrumentID, mdo))
	{
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

			mdMap->update(pDepthMarketData->InstrumentID, mdo);

			// Start to trigger pricing
			TriggerUpdateByMarketData(mdo);
		}
	}
}

void CTPOTCWorkerProcessor::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ }

void CTPOTCWorkerProcessor::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ }

void CTPOTCWorkerProcessor::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (CTPUtility::HasError(pRspInfo))
	{
		LOG_ERROR << _serverCtx->getServerUri() << ": " << pRspInfo->ErrorMsg;
	}
	else
	{
		if (auto session = getMessageSession())
		{
			session->setLoginTimeStamp();
			auto& userInfo = session->getUserInfo();
			userInfo.setBrokerId(pRspUserLogin->BrokerID);
			userInfo.setInvestorId(pRspUserLogin->UserID);
			userInfo.setUserId(CTPUtility::MakeUserID(pRspUserLogin->BrokerID, pRspUserLogin->UserID));
			userInfo.setPermission(ALLOW_TRADING);
			userInfo.setFrontId(pRspUserLogin->FrontID);
			userInfo.setSessionId(pRspUserLogin->SessionID);
			userInfo.setTradingDay(std::atoi(pRspUserLogin->TradingDay));

			_systemUser.setLoginTime(std::time(nullptr));
			_systemUser.setFrontId(pRspUserLogin->FrontID);
			_systemUser.setSessionId(pRspUserLogin->SessionID);
		}

		_isLogged = true;

		_tradingDay = std::atoi(pRspUserLogin->TradingDay);

		ResubMarketData();

		LOG_INFO << getServerContext()->getServerUri() << ": System user " << _systemUser.getUserId() << " has logged on market data server..";
		LOG_FLUSH;
	}
}

int CTPOTCWorkerProcessor::ResubMarketData(void)
{
	int ret = 0;

	if (auto mdMap = PricingDataContext()->GetMarketDataMap())
	{
		auto table = mdMap->lock_table();
		for (auto it : table)
		{
			if (SubscribeMarketData(it.first) == 0)
			{
				ret++;
			}
		}
	}

	return ret;
}