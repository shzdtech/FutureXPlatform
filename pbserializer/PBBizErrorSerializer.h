/***********************************************************************
 * Module:  PBBizErrorSerializer.h
 * Author:  milk
 * Modified: 2014年10月22日 11:25:39
 * Purpose: Declaration of the class PBBizErrorSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBBizErrorSerializer_h)
#define __pbserializer_PBBizErrorSerializer_h

#include "../dataobject/IDataSerializer.h"
#include "../dataobject/BizErrorDO.h"
#include "../utility/singleton_templ.h"
#include "pbserializer_exp.h"

class PBSERIALIZER_CLASS_EXPORTS PBBizErrorSerializer : public IDataSerializer, public singleton_ptr<PBBizErrorSerializer>
{
public:
	dataobj_ptr Deserialize(const data_buffer& rawdata);
	data_buffer Serialize(const dataobj_ptr abstractDO);

protected:
private:

};

#endif