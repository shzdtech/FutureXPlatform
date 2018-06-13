/***********************************************************************
 * Module:  CTPMarketDataSAProcessor.cpp
 * Author:  milk
 * Modified: 2014年10月6日 22:17:16
 * Purpose: Implementation of the class CTPMarketDataSAProcessor
 ***********************************************************************/

#include "../litelogger/LiteLogger.h"
#include "CTPMarketDataSAProcessor.h"
#include "CTPUtility.h"
#include "../databaseop/ContractDAO.h"
#include "../message/DefMessageID.h"
#include "../message/message_macro.h"
#include "../bizutility/ExchangeRouterTable.h"
#include "../common/Attribute_Key.h"
#include "../bizutility/ManualOpHub.h"

#include "CTPConstant.h"

#include <future>
#include <filesystem>

namespace fs = std::experimental::filesystem;

////////////////////////////////////////////////////////////////////////
// Name:       CTPMarketDataSAProcessor::CTPMarketDataSAProcessor(const std::map<std::string, std::string>& configMap)
// Purpose:    Implementation of CTPMarketDataSAProcessor::CTPMarketDataSAProcessor()
// Parameters:
// - frontserver
// Return:     
////////////////////////////////////////////////////////////////////////

CTPMarketDataSAProcessor::CTPMarketDataSAProcessor(IServerContext* pServerCtx, MarketDataDOMap* pMktDataDOMap)
	: _pMarketDataMap(pMktDataDOMap), MarketDataNotifers(SessionContainer<std::string>::NewInstancePtr())
{
	_isLogged = false;
	_isConnected = false;

	setServerContext(pServerCtx);

	std::string defaultCfg;
	_serverCtx->getConfigVal(ExchangeRouterTable::TARGET_MD, defaultCfg);
	ExchangeRouterDO exDO;
	if (ExchangeRouterTable::TryFind(defaultCfg, exDO))
	{
		_systemUser.setBrokerId(exDO.BrokeID);
		_systemUser.setUserId(exDO.UserID);
		_systemUser.setInvestorId(exDO.UserID);
		_systemUser.setPassword(exDO.Password);
		_systemUser.setServer(exDO.Address);
	}
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPMarketDataSAProcessor::~CTPMarketDataSAProcessor()
// Purpose:    Implementation of CTPMarketDataSAProcessor::~CTPMarketDataSAProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPMarketDataSAProcessor::~CTPMarketDataSAProcessor() {
	_closing = true;

	if (_initializer.valid())
	{
		_initializer.wait();
	}

	LOG_DEBUG << __FUNCTION__;
}

void CTPMarketDataSAProcessor::Initialize(IServerContext* pServerCtx)
{
	std::string productTypes;
	if (getServerContext()->getConfigVal("product_type", productTypes))
	{
		std::vector<std::string> productVec;
		stringutility::split(productTypes, productVec);

		for (auto& strProductType : productVec)
		{
			if (auto contracts = ContractDAO::FindContractByProductType(std::stoi(strProductType)))
			{
				for(auto contract : *contracts)
					_pMarketDataMap->insert(contract.InstrumentID(), MarketDataDO(contract.ExchangeID(), contract.InstrumentID()));
			}
		}
	}


	_initializer = std::async(std::launch::async,
		[this]() {
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
}


int CTPMarketDataSAProcessor::LoginSystemUser(void)
{
	int ret = -1;
	if (auto mdAPI = _rawAPI->MdAPIProxy())
	{
		CThostFtdcReqUserLoginField req{};
		std::strncpy(req.BrokerID, _systemUser.getBrokerId().data(), sizeof(req.BrokerID));
		std::strncpy(req.UserID, _systemUser.getUserId().data(), sizeof(req.UserID));
		std::strncpy(req.Password, _systemUser.getPassword().data(), sizeof(req.Password));
		ret = mdAPI->get()->ReqUserLogin(&req, 0);
	}

	return ret;
}

int CTPMarketDataSAProcessor::LoginSystemUserIfNeed(void)
{
	std::lock_guard<std::mutex> lock(_loginMutex);

	int ret = 0;

	if (!_isLogged || !_isConnected)
	{
		std::string address(_systemUser.getServer());
		CreateCTPAPI(this, _systemUser.getUserId(), address);
		ret = LoginSystemUser();
		if (ret == -1)
		{
			LOG_WARN << getServerContext()->getServerUri() << ": System user " << _systemUser.getUserId()
				<< " cannot connect to market data server at: " << address;

			std::string defaultCfg;
			_serverCtx->getConfigVal(ExchangeRouterTable::TARGET_MD_AM, defaultCfg);
			ExchangeRouterDO exDO;
			if (ExchangeRouterTable::TryFind(defaultCfg, exDO))
			{
				CreateCTPAPI(this, _systemUser.getUserId(), exDO.Address);
				ret = LoginSystemUser();
			}

			if (ret == 0)
			{
				LOG_INFO << getServerContext()->getServerUri() << ": System user " << _systemUser.getUserId()
					<< " has connected to post market data server at: " << address;
			}
			else
			{
				LOG_WARN << getServerContext()->getServerUri() << ": System user " << _systemUser.getUserId()
					<< " cannot connect to post market data server at: " << address;
			}
		}
		else
		{
			LOG_INFO << getServerContext()->getServerUri() << ": System user " << _systemUser.getUserId()
				<< " has connected to market data server at: " << address;
		}
	}

	return ret;
}

void CTPMarketDataSAProcessor::DispatchUserMessage(int msgId, int serialId, const std::string& userId, const dataobj_ptr& dataobj_ptr)
{
	if (auto msg = Deserialize(msgId, serialId, dataobj_ptr))
	{
		_userSessionCtn_Ptr->foreach(userId, [msgId, &msg, this](const IMessageSession_Ptr& session_ptr)
		{SendDataObject(session_ptr, msgId, msg); });
	}
}

void CTPMarketDataSAProcessor::DispatchUserExceptionMessage(int msgId, int serialId, const std::string& userId, const MessageException& msgExcept)
{
	if (auto msg = DeserializeException(msgId, serialId, msgExcept))
	{
		_userSessionCtn_Ptr->foreach(userId, [msgId, &msg, this](const IMessageSession_Ptr& session_ptr)
		{ SendDataObject(session_ptr, msgId, msg); } );
	}
}

MarketDataDOMap & CTPMarketDataSAProcessor::GetMarketDataMap()
{
	return *_pMarketDataMap;
}


int CTPMarketDataSAProcessor::ResubMarketData()
{
	if (_isLogged)
	{
		for (auto pair : _pMarketDataMap->lock_table())
		{
			char* contract[] = { const_cast<char*>(pair.first.data()) };
			_rawAPI->MdAPIProxy()->get()->SubscribeMarketData(contract, 1);
		}
	}

	return _pMarketDataMap->size();
}

void CTPMarketDataSAProcessor::OnRspError(CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast) {
	LOG_DEBUG << __FUNCTION__ << ": " << pRspInfo->ErrorMsg;
}

void CTPMarketDataSAProcessor::OnFrontConnected() {
	_isConnected = true;

	LoginSystemUser();

	LOG_INFO << getServerContext()->getServerUri() << ": Front Connected.";
}

void CTPMarketDataSAProcessor::OnFrontDisconnected(int nReason) {
	_isConnected = false;
	if (nReason)
	{
		std::string errMsg;
		CTPUtility::LogFrontDisconnected(nReason, errMsg);
		LOG_WARN << getServerContext()->getServerUri() << " has disconnected: " << errMsg;
	}
}

void CTPMarketDataSAProcessor::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (CTPUtility::HasError(pRspInfo))
	{
		LOG_ERROR << _serverCtx->getServerUri() << ": " << pRspInfo->ErrorMsg;
	}
	else
	{
		_tradingDay = std::atoi(pRspUserLogin->TradingDay);

		_systemUser.setLoginTime(std::time(nullptr));
		_systemUser.setFrontId(pRspUserLogin->FrontID);
		_systemUser.setSessionId(pRspUserLogin->SessionID);
		_systemUser.setTradingDay(_tradingDay);

		if (auto session = getMessageSession())
		{
			session->setLoginTimeStamp();
			auto& userInfo = session->getUserInfo();
			userInfo.setUserId(_systemUser.getUserId());
			userInfo.setBrokerId(_systemUser.getBrokerId());
			userInfo.setInvestorId(_systemUser.getUserId());
			userInfo.setPermission(ALLOW_TRADING);
			userInfo.setFrontId(_systemUser.getFrontId());
			userInfo.setSessionId(_systemUser.getSessionId());
			userInfo.setTradingDay(_systemUser.getTradingDay());
		}

		_isLogged = true;

		ResubMarketData();

		LOG_INFO << getServerContext()->getServerUri() << ": System user " << _systemUser.getUserId() << " has logged on market data server.";
		LOG_FLUSH;
	}
}

void CTPMarketDataSAProcessor::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ }

void CTPMarketDataSAProcessor::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ }

void CTPMarketDataSAProcessor::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {
	_pMarketDataMap->update_fn(pDepthMarketData->InstrumentID, [this, pDepthMarketData](MarketDataDO& mdo)
	{
		mdo.PreClosePrice = pDepthMarketData->PreClosePrice;
		mdo.UpperLimitPrice = pDepthMarketData->UpperLimitPrice;
		mdo.LowerLimitPrice = pDepthMarketData->LowerLimitPrice;
		mdo.PreSettlementPrice = pDepthMarketData->PreSettlementPrice;

		mdo.PreOpenInterest = pDepthMarketData->PreOpenInterest;
		mdo.OpenInterest = pDepthMarketData->OpenInterest;
		mdo.LastPrice = pDepthMarketData->LastPrice;
		mdo.Volume = pDepthMarketData->Volume;

		mdo.OpenPrice = pDepthMarketData->OpenPrice < 1e32 ? pDepthMarketData->OpenPrice : NAN;
		mdo.ClosePrice = pDepthMarketData->ClosePrice < 1e32 ? pDepthMarketData->ClosePrice : NAN;
		mdo.HighestPrice = pDepthMarketData->HighestPrice < 1e32 ? pDepthMarketData->HighestPrice : NAN;
		mdo.LowestPrice = pDepthMarketData->LowestPrice < 1e32 ? pDepthMarketData->LowestPrice : NAN;
		mdo.Turnover = pDepthMarketData->Turnover < 1e32 ? pDepthMarketData->Turnover : NAN;
		mdo.AveragePrice = pDepthMarketData->AveragePrice < 1e32 ? pDepthMarketData->AveragePrice : NAN;
		mdo.SettlementPrice = pDepthMarketData->SettlementPrice < 1e32 ? pDepthMarketData->SettlementPrice : NAN;

		if (pDepthMarketData->BidPrice1 < 1e32)
		{
			mdo.Bid().Price = pDepthMarketData->BidPrice1;
		}
		mdo.Bid().Volume = pDepthMarketData->BidVolume1;

		if (pDepthMarketData->AskPrice1 < 1e32)
		{
			mdo.Ask().Price = pDepthMarketData->AskPrice1;
		}
		mdo.Ask().Volume = pDepthMarketData->AskVolume1;

		if (pDepthMarketData->UpdateTime[0])
		{
			int hour, min, sec;
			std::sscanf(pDepthMarketData->UpdateTime, "%d:%d:%d", &hour, &min, &sec);
			mdo.UpdateTime = hour * 3600 + min * 60 + sec;
		}

		ManualOpHub::Instance()->NotifyUpdateMarketData(mdo);

		if (auto msg = Deserialize(MSG_ID_RET_MARKETDATA, 0, std::make_shared<MarketDataDO>(mdo)))
		{
			MarketDataNotifers->foreach(mdo.InstrumentID(), [this, &msg](const IMessageSession_Ptr& session_ptr)
			{SendDataObject(session_ptr, MSG_ID_RET_MARKETDATA, msg); });
		}
	});
}