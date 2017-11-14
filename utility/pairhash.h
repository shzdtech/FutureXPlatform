#ifndef _utility_pairhash_h
#define _utility_pairhash_h
#include <utility>

template <typename F, typename S>
class pairhash
{
public:
	std::size_t operator()(const std::pair<F, S>& pair) const {
		static std::hash<F> f_hasher;
		static std::hash<S> s_hasher;
		return f_hasher(pair.first) ^ s_hasher(pair.second);
	}
};

template <typename F, typename S, typename H>
class pairhash_first
{
public:
	std::size_t operator()(const std::pair<F, S>& pair) const {
		static H f_hasher;
		return f_hasher(pair.first);
	}
};

#endif