/***********************************************************************
 * Module:  PBStrategySerializer.cpp
 * Author:  milk
 * Modified: 2015年8月23日 12:31:07
 * Purpose: Implementation of the class PBStrategySerializer
 ***********************************************************************/

#include "PBStrategySerializer.h"
#include "PBStringTableSerializer.h"
#include "proto/businessobj.pb.h"
#include "../message/BizError.h"
#include "ExceptionDef.h"
#include "../dataobject/TemplateDO.h"

#include "../databaseop/ContractDAO.h"
#include "../databaseop/StrategyContractDAO.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBStrategySerializer::Serialize(const dataobj_ptr abstractDO)
// Purpose:    Implementation of PBStrategySerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBStrategySerializer::Serialize(const dataobj_ptr abstractDO)
{
	Micro::Future::Message::Business::PBStrategyList PB;

	auto pDO = (VectorDO<StrategyContractDO>*)abstractDO.get();

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
		pStrategy->set_spread(sdo.Spread);
		pStrategy->set_offset(sdo.Offset);
		pStrategy->set_enabled(sdo.Enabled);
		pStrategy->set_quantity(sdo.Quantity);

		for (auto& param : *sdo.ParamMap)
		{
			auto paramDO = pStrategy->add_params();
			paramDO->set_name(param.first);
			paramDO->set_value(param.second);
		}

		for (auto& basecontract : *sdo.BaseContracts)
		{
			auto wtContract = pStrategy->add_weightcontract();
			wtContract->set_exchange(basecontract.ExchangeID());
			wtContract->set_contract(basecontract.InstrumentID());
			wtContract->set_weight(basecontract.Weight);
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

	auto ret = std::make_shared<VectorDO<StrategyContractDO>>();

	if (!PB.ParseFromArray(rawdata.get(), rawdata.size()))
		throw BizError(INVALID_DATAFORMAT_CODE, INVALID_DATAFORMAT_DESC);

	auto& strategyList = PB.strategy();

	for (auto& strategy : strategyList)
	{
		StrategyContractDO sdo(strategy.exchange(), strategy.contract());
		sdo.Depth = strategy.depth();
		sdo.Spread = strategy.spread();
		sdo.Offset = strategy.offset();
		sdo.Trading = strategy.allowtrading();
		sdo.Enabled = strategy.enabled();
		sdo.Quantity = strategy.quantity();

		auto paramMap = std::make_shared<std::map<std::string, double>>();
		for (auto& param : strategy.params())
		{
			paramMap->emplace(param.name(), param.value());
		}
		sdo.ParamMap = paramMap;

		auto bcVec = std::make_shared<std::vector<ContractParam>>();
		for (auto& bc : strategy.weightcontract())
		{
			ContractParam cp(bc.exchange(), bc.contract());
			cp.Weight = bc.weight();
			bcVec->push_back(std::move(cp));
		}
		sdo.BaseContracts = bcVec;

		ret->push_back(std::move(sdo));
	}

	return ret;
}