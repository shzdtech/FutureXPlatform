/***********************************************************************
 * Module:  PBContractParamSerializer.h
 * Author:  milk
 * Modified: 2015年8月23日 13:29:16
 * Purpose: Declaration of the class PBContractParamSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBContractParamSerializer_h)
#define __pbserializer_PBContractParamSerializer_h

#include "../dataobject/IDataSerializer.h"
#include "../utility/singleton_templ.h"
#include "pbbizserializer_exp.h"

class PBBIZSERIALIZER_CLASS_EXPORTS PBContractParamSerializer : public IDataSerializer, public singleton_ptr<PBContractParamSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr& abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif