#include "PBPricingContractSerializer.h"
#include "PBStrategySerializer.h"
#include "../Protos/businessobj.pb.h"
#include "../pbserializer/pbmacros.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/StrategyContractDO.h"

data_buffer PBPricingContractSerializer::Serialize(const dataobj_ptr & abstractDO)
{
	return PBStrategySerializer::Instance()->Serialize(abstractDO);
}

dataobj_ptr PBPricingContractSerializer::Deserialize(const data_buffer & rawdata)
{
	Micro::Future::Message::Business::PBStrategy pbstrtg;
	ParseWithReturn(pbstrtg, rawdata);

	auto sdo = std::make_shared<StrategyContractDO>(pbstrtg.exchange(), pbstrtg.contract());
	FillDOHeader(sdo, pbstrtg);

	sdo->StrategyName = pbstrtg.symbol();

	if (!pbstrtg.pricingcontracts().empty())
	{
		sdo->PricingContracts = std::make_shared<StrategyPricingContract>();
		for (auto& bc : pbstrtg.pricingcontracts())
		{
			PricingContract cp(bc.exchange(), bc.contract());
			cp.Weight = bc.weight();
			cp.Adjust = bc.adjust();
			sdo->PricingContracts->PricingContracts.push_back(std::move(cp));
		}
	}

	if (!pbstrtg.ivmcontracts().empty())
	{
		sdo->IVMContracts = std::make_shared<StrategyPricingContract>();
		for (auto& bc : pbstrtg.ivmcontracts())
		{
			PricingContract cp(bc.exchange(), bc.contract());
			cp.Weight = bc.weight();
			cp.Adjust = bc.adjust();
			sdo->IVMContracts->PricingContracts.push_back(std::move(cp));
		}
	}

	if (!pbstrtg.volcontracts().empty())
	{
		sdo->VolContracts = std::make_shared<StrategyPricingContract>();
		for (auto& bc : pbstrtg.volcontracts())
		{
			PricingContract cp(bc.exchange(), bc.contract());
			cp.Weight = bc.weight();
			cp.Adjust = bc.adjust();
			sdo->VolContracts->PricingContracts.push_back(std::move(cp));
		}
	}

	static const std::string empty;
	// Fill Models
	if (!pbstrtg.pricingmodel().empty())
	{
		sdo->PricingModel = std::make_shared<ModelParamsDO>(pbstrtg.pricingmodel(), empty, empty);
	}

	if (!pbstrtg.ivmodel().empty())
	{
		sdo->IVModel = std::make_shared<ModelParamsDO>(pbstrtg.ivmodel(), empty, empty);
	}

	if (!pbstrtg.volmodel().empty())
	{
		sdo->VolModel = std::make_shared<ModelParamsDO>(pbstrtg.volmodel(), empty, empty);
	}

	return sdo;
}
