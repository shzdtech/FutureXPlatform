#include "PBRiskSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../Protos/businessobj.pb.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/RiskDO.h"


data_buffer PBRiskSerializer::Serialize(const dataobj_ptr & abstractDO)
{
	Micro::Future::Message::Business::PBRiskList PB;
	auto pDO = (VectorDO<RiskDO>*)abstractDO.get();
	FillPBHeader(PB, pDO);

	for (auto& po : *pDO)
	{
		auto pParam = PB.add_risk();
		pParam->set_exchange(po.ExchangeID());
		pParam->set_contract(po.InstrumentID());
		pParam->set_underlying(po.Underlying);
		pParam->set_delta(po.Delta);
		pParam->set_gamma(po.Gamma);
		pParam->set_theta(po.Theta);
		pParam->set_vega(po.Vega);
		pParam->set_rho(po.Rho);
	}

	SerializeWithReturn(PB);
}

dataobj_ptr PBRiskSerializer::Deserialize(const data_buffer & rawdata)
{
	return nullptr;
}
