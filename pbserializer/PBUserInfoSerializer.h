/***********************************************************************
 * Module:  PBUserInfoSerializer.h
 * Author:  milk
 * Modified: 2015年8月23日 23:23:10
 * Purpose: Declaration of the class PBUserInfoSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBUserInfoSerializer_h)
#define __pbserializer_PBUserInfoSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"

#include "pbserializer_exp.h"

class PBSERIALIZER_CLASS_EXPORTS PBUserInfoSerializer : public IDataSerializer, public singleton_ptr<PBUserInfoSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr& abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif