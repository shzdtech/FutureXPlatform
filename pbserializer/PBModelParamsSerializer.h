/***********************************************************************
 * Module:  PBModelParamsSerializer.h
 * Author:  milk
 * Modified: 2016年7月23日 11:25:48
 * Purpose: Declaration of the class PBModelParamsSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBModelParamsSerializer_h)
#define __pbserializer_PBModelParamsSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "pbserializer_exp.h"
#include "../utility/singleton_templ.h"

class PBSERIALIZER_CLASS_EXPORTS PBModelParamsSerializer : public IDataSerializer, public singleton_ptr<PBModelParamsSerializer>
{
public:
   dataobj_ptr Deserialize(const data_buffer& rawdata);
   data_buffer Serialize(const dataobj_ptr& abstractDO);

protected:
private:

};

#endif