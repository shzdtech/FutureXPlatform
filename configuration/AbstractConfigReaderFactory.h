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
#include <vector>


static const char* XMLCONFIG_FORMAT = "xml";
static const char* JSONCONFIG_FORMAT = "json";

class CONFIGURATION_CLASS_EXPORT AbstractConfigReaderFactory
{
public:
	static const std::vector<std::string>& SupportConfigFormats();
	static IConfigReader_Ptr CreateConfigReader(const char* type);
	static IConfigReader_Ptr CreateConfigReader();
	static IConfigReader_Ptr OpenConfigReader(const char* configPath);

	static std::string DEFAULT_CONFIG_FORMAT;

protected:

private:

};


#endif