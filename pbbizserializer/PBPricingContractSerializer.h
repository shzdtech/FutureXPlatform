#if !defined(__pbserializer_PBPricingContractSerializer_h)
#define __pbserializer_PBPricingContractSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"
#include "pbbizserializer_exp.h"

class PBPricingContractSerializer : public IDataSerializer, public singleton_ptr<PBPricingContractSerializer>
{
public:
	data_buffer Serialize(const dataobj_ptr& abstractDO);
	dataobj_ptr Deserialize(const data_buffer& rawdata);
};

#endif