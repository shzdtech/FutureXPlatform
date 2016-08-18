/***********************************************************************
 * Module:  IConfigReader.h
 * Author:  milk
 * Modified: 2014年10月7日 15:25:50
 * Purpose: Declaration of the class IConfigReader
 ***********************************************************************/

#if !defined(__configuration_IConfigReader_h)
#define __configuration_IConfigReader_h
#include <string>
#include <vector>
#include <map>
#include <memory>

class IConfigReader
{
public:
	virtual bool LoadFromStream(std::istream& is) = 0;
	virtual bool LoadFromFile(const std::string& configPath) = 0;
	virtual std::string getValue(const std::string& expression) = 0;
	virtual int getVector(const std::string& expression, std::vector<std::string>& values) = 0;
	virtual int getMap(const std::string& expression, std::map<std::string, std::string>& values) = 0;
	virtual bool Close(void) = 0;

protected:
private:

};

typedef std::shared_ptr<IConfigReader> IConfigReader_Ptr;

#endif