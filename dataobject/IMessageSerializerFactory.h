/***********************************************************************
 * Module:  IMessageSerializerFactory.h
 * Author:  milk
 * Modified: 2015年9月18日 9:53:16
 * Purpose: Declaration of the class IMessageSerializerFactory
 ***********************************************************************/

#if !defined(__dataobject_IMessageSerializerFactory_h)
#define __dataobject_IMessageSerializerFactory_h
#include <map>
#include <memory>
#include "IDataSerializer.h"

class IMessageSerializerFactory
{
public:
	virtual std::map<unsigned int, IDataSerializer_Ptr> CreateDataSerializers(void) = 0;

protected:
private:

};

typedef std::shared_ptr<IMessageSerializerFactory> IMessageSerializerFactory_Ptr;

#endif