/***********************************************************************
 * Module:  PBContractParamSerializer.cpp
 * Author:  milk
 * Modified: 2015年8月23日 13:29:16
 * Purpose: Implementation of the class PBContractParamSerializer
 ***********************************************************************/

#include "PBContractParamSerializer.h"
#include "../message/BizError.h"
#include "ExceptionDef.h"
#include "proto/businessobj.pb.h"
#include "../dataobject/TemplateDO.h"

#include "../dataobject/ContractDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBContractParamSerializer::Serialize(const dataobj_ptr abstractDO)
// Purpose:    Implementation of PBContractParamSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBContractParamSerializer::Serialize(const dataobj_ptr abstractDO)
{
	Micro::Future::Message::Business::PBContractParamList PB;

	auto pDO = (VectorDO<ContractDO>*)abstractDO.get();

	for (auto& cto : *pDO)
	{
		auto pParam = PB.add_params();
		pParam->set_exchange(cto.ExchangeID());
		pParam->set_contract(cto.InstrumentID());
		pParam->set_depthvol(cto.DepthVol);
		pParam->set_gamma(cto.Gamma);
	}

	int bufsize = PB.ByteSize();
	uint8_t* buf = new uint8_t[bufsize];
	PB.SerializeToArray(buf, bufsize);

	return data_buffer(buf, bufsize);
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

	auto ret = std::make_shared<VectorDO<ContractDO>>();

	if (!PB.ParseFromArray(rawdata.get(), rawdata.size()))
		throw BizError(INVALID_DATAFORMAT_CODE, INVALID_DATAFORMAT_DESC);

	auto& params = PB.params();

	for (auto& p : params)
	{
		ContractDO cdo(p.exchange(), p.contract());
		cdo.DepthVol = p.depthvol();
		cdo.Gamma = p.gamma();
		ret->push_back(std::move(cdo));
	}

	return ret;
}