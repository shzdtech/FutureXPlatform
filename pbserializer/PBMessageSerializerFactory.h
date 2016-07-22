/***********************************************************************
 * Module:  PBMessageSerializerFactory.h
 * Author:  milk
 * Modified: 2015年9月18日 10:26:49
 * Purpose: Declaration of the class PBMessageSerializerFactory
 ***********************************************************************/

#if !defined(__pbserializer_PBMessageSerializerFactory_h)
#define __pbserializer_PBMessageSerializerFactory_h

#include "../dataserializer/AbstractDataSerializerFactory.h"
#include "pbserializer_exp.h"

class PBSERIALIZER_CLASS_EXPORTS PBMessageSerializerFactory : public AbstractDataSerializerFactory
{
public:
	PBMessageSerializerFactory();
	~PBMessageSerializerFactory();
	void CreateDataSerializers(std::map<uint32_t, IDataSerializer_Ptr>& serializerMap);

protected:
private:
};

#endif