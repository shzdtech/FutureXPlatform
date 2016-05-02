/***********************************************************************
 * Module:  XMLConfigReader.h
 * Author:  milk
 * Modified: 2014年10月7日 15:33:14
 * Purpose: Declaration of the class XMLConfigReader
 ***********************************************************************/

#if !defined(__configuration_XMLConfigReader_h)
#define __configuration_XMLConfigReader_h

#include "AbstractConfigReader.h"
#include "configuration_exp.h"
#include <boost/property_tree/ptree.hpp>

class CONFIGURATION_CLASS_EXPORT XMLConfigReader : public AbstractConfigReader
{
public:
   bool LoadFromStream(std::istream& is);
   bool LoadFromFile(const std::string& configPath);

protected:

private:

};

#endif