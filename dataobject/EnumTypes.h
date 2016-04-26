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

enum TradingType
{
	TRADINGTYPE_MANUAL = 0,
	TRADINGTYPE_QUOTE = 1,
	TRADINGTYPE_AUTO = 2,
	TRADINGTYPE_HEDGE = 3,
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

#endif