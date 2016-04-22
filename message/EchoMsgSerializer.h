/***********************************************************************
 * Module:  EchoMsgSerializer.h
 * Author:  milk
 * Modified: 2014年10月8日 13:34:34
 * Purpose: Declaration of the class EchoMsgSerializer
 ***********************************************************************/

#if !defined(__dataobject_EchoMsgSerializer_h)
#define __dataobject_EchoMsgSerializer_h
#include "../dataobject/IDataSerializer.h"
#include "../utility/singleton_templ.h"
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT EchoMsgSerializer : public IDataSerializer, public singleton_ptr<EchoMsgSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif