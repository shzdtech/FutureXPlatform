/* 
 * File:   DefMessageID.h
 * Author: milk
 *
 * Created on 2014年10月11日, 下午1:02
 */

#ifndef _DEFMESSAGEID_H
#define	_DEFMESSAGEID_H

enum SystemMessageID
{
	MSG_ID_UNDEFINED = 0,
    MSG_ID_ERROR = 1,
    MSG_ID_ECHO = 2,

	MSG_ID_SESSION_CREATED = 1001,
    MSG_ID_LOGIN = 1002,
    MSG_ID_LOGOUT = 1003,

	MSG_ID_QUERY_MODELPARAMS = 2000,
	MSG_ID_UPDATE_MODELPARAMS = 2001,
	MSG_ID_UPDATE_TEMPMODELPARAMS = 2002,
	MSG_ID_QUERY_SYSPARAMS = 2003,
};

enum AccountManageMessageID
{
	MSG_ID_USER_NEW = 5000,
	MSG_ID_USER_INFO = 5001,
	MSG_ID_USER_INFO_UPDATE = 5002,
	MSG_ID_RESET_PASSWORD = 5003,
};

enum BussinessMessageID
{
	MSG_ID_SUB_MARKETDATA = 10001,
	MSG_ID_UNSUB_MARKETDATA = 10002,
	MSG_ID_RET_MARKETDATA = 10003,
	MSG_ID_QUERY_EXCHANGE = 10004,
	MSG_ID_QUERY_INSTRUMENT = 10005,
	MSG_ID_QUERY_ORDER = 10006,
	MSG_ID_QUERY_TRADE = 10007,
	MSG_ID_QUERY_POSITION = 10008,
	MSG_ID_QUERY_ACCOUNT_INFO = 10009,
	MSG_ID_POSITION_UPDATED = 10010,
	MSG_ID_ORDER_NEW = 10016,
	MSG_ID_ORDER_CANCEL = 10017,
	MSG_ID_ORDER_UPDATE = 10020,
	MSG_ID_TRADE_RTN = 10021,
	MSG_ID_SETTLEMENT_INFO_CONFIRM = 10022,

	MSG_ID_QUERY_TRANSFER_BANK = 10023,
	MSG_ID_QUERY_USER_BANKS = 10024,
	MSG_ID_QUERY_TRANSFER_SERIAL = 10025,
	MSG_ID_REQ_BANK_TO_FUTURE = 10026,
	MSG_ID_REQ_FUTURE_TO_BANK = 10027,
	MSG_ID_RET_BANK_TO_FUTURE = 10028,
	MSG_ID_RET_FUTURE_TO_BANK = 10029,
	MSG_ID_QUERY_USER_BANKACCOUNT = 10030,

	MSG_ID_SUB_PRICING = 15000,
	MSG_ID_RTN_PRICING = 15001,
	MSG_ID_UNSUB_PRICING = 15002,
	MSG_ID_RTN_TRADINGDESK_PRICING = 15003,
	MSG_ID_SUB_TRADINGDESK_PRICING = 15004,
	MSG_ID_UNSUB_TRADINGDESK_PRICING = 15005,
	MSG_ID_UPDATE_VALUATION_PRICING = 15006,
	MSG_ID_QUERY_VALUATION_PRICING = 15007,

	MSG_ID_QUERY_STRATEGY = 20000,
	MSG_ID_QUERY_CONTRACT_PARAM = 20001,
	MSG_ID_MODIFY_USER_PARAM = 20002,
	MSG_ID_MODIFY_STRATEGY = 20003,
	MSG_ID_MODIFY_CONTRACT_PARAM = 20004,
	MSG_ID_QUERY_TRADINGDESK = 20005,
	MSG_ID_QUERY_PORTFOLIO = 20006,
	MSG_ID_PORTFOLIO_NEW = 20007,
	MSG_ID_MODIFY_PRICING_CONTRACT = 20008,
	MSG_ID_MODIFY_PORTFOLIO = 20009,
	MSG_ID_QUERY_RISK = 20010,
	MSG_ID_RISK_UPDATED = 20011,
	MSG_ID_HEDGE_CONTRACT_UPDATE = 20012,
	MSG_ID_QUERY_POSITION_DIFFER = 20013,
	MSG_ID_QUERY_VALUATION_RISK = 20014,
	MSG_ID_SYNC_POSITION = 20015,
	MSG_ID_ADD_MANUAL_TRADE = 20016,
	MSG_ID_DELETE_MANUAL_TRADE = 20017,
};



#endif	/* DEFMESSAGEID_H */

