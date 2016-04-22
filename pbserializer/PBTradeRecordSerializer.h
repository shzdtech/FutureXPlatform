/***********************************************************************
 * Module:  PBTradeRecordSerializer.h
 * Author:  milk
 * Modified: 2015年7月12日 19:11:05
 * Purpose: Declaration of the class PBTradeRecordSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBTradeRecordSerializer_h)
#define __pbserializer_PBTradeRecordSerializer_h

#include "../dataobject/IDataSerializer.h"
#include "../utility/singleton_templ.h"

#include "pbserializer_exp.h"

class PBSERIALIZER_CLASS_EXPORTS PBTradeRecordSerializer : public IDataSerializer, public singleton_ptr<PBTradeRecordSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif