/***********************************************************************
 * Module:  PBStringTableSerializer.cpp
 * Author:  milk
 * Modified: 2015年3月7日 10:49:23
 * Purpose: Implementation of the class PBStringTableSerializer
 ***********************************************************************/

#include "PBStringTableSerializer.h"
#include "../dataobject/TemplateDO.h"
#include "pbmacros.h"
#include <google/protobuf/message.h>
#include "../Protos/simpletable.pb.h"

using namespace Micro::Future::Message;

////////////////////////////////////////////////////////////////////////
// Name:       PBStringTableSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBStringTableSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBStringTableSerializer::Deserialize(const data_buffer& rawdata)
{
	SimpleStringTable simpleTbl;
	ParseWithReturn(simpleTbl, rawdata);

	auto nameTbl = new StringTableDO;
	dataobj_ptr ret(nameTbl);
	FillDOHeader(nameTbl, simpleTbl);

	if (simpleTbl.has_header())
	{
		nameTbl->SerialId = simpleTbl.header().serialid();
		nameTbl->HasMore = simpleTbl.header().hasmore();
	}
	for (auto& col : simpleTbl.columns()) {
		std::vector<std::string> vec(col.entry().begin(), col.entry().end());
		nameTbl->Data[col.name()] = std::move(vec);
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       PBStringTableSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBStringTableSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBStringTableSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	auto nameTbl = (StringTableDO*)abstractDO.get();
	SimpleStringTable simpleTbl;
	FillPBHeader(simpleTbl, nameTbl);

	for (auto& it : nameTbl->Data) {
		auto namedVec = simpleTbl.mutable_columns()->Add();
		namedVec->set_name(it.first);
		for (auto& val : it.second)
			namedVec->add_entry(val);
	}

	SerializeWithReturn(simpleTbl);
}