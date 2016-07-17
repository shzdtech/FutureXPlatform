/***********************************************************************
 * Module:  PBCombineSerializer.h
 * Author:  milk
 * Modified: 2015年8月9日 0:07:54
 * Purpose: Declaration of the class PBCombineSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBCombineSerializer_h)
#define __pbserializer_PBCombineSerializer_h

#include "../dataobject/IDataSerializer.h"
#include "pbserializer_exp.h"

class PBSERIALIZER_CLASS_EXPORTS PBCombineSerializer : public IDataSerializer
{
public:
   PBCombineSerializer(ISerializer_Ptr serializer, IDeserializer_Ptr deserializer);
   ~PBCombineSerializer();
   data_buffer Serialize(const dataobj_ptr& abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:
   ISerializer_Ptr _serializer;
   IDeserializer_Ptr _deserializer;


};

#endif