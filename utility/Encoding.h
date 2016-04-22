/***********************************************************************
 * Module:  Encoding.h
 * Author:  milk
 * Modified: 2014年10月20日 23:30:26
 * Purpose: Declaration of the class Encoding
 ***********************************************************************/

#if !defined(__utility_Encoding_h)
#define __utility_Encoding_h
#include "utility_exp.h"
#include <string>

class UTILITY_CLASS_EXPORT Encoding
{
public:
   static std::string ToUTF8(const std::string& srcStr, const std::string& charset);
   static std::string ToUTF8(const char* begin, const char* end, const std::string& charset);
   static std::string FromUTF8(const std::string& utf8Str, const std::string& charset);
   static std::string FromUTF8(const char* begin, const char* end, const std::string& charset);

protected:
private:

};

#endif