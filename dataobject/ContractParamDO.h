/***********************************************************************
 * Module:  ContractDO.h
 * Author:  milk
 * Modified: 2015年8月21日 22:50:44
 * Purpose: Declaration of the class ContractDO
 ***********************************************************************/

#if !defined(__dataobject_ContractDO_h)
#define __dataobject_ContractDO_h

#include "dataobjectbase.h"
#include "ContractKey.h"

class ContractParamDO : public ContractKey, public dataobjectbase
{
public:
	ContractParamDO(const std::string& exchangeID, const std::string& instrumentID)
		: ContractKey(exchangeID, instrumentID) {}

	double Gamma = 0;
	int DepthVol = 1;
	double Multiplier = 1;
	double TickSize = 1;

protected:

private:

};


#endif