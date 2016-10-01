/***********************************************************************
 * Module:  PricingContract.h
 * Author:  milk
 * Modified: 2015年8月8日 22:09:02
 * Purpose: Declaration of the class PricingContract
 ***********************************************************************/

#if !defined(__dataobject_PricingContract_h)
#define __dataobject_PricingContract_h

#include "ContractKey.h"

class PricingContract : public ContractKey
{
public:
	PricingContract() {}

	PricingContract(const std::string& exchangeID, const std::string& instrumentID)
		: ContractKey(exchangeID, instrumentID){}

	PricingContract(const std::string& exchangeID, const std::string& instrumentID, double weight)
		: ContractKey(exchangeID, instrumentID), Weight(weight) {}

	double Weight = 1;

protected:

private:

};

#endif