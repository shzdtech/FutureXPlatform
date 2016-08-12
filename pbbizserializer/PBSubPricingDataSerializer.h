/***********************************************************************
 * Module:  PBSubPricingDataSerializer.h
 * Author:  milk
 * Modified: 2015年8月21日 12:08:11
 * Purpose: Declaration of the class PBPricingDataSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBSubPricingDataSerializer_h)
#define __pbserializer_PBSubPricingDataSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"

#include "pbbizserializer_exp.h"

class PBBIZSERIALIZER_CLASS_EXPORTS PBSubPricingDataSerializer : public IDataSerializer, public singleton_ptr<PBSubPricingDataSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr& abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif