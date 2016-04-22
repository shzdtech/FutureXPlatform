/***********************************************************************
 * Module:  PBSubMarketDataSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月16日 22:23:16
 * Purpose: Implementation of the class PBSubMarketDataSerializer
 ***********************************************************************/

#include "PBSubMarketDataSerializer.h"
#include "PBStringTableSerializer.h"
#include "../dataobject/TemplateDO.h"
#include "proto/simpletable.pb.h"
////////////////////////////////////////////////////////////////////////
// Name:       PBSubMarketDataSerializer::Serialize(const dataobj_ptr abstractDO)
// Purpose:    Implementation of PBSubMarketDataSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

using namespace Micro::Future::Message::Business;

data_buffer PBSubMarketDataSerializer::Serialize(const dataobj_ptr abstractDO)
{
	StringResponse PB;
	auto pDO = (TMultiRecordDO<std::string>*)abstractDO.get();

	PB.set_eof(pDO->EOFFlag);
	PB.set_value(pDO->Data);

	int bufsize = PB.ByteSize();
	uint8_t* buf = new uint8_t[bufsize];
	PB.SerializeToArray(buf, bufsize);

	return data_buffer(buf, bufsize);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBSubMarketDataSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBSubMarketDataSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBSubMarketDataSerializer::Deserialize(const data_buffer& rawdata)
{
	return PBStringTableSerializer::Instance()->Deserialize(rawdata);
}