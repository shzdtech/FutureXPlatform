/***********************************************************************
 * Module:  AbstractConfigReaderFactory.cpp
 * Author:  milk
 * Modified: 2014年10月1日 13:20:22
 * Purpose: Implementation of the class AbstractConfigReaderFactory
 ***********************************************************************/

#include "AbstractConfigReaderFactory.h"
#include <fstream>

#include "XMLConfigReader.h"
#include "JsonConfigReader.h"

#include "../utility/stringutility.h"

static const std::vector<std::string> SUPPORT_CONFIG_FORMATS = { XMLCONFIG_FORMAT, JSONCONFIG_FORMAT };

std::string AbstractConfigReaderFactory::DEFAULT_CONFIG_FORMAT(XMLCONFIG_FORMAT);

////////////////////////////////////////////////////////////////////////
// Name:       AbstractConfigReaderFactory::CreateConfigReader()
// Purpose:    Implementation of AbstractConfigReaderFactory::CreateConfigReader()
// Return:     IConfigReader_Ptr
////////////////////////////////////////////////////////////////////////

const std::vector<std::string>& AbstractConfigReaderFactory::SupportConfigFormats()
{
	return SUPPORT_CONFIG_FORMATS;
}

IConfigReader_Ptr AbstractConfigReaderFactory::CreateConfigReader(const char* type)
{
	IConfigReader_Ptr ret;

	if (stringutility::compare(type, XMLCONFIG_FORMAT) == 0)
		ret = std::make_shared<XMLConfigReader>();
	else if (stringutility::compare(type, JSONCONFIG_FORMAT) == 0)
		ret = std::make_shared<JsonConfigReader>();

	return ret;
}

IConfigReader_Ptr AbstractConfigReaderFactory::CreateConfigReader()
{
	return CreateConfigReader(DEFAULT_CONFIG_FORMAT.data());
}

IConfigReader_Ptr AbstractConfigReaderFactory::OpenConfigReader(const char* configPath)
{
	IConfigReader_Ptr ret;
	std::string configFile(configPath);
	auto idx = configFile.rfind('.');
	if (idx != std::string::npos)
	{
		std::string type = configFile.substr(idx + 1);
		ret = CreateConfigReader(type.data());
		ret->LoadFromFile(configFile.data());
	}
	else
	{
		auto& types = SupportConfigFormats();
		for (auto& type : types)
		{
			std::string filepath(configFile + '.' + type);
			std::ifstream is(filepath.data());
			if (is && is.good())
			{
				if (ret = CreateConfigReader(type.data()))
					if (ret->LoadFromStream(is))
						break;
			}
		}
	}

	return ret;
}
