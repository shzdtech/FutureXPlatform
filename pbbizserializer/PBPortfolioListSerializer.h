#if !defined(__pbserializer_PBPortfolioListSerializer_h)
#define __pbserializer_PBPortfolioListSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"
#include "pbbizserializer_exp.h"

class PBBIZSERIALIZER_CLASS_EXPORTS PBPortfolioListSerializer : public IDataSerializer, public singleton_ptr<PBPortfolioListSerializer>
{
public:
	data_buffer Serialize(const dataobj_ptr& abstractDO);
	dataobj_ptr Deserialize(const data_buffer& rawdata);
};

#endif

