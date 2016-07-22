/***********************************************************************
 * Module:  PBEchoMessageSerializer.h
 * Author:  milk
 * Modified: 2015年9月18日 12:09:07
 * Purpose: Declaration of the class PBEchoMessageSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBEchoMessageSerializer_h)
#define __pbserializer_PBEchoMessageSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"
#include "pbserializer_exp.h"

class PBSERIALIZER_CLASS_EXPORTS PBEchoMessageSerializer : public IDataSerializer, public singleton_ptr<PBEchoMessageSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr& abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif