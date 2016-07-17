/***********************************************************************
 * Module:  EchoMsgSerializer.cpp
 * Author:  milk
 * Modified: 2014年10月8日 13:34:34
 * Purpose: Implementation of the class DebugPBSerializer
 ***********************************************************************/

#include <string>
#include <iostream>

#include "EchoMsgSerializer.h"
#include "../dataobject/EchoMsgDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       EchoMsgSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of EchoMsgSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer EchoMsgSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	return data_buffer(nullptr, 0);
}

////////////////////////////////////////////////////////////////////////
// Name:       EchoPBSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of DebugPBSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr EchoMsgSerializer::Deserialize(const data_buffer& rawdata)
{
    return dataobj_ptr(new EchoMsgDO(rawdata));
}