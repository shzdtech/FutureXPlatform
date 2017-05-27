/***********************************************************************
 * Module:  KeyAttributeDO.h
 * Author:  milk
 * Modified: 2014年10月8日 13:30:06
 * Purpose: Declaration of the class KeyAttributeDO
 ***********************************************************************/

#if !defined(__dataobject_TemplateDO_h)
#define __dataobject_TemplateDO_h

#include <memory>
#include <vector>
#include <map>
#include <set>
#include "dataobjectbase.h"
#include "data_buffer.h"

template <typename T>
class VectorDO : public std::vector<T>, public dataobjectbase
{
public:
	VectorDO() = default;
	VectorDO(const std::vector<T>& other) : std::vector<T>(other) {}
};
template <typename T> using VectorDO_Ptr = typename std::shared_ptr<VectorDO<T>>;

template <typename T>
class SetDO : public std::set<T>, public dataobjectbase
{
public:
	SetDO() = default;
	SetDO(const std::set<T>& other) : std::set<T>(other) {}
};
template <typename T> using SetDO_Ptr = typename std::shared_ptr<SetDO<T>>;

template <typename T>
class StringMapDO : public std::map<std::string, T>, public dataobjectbase
{
public:
	StringMapDO() = default;

	typedef std::map<std::string, T> _baseMap;
	StringMapDO(const _baseMap& other) : _baseMap(other) {}

	template <typename TIt>
	StringMapDO(TIt first, TIt last)
		: _baseMap(first, last) {}

	const T& TryFind(const std::string& key, const T& defaultVal)
	{
		auto it = find(key);
		return (it != end()) ? it->second : defaultVal;
	}
};
template <typename T> using StringMapDO_Ptr = typename std::shared_ptr<StringMapDO<T>>;

template <typename K, typename V>
class MapDO : public std::map<K, V>, public dataobjectbase
{
public:
	MapDO() = default;

	typedef std::map<K, V> _baseMap;
	MapDO(const _baseMap& other) : _baseMap(other) {}

	template <typename TIt>
	MapDO(TIt first, TIt last)
		: _baseMap(first, last) {}

	const V& TryFind(const K& key, const V& defaultVal)
	{
		auto it = find(key);
		return (it != end()) ? it->second : defaultVal;
	}
};
template <typename K, typename V> using MapDO_Ptr = typename std::shared_ptr<MapDO<K, V>>;

template <typename T>
class TDataObject : public dataobjectbase
{
public:
	TDataObject(){};
	TDataObject(const T& data) :Data(data){};
	T Data;
};

typedef TDataObject<data_buffer> DataBufferDO;
typedef TDataObject<MapIntVector> IntTableDO;
typedef TDataObject<MapDoubleVector> DoubleTableDO;
typedef TDataObject<MapStringVector> StringTableDO;

#endif