/***********************************************************************
 * Module:  InstrumentDO.h
 * Author:  milk
 * Modified: 2015年7月12日 1:23:17
 * Purpose: Declaration of the class InstrumentDO
 ***********************************************************************/

#if !defined(__dataobject_InstrumentDO_h)
#define __dataobject_InstrumentDO_h

#include "ContractKey.h"
#include "dataobjectbase.h"
#include "EnumTypes.h"

class InstrumentDO : public ContractKey, public dataobjectbase
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
	LifePhaseType LifePhase;
	PositionType PositionType;
	PositionDateType PositionDateType;

	int DeliveryYear;
	int DeliveryMonth;
	int MaxMarketOrderVolume;
	int MinMarketOrderVolume;
	int MaxLimitOrderVolume;
	int MinLimitOrderVolume ;
	int VolumeMultiple;
	bool IsTrading;

	double PriceTick;
	double LongMarginRatio;
	double ShortMarginRatio;
	double StrikePrice;
	
	ContractKey UnderlyingContract;
protected:

private:

};

#endif