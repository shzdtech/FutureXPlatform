/***********************************************************************
 * Module:  PBUserParamSerializer.cpp
 * Author:  milk
 * Modified: 2015年8月23日 14:05:30
 * Purpose: Implementation of the class PBUserParamSerializer
 ***********************************************************************/

#include "PBUserParamSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../Protos/businessobj.pb.h"
#include "../dataobject/TemplateDO.h"

#include "../dataobject/UserContractParamDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBUserParamSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBUserParamSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBUserParamSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	Micro::Future::Message::Business::PBOTCUserParamList PB;
	auto pDO = (VectorDO<UserContractParamDO>*)abstractDO.get();
	FillPBHeader(PB, pDO);

	for (auto& ucp : *pDO)
	{
		auto pParam = PB.add_params();
		pParam->set_exchange(ucp.ExchangeID());
		pParam->set_contract(ucp.InstrumentID());
		pParam->set_quantity(ucp.Quantity);
	}

	int bufsize = PB.ByteSize();
	uint8_t* buf = new uint8_t[bufsize];
	PB.SerializeToArray(buf, bufsize);

	return data_buffer(buf, bufsize);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBUserParamSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBUserParamSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBUserParamSerializer::Deserialize(const data_buffer& rawdata)
{
	Micro::Future::Message::Business::PBOTCUserParamList PB;
	ParseWithReturn(PB, rawdata);

	auto ret = std::make_shared<VectorDO<UserContractParamDO>>();
	FillDOHeader(ret, PB);

	auto& params = PB.params();

	for (auto& p : params)
	{
		UserContractParamDO ucp(p.exchange(), p.contract());
		ucp.Quantity = p.quantity();
		ret->push_back(std::move(ucp));
	}

	return ret;
}