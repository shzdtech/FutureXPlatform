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

class PricingDO : public ContractKey, public dataobjectbase
{
public:
	PricingDO(const std::string& exchangeID, const std::string& instrumentID)
		: ContractKey(exchangeID, instrumentID){}

	double BidPrice = 0;
	double AskPrice = 0;

protected:

private:

};

#endif