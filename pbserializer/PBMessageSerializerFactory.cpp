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
	_serializer_map[MSG_ID_ERROR] = PBExceptionSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_MODELPARAMS] = PBModelParamsSerializer::Instance();
	_serializer_map[MSG_ID_UPDATE_MODELPARAMS] = std::make_shared<PBCombineSerializer>
		(PBResultSerializer::Instance(), PBModelParamsSerializer::Instance());
	_serializer_map[MSG_ID_QUERY_MODELPARAMDEF] = PBModelParamDefSerializer::Instance();
	_serializer_map[MSG_ID_UPDATE_TEMPMODELPARAMS] = std::make_shared<PBCombineSerializer>
		(PBResultSerializer::Instance(), PBModelParamsSerializer::Instance());
	_serializer_map[MSG_ID_QUERY_SYSPARAMS] = PBStringMapSerializer::Instance();


	_serializer_map[MSG_ID_SESSION_CREATED] = PBEchoMessageSerializer::Instance();
	_serializer_map[MSG_ID_ECHO] = PBEchoMessageSerializer::Instance();
	_serializer_map[MSG_ID_LOGIN] = std::make_shared<PBCombineSerializer>
		(PBUserInfoSerializer::Instance(), PBStringMapSerializer::Instance());

	//Account
	_serializer_map[MSG_ID_USER_INFO] = std::make_shared<PBCombineSerializer>
		(PBUserInfoSerializer::Instance(), PBStringMapSerializer::Instance());
	_serializer_map[MSG_ID_USER_NEW] = PBUserInfoSerializer::Instance();
	_serializer_map[MSG_ID_USER_INFO_UPDATE] = PBUserInfoSerializer::Instance();
	_serializer_map[MSG_ID_RESET_PASSWORD] = std::make_shared<PBCombineSerializer>
		(PBResultSerializer::Instance(), PBStringMapSerializer::Instance());
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

void PBMessageSerializerFactory::CreateDataSerializers(std::map<uint32_t, IDataSerializer_Ptr>& serializerMap)
{
	serializerMap.insert(_serializer_map.begin(), _serializer_map.end());
}