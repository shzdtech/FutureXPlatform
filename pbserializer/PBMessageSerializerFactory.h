/***********************************************************************
 * Module:  PBMessageSerializerFactory.h
 * Author:  milk
 * Modified: 2015年9月18日 10:26:49
 * Purpose: Declaration of the class PBMessageSerializerFactory
 ***********************************************************************/

#if !defined(__pbserializer_PBMessageSerializerFactory_h)
#define __pbserializer_PBMessageSerializerFactory_h

#include "../dataobject/AbstractDataSerializerFactory.h"
#include "pbserializer_exp.h"

class PBSERIALIZER_CLASS_EXPORTS PBMessageSerializerFactory : public AbstractDataSerializerFactory
{
public:
	PBMessageSerializerFactory();
	~PBMessageSerializerFactory();
	std::map<unsigned int, IDataSerializer_Ptr> CreateDataSerializers(void);

protected:
private:
};

#endif