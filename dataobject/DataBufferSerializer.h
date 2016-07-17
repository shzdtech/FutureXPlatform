/***********************************************************************
 * Module:  DataBufferSerializer.h
 * Author:  milk
 * Modified: 2015年7月11日 13:53:06
 * Purpose: Declaration of the class DataBufferSerializer
 ***********************************************************************/

#if !defined(__dataobject_DataBufferSerializer_h)
#define __dataobject_DataBufferSerializer_h

#include "IDataSerializer.h"
#include "../utility/singleton_templ.h"

class DataBufferSerializer : public IDataSerializer, public singleton_ptr<DataBufferSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr& abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif