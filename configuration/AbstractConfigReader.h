/***********************************************************************
 * Module:  AbstractConfigReader.h
 * Author:  milk
 * Modified: 2016年5月2日 0:53:58
 * Purpose: Declaration of the class AbstractConfigReader
 ***********************************************************************/

#if !defined(__configuration_AbstractConfigReader_h)
#define __configuration_AbstractConfigReader_h

#include "IConfigReader.h"
#include <boost/property_tree/ptree.hpp>
#include "configuration_exp.h"

class CONFIGURATION_CLASS_EXPORT AbstractConfigReader : public IConfigReader
{
public:
   std::string getValue(const std::string& expression);
   int getVector(const std::string& expression, std::vector<std::string>& values);
   int getMap(const std::string& expression, std::map<std::string, std::string>& values);
   bool Close(void);

protected:
	boost::property_tree::ptree _current;

private:

};

#endif