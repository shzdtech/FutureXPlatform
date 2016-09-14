/***********************************************************************
 * Module:  PBTradingDeskParamsSerializer.h
 * Author:  milk
 * Modified: 2015年7月10日 23:34:22
 * Purpose: Declaration of the class PBTradingDeskParamsSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBTradingDeskParamsSerializer_h)
#define __pbserializer_PBTradingDeskParamsSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"

#include "pbbizserializer_exp.h"

class PBBIZSERIALIZER_CLASS_EXPORTS PBTradingDeskParamsSerializer : public IDataSerializer, public singleton_ptr<PBTradingDeskParamsSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr& abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif