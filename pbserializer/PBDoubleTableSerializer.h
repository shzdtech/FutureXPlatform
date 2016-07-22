/***********************************************************************
 * Module:  PBDoubleTableSerializer.h
 * Author:  milk
 * Modified: 2015年3月7日 10:49:13
 * Purpose: Declaration of the class PBDoubleTableSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBDoubleTableSerializer_h)
#define __pbserializer_PBDoubleTableSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "pbserializer_exp.h"
#include "../utility/singleton_templ.h"

class PBSERIALIZER_CLASS_EXPORTS PBDoubleTableSerializer : public IDataSerializer, public singleton_ptr<PBDoubleTableSerializer>
{
public:
   dataobj_ptr Deserialize(const data_buffer& rawdata);
   data_buffer Serialize(const dataobj_ptr& abstractDO);

protected:
private:

};

#endif