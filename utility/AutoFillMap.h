/***********************************************************************
 * Module:  AutoMap.h
 * Author:  milk
 * Modified: 2015年10月28日 14:33:18
 * Purpose: Declaration of the class AutoMap
 ***********************************************************************/

#if !defined(__utility_AutoFillMap_h)
#define __utility_AutoFillMap_h
#include <map>
#include <mutex>
#include <thread>
#include <functional>

template <class K, class V>
class AutoFillMap : public std::map < K, V >
{
public:
	V& getorfill(const K& key)
	{
		auto it = find(key);
		if (it != end())
			return it->second;
		else
		{
			auto iter = emplace(key, V{});
			return (iter.first)->second;
		}
	}

	template <class T>
	V& getorfill(const K& key)
	{
		auto it = find(key);
		if (it != end())
			return it->second;
		else
		{
			auto iter = emplace(key, T{});
			return (iter.first)->second;
		}
	}

	V& getorinitval(const K& key, V&& value)
	{
		auto it = find(key);
		if (it != end())
			return it->second;
		else
		{
			auto iter = emplace(key, std::forward(value));
			return (iter.first)->second;
		}
	}

	template <class T>
	V& getorinitval(const K& key, T& value)
	{
		auto it = find(key);
		if (it != end())
			return it->second;
		else
		{
			auto iter = emplace(key, value);
			return (iter.first)->second;
		}
	}

	template <class Fn, class... Args>
	V& getorinitfn(const K& key, Fn&& fn, Args&&... args)
	{
		auto it = find(key);
		if (it != end())
			return it->second;
		else
		{
			auto finit = std::bind(fn, args ...);
			auto iter = emplace(key, finit(args ...));
			return (iter.first)->second;
		}
	}

protected:


private:

};

#endif