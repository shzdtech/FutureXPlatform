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
	ProductType ProductType = ProductType::PRODUCT_FUTURE;
	ContractType ContractType = ContractType::CONTRACTTYPE_UNSPECIFIED;
	std::string CreateDate;
	std::string OpenDate;
	std::string ExpireDate;
	std::string StartDelivDate;
	std::string EndDelivDate;
	LifePhaseType LifePhase = LifePhaseType::PHASE_NOTSTART;
	PositionType PositionType = PositionType::PT_NET;
	PositionDateType PositionDateType = PositionDateType::PDT_USEHISTORY;

	int DeliveryYear = 0;
	int DeliveryMonth = 0;
	int MaxMarketOrderVolume = 100;
	int MinMarketOrderVolume = 1;
	int MaxLimitOrderVolume = 100;
	int MinLimitOrderVolume = 1;
	int VolumeMultiple = 1;
	bool IsTrading = true;

	double PriceTick = 1;
	double LongMarginRatio = 0;
	double ShortMarginRatio = 0;
	double StrikePrice = 0;
	
	ContractKey UnderlyingContract;
protected:

private:

};

#endif