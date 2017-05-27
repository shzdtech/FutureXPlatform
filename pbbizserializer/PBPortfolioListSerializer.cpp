#include "PBPortfolioListSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../Protos/businessobj.pb.h"
#include "../dataobject/TemplateDO.h"

#include "../dataobject/PortfolioDO.h"

data_buffer PBPortfolioListSerializer::Serialize(const dataobj_ptr & abstractDO)
{
	Micro::Future::Message::Business::PBPortfolioList PB;
	auto pDO = (VectorDO<PortfolioDO>*)abstractDO.get();
	FillPBHeader(PB, pDO);

	for (auto& po : *pDO)
	{
		auto pParam = PB.add_portfolio();
		pParam->set_name(po.PortfolioID());
		pParam->set_hedging(po.Hedging);
		pParam->set_hedgedelay(po.HedgeDelay);
		pParam->set_threshold(po.Threshold);

		for (auto pair : po.HedgeContracts)
		{
			auto pHedgeContracts = pParam->add_hedgecontracts();
			pHedgeContracts->set_exchange(pair.second->ExchangeID());
			pHedgeContracts->set_contract(pair.second->InstrumentID());
			pHedgeContracts->set_underlying(pair.first);
		}
	}

	SerializeWithReturn(PB);
}

dataobj_ptr PBPortfolioListSerializer::Deserialize(const data_buffer & rawdata)
{
	return nullptr;
}
