/***********************************************************************
 * Module:  PBUserPositionSerializer.h
 * Author:  milk
 * Modified: 2015年7月12日 19:11:34
 * Purpose: Declaration of the class PBUserPositionSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBUserPositionSerializer_h)
#define __pbserializer_PBUserPositionSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"

#include "pbbizserializer_exp.h"

class PBBIZSERIALIZER_CLASS_EXPORTS PBUserPositionSerializer : public IDataSerializer, public singleton_ptr<PBUserPositionSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr& abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif