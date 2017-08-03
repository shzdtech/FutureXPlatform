#if !defined(__pbserializer_PBValuationRiskSerializer_h)
#define __pbserializer_PBValuationRiskSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"
#include "pbbizserializer_exp.h"

class PBBIZSERIALIZER_CLASS_EXPORTS PBValuationRiskSerializer : public IDataSerializer, public singleton_ptr<PBValuationRiskSerializer>
{
public:
	data_buffer Serialize(const dataobj_ptr& abstractDO);
	dataobj_ptr Deserialize(const data_buffer& rawdata);
};

#endif

