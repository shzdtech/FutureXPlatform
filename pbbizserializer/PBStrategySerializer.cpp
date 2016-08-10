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
		pStrategy->set_allowtrading(sdo.Trading);
		pStrategy->set_underlying(sdo.Underlying);
		pStrategy->set_symbol(sdo.Strategy);
		pStrategy->set_description(sdo.Description);
		pStrategy->set_depth(sdo.Depth);
		pStrategy->set_enabled(sdo.Enabled);
		pStrategy->set_quantity(sdo.Quantity);

		pStrategy->mutable_params()->insert(sdo.Params.begin(), sdo.Params.end());

		if (sdo.PricingContracts)
		{
			for (auto& pricingContract : *sdo.PricingContracts)
			{
				auto pContract = pStrategy->add_pricingcontracts();
				pContract->set_exchange(pricingContract.ExchangeID());
				pContract->set_contract(pricingContract.InstrumentID());
				pContract->set_weight(pricingContract.Weight);
			}
		}
	}

	int bufsize = PB.ByteSize();
	uint8_t* buf = new uint8_t[bufsize];
	PB.SerializeToArray(buf, bufsize);

	return data_buffer(buf, bufsize);
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
	Micro::Future::Message::Business::PBStrategyList PB;
	ParseWithReturn(PB, rawdata);

	auto ret = std::make_shared<VectorDO<StrategyContractDO>>();
	FillDOHeader(ret, PB);

	auto& strategyList = PB.strategy();

	for (auto& strategy : strategyList)
	{
		StrategyContractDO sdo(strategy.exchange(), strategy.contract());
		sdo.Depth = strategy.depth();
		sdo.Trading = strategy.allowtrading();
		sdo.Enabled = strategy.enabled();
		sdo.Quantity = strategy.quantity();

		sdo.Params.insert(strategy.params().begin(), strategy.params().end());

		if (!strategy.pricingcontracts().empty())
		{
			auto bcVec = std::make_shared<std::vector<PricingContract>>();
			for (auto& bc : strategy.pricingcontracts())
			{
				PricingContract cp(bc.exchange(), bc.contract());
				cp.Weight = bc.weight();
				bcVec->push_back(std::move(cp));
			}
			sdo.PricingContracts = bcVec;
		}

		ret->push_back(std::move(sdo));
	}

	return ret;
}