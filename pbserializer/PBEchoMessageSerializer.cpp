/***********************************************************************
 * Module:  PBEchoMessageSerializer.cpp
 * Author:  milk
 * Modified: 2015年9月18日 12:09:07
 * Purpose: Implementation of the class PBEchoMessageSerializer
 ***********************************************************************/

#include "PBEchoMessageSerializer.h"
#include "../dataobject/EchoMessageDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBEchoMessageSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBEchoMessageSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBEchoMessageSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	return *((data_buffer*)abstractDO.get());
}

////////////////////////////////////////////////////////////////////////
// Name:       PBEchoMessageSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBEchoMessageSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBEchoMessageSerializer::Deserialize(const data_buffer& rawdata)
{
	return std::make_shared<EchoMessageDO>(rawdata);
}