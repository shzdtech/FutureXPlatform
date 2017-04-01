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
#include "../utility/cuckoohashmap_wrapper.h"

template <class K, class V, class Hash = DefaultHasher<K>, class Pred = std::equal_to<K>>
class cuckoohashmap_wrapper
{
public:
	cuckoohashmap_wrapper() = default;

	cuckoohashmap_wrapper(
		bool initilizeMap, 
		size_t initialsize = 128)
	{
		if (initilizeMap)
			_innerMap.reset(new cuckoohash_map<K, V, Hash, Pred>(initialsize));
	}

	std::shared_ptr<cuckoohash_map<K, V, Hash, Pred>>& map()
	{
		return _innerMap;
	}

	bool empty() const
	{
		return !_innerMap || _innerMap->empty();
	}

	operator bool() const
	{
		return _innerMap->operator bool();
	}

protected:
	std::shared_ptr<cuckoohash_map<K, V, Hash, Pred>> _innerMap;
};


#endif