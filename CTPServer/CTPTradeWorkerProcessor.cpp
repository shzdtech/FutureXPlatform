/***********************************************************************
 * Module:  CTPTradeWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2016年6月16日 11:57:29
 * Purpose: Implementation of the class CTPTradeWorkerProcessor
 ***********************************************************************/

#include "CTPTradeWorkerProcessor.h"
#include "CTPUtility.h"
#include "CTPConstant.h"

#include "../common/Attribute_Key.h"
#include "../message/AppContext.h"
#include "../message/DefMessageID.h"
#include "../message/message_macro.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPTradeWorkerProcessor::CTPTradeWorkerProcessor()
 // Purpose:    Implementation of CTPTradeWorkerProcessor::CTPTradeWorkerProcessor()
 // Return:     
 ////////////////////////////////////////////////////////////////////////

CTPTradeWorkerProcessor::CTPTradeWorkerProcessor(const std::map<std::string, std::string>& configMap)
	: CTPTradeProcessor(configMap)
{
	InstrmentsLoaded = false;
	_defaultUser.setBrokerId(SysParam::Get(CTP_TRADER_BROKERID));
	_defaultUser.setUserId(SysParam::Get(CTP_TRADER_USERID));
	_defaultUser.setPassword(SysParam::Get(CTP_TRADER_PASSWORD));
	_defaultUser.setServer(SysParam::Get(CTP_TRADER_SERVER));
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeWorkerProcessor::~CTPTradeWorkerProcessor()
// Purpose:    Implementation of CTPTradeWorkerProcessor::~CTPTradeWorkerProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPTradeWorkerProcessor::~CTPTradeWorkerProcessor()
{
	// TODO : implement
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeWorkerProcessor::OnInit()
// Purpose:    Implementation of CTPTradeWorkerProcessor::Initialize()
// Return:     void
////////////////////////////////////////////////////////////////////////

void CTPTradeWorkerProcessor::Initialize(void)
{
	_rawAPI.TrdAPI = CThostFtdcTraderApi::CreateFtdcTraderApi();
	if (_rawAPI.TrdAPI) {
		_rawAPI.TrdAPI->RegisterSpi(this);
		_rawAPI.TrdAPI->RegisterFront(const_cast<char*> (_defaultUser.getServer().data()));
		_rawAPI.TrdAPI->SubscribePrivateTopic(THOST_TERT_RESUME);
		_rawAPI.TrdAPI->SubscribePublicTopic(THOST_TERT_RESUME);
		_rawAPI.TrdAPI->Init();
	}

	_initializer = std::async(std::launch::async, [this]() {
		std::this_thread::sleep_for(std::chrono::seconds(std::rand() % 10)); //wait some secs for connecting to CTP server
		LoginDefault();
		while (!this->InstrmentsLoaded)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(this->RetryInterval));
			LoginDefault();
		}
	});
}

int CTPTradeWorkerProcessor::LoginDefault(void)
{
	CThostFtdcReqUserLoginField req;
	std::memset(&req, 0, sizeof(req));
	std::strcpy(req.BrokerID, _defaultUser.getBrokerId().data());
	std::strcpy(req.UserID, _defaultUser.getUserId().data());
	std::strcpy(req.Password, _defaultUser.getPassword().data());
	return _rawAPI.TrdAPI->ReqUserLogin(&req, AppContext::GenNextSeq());
}

int CTPTradeWorkerProcessor::LoginIfNeed(void)
{
	int ret = 0;

	if (!_isLogged)
		ret = LoginDefault();

	return ret;
}

///登录请求响应
void CTPTradeWorkerProcessor::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!CTPUtility::HasError(pRspInfo))
	{
		_isLogged = true;

		if (!InstrmentsLoaded)
		{
			CThostFtdcQryInstrumentField req;
			std::memset(&req, 0x0, sizeof(CThostFtdcQryInstrumentField));
			_rawAPI.TrdAPI->ReqQryInstrument(&req, 0);
		}
	}
}

void CTPTradeWorkerProcessor::OnRspQryInstrument(CThostFtdcInstrumentField * pInstrument, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	try
	{
		InstrmentsLoaded = bIsLast;
		ProcessResponseMacro(this, MSG_ID_QUERY_INSTRUMENT, nRequestID, pInstrument, pRspInfo, &nRequestID, &bIsLast);
	}
	catch (...) {}
}


int CTPTradeWorkerProcessor::CreateOrder(const OrderDO& orderInfo, OrderStatus& currStatus)
{
	currStatus = OrderStatus::UNDEFINED;
	// 端登成功,发出报单录入请求
	CThostFtdcInputOrderField req;
	std::memset(&req, 0, sizeof(req));

	//经纪公司代码
	std::strcpy(req.BrokerID, _defaultUser.getBrokerId().data());
	//投资者代码
	std::strcpy(req.InvestorID, _defaultUser.getUserId().data());
	// 合约代码
	std::strcpy(req.InstrumentID, orderInfo.InstrumentID().data());
	///报单引用
	std::sprintf(req.OrderRef, FMT_PADDING_ORDERREF, orderInfo.OrderID);
	// 用户代码
	std::strcpy(req.UserID, orderInfo.UserID().data());
	// 报单价格条件
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	// 买卖方向
	req.Direction = orderInfo.Direction > 0 ? THOST_FTDC_D_Buy : THOST_FTDC_D_Sell;
	///组合开平标志: 开仓
	req.CombOffsetFlag[0] = THOST_FTDC_OF_Open + orderInfo.OpenClose;
	///组合投机套保标志
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	// 价格
	req.LimitPrice = orderInfo.LimitPrice;
	// 数量
	req.VolumeTotalOriginal = orderInfo.Volume;
	// 有效期类型
	req.TimeCondition = orderInfo.TIF == OrderTIFType::IOC ? THOST_FTDC_TC_IOC : THOST_FTDC_TC_GFD;
	// 成交量类型
	req.VolumeCondition = THOST_FTDC_VC_AV;
	// 最小成交量
	req.MinVolume = 1;
	// 触发条件
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	// 止损价
	req.StopPrice = 0;
	// 强平原因
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	// 自动挂起标志
	req.IsAutoSuspend = false;

	currStatus = OrderStatus::OPENNING;

	return _rawAPI.TrdAPI->ReqOrderInsert(&req, AppContext::GenNextSeq());
}


int CTPTradeWorkerProcessor::CancelOrder(const OrderDO& orderInfo, OrderStatus& currStatus)
{
	currStatus = OrderStatus::UNDEFINED;

	CThostFtdcInputOrderActionField req;
	std::memset(&req, 0x0, sizeof(req));

	req.ActionFlag = THOST_FTDC_AF_Delete;
	//经纪公司代码
	std::strcpy(req.BrokerID, _defaultUser.getBrokerId().data());
	std::strcpy(req.InvestorID, _defaultUser.getUserId().data());
	std::strcpy(req.UserID, orderInfo.UserID().data());
	if (orderInfo.OrderSysID != 0)
	{
		std::strcpy(req.ExchangeID, orderInfo.ExchangeID().data());
		std::sprintf(req.OrderSysID, FMT_PADDING_ORDERSYSID, orderInfo.OrderSysID);
	}
	else
	{
		auto pUser = getSession()->getUserInfo();
		req.SessionID = pUser->getSessionId();
		req.FrontID = pUser->getFrontId();
		std::strcpy(req.InstrumentID, orderInfo.InstrumentID().data());
		std::sprintf(req.OrderRef, FMT_PADDING_ORDERREF, orderInfo.OrderID);
	}

	currStatus = OrderStatus::CANCELING;

	return _rawAPI.TrdAPI->ReqOrderAction(&req, AppContext::GenNextSeq());
}


