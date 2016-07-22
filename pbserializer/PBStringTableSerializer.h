/***********************************************************************
 * Module:  PBStringTableSerializer.h
 * Author:  milk
 * Modified: 2015年3月7日 10:49:23
 * Purpose: Declaration of the class PBStringTableSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBStringTableSerializer_h)
#define __pbserializer_PBStringTableSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "pbserializer_exp.h"
#include "../utility/singleton_templ.h"

class PBSERIALIZER_CLASS_EXPORTS PBStringTableSerializer : public IDataSerializer, public singleton_ptr<PBStringTableSerializer>
{
public:
   dataobj_ptr Deserialize(const data_buffer& rawdata);
   data_buffer Serialize(const dataobj_ptr& abstractDO);

protected:
private:

};

#endif