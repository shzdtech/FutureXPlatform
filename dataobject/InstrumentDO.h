/***********************************************************************
 * Module:  InstrumentDO.h
 * Author:  milk
 * Modified: 2015年7月12日 1:23:17
 * Purpose: Declaration of the class InstrumentDO
 ***********************************************************************/

#if !defined(__dataobject_InstrumentDO_h)
#define __dataobject_InstrumentDO_h

#include "ContractKey.h"
#include "AbstractDataObj.h"
#include "EnumTypes.h"

class InstrumentDO : public ContractKey, public AbstractDataObj
{
public:
	InstrumentDO() {}

	InstrumentDO(const std::string& exchangeID, const std::string& instrumentID)
		: ContractKey(exchangeID, instrumentID)	{}

	std::string Name;
	std::string ProductID;
	ProductType ProductType;
	std::string CreateDate;
	std::string OpenDate;
	std::string ExpireDate;
	std::string StartDelivDate;
	std::string EndDelivDate;
	std::string LifePhase;
	PositionType PositionType;
	PositionDateType PositionDateType;
	std::string MaxMarginSideAlgorithm;

	int DeliveryYear = 0;
	int DeliveryMonth = 0;
	int MaxMarketOrderVolume = 0;
	int MinMarketOrderVolume = 0;
	int MaxLimitOrderVolume = 0;
	int MinLimitOrderVolume = 0;
	int VolumeMultiple = 0;
	int IsTrading = 0;

	double PriceTick = 0;
	double LongMarginRatio = 0;
	double ShortMarginRatio = 0;
	

protected:

private:

};

#endif