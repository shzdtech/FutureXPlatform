#if !defined(__pbserializer_PBStringMapSerializer_h)
#define __pbserializer_PBStringMapSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../dataobject/TemplateDO.h"
#include "pbserializer_exp.h"
#include "../utility/singleton_templ.h"

class PBSERIALIZER_CLASS_EXPORTS PBStringMapSerializer : public IDataSerializer, public singleton_ptr<PBStringMapSerializer>
{
public:
	dataobj_ptr Deserialize(const data_buffer& rawdata);

	data_buffer Serialize(const dataobj_ptr& abstractDO);
};

#endif