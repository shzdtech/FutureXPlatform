/***********************************************************************
 * Module:  AbstractConfigReader.cpp
 * Author:  milk
 * Modified: 2016年5月2日 0:53:58
 * Purpose: Implementation of the class AbstractConfigReader
 ***********************************************************************/

#include "AbstractConfigReader.h"

using namespace boost::property_tree;

////////////////////////////////////////////////////////////////////////
// Name:       AbstractConfigReader::getValue(const std::string& expression)
// Purpose:    Implementation of AbstractConfigReader::getValue()
// Parameters:
// - expression
// Return:     std::string
////////////////////////////////////////////////////////////////////////

std::string AbstractConfigReader::getValue(const std::string& expression)
{
	return _current.get<std::string>(expression);
}

////////////////////////////////////////////////////////////////////////
// Name:       AbstractConfigReader::getVector(const std::string& expression, std::vector<std::string>& values)
// Purpose:    Implementation of AbstractConfigReader::getVector()
// Parameters:
// - expression
// - values
// Return:     int
////////////////////////////////////////////////////////////////////////

int AbstractConfigReader::getVector(const std::string& expression, std::vector<std::string>& values)
{
	ptree ptc = _current.get_child(expression);
	for (ptree::value_type v : ptc)
	{
		values.push_back(v.second.get_value<std::string>());
	}
	return ptc.size();
}

////////////////////////////////////////////////////////////////////////
// Name:       AbstractConfigReader::getMap(const std::string& expression, std::map<std::string, std::string>& values)
// Purpose:    Implementation of AbstractConfigReader::getMap()
// Parameters:
// - expression
// - values
// Return:     int
////////////////////////////////////////////////////////////////////////

int AbstractConfigReader::getMap(const std::string& expression, std::map<std::string, std::string>& values)
{
	ptree ptc = _current.get_child(expression);
	for (ptree::value_type v : ptc)
	{
		values[v.first] = v.second.get_value<std::string>();
	}
	return ptc.size();
}

////////////////////////////////////////////////////////////////////////
// Name:       AbstractConfigReader::Close()
// Purpose:    Implementation of AbstractConfigReader::Close()
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool AbstractConfigReader::Close(void)
{
	return true;
}