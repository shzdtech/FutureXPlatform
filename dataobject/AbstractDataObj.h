/***********************************************************************
 * Module:  AbstractDataObj.h
 * Author:  milk
 * Modified: 2014年10月4日 0:57:49
 * Purpose: Declaration of the class AbstractDataObj
 ***********************************************************************/

#if !defined(__data_AbstractDataObj_h)
#define __data_AbstractDataObj_h

#include <string>
#include <vector>
#include <memory>
#include <map>

class AbstractDataObj
{
public:

protected:

private:

};

typedef std::shared_ptr<AbstractDataObj> dataobj_ptr;

typedef std::map<std::string, std::vector<int>> MapIntVector;
typedef std::map<std::string, std::vector<double>> MapDoubleVector;
typedef std::map<std::string, std::vector<std::string>> MapStringVector;

#endif