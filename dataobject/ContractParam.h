/***********************************************************************
 * Module:  ContractParam.h
 * Author:  milk
 * Modified: 2015年8月8日 22:09:02
 * Purpose: Declaration of the class ContractParam
 ***********************************************************************/

#if !defined(__dataobject_ContractParam_h)
#define __dataobject_ContractParam_h

#include "ContractKey.h"

class ContractParam : public ContractKey
{
public:
	ContractParam(const std::string& exchangeID, const std::string& instrumentID)
		: ContractKey(exchangeID, instrumentID){}

	double Beta = 0;
	double Weight = 1;

protected:

private:

};

#endif