/***********************************************************************
 * Module:  PBIntTableSerializer.cpp
 * Author:  milk
 * Modified: 2015年3月7日 10:49:02
 * Purpose: Implementation of the class PBIntTableSerializer
 ***********************************************************************/

#include "PBIntTableSerializer.h"
#include "../dataobject/TemplateDO.h"
#include "pbmacros.h"
#include "../Protos/simpletable.pb.h"

using namespace Micro::Future::Message;
////////////////////////////////////////////////////////////////////////
// Name:       PBIntTableSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBIntTableSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBIntTableSerializer::Deserialize(const data_buffer& rawdata)
{
	SimpleIntTable simpleTbl;
	ParseWithThrow(simpleTbl, rawdata);

	auto nameTbl = new IntTableDO;
	dataobj_ptr ret(nameTbl);
	FillDOHeader(nameTbl, simpleTbl);

	for (auto& col : simpleTbl.columns()) {
		std::vector<int> vec(col.entry().begin(), col.entry().end());
		nameTbl->Data[col.name()] = std::move(vec);
	}
	return dataobj_ptr(nameTbl);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBIntTableSerializer::Serialize(const dataobj_ptr abstractDO)
// Purpose:    Implementation of PBIntTableSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBIntTableSerializer::Serialize(const dataobj_ptr abstractDO)
{
	auto nameTbl = (IntTableDO*)abstractDO.get();
	SimpleIntTable simpleTbl;
	FillPBHeader(simpleTbl, nameTbl);

	for (auto& it : nameTbl->Data) {
		auto namedVec = simpleTbl.mutable_columns()->Add();
		namedVec->set_name(it.first);
		for (auto& val : it.second)
			namedVec->add_entry(val);
	}

	int bufsize = simpleTbl.ByteSize();
	uint8_t* buf = new uint8_t[bufsize];
	simpleTbl.SerializeToArray(buf, bufsize);

	return data_buffer(buf, bufsize);
}