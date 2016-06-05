#ifndef _COMMON_TYPEDEF_H_
#define _COMMON_TYPEDEF_H_

#include <memory>
#include <vector>
#include <map>

typedef std::shared_ptr<void> shared_void_ptr;
typedef shared_void_ptr buffer_ptr;
typedef shared_void_ptr attribute_ptr;
typedef std::vector<const void*> param_vector;
typedef std::shared_ptr<unsigned char> byte_ptr;

template <typename T>
using vector_ptr = typename std::shared_ptr<std::vector<T>>;

template <typename K, typename V>
using map_ptr = typename std::shared_ptr<std::map<K,V>>;
	
typedef unsigned int uint;
typedef unsigned char byte;

#endif