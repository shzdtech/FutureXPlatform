/***********************************************************************
 * Module:  BizErrorSerializer.h
 * Author:  milk
 * Modified: 2014年10月22日 13:54:45
 * Purpose: Declaration of the class BizErrorSerializer
 ***********************************************************************/

#if !defined(__dataserializer_ExceptionSerializer_h)
#define __dataserializer_ExceptionSerializer_h

#include "dataserializer_exp.h"
#include "IDataSerializer.h"
#include "../dataobject/MessageExceptionDO.h"

class DATASERIALIZER_CLASS_EXPORT ExceptionSerializer : public IDataSerializer
{
public:
	static IDataSerializer_Ptr Instance(void);

protected:
private:
};

#endif