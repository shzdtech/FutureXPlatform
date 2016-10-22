/***********************************************************************
 * Module:  PBUserAccountSerializer.h
 * Author:  milk
 * Modified: 2015年7月12日 19:11:15
 * Purpose: Declaration of the class PBUserAccountSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBUserAccountSerializer_h)
#define __pbserializer_PBUserAccountSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"

#include "pbbizserializer_exp.h"

class PBBIZSERIALIZER_CLASS_EXPORTS PBUserAccountSerializer : public IDataSerializer, public singleton_ptr<PBUserAccountSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr& abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif