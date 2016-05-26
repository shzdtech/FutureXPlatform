/***********************************************************************
 * Module:  CTPTradeProcessor.cpp
 * Author:  milk
 * Modified: 2015年3月8日 11:35:24
 * Purpose: Implementation of the class CTPTradeProcessor
 ***********************************************************************/

#include "CTPTradeProcessor.h"
#include "CTPUtility.h"
#include <glog/logging.h>
#include "../message/DefMessageID.h"
#include "../message/message_macro.h"
#include "../bizutility/InstrumentCache.h"


bool CTPTradeProcessor::_requestInsInfo = false;
////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeProcessor::CTPTradeProcessor(const std::map<std::string, std::string>& configMap)
// Purpose:    Implementation of CTPTradeProcessor::CTPTradeProcessor()
// Parameters:
// - frontserver
// Return:     
////////////////////////////////////////////////////////////////////////

CTPTradeProcessor::CTPTradeProcessor(const std::map<std::string, std::string>& configMap)
	: CTPProcessor(configMap)
{
	OnInit();
}


////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeProcessor::~CTPTradeProcessor()
// Purpose:    Implementation of CTPTradeProcessor::~CTPTradeProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPTradeProcessor::~CTPTradeProcessor()
{
	DLOG(INFO) << __FUNCTION__ << std::endl;
	if (_rawAPI.TrdAPI)
		_rawAPI.TrdAPI->Release();
}


////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeProcessor::OnInit()
// Purpose:    Implementation of CTPTradeProcessor::OnInit()
// Return:     void
////////////////////////////////////////////////////////////////////////

void CTPTradeProcessor::OnInit(void) {
	_rawAPI.TrdAPI = CThostFtdcTraderApi::CreateFtdcTraderApi();
	if (_rawAPI.TrdAPI) {
		_rawAPI.TrdAPI->RegisterSpi(this);

		std::string frontserver = _configMap[STR_FRONT_SERVER];
		DLOG(INFO) << __FUNCTION__ << ":" << frontserver;

		_rawAPI.TrdAPI->RegisterFront(const_cast<char*> (frontserver.data()));
		_rawAPI.TrdAPI->SubscribePrivateTopic(THOST_TERT_RESUME);
		_rawAPI.TrdAPI->SubscribePublicTopic(THOST_TERT_RESUME);
		_rawAPI.TrdAPI->Init();
	}
}



///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
void CTPTradeProcessor::OnFrontConnected()
{}

///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
///@param nReason 错误原因
///        0x1001 网络读失败
///        0x1002 网络写失败
///        0x2001 接收心跳超时
///        0x2002 发送心跳失败
///        0x2003 收到错误报文
void CTPTradeProcessor::OnFrontDisconnected(int nReason) {};

///心跳超时警告。当长时间未收到报文时，该方法被调用。
///@param nTimeLapse 距离上次接收报文的时间
void CTPTradeProcessor::OnHeartBeatWarning(int nTimeLapse) {};

///客户端认证响应
void CTPTradeProcessor::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};


///登录请求响应
void CTPTradeProcessor::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!CTPUtility::HasError(pRspInfo))
	{
		_isLogged = true;

		if (!_requestInsInfo)
		{
			CThostFtdcQryInstrumentField req;
			std::memset(&req, 0x0, sizeof(CThostFtdcQryInstrumentField));

			_requestInsInfo = _rawAPI.TrdAPI->ReqQryInstrument(&req, 0) == 0;
		}
	}

	OnResponseMacro(MSG_ID_LOGIN, nRequestID, pRspUserLogin, pRspInfo, &nRequestID, &bIsLast)
}

///登出请求响应
void CTPTradeProcessor::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	_isLogged = false;
	OnResponseMacro(MSG_ID_LOGOUT, nRequestID, pUserLogout, pRspInfo, &nRequestID, &bIsLast)
}

///用户口令更新请求响应
void CTPTradeProcessor::OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///资金账户口令更新请求响应
void CTPTradeProcessor::OnRspTradingAccountPasswordUpdate(CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///报单录入请求响应
void CTPTradeProcessor::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OnResponseMacro(MSG_ID_ORDER_NEW, nRequestID, pInputOrder, pRspInfo, &nRequestID, &bIsLast)
}

///预埋单录入请求响应
void CTPTradeProcessor::OnRspParkedOrderInsert(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///预埋撤单录入请求响应
void CTPTradeProcessor::OnRspParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///报单操作请求响应
void CTPTradeProcessor::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrderAction->ActionFlag == THOST_FTDC_AF_Delete)
		OnResponseMacro(MSG_ID_ORDER_CANCEL, nRequestID, pInputOrderAction, pRspInfo, &nRequestID, &bIsLast)
}

///查询最大报单数量响应
void CTPTradeProcessor::OnRspQueryMaxOrderVolume(CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///投资者结算结果确认响应
void CTPTradeProcessor::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OnResponseMacro(MSG_ID_SETTLEMENT_INFO_CONFIRM, nRequestID, pSettlementInfoConfirm, pRspInfo, &nRequestID, &bIsLast)
}

///删除预埋单响应
void CTPTradeProcessor::OnRspRemoveParkedOrder(CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///删除预埋撤单响应
void CTPTradeProcessor::OnRspRemoveParkedOrderAction(CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///请求查询报单响应
void CTPTradeProcessor::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OnResponseMacro(MSG_ID_QUERY_ORDER, nRequestID, pOrder, pRspInfo, &nRequestID, &bIsLast)
}

///请求查询成交响应
void CTPTradeProcessor::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OnResponseMacro(MSG_ID_QUERY_TRADE, nRequestID, pTrade, pRspInfo, &nRequestID, &bIsLast)
}

///请求查询投资者持仓响应
void CTPTradeProcessor::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OnResponseMacro(MSG_ID_QUERY_POSITION, nRequestID, pInvestorPosition, pRspInfo, &nRequestID, &bIsLast)
}

///请求查询资金账户响应
void CTPTradeProcessor::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OnResponseMacro(MSG_ID_QUERY_ACCOUNT_INFO, nRequestID, pTradingAccount, pRspInfo, &nRequestID, &bIsLast)
}

///请求查询投资者响应
void CTPTradeProcessor::OnRspQryInvestor(CThostFtdcInvestorField *pInvestor, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///请求查询交易编码响应
void CTPTradeProcessor::OnRspQryTradingCode(CThostFtdcTradingCodeField *pTradingCode, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///请求查询合约保证金率响应
void CTPTradeProcessor::OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///请求查询合约手续费率响应
void CTPTradeProcessor::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///请求查询交易所响应
void CTPTradeProcessor::OnRspQryExchange(CThostFtdcExchangeField *pExchange, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OnResponseMacro(MSG_ID_QUERY_EXCHANGE, nRequestID, pExchange, pRspInfo, &nRequestID, &bIsLast)
}

///请求查询合约响应
void CTPTradeProcessor::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OnResponseMacro(MSG_ID_QUERY_INSTRUMENT, nRequestID, pInstrument, pRspInfo, &nRequestID, &bIsLast)
}

///请求查询行情响应
void CTPTradeProcessor::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///请求查询投资者结算结果响应
void CTPTradeProcessor::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///请求查询转帐银行响应
void CTPTradeProcessor::OnRspQryTransferBank(CThostFtdcTransferBankField *pTransferBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///请求查询投资者持仓明细响应
void CTPTradeProcessor::OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///请求查询客户通知响应
void CTPTradeProcessor::OnRspQryNotice(CThostFtdcNoticeField *pNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///请求查询结算信息确认响应
void CTPTradeProcessor::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

///请求查询投资者持仓明细响应
void CTPTradeProcessor::OnRspQryInvestorPositionCombineDetail(CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///查询保证金监管系统经纪公司资金账户密钥响应
void CTPTradeProcessor::OnRspQryCFMMCTradingAccountKey(CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///请求查询仓单折抵信息响应
void CTPTradeProcessor::OnRspQryEWarrantOffset(CThostFtdcEWarrantOffsetField *pEWarrantOffset, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///请求查询转帐流水响应
void CTPTradeProcessor::OnRspQryTransferSerial(CThostFtdcTransferSerialField *pTransferSerial, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///请求查询银期签约关系响应
void CTPTradeProcessor::OnRspQryAccountregister(CThostFtdcAccountregisterField *pAccountregister, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///错误应答
void CTPTradeProcessor::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///报单通知
void CTPTradeProcessor::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	if (pOrder)
	{
		int msgId;

		switch (CTPUtility::CheckOrderStatus(pOrder->OrderStatus, pOrder->OrderSubmitStatus))
		{
		case OrderStatus::CANCELED:
		case OrderStatus::CANCEL_REJECTED:
			msgId = MSG_ID_ORDER_CANCEL;
			break;
		case OrderStatus::OPENNING:
		case OrderStatus::OPEN_REJECTED:
			msgId = MSG_ID_ORDER_NEW;
			break;
		case OrderStatus::PARTIAL_TRADING:
		case OrderStatus::ALL_TRADED:
			msgId = MSG_ID_ORDER_UPDATE;
			break;
		default:
			return;
		}

		OnResponseMacro(msgId, 0, pOrder);
	}
}

///成交通知
void CTPTradeProcessor::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	bool bIsLast = true;
	int nRequestID = 0;
	OnResponseMacro(MSG_ID_TRADE_RTN, 0, pTrade, nullptr, &nRequestID, &bIsLast)
}

///报单录入错误回报
void CTPTradeProcessor::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
	OnResponseMacro(MSG_ID_ORDER_NEW, 0, pInputOrder, pRspInfo);
}

///报单操作错误回报
void CTPTradeProcessor::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{}

///合约交易状态通知
void CTPTradeProcessor::OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus) {};

///交易通知
void CTPTradeProcessor::OnRtnTradingNotice(CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo) {};

///提示条件单校验错误
void CTPTradeProcessor::OnRtnErrorConditionalOrder(CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder) {};

///请求查询签约银行响应
void CTPTradeProcessor::OnRspQryContractBank(CThostFtdcContractBankField *pContractBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///请求查询预埋单响应
void CTPTradeProcessor::OnRspQryParkedOrder(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///请求查询预埋撤单响应
void CTPTradeProcessor::OnRspQryParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///请求查询交易通知响应
void CTPTradeProcessor::OnRspQryTradingNotice(CThostFtdcTradingNoticeField *pTradingNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///请求查询经纪公司交易参数响应
void CTPTradeProcessor::OnRspQryBrokerTradingParams(CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///请求查询经纪公司交易算法响应
void CTPTradeProcessor::OnRspQryBrokerTradingAlgos(CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///银行发起银行资金转期货通知
void CTPTradeProcessor::OnRtnFromBankToFutureByBank(CThostFtdcRspTransferField *pRspTransfer) {};

///银行发起期货资金转银行通知
void CTPTradeProcessor::OnRtnFromFutureToBankByBank(CThostFtdcRspTransferField *pRspTransfer) {};

///银行发起冲正银行转期货通知
void CTPTradeProcessor::OnRtnRepealFromBankToFutureByBank(CThostFtdcRspRepealField *pRspRepeal) {};

///银行发起冲正期货转银行通知
void CTPTradeProcessor::OnRtnRepealFromFutureToBankByBank(CThostFtdcRspRepealField *pRspRepeal) {};

///期货发起银行资金转期货通知
void CTPTradeProcessor::OnRtnFromBankToFutureByFuture(CThostFtdcRspTransferField *pRspTransfer) {};

///期货发起期货资金转银行通知
void CTPTradeProcessor::OnRtnFromFutureToBankByFuture(CThostFtdcRspTransferField *pRspTransfer) {};

///系统运行时期货端手工发起冲正银行转期货请求，银行处理完毕后报盘发回的通知
void CTPTradeProcessor::OnRtnRepealFromBankToFutureByFutureManual(CThostFtdcRspRepealField *pRspRepeal) {};

///系统运行时期货端手工发起冲正期货转银行请求，银行处理完毕后报盘发回的通知
void CTPTradeProcessor::OnRtnRepealFromFutureToBankByFutureManual(CThostFtdcRspRepealField *pRspRepeal) {};

///期货发起查询银行余额通知
void CTPTradeProcessor::OnRtnQueryBankBalanceByFuture(CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount) {};

///期货发起银行资金转期货错误回报
void CTPTradeProcessor::OnErrRtnBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo) {};

///期货发起期货资金转银行错误回报
void CTPTradeProcessor::OnErrRtnFutureToBankByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo) {};

///系统运行时期货端手工发起冲正银行转期货错误回报
void CTPTradeProcessor::OnErrRtnRepealBankToFutureByFutureManual(CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo) {};

///系统运行时期货端手工发起冲正期货转银行错误回报
void CTPTradeProcessor::OnErrRtnRepealFutureToBankByFutureManual(CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo) {};

///期货发起查询银行余额错误回报
void CTPTradeProcessor::OnErrRtnQueryBankBalanceByFuture(CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo) {};

///期货发起冲正银行转期货请求，银行处理完毕后报盘发回的通知
void CTPTradeProcessor::OnRtnRepealFromBankToFutureByFuture(CThostFtdcRspRepealField *pRspRepeal) {};

///期货发起冲正期货转银行请求，银行处理完毕后报盘发回的通知
void CTPTradeProcessor::OnRtnRepealFromFutureToBankByFuture(CThostFtdcRspRepealField *pRspRepeal) {};

///期货发起银行资金转期货应答
void CTPTradeProcessor::OnRspFromBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///期货发起期货资金转银行应答
void CTPTradeProcessor::OnRspFromFutureToBankByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///期货发起查询银行余额应答
void CTPTradeProcessor::OnRspQueryBankAccountMoneyByFuture(CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

///银行发起银期开户通知
void CTPTradeProcessor::OnRtnOpenAccountByBank(CThostFtdcOpenAccountField *pOpenAccount) {};

///银行发起银期销户通知
void CTPTradeProcessor::OnRtnCancelAccountByBank(CThostFtdcCancelAccountField *pCancelAccount) {};

///银行发起变更银行账号通知
void CTPTradeProcessor::OnRtnChangeAccountByBank(CThostFtdcChangeAccountField *pChangeAccount) {};