#if !defined(__OTCOPTION_OTCOptionPricingParams_h)
#define __OTCOPTION_OTCOptionPricingParams_h

#include "../dataobject/StrategyContractDO.h"
#include "../pricingengine/IPricingDataContext.h"
#include "otcoption_export.h"

class OPTION_SERVER_CLASS_EXPORT OTCOptionPricingParams
{
public:
	static dataobj_ptr GenerateTradingDeskData(const StrategyContractDO& sto, const IPricingDataContext_Ptr& pricingCtx, bool triggerPricing = true);
};

#endif