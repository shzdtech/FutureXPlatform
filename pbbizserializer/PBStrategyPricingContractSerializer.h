#if !defined(__pbserializer_PBStrategyPricingContractSerializer_h)
#define __pbserializer_PBStrategyPricingContractSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"
#include "pbbizserializer_exp.h"

class PBStrategyPricingContractSerializer : public IDataSerializer, public singleton_ptr<PBStrategyPricingContractSerializer>
{
public:
	data_buffer Serialize(const dataobj_ptr& abstractDO);
	dataobj_ptr Deserialize(const data_buffer& rawdata);
};

#endif