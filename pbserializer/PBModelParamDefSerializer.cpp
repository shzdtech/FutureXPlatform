/***********************************************************************
* Module:  PBModelParamDefSerializer.cpp
* Author:  milk
* Modified: 2014年10月19日 20:49:41
* Purpose: Implementation of the class PBModelParamDefSerializer
***********************************************************************/

#include "pbmacros.h"
#include "PBModelParamDefSerializer.h"
#include "PBStringMapSerializer.h"
#include "../Protos/modelalgorithm.pb.h"
#include "../dataobject/ModelParamDefDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBModelParamDefSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBModelParamDefSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBModelParamDefSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	auto pDO = (ModelParamDefDO*)abstractDO.get();
	Micro::Future::Message::ModelParamDef pb;
	FillPBHeader(pb, pDO);

	pb.set_modelname(pDO->ModelName);
	auto& params = *pb.mutable_params();

	for (auto pair : pDO->ModelDefMap)
	{
		Micro::Future::Message::ModelDef modelDef;
		modelDef.set_datatype(pair.second.DataType);
		modelDef.set_defaultval(pair.second.DefaultVal);
		modelDef.set_digits(pair.second.Digits);
		modelDef.set_enable(pair.second.Enable);
		modelDef.set_maxval(pair.second.MaxVal);
		modelDef.set_minval(pair.second.MinVal);
		modelDef.set_step(pair.second.Step);
		modelDef.set_stringval(pair.second.StringVal);
		modelDef.set_visible(pair.second.Visible);

		params[pair.first] = modelDef;
	}

	SerializeWithReturn(pb);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBModelParamDefSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBModelParamDefSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBModelParamDefSerializer::Deserialize(const data_buffer& rawdata)
{
	return PBStringMapSerializer::Instance()->Deserialize(rawdata);
}