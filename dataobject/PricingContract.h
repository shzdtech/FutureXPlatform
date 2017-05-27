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

	PricingContract(const std::string& exchangeID, const std::string& instrumentID, const std::string& underlying, double weight, double adjust = 0)
		: ContractKey(exchangeID, instrumentID), Underlying(underlying), Weight(weight), Adjust(adjust){}

	double Adjust = 0;
	double Weight = 1;

	std::string Underlying;

protected:

private:

};

#endif