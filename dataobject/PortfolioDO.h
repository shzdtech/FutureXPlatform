/***********************************************************************
 * Module:  PortfolioDO.h
 * Author:  milk
 * Modified: 2016年2月7日 10:54:28
 * Purpose: Declaration of the class PortfolioDO
 ***********************************************************************/

#if !defined(__dataobject_PortfolioDO_h)
#define __dataobject_PortfolioDO_h

#include "dataobjectbase.h"
#include "ContractKey.h"
#include <chrono>

class PortfolioKey : virtual public UserKey
{
public:
	PortfolioKey(const std::string& portfolioID, const std::string& userID) :
		_portfolioID(portfolioID), UserKey(userID){}

	PortfolioKey(const char* portfolioID, const char* userID) :
		_portfolioID(portfolioID), UserKey(userID) {}

	PortfolioKey& operator= (const PortfolioKey& contractKey)
	{
		return *this;
	}

	inline int compare(const PortfolioKey& portfolioKey) const
	{
		int cmp = stringutility::compare(_portfolioID.data(), portfolioKey._portfolioID.data());

		return cmp != 0 ? cmp : _userID.compare(portfolioKey._userID);
	}

	bool operator< (const PortfolioKey& portfolioKey) const
	{
		return compare(portfolioKey) < 0;
	}

	bool operator== (const PortfolioKey& portfolioKey) const
	{
		return compare(portfolioKey) == 0;
	}

	void SetPortfolioID(const std::string& portfolioID)
	{
		_portfolioID = portfolioID;
	}

	const std::string& PortfolioID() const
	{
		return _portfolioID;
	}

	std::string UserPortfolioID()
	{
		return _userID + ':' + _portfolioID;
	}

protected:
	std::string _portfolioID;

private:
};


class PortfolioKeyHash
{
public:
	std::size_t operator()(const PortfolioKey& k) const {
		static std::hash<std::string> hasher;
		return hasher(k.PortfolioID()) ^ hasher(k.UserID());
	}
};


class PortfolioDO : public PortfolioKey, public dataobjectbase
{
public:
	PortfolioDO(const std::string& portfolioID, const std::string& userID) :
		PortfolioKey(portfolioID, userID), UserKey(userID) {}

	long HedgeDelay;
	std::chrono::steady_clock::time_point LastHedge;

protected:
private:

};

typedef std::shared_ptr<PortfolioDO> PortfolioDO_Ptr;

#endif