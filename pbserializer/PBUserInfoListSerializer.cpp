/***********************************************************************
 * Module:  PBUserInfoListSerializer.cpp
 * Author:  milk
 * Modified: 2015年8月27日 20:21:02
 * Purpose: Implementation of the class PBUserInfoListSerializer
 ***********************************************************************/

#include "PBUserInfoListSerializer.h"
#include "proto/businessobj.pb.h"
#include "../message/BizError.h"
#include "ExceptionDef.h"
#include "../dataobject/UserInfoDO.h"
#include "../dataobject/TemplateDO.h"
////////////////////////////////////////////////////////////////////////
// Name:       PBUserInfoListSerializer::Serialize(const dataobj_ptr abstractDO)
// Purpose:    Implementation of PBUserInfoListSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBUserInfoListSerializer::Serialize(const dataobj_ptr abstractDO)
{
	Micro::Future::Message::Business::PBUserInfoList PB;

	auto pDO = (VectorDO<UserInfoDO>*)abstractDO.get();

	for (auto& userDO : *pDO)
	{
		auto pUserInfo = PB.add_userinfo();
		pUserInfo->set_brokerid(userDO.BrokerId);
		pUserInfo->set_company(userDO.Company);
		pUserInfo->set_contactnum(userDO.ContactNum);
		pUserInfo->set_email(userDO.Email);
		pUserInfo->set_name(userDO.Name);
		pUserInfo->set_permission(userDO.Permission);
		pUserInfo->set_role(userDO.Role);
		pUserInfo->set_userid(userDO.UserId);
	}

	int bufsize = PB.ByteSize();
	uint8_t* buf = new uint8_t[bufsize];
	PB.SerializeToArray(buf, bufsize);

	return data_buffer(buf, bufsize);
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
	return nullptr;
}