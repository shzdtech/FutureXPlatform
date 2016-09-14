/***********************************************************************
 * Module:  PricingDO.h
 * Author:  milk
 * Modified: 2015年8月21日 10:15:18
 * Purpose: Declaration of the class PricingDO
 ***********************************************************************/

#if !defined(__dataobject_PricingDO_h)
#define __dataobject_PricingDO_h

#include "dataobjectbase.h"
#include "ContractKey.h"
#include "Pricing.h"
#include <memory>

class IPricingDO : public ContractKey, public dataobjectbase
{
public:
	IPricingDO(const char* exchangeID, const char* instrumentID)
		: ContractKey(exchangeID, instrumentID) {}

	IPricingDO(const std::string& exchangeID, const std::string& instrumentID)
		: ContractKey(exchangeID, instrumentID) {}

	virtual Pricing& Bid() = 0;
	virtual Pricing& Ask() = 0;
	virtual const Pricing& Bid() const = 0;
	virtual const Pricing& Ask() const = 0;
};

template <class T>
class TPricingDO : public IPricingDO
{
public:
	TPricingDO(const char* exchangeID, const char* instrumentID)
		: IPricingDO(exchangeID, instrumentID) {}

	TPricingDO(const std::string& exchangeID, const std::string& instrumentID)
		: IPricingDO(exchangeID, instrumentID) {}

	virtual Pricing& Bid() { return _bid; };
	virtual Pricing& Ask() { return _ask; };
	virtual const Pricing& Bid() const { return _bid; };
	virtual const Pricing& Ask() const { return _ask; };
	T& TBid() { return _bid; };
	T& TAsk() { return _ask; };
	T& const TBid() const { return _bid; };
	T& const TAsk() const { return _ask; };

private:
	T _bid;
	T _ask;
};

typedef TPricingDO<Pricing> PricingDO;
typedef TPricingDO<OptionPricing> OptionPricingDO;
typedef std::shared_ptr<IPricingDO> IPricingDO_Ptr;
typedef std::shared_ptr<PricingDO> PricingDO_Ptr;
typedef std::shared_ptr<OptionPricingDO> OptionPricingDO_Ptr;

#endif