/***********************************************************************
 * Module:  PBMarketDataSerializer.h
 * Author:  milk
 * Modified: 2015年7月10日 23:34:22
 * Purpose: Declaration of the class PBMarketDataSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBMarketDataSerializer_h)
#define __pbserializer_PBMarketDataSerializer_h

#include "../dataobject/IDataSerializer.h"
#include "../utility/singleton_templ.h"

#include "pbserializer_exp.h"

class PBSERIALIZER_CLASS_EXPORTS PBMarketDataSerializer : public IDataSerializer, public singleton_ptr<PBMarketDataSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif