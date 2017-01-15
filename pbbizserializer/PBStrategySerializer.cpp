/***********************************************************************
 * Module:  PBStrategySerializer.cpp
 * Author:  milk
 * Modified: 2015年8月23日 12:31:07
 * Purpose: Implementation of the class PBStrategySerializer
 ***********************************************************************/

#include "PBStrategySerializer.h"
#include "../Protos/businessobj.pb.h"
#include "../pbserializer/pbmacros.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/StrategyContractDO.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       PBStrategySerializer::Serialize(const dataobj_ptr& abstractDO)
 // Purpose:    Implementation of PBStrategySerializer::Serialize()
 // Parameters:
 // - abstractDO
 // Return:     data_buffer
 ////////////////////////////////////////////////////////////////////////

data_buffer PBStrategySerializer::Serialize(const dataobj_ptr& abstractDO)
{
	Micro::Future::Message::Business::PBStrategyList PB;
	auto pDO = (VectorDO<StrategyContractDO>*)abstractDO.get();
	FillPBHeader(PB, pDO);

	for (auto& sdo : *pDO)
	{
		auto pStrategy = PB.add_strategy();
		pStrategy->set_exchange(sdo.ExchangeID());
		pStrategy->set_contract(sdo.InstrumentID());
		pStrategy->set_hedging(sdo.Hedging);
		pStrategy->set_underlying(sdo.Underlying);
		pStrategy->set_symbol(sdo.StrategyName);
		pStrategy->set_description(sdo.Description);
		pStrategy->set_depth(sdo.Depth);
		pStrategy->set_bidenabled(sdo.BidEnabled);
		pStrategy->set_askenabled(sdo.AskEnabled);
		pStrategy->set_bidqt(sdo.BidQT);
		pStrategy->set_askqt(sdo.AskQT);

		if (!sdo.PricingContracts.empty())
		{
			for (auto& pricingContract : sdo.PricingContracts)
			{
				auto pContract = pStrategy->add_pricingcontracts();
				pContract->set_exchange(pricingContract.ExchangeID());
				pContract->set_contract(pricingContract.InstrumentID());
				pContract->set_weight(pricingContract.Weight);
				pContract->set_adjust(pricingContract.Adjust);
			}
		}

		// Fill Models
		if (sdo.PricingModel)
		{
			pStrategy->set_pricingmodel(sdo.PricingModel->InstanceName);
		}

		if (sdo.IVModel)
		{
			pStrategy->set_ivmodel(sdo.IVModel->InstanceName);
		}

		if (sdo.VolModel)
		{
			pStrategy->set_volmodel(sdo.VolModel->InstanceName);
		}

	}

	SerializeWithReturn(PB);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBStrategySerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBStrategySerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBStrategySerializer::Deserialize(const data_buffer& rawdata)
{
	Micro::Future::Message::Business::PBStrategy pbstrtg;
	ParseWithReturn(pbstrtg, rawdata);

	auto sdo = std::make_shared<StrategyContractDO>(pbstrtg.exchange(), pbstrtg.contract());
	FillDOHeader(sdo, pbstrtg);

	sdo->Depth = pbstrtg.depth();
	sdo->Hedging = pbstrtg.hedging();
	sdo->BidEnabled = pbstrtg.bidenabled();
	sdo->AskEnabled = pbstrtg.askenabled();
	sdo->BidQT = pbstrtg.bidqt();
	sdo->AskQT = pbstrtg.askqt();

	if (!pbstrtg.pricingcontracts().empty())
	{
		for (auto& bc : pbstrtg.pricingcontracts())
		{
			PricingContract cp(bc.exchange(), bc.contract());
			cp.Weight = bc.weight();
			cp.Adjust = bc.adjust();
			sdo->PricingContracts.push_back(std::move(cp));
		}
	}

	// Fill Models
	if (!pbstrtg.pricingmodel().empty())
	{
		sdo->PricingModel = std::make_shared<ModelParamsDO>(pbstrtg.pricingmodel(), "", "");
	}

	if (!pbstrtg.ivmodel().empty())
	{
		sdo->IVModel = std::make_shared<ModelParamsDO>(pbstrtg.ivmodel(), "", "");
	}

	if (!pbstrtg.volmodel().empty())
	{
		sdo->VolModel = std::make_shared<ModelParamsDO>(pbstrtg.volmodel(), "", "");
	}

	return sdo;
}