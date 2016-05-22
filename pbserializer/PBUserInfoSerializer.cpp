/***********************************************************************
 * Module:  PBUserInfoSerializer.cpp
 * Author:  milk
 * Modified: 2015年8月23日 23:23:10
 * Purpose: Implementation of the class PBUserInfoSerializer
 ***********************************************************************/

#include "PBUserInfoSerializer.h"
#include "proto/usermanager.pb.h"
#include "../message/BizError.h"
#include "ExceptionDef.h"
#include "../dataobject/UserInfoDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBUserInfoSerializer::Serialize(const dataobj_ptr abstractDO)
// Purpose:    Implementation of PBUserInfoSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBUserInfoSerializer::Serialize(const dataobj_ptr abstractDO)
{
	using namespace Micro::Future::Message::Business;
	
	PBUserInfo PB;

	auto pDO = (UserInfoDO*)abstractDO.get();

	PB.set_address(pDO->Address);
	PB.set_company(pDO->Company);
	PB.set_contactnum(pDO->ContactNum);
	PB.set_email(pDO->Email);
	PB.set_firstname(pDO->FirstName);
	PB.set_lastname(pDO->LastName);
	PB.set_gender(pDO->Gender);
	PB.set_identitynum(pDO->IdentityNum);
	PB.set_zipcode(pDO->ZipCode);
	PB.set_userid(pDO->UserId);
	PB.set_password(pDO->Password);

	PB.set_permission(pDO->Permission);
	PB.set_role(pDO->Role);

	int bufsize = PB.ByteSize();
	uint8_t* buf = new uint8_t[bufsize];
	PB.SerializeToArray(buf, bufsize);

	return data_buffer(buf, bufsize);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBUserInfoSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBUserInfoSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBUserInfoSerializer::Deserialize(const data_buffer& rawdata)
{
	Micro::Future::Message::Business::PBUserInfo PB;

	auto ret = std::make_shared<UserInfoDO>();

	if (!PB.ParseFromArray(rawdata.get(), rawdata.size()))
		throw BizError(INVALID_DATAFORMAT_CODE, INVALID_DATAFORMAT_DESC);

	ret->Address = PB.address();
	ret->Company = PB.company();
	ret->ContactNum = PB.contactnum();
	ret->Email = PB.email();
	ret->FirstName = PB.firstname();
	ret->LastName = PB.lastname();
	ret->Gender = (GenderType)PB.gender();
	ret->IdentityNum = PB.identitynum();
	ret->ZipCode = PB.zipcode();
	ret->UserId = PB.userid();
	ret->Password = PB.password();

	ret->Role = PB.role();

	return ret;
}