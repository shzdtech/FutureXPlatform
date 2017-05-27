#include "PBPortfolioSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../Protos/businessobj.pb.h"
#include "../dataobject/TemplateDO.h"

#include "../dataobject/PortfolioDO.h"

data_buffer PBPortfolioSerializer::Serialize(const dataobj_ptr & abstractDO)
{
	Micro::Future::Message::Business::PBPortfolio PB;
	auto pDO = (PortfolioDO*)abstractDO.get();
	FillPBHeader(PB, pDO);

	PB.set_name(pDO->PortfolioID());
	PB.set_hedging(pDO->Hedging);
	PB.set_hedgedelay(pDO->HedgeDelay);
	PB.set_threshold(pDO->Threshold);

	for (auto pair : pDO->HedgeContracts)
	{
		auto pHedgeContracts = PB.add_hedgecontracts();
		pHedgeContracts->set_exchange(pair.second->ExchangeID());
		pHedgeContracts->set_contract(pair.second->InstrumentID());
		pHedgeContracts->set_underlying(pair.first);
	}

	SerializeWithReturn(PB);
}

dataobj_ptr PBPortfolioSerializer::Deserialize(const data_buffer & rawdata)
{
	Micro::Future::Message::Business::PBPortfolio PB;
	ParseWithReturn(PB, rawdata);

	auto ret = std::make_shared<PortfolioDO>(PB.name(), "");
	FillDOHeader(ret, PB);

	ret->Hedging = PB.hedging();
	ret->HedgeDelay = PB.hedgedelay();
	ret->Threshold = PB.threshold();

	for (auto& hc : PB.hedgecontracts())
	{
		ret->HedgeContracts[hc.underlying()] = std::make_shared<ContractKey>(hc.exchange(), hc.contract());
	}

	return ret;
}
