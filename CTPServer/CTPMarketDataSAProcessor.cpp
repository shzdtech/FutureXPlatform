/***********************************************************************
 * Module:  CTPMarketDataSAProcessor.cpp
 * Author:  milk
 * Modified: 2014年10月6日 22:17:16
 * Purpose: Implementation of the class CTPMarketDataSAProcessor
 ***********************************************************************/

#include "../litelogger/LiteLogger.h"
#include "CTPMarketDataSAProcessor.h"
#include "CTPUtility.h"
#include "../message/DefMessageID.h"
#include "../message/message_macro.h"
#include "../bizutility/ExchangeRouterTable.h"

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

CTPMarketDataSAProcessor::CTPMarketDataSAProcessor(IServerContext* pServerCtx)
	: _marketDataMap(16), 
	MarketDataNotifers(SessionContainer<std::string>::NewInstancePtr())
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
	_initializer = std::async(std::launch::async,
		[this]() {
		auto millsec = std::chrono::milliseconds(500);
		while (!_closing)
		{
			if (!CTPUtility::HasReturnError(LoginSystemUserIfNeed()))
				break;

			for (int cum_ms = 0; cum_ms < RetryInterval; cum_ms += millsec.count())
			{
				std::this_thread::sleep_for(millsec);
			}
		}
	});
}


int CTPMarketDataSAProcessor::LoginSystemUser(void)
{
	CThostFtdcReqUserLoginField req{};
	std::strncpy(req.BrokerID, _systemUser.getBrokerId().data(), sizeof(req.BrokerID));
	std::strncpy(req.UserID, _systemUser.getInvestorId().data(), sizeof(req.UserID));
	std::strncpy(req.Password, _systemUser.getPassword().data(), sizeof(req.Password));
	return _rawAPI->MdAPIProxy()->get()->ReqUserLogin(&req, 0);
}

int CTPMarketDataSAProcessor::LoginSystemUserIfNeed(void)
{
	std::lock_guard<std::mutex> lock(_loginMutex);

	int ret = 0;

	if (!_isLogged || !_isConnected)
	{
		std::string address(_systemUser.getServer());
		CreateCTPAPI(_systemUser.getInvestorId(), address);
		ret = LoginSystemUser();
		if (ret == -1)
		{
			if (ExchangeRouterTable::TryFind(_systemUser.getBrokerId() + ':' + ExchangeRouterTable::TARGET_MD_AM, address))
			{
				CreateCTPAPI(_systemUser.getInvestorId(), address);
				ret = LoginSystemUser();
			}
		}

		if (ret == 0)
		{
			LOG_INFO << getServerContext()->getServerUri() << ": System user " << _systemUser.getInvestorId()
				<< " has connected to market data server at: " << address;
		}
		else
		{
			LOG_WARN << getServerContext()->getServerUri() << ": System user " << _systemUser.getInvestorId()
				<< " cannot connect to market data server at: " << address;
		}
	}

	return ret;
}

void CTPMarketDataSAProcessor::DispatchUserMessage(int msgId, int serialId, const std::string& userId, const dataobj_ptr& dataobj_ptr)
{
	_userSessionCtn_Ptr->foreach(userId, [msgId, serialId, dataobj_ptr, this](const IMessageSession_Ptr& session_ptr)
	{ SendDataObject(session_ptr, msgId, serialId, dataobj_ptr); });
}

MarketDataDOMap & CTPMarketDataSAProcessor::GetMarketDataMap()
{
	return _marketDataMap;
}


int CTPMarketDataSAProcessor::ResubMarketData()
{
	if (_isLogged)
	{
		for (auto pair : _marketDataMap.lock_table())
		{
			char* contract[] = { const_cast<char*>(pair.first.data()) };
			_rawAPI->MdAPIProxy()->get()->SubscribeMarketData(contract, 1);
		}
	}

	return _marketDataMap.size();
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
			userInfo.setInvestorId(_systemUser.getInvestorId());
			userInfo.setPermission(ALLOW_TRADING);
			userInfo.setFrontId(_systemUser.getFrontId());
			userInfo.setSessionId(_systemUser.getSessionId());
			userInfo.setTradingDay(_systemUser.getTradingDay());
		}

		_isLogged = true;

		ResubMarketData();

		LOG_INFO << getServerContext()->getServerUri() << ": System user " << _systemUser.getUserId() << " has logged on market data server..";
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
	_marketDataMap.update_fn(pDepthMarketData->InstrumentID, [this, pDepthMarketData](MarketDataDO& mdo)
	{
		mdo.PreClosePrice = pDepthMarketData->PreClosePrice;
		mdo.UpperLimitPrice = pDepthMarketData->UpperLimitPrice;
		mdo.LowerLimitPrice = pDepthMarketData->LowerLimitPrice;
		mdo.PreSettlementPrice = pDepthMarketData->PreSettlementPrice;

		mdo.PreOpenInterest = pDepthMarketData->PreOpenInterest;
		mdo.OpenInterest = pDepthMarketData->OpenInterest;
		mdo.LastPrice = pDepthMarketData->LastPrice;
		mdo.Volume = pDepthMarketData->Volume;
		mdo.TradingDay = _tradingDay;

		if (pDepthMarketData->OpenPrice < 1e32)
		{
			mdo.OpenPrice = pDepthMarketData->OpenPrice;
			mdo.HighestPrice = pDepthMarketData->HighestPrice;
			mdo.LowestPrice = pDepthMarketData->LowestPrice;
			mdo.Turnover = pDepthMarketData->Turnover;
			mdo.AveragePrice = pDepthMarketData->AveragePrice;
		}

		if (pDepthMarketData->SettlementPrice < 1e32)
		{
			mdo.SettlementPrice = pDepthMarketData->SettlementPrice;
		}

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

		MarketDataNotifers->foreach(mdo.InstrumentID(), [this, &mdo](const IMessageSession_Ptr& session_ptr)
		{ SendDataObject(session_ptr, MSG_ID_RET_MARKETDATA, 0, std::make_shared<MarketDataDO>(mdo)); });
	});
}