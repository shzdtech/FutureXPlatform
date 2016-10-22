/***********************************************************************
 * Module:  PBUserAccountSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月12日 19:11:15
 * Purpose: Implementation of the class PBUserAccountSerializer
 ***********************************************************************/

#include "PBUserAccountSerializer.h"
#include "PBBankOperationSerializer.h"
#include "../Protos/businessobj.pb.h"
#include "../pbserializer/pbmacros.h"
#include "../dataobject/UserAccountRegisterDO.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       PBUserAccountSerializer::Serialize(const dataobj_ptr& abstractDO)
 // Purpose:    Implementation of PBUserAccountSerializer::Serialize()
 // Parameters:
 // - abstractDO
 // Return:     data_buffer
 ////////////////////////////////////////////////////////////////////////

data_buffer PBUserAccountSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	using namespace Micro::Future::Message::Business;

	PBUserAccountInfo PB;
	auto pDO = (UserAccountRegisterDO*)abstractDO.get();

	FillPBHeader(PB, pDO);

	PB.set_accountid(pDO->AccountID);
	PB.set_bankaccount(pDO->BankAccount);
	PB.set_bankacctype(pDO->BankAccType);
	PB.set_bankid(pDO->BankID);
	PB.set_bankbranchid(pDO->BankBranchID);
	PB.set_brokerid(pDO->BrokerID);
	PB.set_brokerbranchid(pDO->BrokerBranchID);
	PB.set_currencyid(pDO->CurrencyID);
	PB.set_customername(pDO->CustomerName);
	PB.set_custtype(pDO->CustType);
	PB.set_idcardno(pDO->IdentifiedCardNo);

	if (pDO->BankUseAmount > 1e-3)	PB.set_bankuseamount(pDO->BankUseAmount);
	if (pDO->BankUseAmount > 1e-3)	PB.set_bankfetchamount(pDO->BankUseAmount);

	SerializeWithReturn(PB);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBUserAccountSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBUserAccountSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBUserAccountSerializer::Deserialize(const data_buffer& rawdata)
{
	return PBBankOperationSerializer::Instance()->Deserialize(rawdata);
}