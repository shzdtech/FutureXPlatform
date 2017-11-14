#include "UnderlyingMap.h"
#include <map>



bool UnderlyingMap::TryMap(const InstrumentDO& instDO, std::string & mapped)
{
	if (instDO.ProductID == "SRC" || instDO.ProductID == "SRP")
	{
		mapped = "SR_O";
		return true;
	}

	if (instDO.ProductType == ProductType::PRODUCT_ETFOPTION || instDO.ProductType == ProductType::PRODUCT_STOCK)
	{
		if (instDO.UnderlyingContract.InstrumentID().empty())
		{
			mapped = instDO.InstrumentID();
		}
		else
		{
			mapped = instDO.UnderlyingContract.InstrumentID() + "_O";
		}

		return true;
	}

	mapped = instDO.ProductID;

	return false;
}
