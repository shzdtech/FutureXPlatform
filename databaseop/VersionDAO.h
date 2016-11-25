#if !defined(__databaseop_VersionDAO_h)
#define __databaseop_VersionDAO_h
#include <string>
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS VersionDAO
{
public:

	static bool UpsertVersion(const std::string & item, const std::string & version);

	static bool GetVersion(const std::string & item, std::string & version);

};

#endif