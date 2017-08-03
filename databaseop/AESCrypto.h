#if !defined(__databaseop_AESCrypto_h)
#define __databaseop_AESCrypto_h

#include <string>
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS AESCrypto
{
public:
	std::string Encrypt(const std::string& plain);
	std::string Descrypt(const std::string& cipher);
};

#endif

