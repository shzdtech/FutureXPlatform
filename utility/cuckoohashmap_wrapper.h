/***********************************************************************
 * Module:  cuckoohashmap_wrapper.h
 * Author:  milk
 * Modified: 2016年10月28日 14:33:18
 * Purpose: Declaration of the class cuckoohashmap_wrapper
 ***********************************************************************/

#if !defined(__utility_cuckoohashmap_wrapper_h)
#define __utility_cuckoohashmap_wrapper_h
#include <map>
#include <memory>
#include <functional>
#include "libcuckoo/cuckoohash_map.hh"

template <class K, class V, class Hash = DefaultHasher<K>, class Pred = std::equal_to<K>>
class cuckoohashmap_wrapper
{
public:
	cuckoohashmap_wrapper()
		: _innerMap(new cuckoohash_map<K, V, Hash, Pred>()) {}

	cuckoohash_map<K, V, Hash, Pred>& map()
	{
		return *_innerMap;
	}

	operator bool() const
	{
		return _innerMap->operator bool();
	}

protected:
	std::shared_ptr<cuckoohash_map<K, V, Hash, Pred>> _innerMap;
};


#endif