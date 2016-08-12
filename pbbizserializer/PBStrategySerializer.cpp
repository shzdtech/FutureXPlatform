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
		pStrategy->set_symbol(sdo.StrategyName);
		pStrategy->set_description(sdo.Description);
		pStrategy->set_depth(sdo.Depth);
		pStrategy->set_enabled(sdo.Enabled);
		pStrategy->set_quantity(sdo.Quantity);

		if (sdo.PricingContracts.size() > 0)
		{
			for (auto& pricingContract : sdo.PricingContracts)
			{
				auto pContract = pStrategy->add_pricingcontracts();
				pContract->set_exchange(pricingContract.ExchangeID());
				pContract->set_contract(pricingContract.InstrumentID());
				pContract->set_weight(pricingContract.Weight);
			}
		}

		// Fill Model Param
		auto pbModelParams = pStrategy->mutable_modelparams();

		pbModelParams->set_modelname(sdo.ModelParams.ModelName);

		pbModelParams->mutable_scalaparams()->insert(sdo.ModelParams.ScalaParams.begin(), sdo.ModelParams.ScalaParams.end());

		for (auto& pair : sdo.ModelParams.VectorParams)
		{
			Micro::Future::Message::ModelParams_double_vector double_vector;
			for (double val : pair.second)
				double_vector.add_entry(val);

			(*pbModelParams->mutable_vectorparams())[pair.first] = double_vector;
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
	Micro::Future::Message::Business::PBStrategy pbstrtg;
	ParseWithReturn(pbstrtg, rawdata);

	auto sdo = std::make_shared<StrategyContractDO>(pbstrtg.exchange(), pbstrtg.contract());
	FillDOHeader(sdo, pbstrtg);

	sdo->Depth = pbstrtg.depth();
	sdo->Trading = pbstrtg.allowtrading();
	sdo->Enabled = pbstrtg.enabled();
	sdo->Quantity = pbstrtg.quantity();

	if (!pbstrtg.pricingcontracts().empty())
	{
		for (auto& bc : pbstrtg.pricingcontracts())
		{
			PricingContract cp(bc.exchange(), bc.contract());
			cp.Weight = bc.weight();
			sdo->PricingContracts.push_back(std::move(cp));
		}
	}

	// Model Params
	sdo->ModelParams.ScalaParams.insert(
		pbstrtg.modelparams().scalaparams().begin(), pbstrtg.modelparams().scalaparams().end());

	auto& vectorParams = pbstrtg.modelparams().vectorparams();
	for (auto& pair : vectorParams)
	{
		std::vector<double> double_vec(pair.second.entry().begin(),
			pair.second.entry().end());
		sdo->ModelParams.VectorParams.emplace(pair.first, double_vec);
	}

	return sdo;
}