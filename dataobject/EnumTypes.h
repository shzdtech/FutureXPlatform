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

enum PositionType
{
	///���ֲ�
	PT_NET = 0,
	///�ۺϳֲ�
	PT_GROSS = 1
};

enum PositionDateType
{
	///ʹ����ʷ�ֲ�
	PDT_USEHISTORY = 0,
	///��ʹ����ʷ�ֲ�
	PDT_NOUSEHISTORY = 1
};

enum PositionDateFlagType
{
	///���ճֲ�
	PSD_TODAY = 0,
	///��ʷ�ֲ�
	PSD_HISTORY = 1
};

enum TradingType
{
	TRADINGTYPE_MANUAL = 0,
	TRADINGTYPE_QUOTE = 1,
	TRADINGTYPE_AUTO = 2,
	TRADINGTYPE_HEDGE = 3,
};

enum OrderCallPutType
{
	CALL = 0,
	PUT = 1
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

enum ProductType
{
	PRODUCT_UNSPECIFIED = -1,
	///�ڻ�
	PRODUCT_FUTURE = 0,
	///�ڻ���Ȩ
	PRODUCT_OPTIONS = 1,
	///���
	PRODUCT_COMBINATION = 2,
	///����
	PRODUCT_SPOT = 3,
	///��ת��
	PRODUCT_EFP = 4,
	///�ֻ���Ȩ
	PRODUCT_SPOTOPTION = 5,
	///OTC��Լ
	PRODUCT_OTC = 6,
	///OTC��Ȩ
	PRODUCT_OTC_OPTION = 7,
	///
	PRODUCT_UPPERBOUND,
};

enum LifePhaseType
{
	///δ����
	PHASE_NOTSTART = 0,
	///����
	PHASE_STARTED = 1,
	///ͣ��
	PHASE_PAUSE = 2,
	///����
	PHASE_EXPIRED = 3,
};

enum ContractType
{
	CONTRACTTYPE_UNSPECIFIED = 0,
	CONTRACTTYPE_FUTURE =1,
	CONTRACTTYPE_CALL_OPTION = 2,
	CONTRACTTYPE_PUT_OPTION = 3,
	CONTRACTTYPE_SPREAD = 4,
	CONTRACTTYPE_BUTTERFLY = 5,
	CONTRACTTYPE_INDEX_2_LEGS = 6,
	CONTRACTTYPE_INDEX_4_LEGS = 7,
	CONTRACTTYPE_CASHSPOT = 8,
};

#endif