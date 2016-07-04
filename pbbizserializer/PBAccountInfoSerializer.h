/***********************************************************************
 * Module:  PBAccountInfoSerializer.h
 * Author:  milk
 * Modified: 2015年7月12日 19:11:15
 * Purpose: Declaration of the class PBAccountInfoSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBAccountInfoSerializer_h)
#define __pbserializer_PBAccountInfoSerializer_h

#include "../dataobject/IDataSerializer.h"
#include "../utility/singleton_templ.h"

#include "pbbizserializer_exp.h"

class PBBIZSERIALIZER_CLASS_EXPORTS PBAccountInfoSerializer : public IDataSerializer, public singleton_ptr<PBAccountInfoSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif