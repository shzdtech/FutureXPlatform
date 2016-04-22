/***********************************************************************
 * Module:  UserContractParam.h
 * Author:  milk
 * Modified: 2015年8月19日 23:56:54
 * Purpose: Declaration of the class UserContractParam
 ***********************************************************************/

#if !defined(__dataobject_UserContractParam_h)
#define __dataobject_UserContractParam_h

#include "ContractKey.h"
#include "AbstractDataObj.h"

class UserContractParam : public ContractKey, public AbstractDataObj
{
public:
	UserContractParam(const std::string& exchangeID, const std::string& instrumentID,
		int quantity) : ContractKey(exchangeID, instrumentID){
		Quantity = quantity;
	}

	UserContractParam(const std::string& exchangeID, const std::string& instrumentID)
		: UserContractParam(exchangeID, instrumentID, 1){}

	int Quantity;

protected:

private:

};

#endif