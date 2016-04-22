/***********************************************************************
 * Module:  XMLConfigReader.h
 * Author:  milk
 * Modified: 2014年10月7日 15:33:14
 * Purpose: Declaration of the class XMLConfigReader
 ***********************************************************************/

#if !defined(__configuration_XMLConfigReader_h)
#define __configuration_XMLConfigReader_h

#include "IConfigReader.h"
#include "configuration_exp.h"
#include <boost/property_tree/ptree.hpp>

class CONFIGURATION_CLASS_EXPORT XMLConfigReader : public IConfigReader
{
public:
   bool LoadFromStream(std::istream& is);
   bool LoadFromFile(const std::string& configPath);
   std::string getValue(const std::string& expression);
   int getVector(const std::string& expression, std::vector<std::string>& values);
   int getMap(const std::string& expression, std::map<std::string, std::string>& values);
   bool Close(void);

protected:
   boost::property_tree::ptree _current;

private:

};

#endif