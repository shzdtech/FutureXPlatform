/***********************************************************************
* Module:  ValuationRiskDO.h
* Author:  milk
* Modified: 2017Äê4ÔÂ25ÈÕ 0:54:28
* Purpose: Declaration of the class RiskDO
***********************************************************************/

#if !defined(__dataobject_ValuationRiskDO_h)
#define __dataobject_ValuationRiskDO_h

#include "dataobjectbase.h"

class ValuationParam
{
public:
	double Price;
	double Volatility;
};

class ValuationRiskDO : public dataobjectbase
{
public:
	std::map<std::string, ValuationParam> ValuationPrice;
	std::string PortfolioID;
	int DaysRemain = 0;
	double Interest = 0;

protected:
private:

};

typedef std::shared_ptr<ValuationRiskDO> ValuationRiskDO_Ptr;

#endif