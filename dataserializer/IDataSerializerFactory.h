/***********************************************************************
 * Module:  IMessageSerializerFactory.h
 * Author:  milk
 * Modified: 2015年9月18日 9:53:16
 * Purpose: Declaration of the class IMessageSerializerFactory
 ***********************************************************************/

#if !defined(__dataserializer_IMessageSerializerFactory_h)
#define __dataserializer_IMessageSerializerFactory_h
#include <map>
#include <memory>
#include "IDataSerializer.h"
#include "../message/IServerContext.h"

class IDataSerializerFactory
{
public:
	virtual void CreateDataSerializers(std::map<uint32_t, IDataSerializer_Ptr>& serializerMap) = 0;

protected:
private:

};

typedef std::shared_ptr<IDataSerializerFactory> IDataSerializerFactory_Ptr;

#endif