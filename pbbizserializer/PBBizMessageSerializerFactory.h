/***********************************************************************
 * Module:  PBBizMessageSerializerFactory.h
 * Author:  milk
 * Modified: 2015年9月18日 10:26:49
 * Purpose: Declaration of the class PBBizMessageSerializerFactory
 ***********************************************************************/

#if !defined(__pbbizserializer_PBBizMessageSerializerFactory_h)
#define __pbbizserializer_PBBizMessageSerializerFactory_h

#include "../dataserializer/AbstractDataSerializerFactory.h"
#include "pbbizserializer_exp.h"

static const char* CLASS_UUID_MSG_BIZ_SERIALIZER_FACTORY = "factory.message.biz.serializer";

class PBBIZSERIALIZER_CLASS_EXPORTS PBBizMessageSerializerFactory : public AbstractDataSerializerFactory
{
public:
	PBBizMessageSerializerFactory();
	~PBBizMessageSerializerFactory();
	void CreateDataSerializers(std::map<uint32_t, IDataSerializer_Ptr>& serializerMap);

protected:
private:
};

#endif