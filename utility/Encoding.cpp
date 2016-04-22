/***********************************************************************
 * Module:  Encoding.cpp
 * Author:  milk
 * Modified: 2014年10月20日 23:30:26
 * Purpose: Implementation of the class Encoding
 ***********************************************************************/

#include "Encoding.h"
#include <boost\locale\encoding.hpp>

using boost::locale::conv::to_utf;
using boost::locale::conv::from_utf;

////////////////////////////////////////////////////////////////////////
// Name:       Encoding::ToUTF8(const std::string& srcStr, const std::string& charset)
// Purpose:    Implementation of Encoding::ToUTF8()
// Parameters:
// - srcStr
// - charset
// Return:     std::string
////////////////////////////////////////////////////////////////////////

std::string Encoding::ToUTF8(const std::string& srcStr, const std::string& charset)
{
	return to_utf<char>(srcStr, charset);
}

////////////////////////////////////////////////////////////////////////
// Name:       Encoding::ToUTF8(const char* begin, const char* end, const std::string& charset)
// Purpose:    Implementation of Encoding::ToUTF8()
// Parameters:
// - begin
// - end
// - charset
// Return:     std::string
////////////////////////////////////////////////////////////////////////

std::string Encoding::ToUTF8(const char* begin, const char* end, const std::string& charset)
{
	return to_utf<char>(begin, end, charset);
}

////////////////////////////////////////////////////////////////////////
// Name:       Encoding::FromUTF8(const std::string& utf8Str, const std::string& charset)
// Purpose:    Implementation of Encoding::FromUTF8()
// Parameters:
// - utf8Str
// - charset
// Return:     std::string
////////////////////////////////////////////////////////////////////////

std::string Encoding::FromUTF8(const std::string& utf8Str, const std::string& charset)
{
	return from_utf<char>(utf8Str, charset);
}

////////////////////////////////////////////////////////////////////////
// Name:       Encoding::FromUTF8(const char* begin, const char* end, const std::string& charset)
// Purpose:    Implementation of Encoding::FromUTF8()
// Parameters:
// - begin
// - end
// - charset
// Return:     std::string
////////////////////////////////////////////////////////////////////////

std::string Encoding::FromUTF8(const char* begin, const char* end, const std::string& charset)
{
	return from_utf<char>(begin, end, charset);
}