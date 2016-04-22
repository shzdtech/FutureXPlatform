/***********************************************************************
 * Module:  PBUserParamSerializer.h
 * Author:  milk
 * Modified: 2015年8月23日 14:05:30
 * Purpose: Declaration of the class PBUserParamSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBUserParamSerializer_h)
#define __pbserializer_PBUserParamSerializer_h

#include "../dataobject/IDataSerializer.h"
#include "../utility/singleton_templ.h"

#include "pbserializer_exp.h"

class PBSERIALIZER_CLASS_EXPORTS PBUserParamSerializer : public IDataSerializer, public singleton_ptr<PBUserParamSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif