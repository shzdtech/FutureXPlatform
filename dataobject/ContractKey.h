/***********************************************************************
 * Module:  ContractKey.h
 * Author:  milk
 * Modified: 2015年8月8日 22:09:37
 * Purpose: Declaration of the class ContractKey
 ***********************************************************************/

#if !defined(__dataobject_ContractKey_h)
#define __dataobject_ContractKey_h

#include <string>
#include "../utility/stringutility.h"

static const char* EXCHANGE_OTC = "otc";

class ContractKey
{
public:
	ContractKey() {}

	ContractKey(const char* exchangeID, const char* instrumentID) :
		_exchangeID(exchangeID), _instrumentID(instrumentID){}

	ContractKey(const std::string& exchangeID, const std::string& instrumentID) :
		_exchangeID(exchangeID), _instrumentID(instrumentID){}

	inline int compare(const ContractKey& contractKey) const
	{
		int cmp = stringutility::compare(_instrumentID.data(), contractKey._instrumentID.data());

		return cmp != 0 ? cmp : 
			stringutility::compare(_exchangeID.data(), contractKey._exchangeID.data());
	}

	inline bool operator< (const ContractKey& contractKey) const
	{
		return compare(contractKey) < 0;
	}

	inline bool operator== (const ContractKey& contractKey) const
	{
		return compare(contractKey) == 0;
	}

	const std::string& ExchangeID() const
	{
		return _exchangeID;
	}

	const std::string& InstrumentID() const
	{
		return _instrumentID;
	}

	bool IsOTC() const
	{
		return _exchangeID == EXCHANGE_OTC;
	}

protected:
	std::string _exchangeID;
	std::string _instrumentID;

private:
};

class UserKey
{
public:
	UserKey() {};

	UserKey(const std::string& userID) : _userID(userID) {};
	UserKey(const char* userID) : _userID(userID) {};

	const std::string& UserID() const
	{
		return _userID;
	}

	void SetUserID(const char* userID)
	{
		_userID = userID;
	}

	void SetUserID(const std::string& userID)
	{
		_userID = userID;
	}

protected:
	std::string _userID;
};

class UserContractKey : public ContractKey, virtual public UserKey
{
public:
	UserContractKey(const char* exchangeID, const char* instrumentID, const char* userID) :
		ContractKey(exchangeID, instrumentID), UserKey(userID) {}

	UserContractKey(const std::string& exchangeID, const std::string& instrumentID, const std::string& userID) :
		ContractKey(exchangeID, instrumentID), UserKey(userID){}

	UserContractKey& operator= (const UserContractKey& userContractKey)
	{
		return *this;
	}

	inline int compare(const UserContractKey& userContractKey) const
	{
		int cmp = ContractKey::compare(userContractKey);

		return cmp != 0 ? cmp : _userID.compare(userContractKey._userID);
	}

	bool operator< (const UserContractKey& userContractKey) const
	{
		return compare(userContractKey) < 0;
	}

	bool operator== (const UserContractKey& userContractKey) const
	{
		return compare(userContractKey) == 0;
	}	
};

#endif