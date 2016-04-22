/***********************************************************************
 * Module:  PBUserInfoListSerializer.h
 * Author:  milk
 * Modified: 2015年8月27日 20:21:02
 * Purpose: Declaration of the class PBUserInfoListSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBUserInfoListSerializer_h)
#define __pbserializer_PBUserInfoListSerializer_h

#include "../dataobject/IDataSerializer.h"
#include "pbserializer_exp.h"
#include "../utility/singleton_templ.h"

class PBSERIALIZER_CLASS_EXPORTS PBUserInfoListSerializer : public IDataSerializer, public singleton_ptr<PBUserInfoListSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif