/***********************************************************************
 * Module:  PBComplexTableSerializer.h
 * Author:  milk
 * Modified: 2014年10月22日 11:25:48
 * Purpose: Declaration of the class PBComplexTableSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBComplexTableSerializer_h)
#define __pbserializer_PBComplexTableSerializer_h

#include "../dataobject/IDataSerializer.h"
#include "pbserializer_exp.h"
#include "../utility/singleton_templ.h"

class PBSERIALIZER_CLASS_EXPORTS PBComplexTableSerializer : public IDataSerializer, public singleton_ptr<PBComplexTableSerializer>
{
public:
   dataobj_ptr Deserialize(const data_buffer& rawdata);
   data_buffer Serialize(const dataobj_ptr abstractDO);

protected:
private:

};

#endif