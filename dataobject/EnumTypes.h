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
	///��ͷ
	PD_LONG = 1,
	///��ͷ
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
	///����
	OPEN = 0,
	///ƽ��
	CLOSE = 1,
	///ǿƽ
	FORCECLOSE = 2,
	///ƽ��
	CLOSETODAY = 3,
	///ƽ��
	CLOSEYESTERDAY = 4,
	///ǿ��
	FORCEOFF = 5,
	///����ǿƽ
	LOCALFORCECLOSE = 6,
};

enum HedgeType
{
	///Ͷ��
	HEDGETYPE_SPECULATION = 0,
	///����
	HEDGETYPE_ARBITRAGE = 1,
	///�ױ�
	HEDGETYPE_HEDGE = 2,
};

#endif