#include "InstrumentCache.h"
#include <boolinq/boolinq.h>

void InstrumentCache::Add(const InstrumentDO & instrumentDO)
{
	_instrumentDOMap[instrumentDO.InstrumentID()] = instrumentDO;
}

VectorDO_Ptr<InstrumentDO> InstrumentCache::QueryInstrument(const std::string & instrumentId,
	const std::string & exchangeId, const std::string & productId)
{

	VectorDO_Ptr<InstrumentDO> ret;

	if (instrumentId == EMPTY_STRING && exchangeId == EMPTY_STRING && productId == EMPTY_STRING)
		return ret;

	if (auto pInstrument = QueryInstrumentById(instrumentId))
	{
		ret = std::make_shared<VectorDO<InstrumentDO>>();
		ret->push_back(*pInstrument);
	}
	else
	{
		using namespace boolinq;
		ret = std::make_shared<VectorDO<InstrumentDO>>();
		if (instrumentId != EMPTY_STRING)
		{
			auto enumerator = from(_instrumentDOMap)
				.where([&instrumentId](const std::pair<const std::string, InstrumentDO>& pair)
			{ return pair.first.find_first_of(instrumentId) == 0; })._enumerator;

			try { for (;;) ret->push_back(enumerator.nextObject().second); }
			catch (EnumeratorEndException &) {}
		}
		else if (productId != EMPTY_STRING)
		{
			auto enumerator = from(_instrumentDOMap)
				.where([&productId](const std::pair<const std::string, InstrumentDO>& pair)
			{ return  pair.second.ProductID.find_first_of(productId) == 0; })._enumerator;

			try { for (;;) ret->push_back(enumerator.nextObject().second); }
			catch (EnumeratorEndException &) {}
		}
		else if (exchangeId != EMPTY_STRING)
		{
			auto enumerator = from(_instrumentDOMap)
				.where([&exchangeId](const std::pair<const std::string, InstrumentDO>& pair)
			{ return stringutility::compare(pair.second.ExchangeID().data(), exchangeId.data()) == 0; })._enumerator;

			try { for (;;) ret->push_back(enumerator.nextObject().second); }
			catch (EnumeratorEndException &) {}
		}
	}

	return ret;
}

InstrumentDO * InstrumentCache::QueryInstrumentById(const std::string & instrumentId)
{
	InstrumentDO* ret = nullptr;
	auto it = _instrumentDOMap.find(instrumentId);
	if (it != _instrumentDOMap.end())
		ret = &it->second;

	return ret;
}

VectorDO_Ptr<InstrumentDO> InstrumentCache::AllInstruments(void)
{
	auto ret = std::make_shared<VectorDO<InstrumentDO>>();
	for (auto pair : _instrumentDOMap)
	{
		ret->push_back(pair.second);
	}

	return ret;
}

void InstrumentCache::Clear(void)
{
	_instrumentDOMap.clear();
}
