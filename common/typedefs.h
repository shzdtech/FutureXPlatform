#ifndef _COMMON_TYPEDEF_H_
#define _COMMON_TYPEDEF_H_

#include <string>
#include <memory>
#include <array>
#include <vector>
#include <map>
#include <functional>

typedef std::shared_ptr<void> shared_void_ptr;
typedef shared_void_ptr attribute_ptr;
template <size_t _SZ> using param_array = typename std::array<const void*, _SZ>;
typedef std::vector<const void*> param_vector;
typedef std::shared_ptr<unsigned char> byte_ptr;

typedef std::map<std::string, std::vector<int>> MapIntVector;
typedef std::map<std::string, std::vector<double>> MapDoubleVector;
typedef std::map<std::string, std::vector<std::string>> MapStringVector;

template <typename T>
using vector_ptr = typename std::shared_ptr<std::vector<T>>;

template <typename K, typename V>
using map_ptr = typename std::shared_ptr<std::map<K,V>>;
	
typedef unsigned int uint;
typedef unsigned char byte;

typedef class UINT128
{
public:
	UINT128(uint64_t low64, uint64_t high64)
	{
		*(uint64_t*)_value = low64;
		*(uint64_t*)(_value + 8) = high64;
	}

	inline bool operator==(const UINT128& val128) const {
		return low64() == val128.low64() &&
			high64() == val128.high64();
	}

	inline uint64_t low64() const
	{
		return *(uint64_t*)_value;
	}

	inline uint64_t high64() const
	{
		return *(uint64_t*)(_value + 8);
	}

private:
	byte _value[16];

} uint128;


class UINT128Hash
{
public:
	std::size_t operator()(const UINT128& val128) const {
		static std::hash<uint64_t> f_hasher;
		return f_hasher(val128.low64());
	}
};

#endif