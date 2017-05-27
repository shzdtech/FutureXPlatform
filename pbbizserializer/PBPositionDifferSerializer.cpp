/***********************************************************************
 * Module:  PBPositionDifferSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月12日 19:11:24
 * Purpose: Implementation of the class PBPositionDifferSerializer
 ***********************************************************************/

#include "PBPositionDifferSerializer.h"
#include "../pbserializer/PBStringMapSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../Protos/businessobj.pb.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/UserPositionDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBPositionDifferSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBPositionDifferSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBPositionDifferSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	using namespace Micro::Future::Message::Business;

	PBPositionCompareList PB;
	auto pDO = (MapDO<std::pair<std::string, PositionDirectionType>, std::pair<int, int>>*)abstractDO.get();
	FillPBHeader(PB, pDO);

	for (auto& pair : *pDO)
	{
		auto pPosition = PB.add_positions();
		pPosition->set_contract(pair.first.first);
		pPosition->set_direction(pair.first.second);
		pPosition->set_dbposition(pair.second.first);
		pPosition->set_sysposition(pair.second.second);
	}

	SerializeWithReturn(PB);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBPositionDifferSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBPositionDifferSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBPositionDifferSerializer::Deserialize(const data_buffer& rawdata)
{
	return PBStringMapSerializer::Instance()->Deserialize(rawdata);
}