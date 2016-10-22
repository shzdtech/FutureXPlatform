#if !defined(__pbserializer_PBBankOperationSerializer_h)
#define __pbserializer_PBBankOperationSerializer_h

#include "../dataserializer/IDataSerializer.h"
#include "../utility/singleton_templ.h"

#include "pbbizserializer_exp.h"

class PBBIZSERIALIZER_CLASS_EXPORTS PBBankOperationSerializer : public IDataSerializer, public singleton_ptr<PBBankOperationSerializer>
{
public:
	data_buffer Serialize(const dataobj_ptr& abstractDO);
	dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif