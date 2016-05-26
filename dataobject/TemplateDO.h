/***********************************************************************
 * Module:  KeyAttributeDO.h
 * Author:  milk
 * Modified: 2014年10月8日 13:30:06
 * Purpose: Declaration of the class KeyAttributeDO
 ***********************************************************************/

#if !defined(__message_KeyAttributeDO_h)
#define __message_KeyAttributeDO_h

#include <memory>
#include <vector>
#include <map>
#include <set>
#include "dataobjectbase.h"
#include "data_buffer.h"

template <typename T>
class VectorDO : public std::vector<T>, public dataobjectbase
{};
template <typename T> using VectorDO_Ptr = typename std::shared_ptr<VectorDO<T>>;

template <typename T>
class SetDO : public std::set<T>, public dataobjectbase
{};
template <typename T> using SetDO_Ptr = typename std::shared_ptr<SetDO<T>>;

template <typename T>
class MapDO : public std::map<std::string, T>, public dataobjectbase
{
public:
	MapDO() {};

	typedef std::map<std::string, T> _baseMap;
	template <typename TIt>
	MapDO(TIt first, TIt last)
		: _baseMap(first, last) {}

	const T& TryFind(const std::string& key, const T& defaultVal)
	{
		auto it = find(key);
		return (it != end()) ? it->second : defaultVal;
	}
};
template <typename T> using MapDO_Ptr = typename std::shared_ptr<MapDO<T>>;


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