/***********************************************************************
* Module:  PBModelParamsSerializer.cpp
* Author:  milk
* Modified: 2014年10月19日 20:49:41
* Purpose: Implementation of the class PBModelParamsSerializer
***********************************************************************/

#include "pbmacros.h"
#include "PBModelParamsSerializer.h"
#include "../Protos/modelalgorithm.pb.h"
#include "../dataobject/ModelParamsDO.h"

using namespace Micro::Future::Message;

////////////////////////////////////////////////////////////////////////
// Name:       PBModelParamsSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBModelParamsSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBModelParamsSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	auto pDO = (ModelParamsDO*)abstractDO.get();
	ModelParams pb;
	FillPBHeader(pb, pDO);

	pb.set_instancename(pDO->InstanceName);
	pb.set_model(pDO->Model);
	pb.set_modelaim(pDO->ModelAim);

	pb.mutable_params()->insert(pDO->Params.begin(), pDO->Params.end());

	SerializeWithReturn(pb);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBModelParamsSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBModelParamsSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBModelParamsSerializer::Deserialize(const data_buffer& rawdata)
{
	ModelParams pb;
	ParseWithReturn(pb, rawdata)

	auto pDO = new ModelParamsDO;
	FillDOHeader(pDO, pb);

	dataobj_ptr ret(pDO);
	pDO->InstanceName = pb.instancename();
	pDO->Model = pb.model();	
	pDO->ModelAim = pb.modelaim();

	pDO->Params.insert(pb.params().begin(), pb.params().end());

	return ret;
}