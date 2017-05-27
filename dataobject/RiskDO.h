/***********************************************************************
* Module:  RiskDO.h
* Author:  milk
* Modified: 2017Äê4ÔÂ25ÈÕ 0:54:28
* Purpose: Declaration of the class RiskDO
***********************************************************************/

#if !defined(__dataobject_RiskDO_h)
#define __dataobject_RiskDO_h

#include "dataobjectbase.h"
#include "ContractKey.h"

class RiskDO : public UserContractKey, public dataobjectbase
{
public:
	RiskDO() = default;

	RiskDO(const std::string& exchangeID, const std::string& instrumentID, const std::string& userID) 
		: UserKey(userID), UserContractKey(exchangeID, instrumentID, userID) {}

	std::string Underlying;

	double Delta = 0;
	double Gamma = 0;
	double Theta = 0;
	double Rho = 0;
	double Vega = 0;

protected:
private:

};

typedef std::shared_ptr<RiskDO> RiskDO_Ptr;

#endif