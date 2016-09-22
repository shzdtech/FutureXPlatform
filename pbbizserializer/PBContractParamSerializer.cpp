
/***********************************************************************
 * Module:  PBContractParamSerializer.cpp
 * Author:  milk
 * Modified: 2015年8月23日 13:29:16
 * Purpose: Implementation of the class PBContractParamSerializer
 ***********************************************************************/

#include "PBContractParamSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../Protos/businessobj.pb.h"
#include "../dataobject/TemplateDO.h"

#include "../dataobject/ContractParamDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBContractParamSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBContractParamSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBContractParamSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	Micro::Future::Message::Business::PBContractParamList PB;
	auto pDO = (VectorDO<ContractParamDO>*)abstractDO.get();
	FillPBHeader(PB, pDO);

	for (auto& cto : *pDO)
	{
		auto pParam = PB.add_params();
		pParam->set_exchange(cto.ExchangeID());
		pParam->set_contract(cto.InstrumentID());
		pParam->set_depthvol(cto.DepthVol);
		pParam->set_gamma(cto.Gamma);
	}

	SerializeWithReturn(PB);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBContractParamSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBContractParamSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBContractParamSerializer::Deserialize(const data_buffer& rawdata)
{
	Micro::Future::Message::Business::PBContractParamList PB;
	ParseWithReturn(PB, rawdata);

	auto ret = std::make_shared<VectorDO<ContractParamDO>>();
	FillDOHeader(ret, PB);

	auto& params = PB.params();

	for (auto& p : params)
	{
		ContractParamDO cdo(p.exchange(), p.contract());
		cdo.DepthVol = p.depthvol();
		cdo.Gamma = p.gamma();
		ret->push_back(std::move(cdo));
	}

	return ret;
}