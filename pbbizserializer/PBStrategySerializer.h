/***********************************************************************
 * Module:  PBStrategySerializer.h
 * Author:  milk
 * Modified: 2015年8月23日 12:31:07
 * Purpose: Declaration of the class PBStrategySerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBStrategySerializer_h)
#define __pbserializer_PBStrategySerializer_h

#include "../dataobject/IDataSerializer.h"
#include "../utility/singleton_templ.h"
#include "pbbizserializer_exp.h"

class PBBIZSERIALIZER_CLASS_EXPORTS PBStrategySerializer : public IDataSerializer, public singleton_ptr<PBStrategySerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif