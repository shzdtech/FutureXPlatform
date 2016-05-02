/***********************************************************************
 * Module:  JsonConfigReader.h
 * Author:  milk
 * Modified: 2016年5月2日 0:49:13
 * Purpose: Declaration of the class JsonConfigReader
 ***********************************************************************/

#if !defined(__configuration_JsonConfigReader_h)
#define __configuration_JsonConfigReader_h

#include "AbstractConfigReader.h"

class JsonConfigReader : public AbstractConfigReader
{
public:
   bool LoadFromStream(std::istream& is);
   bool LoadFromFile(const std::string& configPath);

protected:
private:

};

#endif