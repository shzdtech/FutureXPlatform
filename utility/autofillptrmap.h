/***********************************************************************
 * Module:  autofillptrmap.h
 * Author:  milk
 * Modified: 2016年10月28日 14:33:18
 * Purpose: Declaration of the class autofillmap
 ***********************************************************************/

#if !defined(__utility_autofillptrmap_h)
#define __utility_autofillptrmap_h
#include <map>
#include <memory>
#include <functional>

template <class K, class V>
class autofillptrmap : public std::map<K, std::shared_ptr<V>>
{
public:
	autofillptrmap() = default;
	autofillptrmap(const std::map<K, std::shared_ptr<V>>& others) :std::map<K, std::shared_ptr<V>>(others) {}

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

	std::shared_ptr<V> tryfind(const K& key)
	{
		auto it = find(key);
		if (it != end())
		{
			return it->second;
		}

		return nullptr;
	}

	std::shared_ptr<V>& getorfill(const K& key)
	{
		auto it = find(key);
		if (it != end())
			return it->second;
		else
		{
			auto iter = emplace(key, std::make_shared<V>());
			return (iter.first)->second;
		}
	}

	template <class T>
	std::shared_ptr<V>& getorfill(const K& key)
	{
		auto it = find(key);
		if (it != end())
			return it->second;
		else
		{
			auto iter = emplace(key, std::make_shared<T>());
			return (iter.first)->second;
		}
	}

	std::shared_ptr<V>& getorfill(const K& key, V&& value)
	{
		auto it = find(key);
		if (it != end())
			return it->second;
		else
		{
			auto iter = emplace(key, std::make_shared<T>(std::move(value)));
			return (iter.first)->second;
		}
	}

	template <class T>
	std::shared_ptr<V>& getorfill(const K& key, const T& value)
	{
		auto it = find(key);
		if (it != end())
			return it->second;
		else
		{
			auto iter = emplace(key, std::make_shared<T>(value));
			return (iter.first)->second;
		}
	}

	template <class Fn, class... Args>
	std::shared_ptr<V>& getorfillfunc(const K& key, Fn&& fn, Args&&... args)
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