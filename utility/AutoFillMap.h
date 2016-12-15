/***********************************************************************
 * Module:  AutoMap.h
 * Author:  milk
 * Modified: 2015年10月28日 14:33:18
 * Purpose: Declaration of the class autofillmap
 ***********************************************************************/

#if !defined(__utility_autofillmap_h)
#define __utility_autofillmap_h
#include <map>
#include <functional>

template <class K, class V, class Compare = std::less<K>, class Allocator = std::allocator<std::pair<const K, V>>>
class autofillmap : public std::map<K, V, Compare, Allocator>
{
public:
	autofillmap() = default;
	autofillmap(const std::map<K, V, Compare, Allocator>& others) : std::map<K, V, Compare, Allocator>(others) {}

	bool tryfind(const K& key, V& value)
	{
		auto it = find(key);
		if (it != end())
		{
			value = it->second;
			return true;
		}

		return false;
	}

	V* tryfind(const K& key)
	{
		auto it = find(key);
		if (it != end())
		{
			return &it->second;
		}

		return nullptr;
	}

	V& getorfill(const K& key)
	{
		auto it = find(key);
		if (it != end())
			return it->second;
		else
		{
			auto iter = emplace(key, std::move(V{}));
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
			auto iter = emplace(key, std::move(T{}));
			return (iter.first)->second;
		}
	}

	V& getorfill(const K& key, V&& value)
	{
		auto it = find(key);
		if (it != end())
			return it->second;
		else
		{
			auto iter = emplace(key, std::move(value));
			return (iter.first)->second;
		}
	}

	template <class T>
	V& getorfill(const K& key, const T& value)
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
	V& getorfillfunc(const K& key, Fn&& fn, Args&&... args)
	{
		auto it = find(key);
		if (it != end())
			return it->second;
		else
		{
			auto finit = std::bind(fn, args ...);
			auto iter = emplace(key, std::move(finit(args ...)));
			return (iter.first)->second;
		}
	}

protected:


private:

};

#endif