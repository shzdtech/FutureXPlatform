/***********************************************************************
 * Module:  PBCombineSerializer.cpp
 * Author:  milk
 * Modified: 2015年8月9日 0:07:54
 * Purpose: Implementation of the class PBCombineSerializer
 ***********************************************************************/

#include "PBCombineSerializer.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBCombineSerializer::PBCombineSerializer(ISerializer_Ptr serializer, IDeserializer_Ptr deserializer)
// Purpose:    Implementation of PBCombineSerializer::PBCombineSerializer()
// Parameters:
// - serializer
// - deserializer
// Return:     
////////////////////////////////////////////////////////////////////////

PBCombineSerializer::PBCombineSerializer(ISerializer_Ptr serializer, IDeserializer_Ptr deserializer):
_serializer(serializer), _deserializer(deserializer)
{
}

////////////////////////////////////////////////////////////////////////
// Name:       PBCombineSerializer::~PBCombineSerializer()
// Purpose:    Implementation of PBCombineSerializer::~PBCombineSerializer()
// Return:     
////////////////////////////////////////////////////////////////////////

PBCombineSerializer::~PBCombineSerializer()
{
   // TODO : implement
}

////////////////////////////////////////////////////////////////////////
// Name:       PBCombineSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBCombineSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBCombineSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	return _serializer->Serialize(abstractDO);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBCombineSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBCombineSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBCombineSerializer::Deserialize(const data_buffer& rawdata)
{
	return _deserializer->Deserialize(rawdata);
}