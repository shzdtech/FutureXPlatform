/***********************************************************************
 * Module:  PBPositionPnLSerializer.h
 * Author:  milk
 * Modified: 2018年1月12日 19:11:34
 * Purpose: Declaration of the class PBPositionPnLSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBPositionPnLSerializer_h)
#define __pbserializer_PBPositionPnLSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"

#include "pbbizserializer_exp.h"

class PBBIZSERIALIZER_CLASS_EXPORTS PBPositionPnLSerializer : public IDataSerializer, public singleton_ptr<PBPositionPnLSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr& abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif