#include "InstrumentCache.h"
#include <boolinq/boolinq.h>
#include "../databaseop/ContractDAO.h"

void InstrumentCache::Add(const InstrumentDO & instrumentDO)
{
	_instrumentDOMap[instrumentDO.InstrumentID()] = instrumentDO;
}

VectorDO_Ptr<InstrumentDO> InstrumentCache::QueryInstrument(const std::string & instrumentId,
	const std::string & exchangeId, const std::string & productId)
{

	VectorDO_Ptr<InstrumentDO> ret;

	if (instrumentId.empty() && exchangeId.empty() && productId.empty())
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
		auto& instrumentMap = static_cast<std::map<std::string, InstrumentDO, ci_less>>(_instrumentDOMap);

		if (!instrumentId.empty())
		{
			auto enumerator = from(instrumentMap)
				.where([&instrumentId](const std::pair<const std::string, InstrumentDO>& pair)
			{ return pair.first.find_first_of(instrumentId) == 0; })._enumerator;

			try { for (;;) ret->push_back(enumerator.nextObject().second); }
			catch (EnumeratorEndException &) {}
		}
		else if (!productId.empty())
		{
			auto enumerator = from(instrumentMap)
				.where([&productId](const std::pair<const std::string, InstrumentDO>& pair)
			{ return  pair.second.ProductID.find_first_of(productId) == 0; })._enumerator;

			try { for (;;) ret->push_back(enumerator.nextObject().second); }
			catch (EnumeratorEndException &) {}
		}
		else if (!exchangeId.empty())
		{
			auto enumerator = from(instrumentMap)
				.where([&exchangeId](const std::pair<const std::string, InstrumentDO>& pair)
			{ return stringutility::compare(pair.second.ExchangeID().data(), exchangeId.data()) == 0; })._enumerator;

			try { for (;;) ret->push_back(enumerator.nextObject().second); }
			catch (EnumeratorEndException &) {}
		}
	}

	return ret;
}

VectorDO_Ptr<InstrumentDO> InstrumentCache::QueryInstrumentByProductType(ProductType productType)
{
	VectorDO_Ptr<InstrumentDO> ret;

	using namespace boolinq;
	ret = std::make_shared<VectorDO<InstrumentDO>>();
	auto& instrumentMap = static_cast<std::map<std::string, InstrumentDO, ci_less>>(_instrumentDOMap);

	auto enumerator = from(instrumentMap)
		.where([&productType](const std::pair<const std::string, InstrumentDO>& pair)
	{ return pair.second.ProductType == productType; })._enumerator;

	try { for (;;) ret->push_back(enumerator.nextObject().second); }
	catch (EnumeratorEndException &) {}

	return ret;
}

InstrumentDO * InstrumentCache::QueryInstrumentById(const std::string & instrumentId)
{
	return _instrumentDOMap.tryfind(instrumentId);
}

InstrumentDO * InstrumentCache::QueryInstrumentOrAddById(const std::string & instrumentId)
{
	auto ret = _instrumentDOMap.tryfind(instrumentId);

	if (!ret)
	{
		InstrumentDO ido;
		if (ContractDAO::FindExchangeContractById(instrumentId, ido))
		{
			Add(ido);
		}

		ret = _instrumentDOMap.tryfind(instrumentId);
	}

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
