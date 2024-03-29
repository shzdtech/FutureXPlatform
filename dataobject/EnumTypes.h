#if !defined(__dataobject_EnumTypes_h)
#define __dataobject_EnumTypes_h

enum DirectionType
{
	SELL = 0,
	BUY = 1
};

enum PositionDirectionType
{
	PD_NET = 0,
	///多头
	PD_LONG = 1,
	///空头
	PD_SHORT = 2
};

enum PositionType
{
	///净持仓
	PT_NET = 0,
	///综合持仓
	PT_GROSS = 1
};

enum PositionDateType
{
	///使用历史持仓
	PDT_USEHISTORY = 0,
	///不使用历史持仓
	PDT_NOUSEHISTORY = 1
};

enum PositionDateFlagType
{
	///今日持仓
	PSD_TODAY = 0,
	///历史持仓
	PSD_HISTORY = 1,
	///
	PSD_TODAY_HISTORY = 2
};

enum OrderTradingType
{
	TRADINGTYPE_MANUAL = 0,
	TRADINGTYPE_QUOTE = 1,
	TRADINGTYPE_AUTO = 2,
	TRADINGTYPE_HEDGE = 3,
	TRADINGTYPE_INPUT = 4,
	TRADINGTYPE_CLOUD = 9
};

enum OrderConditionFiled
{
	CON_FLD_UNSPECIFIED = 0,
	CON_FLD_LASTPRICE = 1,
};

enum OrderCallPutType
{
	CALL = 0,
	PUT = 1
};

enum OrderOpenCloseType
{
	///开仓
	OPEN = 0,
	///平仓
	CLOSE = 1,
	///强平
	FORCECLOSE = 2,
	///平今
	CLOSETODAY = 3,
	///平昨
	CLOSEYESTERDAY = 4,
	///强减
	FORCEOFF = 5,
	///本地强平
	LOCALFORCECLOSE = 6,
};

enum HedgeType
{
	///投机
	HEDGETYPE_SPECULATION = 0,
	///套利
	HEDGETYPE_ARBITRAGE = 1,
	///套保
	HEDGETYPE_HEDGE = 2,
};

enum ProductType
{
	PRODUCT_UNSPECIFIED = -1,
	///期货
	PRODUCT_FUTURE = 0,
	///期货期权
	PRODUCT_OPTIONS = 1,
	///组合
	PRODUCT_COMBINATION = 2,
	///即期
	PRODUCT_SPOT = 3,
	///期转现
	PRODUCT_EFP = 4,
	///现货期权
	PRODUCT_SPOTOPTION = 5,
	///个股期权
	PRODUCT_ETFOPTION = 6,
	///证券
	PRODUCT_STOCK = 7,
	///OTC合约
	PRODUCT_OTC = 8,
	///OTC期权
	PRODUCT_OTC_OPTION = 9,
	///OTC个股期权
	PRODUCT_OTC_ETFOPTION = 10,
	///OTC证券
	PRODUCT_OTC_STOCK = 11,
	///
	PRODUCT_UPPERBOUND,
};

enum LifePhaseType
{
	///未上市
	PHASE_NOTSTART = 0,
	///上市
	PHASE_STARTED = 1,
	///停牌
	PHASE_PAUSE = 2,
	///到期
	PHASE_EXPIRED = 3,
};

enum ContractType
{
	CONTRACTTYPE_UNSPECIFIED = 0,
	CONTRACTTYPE_FUTURE = 1,
	CONTRACTTYPE_CALL_OPTION = 2,
	CONTRACTTYPE_PUT_OPTION = 3,
	CONTRACTTYPE_SPREAD = 4,
	CONTRACTTYPE_BUTTERFLY = 5,
	CONTRACTTYPE_INDEX_2_LEGS = 6,
	CONTRACTTYPE_INDEX_4_LEGS = 7,
	CONTRACTTYPE_CASHSPOT = 8,
};

enum BankAccountType
{
	BANKACCOUNTTYPE_UNSPECIFIED = 0,
	///银行存折
	BANKACCOUNTTYPE_BANKBOOK = 1,
	///储蓄卡
	BANKACCOUNTTYPE_SAVINGCARD = 2,
	///信用卡
	BANKACCOUNTTYPE_CREDITCARD = 3,
};

enum CustomerType
{
	///自然人
	CUSTOMERTYPE_PERSON = 0,
	///机构户
	CUSTOMERTYPE_INSTITUTION = 1
};

#endif