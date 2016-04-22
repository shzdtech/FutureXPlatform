/***********************************************************************
 * Module:  PBEchoMsgSerializer.h
 * Author:  milk
 * Modified: 2015年9月18日 12:09:07
 * Purpose: Declaration of the class PBEchoMsgSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBEchoMsgSerializer_h)
#define __pbserializer_PBEchoMsgSerializer_h

#include "../dataobject/IDataSerializer.h"
#include "../utility/singleton_templ.h"
#include "pbserializer_exp.h"

class PBSERIALIZER_CLASS_EXPORTS PBEchoMsgSerializer : public IDataSerializer, public singleton_ptr<PBEchoMsgSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif