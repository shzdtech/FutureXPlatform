/***********************************************************************
 * Module:  PBContractInfoSerializer.h
 * Author:  milk
 * Modified: 2015年7月12日 19:10:41
 * Purpose: Declaration of the class PBContractInfoSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBContractInfoSerializer_h)
#define __pbserializer_PBContractInfoSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"

#include "pbbizserializer_exp.h"

class PBBIZSERIALIZER_CLASS_EXPORTS PBContractInfoSerializer : public IDataSerializer, public singleton_ptr<PBContractInfoSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr& abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif