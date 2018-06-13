#ifndef __XTMAPPING_H
#define	__XTMAPPING_H
#include <map>
#include "../dataobject/OrderDO.h"
#include "CTPAPISwitch.h"

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

static const std::map<OrderVolType, TThostFtdcVolumeConditionType> CTPVolCondMapping =
{
	{ OrderVolType::ANYVOLUME, THOST_FTDC_VC_AV },
	{ OrderVolType::MINIVOLUME, THOST_FTDC_VC_MV },
	{ OrderVolType::ALLVOLUME, THOST_FTDC_VC_CV }
};

#endif	/* __XTMAPPING_H */