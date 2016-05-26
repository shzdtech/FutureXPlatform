#include "InstrumentCache.h"
#include <boolinq/boolinq.h>

static InstrumentDOMap instrumentDOMap;

void InstrumentCache::Add(const InstrumentDO & instrumentDO)
{
	instrumentDOMap[instrumentDO.InstrumentID()] = instrumentDO;
}

VectorDO_Ptr<InstrumentDO> InstrumentCache::QueryInstrument(const std::string & instrumentId, 
	const std::string & exchangeId, const std::string & productId)
{

	VectorDO_Ptr<InstrumentDO> ret;

	if (instrumentId == EMPTY_STRING && exchangeId == EMPTY_STRING && productId == EMPTY_STRING)
		return ret;

	ret = std::make_shared<VectorDO<InstrumentDO>>();

	auto it = instrumentDOMap.find(instrumentId);
	if (it != instrumentDOMap.end())
	{
		ret->push_back(it->second);
	}
	else
	{
		using namespace boolinq;

		if (instrumentId != EMPTY_STRING)
		{
			auto enumerator = from(*ret).where([&instrumentId](const InstrumentDO& insDO)
			{ return insDO.InstrumentID().find_first_of(instrumentId) == 0; })._enumerator;

			try { for (;;) ret->push_back(enumerator.nextObject()); }
			catch (EnumeratorEndException &) {}
		}
		else if (productId != EMPTY_STRING)
		{
			auto enumerator = from(*ret).where([&productId](const InstrumentDO& insDO)
			{ return insDO.ProductID.find_first_of(productId) == 0; })._enumerator;

			try { for (;;) ret->push_back(enumerator.nextObject()); }
			catch (EnumeratorEndException &) {}
		}
		else if (exchangeId != EMPTY_STRING)
		{
			auto enumerator = from(*ret).where([&exchangeId](const InstrumentDO& insDO)
			{ return stringutility::compare(insDO.ExchangeID().data(), exchangeId.data()) == 0; })._enumerator;

			try { for (;;) ret->push_back(enumerator.nextObject()); }
			catch (EnumeratorEndException &) {}
		}
	}

	return ret;
}

void InstrumentCache::Clear(void)
{
	instrumentDOMap.clear();
}
