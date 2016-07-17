/***********************************************************************
 * Module:  PBEchoMsgSerializer.cpp
 * Author:  milk
 * Modified: 2015年9月18日 12:09:07
 * Purpose: Implementation of the class PBEchoMsgSerializer
 ***********************************************************************/

#include "PBEchoMsgSerializer.h"
#include "../dataobject/EchoMsgDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBEchoMsgSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBEchoMsgSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBEchoMsgSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	return *((data_buffer*)abstractDO.get());
}

////////////////////////////////////////////////////////////////////////
// Name:       PBEchoMsgSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBEchoMsgSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBEchoMsgSerializer::Deserialize(const data_buffer& rawdata)
{
	return std::make_shared<EchoMsgDO>(rawdata);
}