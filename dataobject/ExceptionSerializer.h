/***********************************************************************
 * Module:  BizErrorSerializer.h
 * Author:  milk
 * Modified: 2014年10月22日 13:54:45
 * Purpose: Declaration of the class BizErrorSerializer
 ***********************************************************************/

#if !defined(__dataobject_ExceptionSerializer_h)
#define __dataobject_ExceptionSerializer_h

#include "dataobject_exp.h"
#include "IDataSerializer.h"
#include "MessageExceptionDO.h"

class DATAOBJECT_CLASS_EXPORT ExceptionSerializer : public IDataSerializer
{
public:
	static IDataSerializer_Ptr Instance(void);

protected:
private:
};

#endif