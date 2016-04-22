/***********************************************************************
 * Module:  PBInstrumentSerializer.h
 * Author:  milk
 * Modified: 2015年7月12日 19:10:41
 * Purpose: Declaration of the class PBInstrumentSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBInstrumentSerializer_h)
#define __pbserializer_PBInstrumentSerializer_h

#include "../dataobject/IDataSerializer.h"
#include "../utility/singleton_templ.h"

#include "pbserializer_exp.h"

class PBSERIALIZER_CLASS_EXPORTS PBInstrumentSerializer : public IDataSerializer, public singleton_ptr<PBInstrumentSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif