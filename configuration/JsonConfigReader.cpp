/***********************************************************************
 * Module:  JsonConfigReader.cpp
 * Author:  milk
 * Modified: 2016年5月2日 0:49:13
 * Purpose: Implementation of the class JsonConfigReader
 ***********************************************************************/

#include "JsonConfigReader.h"
#include <boost/property_tree/json_parser.hpp>

 ////////////////////////////////////////////////////////////////////////
 // Name:       JsonConfigReader::LoadFromStream(std::istream& is)
 // Purpose:    Implementation of JsonConfigReader::LoadFromStream()
 // Parameters:
 // - is
 // Return:     bool
 ////////////////////////////////////////////////////////////////////////

bool JsonConfigReader::LoadFromStream(std::istream& is)
{
	bool ret = false;
	read_json(is, _current);
	ret = true;
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       JsonConfigReader::LoadFromFile(const std::string& configPath)
// Purpose:    Implementation of JsonConfigReader::LoadFromFile()
// Parameters:
// - configPath
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool JsonConfigReader::LoadFromFile(const std::string& configPath)
{
	bool ret = false;
	read_json(configPath, _current);
	ret = true;
	return ret;
}