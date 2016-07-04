/***********************************************************************
 * Module:  PBMessageSerializerFactory.cpp
 * Author:  milk
 * Modified: 2015年9月18日 10:26:49
 * Purpose: Implementation of the class PBMessageSerializerFactory
 ***********************************************************************/

#include "PBMessageSerializerFactory.h"
#include "pbserializers.h"
#include "../message/DefMessageID.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       PBMessageSerializerFactory::PBMessageSerializerFactory()
 // Purpose:    Implementation of PBMessageSerializerFactory::PBMessageSerializerFactory()
 // Return:     
 ////////////////////////////////////////////////////////////////////////

PBMessageSerializerFactory::PBMessageSerializerFactory()
{
	//Common
	_serializer_map[MSG_ID_ERROR] = PBBizErrorSerializer::Instance();
	_serializer_map[MSG_ID_SESSION_CREATED] = PBEchoMsgSerializer::Instance();
	_serializer_map[MSG_ID_ECHO] = PBEchoMsgSerializer::Instance();
	_serializer_map[MSG_ID_LOGIN] = std::make_shared<PBCombineSerializer>
		(PBUserInfoSerializer::Instance(), PBStringMapSerializer::Instance());

	//Account
	_serializer_map[MSG_ID_USER_INFO] = std::make_shared<PBCombineSerializer>
		(PBUserInfoSerializer::Instance(), PBStringMapSerializer::Instance());
	_serializer_map[MSG_ID_USER_NEW] = std::make_shared<PBCombineSerializer>
		(PBResultSerializer::Instance(), PBUserInfoSerializer::Instance());
	_serializer_map[MSG_ID_USER_INFO_UPDATE] = std::make_shared<PBCombineSerializer>
		(PBResultSerializer::Instance(), PBUserInfoSerializer::Instance());
}

////////////////////////////////////////////////////////////////////////
// Name:       PBMessageSerializerFactory::~PBMessageSerializerFactory()
// Purpose:    Implementation of PBMessageSerializerFactory::~PBMessageSerializerFactory()
// Return:     
////////////////////////////////////////////////////////////////////////

PBMessageSerializerFactory::~PBMessageSerializerFactory()
{
	// TODO : implement
}

////////////////////////////////////////////////////////////////////////
// Name:       PBMessageSerializerFactory::CreateDataSerializers()
// Purpose:    Implementation of PBMessageSerializerFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<unsigned int, IDataSerializer_Ptr> PBMessageSerializerFactory::CreateDataSerializers(void)
{
	return _serializer_map;
}