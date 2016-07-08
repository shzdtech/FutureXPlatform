/***********************************************************************
 * Module:  PBOrderCancelSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月17日 22:15:34
 * Purpose: Implementation of the class PBOrderCancelSerializer
 ***********************************************************************/

#include "PBOrderCancelSerializer.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBOrderCancelSerializer::Serialize(const dataobj_ptr abstractDO)
// Purpose:    Implementation of PBOrderCancelSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBOrderCancelSerializer::Serialize(const dataobj_ptr abstractDO)
{
	return data_buffer(nullptr, 0);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBOrderCancelSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBOrderCancelSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBOrderCancelSerializer::Deserialize(const data_buffer& rawdata)
{
	return nullptr;
}