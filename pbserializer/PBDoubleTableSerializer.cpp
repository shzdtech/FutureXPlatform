/***********************************************************************
 * Module:  PBDoubleTableSerializer.cpp
 * Author:  milk
 * Modified: 2015年3月7日 10:49:13
 * Purpose: Implementation of the class PBDoubleTableSerializer
 ***********************************************************************/

#include "PBDoubleTableSerializer.h"
#include "pbmacros.h"
#include "../dataobject/TemplateDO.h"
#include "../Protos/simpletable.pb.h"

using namespace Micro::Future::Message;
////////////////////////////////////////////////////////////////////////
// Name:       PBDoubleTableSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBDoubleTableSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBDoubleTableSerializer::Deserialize(const data_buffer& rawdata)
{
	SimpleDoubleTable simpleTbl;
	ParseWithThrow(simpleTbl, rawdata)
	
	auto nameTbl = new DoubleTableDO;
	dataobj_ptr ret(nameTbl);
	FillDOHeader(nameTbl, simpleTbl)

	for (auto& col : simpleTbl.columns()) {
		std::vector<double> vec(col.entry().begin(), col.entry().end());
		nameTbl->Data[col.name()] = std::move(vec);
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       PBDoubleTableSerializer::Serialize(const dataobj_ptr abstractDO)
// Purpose:    Implementation of PBDoubleTableSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBDoubleTableSerializer::Serialize(const dataobj_ptr abstractDO)
{
	auto nameTbl = (DoubleTableDO*)abstractDO.get();
	SimpleDoubleTable simpleTbl;
	FillPBHeader(simpleTbl, nameTbl)

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