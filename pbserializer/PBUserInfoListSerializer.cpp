/***********************************************************************
 * Module:  PBUserInfoListSerializer.cpp
 * Author:  milk
 * Modified: 2015年8月27日 20:21:02
 * Purpose: Implementation of the class PBUserInfoListSerializer
 ***********************************************************************/

#include "PBUserInfoListSerializer.h"
#include "PBStringMapSerializer.h"
#include "../Protos/usermanager.pb.h"
#include "pbmacros.h"
#include "../dataobject/UserInfoDO.h"
#include "../dataobject/TemplateDO.h"
////////////////////////////////////////////////////////////////////////
// Name:       PBUserInfoListSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBUserInfoListSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBUserInfoListSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	Micro::Future::Message::PBUserInfoList PB;
	auto pDO = (VectorDO<UserInfoDO>*)abstractDO.get();
	FillPBHeader(PB, pDO);

	for (auto& userDO : *pDO)
	{
		auto pUserInfo = PB.add_userinfo();
		pUserInfo->set_address(userDO.Address);
		pUserInfo->set_company(userDO.Company);
		pUserInfo->set_contactnum(userDO.ContactNum);
		pUserInfo->set_email(userDO.Email);
		pUserInfo->set_firstname(userDO.FirstName);
		pUserInfo->set_lastname(userDO.LastName);
		pUserInfo->set_gender(userDO.Gender);
		pUserInfo->set_identitynum(userDO.IdentityNum);
		pUserInfo->set_zipcode(userDO.ZipCode);
		pUserInfo->set_userid(userDO.UserId);
		pUserInfo->set_username(userDO.UserName);
		pUserInfo->set_permission(userDO.Permission);
		pUserInfo->set_role(userDO.Role);
	}

	SerializeWithReturn(PB);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBUserInfoListSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBUserInfoListSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBUserInfoListSerializer::Deserialize(const data_buffer& rawdata)
{
	return PBStringMapSerializer::Instance()->Deserialize(rawdata);
}