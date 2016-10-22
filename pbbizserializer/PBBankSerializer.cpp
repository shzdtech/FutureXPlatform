/***********************************************************************
 * Module:  PBBankSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月12日 19:11:24
 * Purpose: Implementation of the class PBBankSerializer
 ***********************************************************************/

#include "PBBankSerializer.h"
#include "../pbserializer/PBStringMapSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../Protos/businessobj.pb.h"
#include "../dataobject/BankDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBBankSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBBankSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBBankSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	using namespace Micro::Future::Message::Business;

	PBBankInfo PB;
	auto pDO = (BankDO*)abstractDO.get();
	FillPBHeader(PB, pDO)

	PB.set_bankid(pDO->BankID);
	PB.set_branchid(pDO->BranchID);
	PB.set_name(pDO->BankName);

	SerializeWithReturn(PB);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBBankSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBBankSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBBankSerializer::Deserialize(const data_buffer& rawdata)
{
	return PBStringMapSerializer::Instance()->Deserialize(rawdata);
}