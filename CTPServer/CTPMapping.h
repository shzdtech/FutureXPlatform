#ifndef __CTPMAPPING_H
#define	__CTPMAPPING_H
#include <map>
#include "../dataobject/OrderDO.h"
#include "tradeapi/ThostFtdcUserApiDataType.h"

static const std::map<OrderExecType, TThostFtdcOrderPriceTypeType> CTPExecPriceMapping =
{
	{ OrderExecType::LIMIT, THOST_FTDC_OPT_LimitPrice },
	{ OrderExecType::MARKET, THOST_FTDC_OPT_AnyPrice }
};

static const std::map<OrderTIFType, TThostFtdcTimeConditionType> CTPTIFMapping =
{
	{ OrderTIFType::IOC, THOST_FTDC_TC_IOC },
	{ OrderTIFType::GFD, THOST_FTDC_TC_GFD }
};



#endif	/* __CTPMAPPING_H */