/***********************************************************************
 * Module:  PBExceptionSerializer.h
 * Author:  milk
 * Modified: 2014年10月22日 11:25:39
 * Purpose: Declaration of the class PBExceptionSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBExceptionSerializer_h)
#define __pbserializer_PBExceptionSerializer_h

#include "../dataobject/IDataSerializer.h"
#include "../dataobject/MessageExceptionDO.h"
#include "../utility/singleton_templ.h"
#include "pbserializer_exp.h"

class PBSERIALIZER_CLASS_EXPORTS PBExceptionSerializer : public IDataSerializer, public singleton_ptr<PBExceptionSerializer>
{
public:
	dataobj_ptr Deserialize(const data_buffer& rawdata);
	data_buffer Serialize(const dataobj_ptr abstractDO);

protected:
private:

};

#endif