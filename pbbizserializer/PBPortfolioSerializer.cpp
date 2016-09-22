#include "PBPortfolioSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../Protos/businessobj.pb.h"
#include "../dataobject/TemplateDO.h"

#include "../dataobject/PortfolioDO.h"

data_buffer PBPortfolioSerializer::Serialize(const dataobj_ptr & abstractDO)
{
	Micro::Future::Message::Business::PBPortfolioList PB;
	auto pDO = (VectorDO<PortfolioDO>*)abstractDO.get();
	FillPBHeader(PB, pDO);

	for (auto& po : *pDO)
	{
		auto pParam = PB.add_portfolio();
		pParam->set_name(po.PortfolioID());
		pParam->set_hedgedelay(po.HedgeDelay);
		// pParam->set_description(po.di)
	}

	SerializeWithReturn(PB);
}

dataobj_ptr PBPortfolioSerializer::Deserialize(const data_buffer & rawdata)
{
	Micro::Future::Message::Business::PBPortfolioList PB;
	ParseWithReturn(PB, rawdata);

	auto ret = std::make_shared<VectorDO<PortfolioDO>>();
	FillDOHeader(ret, PB);

	auto& params = PB.portfolio();

	for (auto& p : params)
	{
		PortfolioDO pdo(p.name(), "");
		pdo.HedgeDelay = p.hedgedelay();
		ret->push_back(std::move(pdo));
	}

	return ret;
}
