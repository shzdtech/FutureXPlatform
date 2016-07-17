/***********************************************************************
 * Module:  PBIntTableSerializer.h
 * Author:  milk
 * Modified: 2015年3月7日 10:49:02
 * Purpose: Declaration of the class PBIntTableSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBIntTableSerializer_h)
#define __pbserializer_PBIntTableSerializer_h

#include "../dataobject/IDataSerializer.h"
#include "pbserializer_exp.h"
#include "../utility/singleton_templ.h"

class PBSERIALIZER_CLASS_EXPORTS PBIntTableSerializer : public IDataSerializer, public singleton_ptr<PBIntTableSerializer>
{
public:
   dataobj_ptr Deserialize(const data_buffer& rawdata);
   data_buffer Serialize(const dataobj_ptr& abstractDO);

protected:
private:

};

#endif