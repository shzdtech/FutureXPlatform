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
	pb.set_serialid(pDO->SerialId);
	pb.set_modelname(pDO->ModelName);

	for (double val : pDO->Values)
		pb.mutable_values()->Add(val);

	pb.mutable_scalaparams()->insert(pDO->ScalaParams.begin(), pDO->ScalaParams.end());

	for(auto& pair : pDO->VectorParams)
	{ 
		ModelParams_double_vector double_vector;
		for(double val : pair.second)
			double_vector.add_entry(val);

		(*pb.mutable_vectorparams())[pair.first] = double_vector;
	}

	int bufsize = pb.ByteSize();
	uint8_t* buf = new uint8_t[bufsize];
	pb.SerializeToArray(buf, bufsize);

	return data_buffer(buf, bufsize);
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
	dataobj_ptr ret(pDO);
	pDO->SerialId = pb.serialid();
	pDO->ModelName = pb.modelname();	

	pDO->Values.assign(pb.values().begin(), pb.values().end());

	pDO->ScalaParams.insert(pb.scalaparams().begin(), pb.scalaparams().end());

	auto& vectorParams = pb.vectorparams();
	for (auto& pair : vectorParams)
	{
		std::vector<double> double_vec(pair.second.entry().begin(),
			pair.second.entry().end());
		pDO->VectorParams.emplace(pair.first, double_vec);
	}

	return ret;
}