/***********************************************************************
 * Module:  PBSubMarketDataSerializer.h
 * Author:  milk
 * Modified: 2015年7月16日 22:23:16
 * Purpose: Declaration of the class PBSubMarketDataSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBSubMarketDataSerializer_h)
#define __pbserializer_PBSubMarketDataSerializer_h

#include "../dataobject/IDataSerializer.h"
#include "../utility/singleton_templ.h"
#include "pbbizserializer_exp.h"

class PBBIZSERIALIZER_CLASS_EXPORTS PBSubMarketDataSerializer : public IDataSerializer, public singleton_ptr<PBSubMarketDataSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif