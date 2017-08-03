#include "PBValuationRiskSerializer.h"
#include "PBRiskSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../Protos/businessobj.pb.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/ValuationRiskDO.h"


data_buffer PBValuationRiskSerializer::Serialize(const dataobj_ptr & abstractDO)
{
	return PBRiskSerializer::Instance()->Serialize(abstractDO);
}

dataobj_ptr PBValuationRiskSerializer::Deserialize(const data_buffer & rawdata)
{
	Micro::Future::Message::Business::PBValuationRisk PB;
	ParseWithReturn(PB, rawdata);

	auto vdo = std::make_shared<ValuationRiskDO>();
	FillDOHeader(vdo, PB);

	vdo->PortfolioID = PB.portfolio();
	vdo->Interest = PB.interest();
	vdo->DaysRemain = PB.daysremain();

	for (auto& it : PB.contractvalue())
	{
		ValuationParam param;
		param.Price = it.price();
		param.Volatility = it.volatility();
		vdo->ValuationPrice.emplace(it.contract(), param);
	}

	return vdo;
}
