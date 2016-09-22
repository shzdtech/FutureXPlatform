#ifndef __pbserializer_pbmacro_h
#define __pbserializer_pbmacro_h

#include "../Protos/commondefine.pb.h"
#include "../message/BizError.h"
#include "ExceptionDef.h"
#include "../dataobject/data_buffer.h"

#define FillPBHeader(PbO, pDO)\
if (pDO->SerialId != 0) { \
	auto pHeader = new Micro::Future::Message::DataHeader(); \
	pHeader->set_serialid(pDO->SerialId); \
	if (pDO->HasMore) \
		pHeader->set_hasmore(pDO->HasMore); \
	PbO.set_allocated_header(pHeader); }

#define FillDOHeader(pDO, PbO) \
if (PbO.has_header()) { \
	pDO->SerialId = PbO.header().serialid(); \
	pDO->HasMore = PbO.header().hasmore(); } \

#define ParseWithReturn(PbO, rawdata) \
if (!PbO.ParseFromArray(rawdata.get(), rawdata.size())) \
	return nullptr;

#define SerializeWithReturn(PbO) \
int bufSz = PbO.ByteSize(); \
auto buffer = new byte[bufSz]; \
if(PbO.SerializePartialToArray(buffer, bufSz)) return data_buffer(buffer, bufSz); \
else { delete []buffer; return data_buffer(); }

#endif