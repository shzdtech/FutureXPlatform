/***********************************************************************
 * Module:  UserContractParamDO.h
 * Author:  milk
 * Modified: 2015年8月19日 23:56:54
 * Purpose: Declaration of the class UserContractParamDO
 ***********************************************************************/

#if !defined(__dataobject_UserContractParamDO_h)
#define __dataobject_UserContractParamDO_h

#include "ContractKey.h"
#include "dataobjectbase.h"

class UserContractParamDO : public ContractKey, public dataobjectbase
{
public:
	UserContractParamDO(const std::string& exchangeID, const std::string& instrumentID,
		int quantity) : ContractKey(exchangeID, instrumentID){
		Quantity = quantity;
	}

	UserContractParamDO(const std::string& exchangeID, const std::string& instrumentID)
		: UserContractParamDO(exchangeID, instrumentID, 1){}

	int Quantity;

protected:

private:

};

#endif