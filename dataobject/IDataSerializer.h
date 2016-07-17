/***********************************************************************
 * Module:  IDataSerializer.h
 * Author:  milk
 * Modified: 2014年10月22日 10:21:53
 * Purpose: Declaration of the class IDataSerializer
 ***********************************************************************/

#if !defined(__dataobject_IDataSerializer_h)
#define __dataobject_IDataSerializer_h

#include <memory>
#include "data_buffer.h"
#include "dataobjectbase.h"

class ISerializer
{
public:
	virtual data_buffer Serialize(const dataobj_ptr& abstractDO) = 0;
};

class IDeserializer
{
public:
	virtual dataobj_ptr Deserialize(const data_buffer& rawdata) = 0;
};

class IDataSerializer : public ISerializer, public IDeserializer
{
};

typedef std::shared_ptr<ISerializer> ISerializer_Ptr;
typedef std::shared_ptr<IDeserializer> IDeserializer_Ptr;
typedef std::shared_ptr<IDataSerializer> IDataSerializer_Ptr;

#endif