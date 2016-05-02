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

IConfigReader_Ptr AbstractConfigReaderFactory::CreateConfigReader(const std::string& type)
{
	IConfigReader_Ptr ret;

	if (stringutility::compare(type.data(), XMLCONFIG_FORMAT) == 0)
		ret = std::make_shared<XMLConfigReader>();
	else if (stringutility::compare(type.data(), JSONCONFIG_FORMAT) == 0)
		ret = std::make_shared<JsonConfigReader>();

	return ret;
}

IConfigReader_Ptr AbstractConfigReaderFactory::CreateConfigReader()
{
	return CreateConfigReader(DEFAULT_CONFIG_FORMAT);
}

IConfigReader_Ptr AbstractConfigReaderFactory::OpenConfigReader(const std::string & configPath)
{
	IConfigReader_Ptr ret;
	auto idx = configPath.rfind('.');
	if (idx != std::string::npos)
	{
		std::string type = configPath.substr(idx + 1);
		ret = CreateConfigReader(type);
		ret->LoadFromFile(configPath);
	}
	else
	{
		auto& types = SupportConfigFormats();
		for (auto& type : types)
		{
			std::string filepath(configPath + '.' + type);
			std::ifstream is(filepath.data());
			if (is && is.good())
			{
				if (ret = CreateConfigReader(type))
					if (ret->LoadFromStream(is))
						break;
			}
		}
	}

	return ret;
}
