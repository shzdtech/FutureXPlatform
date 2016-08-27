#if !defined(__pbserializer_PBPortfolioSerializer_h)
#define __pbserializer_PBPortfolioSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"
#include "pbbizserializer_exp.h"

class PBBIZSERIALIZER_CLASS_EXPORTS PBPortfolioSerializer : public IDataSerializer, public singleton_ptr<PBPortfolioSerializer>
{
public:
	data_buffer Serialize(const dataobj_ptr& abstractDO);
	dataobj_ptr Deserialize(const data_buffer& rawdata);
};

#endif

