/***********************************************************************
 * Module:  AbstractConfigReaderFactory.h
 * Author:  milk
 * Modified: 2014年10月1日 13:20:22
 * Purpose: Declaration of the class AbstractConfigReaderFactory
 ***********************************************************************/

#if !defined(__configuration_AbstractConfigReaderFactory_h)
#define __configuration_AbstractConfigReaderFactory_h

#include "IConfigReader.h"
#include "configuration_exp.h"

class CONFIGURATION_CLASS_EXPORT AbstractConfigReaderFactory
{
public:
   static IConfigReader_Ptr CreateConfigReader(void);

protected:
private:

};

#endif