/***********************************************************************
 * Module:  PBOrderSerializer.h
 * Author:  milk
 * Modified: 2015年7月12日 19:10:55
 * Purpose: Declaration of the class PBOrderSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBOrderSerializer_h)
#define __pbserializer_PBOrderSerializer_h

#include "../dataobject/IDataSerializer.h"
#include "../utility/singleton_templ.h"

#include "pbserializer_exp.h"

class PBSERIALIZER_CLASS_EXPORTS PBOrderSerializer : public IDataSerializer, public singleton_ptr<PBOrderSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif