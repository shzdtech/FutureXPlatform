/***********************************************************************
 * Module:  XMLConfigReader.cpp
 * Author:  milk
 * Modified: 2014年10月7日 15:33:14
 * Purpose: Implementation of the class XMLConfigReader
 ***********************************************************************/

#include "XMLConfigReader.h"
#include <boost/property_tree/xml_parser.hpp>

////////////////////////////////////////////////////////////////////////
// Name:       XMLConfigReader::LoadFromStream(std::istream& is)
// Purpose:    Implementation of XMLConfigReader::LoadFromStream()
// Parameters:
// - is
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool XMLConfigReader::LoadFromStream(std::istream& is)
{
    bool ret = false;
    read_xml(is, _root);
    ret = true;
    return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       XMLConfigReader::LoadFromFile(const char* configPath)
// Purpose:    Implementation of XMLConfigReader::LoadFromFile()
// Parameters:
// - configPath
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool XMLConfigReader::LoadFromFile(const std::string& configPath)
{
	bool ret = false;
	read_xml(configPath, _root);
	ret = true;
	return ret;
}