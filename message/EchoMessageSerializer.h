/***********************************************************************
 * Module:  EchoMessageSerializer.h
 * Author:  milk
 * Modified: 2014年10月8日 13:34:34
 * Purpose: Declaration of the class EchoMessageSerializer
 ***********************************************************************/

#if !defined(__dataserializer_EchoMessageSerializer_h)
#define __dataserializer_EchoMessageSerializer_h
#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT EchoMessageSerializer : public IDataSerializer, public singleton_ptr<EchoMessageSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr& abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif