/***********************************************************************
* Module:  PBComplexTableSerializer.cpp
* Author:  milk
* Modified: 2014年10月19日 20:49:41
* Purpose: Implementation of the class PBComplexTableSerializer
***********************************************************************/

#include "../dataobject/ComplexTableDO.h"
#include "pbmacros.h"
#include "PBComplexTableSerializer.h"
#include "../Protos/simpletable.pb.h"

using namespace Micro::Future::Message;

void fillTable(ComplexTableDO* ctDO, ComplexTable* ct)
{
	for (auto& it : ctDO->intColumns) {
		auto namedVec = ct->mutable_inttable()->mutable_columns()->Add();
		namedVec->set_name(it.first);
		for (auto& val : it.second)
			namedVec->add_entry(val);
	}

	for (auto& it : ctDO->doubleColumns) {
		auto namedVec = ct->mutable_doubletable()->mutable_columns()->Add();
		namedVec->set_name(it.first);
		for (auto& val : it.second)
			namedVec->add_entry(val);
	}

	for (auto& it : ctDO->stringColumns) {
		auto namedVec = ct->mutable_stringtable()->mutable_columns()->Add();
		namedVec->set_name(it.first);
		for (auto& val : it.second)
			namedVec->add_entry(val);
	}
}
////////////////////////////////////////////////////////////////////////
// Name:       PBComplexTableSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBComplexTableSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBComplexTableSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	auto ctDO = (ComplexTableDO*)abstractDO.get();
	ComplexTable ct;
	FillPBHeader(ct, ctDO)

	fillTable(ctDO, &ct);
	ComplexTableDO* nestDO = ctDO;
	ComplexTable* nestTbl = &ct;
	while (nestDO->NestedTable){
		nestDO = nestDO->NestedTable.get();
		nestTbl = nestTbl->mutable_nestedtable();
		fillTable(nestDO, nestTbl);
	}

	int bufsize = ct.ByteSize();
	uint8_t* buf = new uint8_t[bufsize];
	ct.SerializeToArray(buf, bufsize);

	return data_buffer(buf, bufsize);
}

void fillDO(ComplexTable& ct, ComplexTableDO* ctDO)
{
	if (ct.has_inttable())
		for (auto& col : ct.inttable().columns()) {
		std::vector<int> vec(col.entry().begin(), col.entry().end());
		ctDO->intColumns[col.name()] = std::move(vec);
		}

	if (ct.has_doubletable())
		for (auto& col : ct.doubletable().columns()) {
		std::vector<double> vec(col.entry().begin(), col.entry().end());
		ctDO->doubleColumns[col.name()] = std::move(vec);
		}

	if (ct.has_stringtable())
		for (auto& col : ct.stringtable().columns()) {
		std::vector<std::string> vec(col.entry().begin(), col.entry().end());
		ctDO->stringColumns[col.name()] = std::move(vec);
		}
}
////////////////////////////////////////////////////////////////////////
// Name:       PBComplexTableSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBComplexTableSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBComplexTableSerializer::Deserialize(const data_buffer& rawdata)
{
	ComplexTable ct;
	ParseWithThrow(ct, rawdata);

	auto ctDO = new ComplexTableDO;
	dataobj_ptr ret(ctDO);
	FillDOHeader(ctDO, ct);

	fillDO(ct, ctDO);
	ComplexTableDO* nestDO = ctDO;
	ComplexTable &nestTbl = ct;
	while (nestTbl.has_nestedtable()){
		nestTbl = nestTbl.nestedtable();
		nestDO = nestDO->NestedTable.get();
		fillDO(nestTbl, nestDO);
	}

	return ret;
}