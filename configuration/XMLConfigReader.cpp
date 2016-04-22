/***********************************************************************
 * Module:  XMLConfigReader.cpp
 * Author:  milk
 * Modified: 2014年10月7日 15:33:14
 * Purpose: Implementation of the class XMLConfigReader
 ***********************************************************************/

#include "XMLConfigReader.h"
#include <boost/property_tree/xml_parser.hpp>

using namespace boost::property_tree;
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
    read_xml(is, _current);
    ret = true;
    return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       XMLConfigReader::LoadFromFile(const std::string& configPath)
// Purpose:    Implementation of XMLConfigReader::LoadFromFile()
// Parameters:
// - configPath
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool XMLConfigReader::LoadFromFile(const std::string& configPath)
{
	bool ret = false;
	read_xml(configPath, _current);
	ret = true;
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       XMLConfigReader::getValue(const std::string& expression)
// Purpose:    Implementation of XMLConfigReader::getValue()
// Parameters:
// - expression
// Return:     std::string
////////////////////////////////////////////////////////////////////////

std::string XMLConfigReader::getValue(const std::string& expression)
{
   return _current.get<std::string>(expression);
}

////////////////////////////////////////////////////////////////////////
// Name:       XMLConfigReader::getVector(const std::string& expression, std::vector<std::string>& values)
// Purpose:    Implementation of XMLConfigReader::getVector()
// Parameters:
// - expression
// - values
// Return:     int
////////////////////////////////////////////////////////////////////////

int XMLConfigReader::getVector(const std::string& expression, std::vector<std::string>& values)
{
    ptree ptc = _current.get_child(expression);
    for(ptree::value_type v:ptc)
    {
        values.push_back(v.second.get_value<std::string>());
    }
    return ptc.size();
}

////////////////////////////////////////////////////////////////////////
// Name:       XMLConfigReader::getMap(const std::string& expression, std::map<std::string, std::string>& values)
// Purpose:    Implementation of XMLConfigReader::getMap()
// Parameters:
// - expression
// - values
// Return:     int
////////////////////////////////////////////////////////////////////////

int XMLConfigReader::getMap(const std::string& expression, std::map<std::string, std::string>& values)
{
    ptree ptc = _current.get_child(expression);
    for(ptree::value_type v:ptc)
    {
        values[v.first] = v.second.get_value<std::string>();
    }
    return ptc.size();
}

////////////////////////////////////////////////////////////////////////
// Name:       XMLConfigReader::Close()
// Purpose:    Implementation of XMLConfigReader::Close()
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool XMLConfigReader::Close(void)
{
	return true;
}