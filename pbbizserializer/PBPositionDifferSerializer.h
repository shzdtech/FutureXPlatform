/***********************************************************************
 * Module:  PBPositionDifferSerializer.h
 * Author:  milk
 * Modified: 2017年5月17日 19:11:24
 * Purpose: Declaration of the class PBPositionDifferSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBPositionDifferSerializer_h)
#define __pbserializer_PBPositionDifferSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"

#include "pbbizserializer_exp.h"

class PBBIZSERIALIZER_CLASS_EXPORTS PBPositionDifferSerializer : public IDataSerializer, public singleton_ptr<PBPositionDifferSerializer>
{
public:
   data_buffer Serialize(const dataobj_ptr& abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif