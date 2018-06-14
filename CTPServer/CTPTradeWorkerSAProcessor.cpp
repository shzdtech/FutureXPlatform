#include "CTPTradeWorkerSAProcessor.h"
#include "CTPTradeWorkerProcessor.h"
#include "CTPUtility.h"
#include "CTPConstant.h"
#include "../dataobject/TradeRecordDO.h"
#include "../common/Attribute_Key.h"
#include "../systemsettings/AppContext.h"
#include "../message/DefMessageID.h"
#include "../message/message_macro.h"
#include "../bizutility/ExchangeRouterTable.h"
#include "../bizutility/PositionPortfolioMap.h"

#include "../databaseop/VersionDAO.h"
#include "../databaseop/ContractDAO.h"
#include "../databaseop/TradeDAO.h"
#include "../ordermanager/OrderSeqGen.h"
#include "../ordermanager/OrderPortfolioCache.h"

#include "../bizutility/ContractCache.h"
#include "../litelogger/LiteLogger.h"

#include <boost/locale/encoding.hpp>

CTPTradeWorkerSAProcessor::CTPTradeWorkerSAProcessor(CTPTradeWorkerProcessor* pTradeWorkProc) : 
	_pTradeWorkProc(pTradeWorkProc)
{
}

CTPTradeWorkerSAProcessor::CTPTradeWorkerSAProcessor(const CTPRawAPI_Ptr & rawAPI, CTPTradeWorkerProcessor* pTradeWorkProc) :
	CTPTradeProcessor(rawAPI), _pTradeWorkProc(pTradeWorkProc)
{
}

CTPTradeWorkerSAProcessor::~CTPTradeWorkerSAProcessor()
{
}

int CTPTradeWorkerSAProcessor::Login(const std::string & brokerId, const std::string & investorId, const std::string & password, const std::string & serverName)
{
	int retcode = 0;
	auto& sysUser = getSystemUser();
	if (sysUser.getLoginTime() > 0 && sysUser.getBrokerId() == brokerId && sysUser.getInvestorId() == investorId && sysUser.getPassword() == password)
	{
		return retcode;
	}

	while (_loginFlag.test_and_set(std::memory_order_acquire));

	if (sysUser.getLoginTime() > 0)
	{
		return retcode;
	}

	_lastErrorCode = 0;
	_lastError.clear();

	std::string server = serverName;
	if (server.empty())
		getServerContext()->getConfigVal(ExchangeRouterTable::TARGET_TD, server);

	ExchangeRouterDO exDO;
	ExchangeRouterTable::TryFind(server, exDO);

	sysUser.setPassword(password);
	sysUser.setLoginTime(0);

	CThostFtdcReqUserLoginField req{};
	std::strncpy(req.BrokerID, brokerId.data(), sizeof(req.BrokerID));
	std::strncpy(req.UserID, investorId.data(), sizeof(req.UserID));
	std::strncpy(req.Password, password.data(), sizeof(req.Password));

	CreateBackendAPI(this, investorId, exDO.Address);

	CThostFtdcReqAuthenticateField reqAuth{};

	if (!exDO.AuthCode.empty())
	{
		std::strncpy(reqAuth.BrokerID, exDO.BrokeID.data(), sizeof(reqAuth.BrokerID));
		std::strncpy(reqAuth.UserID, investorId.data(), sizeof(reqAuth.UserID));
		std::strncpy(reqAuth.AuthCode, exDO.AuthCode.data(), sizeof(reqAuth.AuthCode));
		std::strncpy(reqAuth.UserProductInfo, exDO.ProductInfo.data(), sizeof(reqAuth.UserProductInfo));

		retcode = TradeApi()->get()->ReqAuthenticate(&reqAuth, 0);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	if (retcode == 0)
		retcode = TradeApi()->get()->ReqUserLogin(&req, 0);

	// try after market server
	if (retcode == -1)
	{
		getServerContext()->getConfigVal(ExchangeRouterTable::TARGET_TD_AM, server);
		if (ExchangeRouterTable::TryFind(server, exDO))
		{
			CreateBackendAPI(this, investorId, exDO.Address);
			if (exDO.AuthCode.empty())
			{
				retcode = TradeApi()->get()->ReqAuthenticate(&reqAuth, 0);
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
			retcode = TradeApi()->get()->ReqUserLogin(&req, 0);
		}
	}

	if (retcode == 0)
	{
		while (_loginFlag.test_and_set(std::memory_order_acquire));
		if (_lastErrorCode)
		{
			_loginFlag.clear(std::memory_order_release);
			throw ApiException(_lastErrorCode, boost::locale::conv::to_utf<char>(_lastError, CHARSET_GB2312));
		}
	}
	
	_loginFlag.clear(std::memory_order_release);

	return retcode;
}

bool CTPTradeWorkerSAProcessor::Logged(void)
{
	return (bool)getSystemUser().getLoginTime();
}

IUserInfo & CTPTradeWorkerSAProcessor::getSystemUser()
{
	return getMessageSession()->getUserInfo();
}

void CTPTradeWorkerSAProcessor::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{}

void CTPTradeWorkerSAProcessor::OnRspUserLogin(CThostFtdcRspUserLoginField * pRspUserLogin, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	if (CTPUtility::HasError(pRspInfo))
	{
		_lastErrorCode = pRspInfo->ErrorID;
		_lastError = pRspInfo->ErrorMsg;
	}
	else
	{
		auto& sysUser = getSystemUser();
		sysUser.setFrontId(pRspUserLogin->FrontID);
		sysUser.setSessionId(pRspUserLogin->SessionID);
		sysUser.setTradingDay(std::atoi(pRspUserLogin->TradingDay));
		sysUser.setBrokerId(pRspUserLogin->BrokerID);
		sysUser.setInvestorId(pRspUserLogin->UserID);
		sysUser.setPermission(ALLOW_TRADING);
		sysUser.setLoginTime(time(nullptr));

		CThostFtdcSettlementInfoConfirmField reqsettle{};
		std::strncpy(reqsettle.BrokerID, sysUser.getBrokerId().data(), sizeof(reqsettle.BrokerID));
		std::strncpy(reqsettle.InvestorID, sysUser.getInvestorId().data(), sizeof(reqsettle.InvestorID));
		TradeApi()->get()->ReqSettlementInfoConfirm(&reqsettle, 0);

		QueryUserPositionAsyncIfNeed();
	}

	_loginFlag.clear(std::memory_order_release);
}

void CTPTradeWorkerSAProcessor::OnErrRtnOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo)
{
	_pTradeWorkProc->OnErrRtnOrderInsert(pInputOrder, pRspInfo);
}

void CTPTradeWorkerSAProcessor::OnErrRtnOrderAction(CThostFtdcOrderActionField * pOrderAction, CThostFtdcRspInfoField * pRspInfo)
{
	_pTradeWorkProc->OnErrRtnOrderAction(pOrderAction, pRspInfo);
}

void CTPTradeWorkerSAProcessor::OnRspOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	_pTradeWorkProc->OnRspOrderInsert(pInputOrder, pRspInfo, nRequestID, bIsLast);
}

void CTPTradeWorkerSAProcessor::OnRspOrderAction(CThostFtdcInputOrderActionField * pInputOrderAction, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	_pTradeWorkProc->OnRspOrderAction(pInputOrderAction, pRspInfo, nRequestID, bIsLast);
}

void CTPTradeWorkerSAProcessor::OnRtnOrder(CThostFtdcOrderField * pOrder)
{
	_pTradeWorkProc->OnRtnOrder(pOrder);
}

void CTPTradeWorkerSAProcessor::OnRtnTrade(CThostFtdcTradeField * pTrade)
{
	_pTradeWorkProc->OnRtnTrade(pTrade);
}
