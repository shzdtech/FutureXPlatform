#include "PBStringMapSerializer.h"
#include "../Protos/commondefine.pb.h"
#include "pbmacros.h"

using namespace Micro::Future::Message;

dataobj_ptr PBStringMapSerializer::Deserialize(const data_buffer & rawdata)
{
	StringMap PBMap;
	ParseWithReturn(PBMap, rawdata);

	auto ret = std::make_shared<MapDO<std::string>>(PBMap.entry().begin(), PBMap.entry().end());
	FillDOHeader(ret, PBMap);

	return ret;
}

data_buffer PBStringMapSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	auto pMapDO = (MapDO<std::string>*)abstractDO.get();
	StringMap PBMap;
	FillPBHeader(PBMap, pMapDO);

	auto pEntry = PBMap.mutable_entry();
	pEntry->insert(pMapDO->begin(), pMapDO->end());

	SerializeWithReturn(PBMap);
}
