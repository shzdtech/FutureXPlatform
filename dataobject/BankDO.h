#if !defined(__dataobject_BankDO_h)
#define __dataobject_BankDO_h

#include "dataobjectbase.h"
#include "../utility/stringutility.h"

class BankDO : public dataobjectbase
{
public:
	std::string BankID;
	std::string BranchID;
	std::string BankName;
};

#endif