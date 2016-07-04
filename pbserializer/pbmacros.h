#ifndef __pbserializer_pbmacro_h
#define __pbserializer_pbmacro_h

#include "../Protos/commondefine.pb.h"
#include "../message/BizError.h"
#include "ExceptionDef.h"

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

#define ParseWithThrow(PbO, rawdata) \
if (!PbO.ParseFromArray(rawdata.get(), rawdata.size())) \
	throw BizError(INVALID_DATAFORMAT_CODE, INVALID_DATAFORMAT_DESC); \

#endif