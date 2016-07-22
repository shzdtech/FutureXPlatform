/***********************************************************************
 * Module:  PBResultSerializer.h
 * Author:  milk
 * Modified: 2015年8月23日 21:57:11
 * Purpose: Declaration of the class PBResultSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBResultSerializer_h)
#define __pbserializer_PBResultSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"
#include "pbserializer_exp.h"

class PBSERIALIZER_CLASS_EXPORTS PBResultSerializer : public IDataSerializer, public singleton_ptr<PBResultSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr& abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif