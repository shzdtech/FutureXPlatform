/***********************************************************************
 * Module:  PBExchangeSerializer.h
 * Author:  milk
 * Modified: 2015年7月12日 19:11:24
 * Purpose: Declaration of the class PBExchangeSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBExchangeSerializer_h)
#define __pbserializer_PBExchangeSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"

#include "pbbizserializer_exp.h"

class PBBIZSERIALIZER_CLASS_EXPORTS PBExchangeSerializer : public IDataSerializer, public singleton_ptr<PBExchangeSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr& abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif