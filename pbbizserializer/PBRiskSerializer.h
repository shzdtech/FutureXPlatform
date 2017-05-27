#if !defined(__pbserializer_PBRiskSerializer_h)
#define __pbserializer_PBRiskSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"
#include "pbbizserializer_exp.h"

class PBBIZSERIALIZER_CLASS_EXPORTS PBRiskSerializer : public IDataSerializer, public singleton_ptr<PBRiskSerializer>
{
public:
	data_buffer Serialize(const dataobj_ptr& abstractDO);
	dataobj_ptr Deserialize(const data_buffer& rawdata);
};

#endif

