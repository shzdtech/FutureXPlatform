/***********************************************************************
 * Module:  EchoMessageSerializer.cpp
 * Author:  milk
 * Modified: 2014年10月8日 13:34:34
 * Purpose: Implementation of the class DebugPBSerializer
 ***********************************************************************/

#include <string>
#include <iostream>

#include "EchoMessageSerializer.h"
#include "../dataobject/EchoMessageDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       EchoMessageSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of EchoMessageSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer EchoMessageSerializer::Serialize(const dataobj_ptr& abstractDO)
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

dataobj_ptr EchoMessageSerializer::Deserialize(const data_buffer& rawdata)
{
    return dataobj_ptr(new EchoMessageDO(rawdata));
}