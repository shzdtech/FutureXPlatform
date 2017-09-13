#ifndef _utility_weakptrhash_h
#define _utility_weakptrhash_h

#include <memory>

template<typename T>
struct WeakPtrHash : public std::unary_function<std::weak_ptr<T>, size_t> {
	size_t operator()(const std::weak_ptr<T>& wp) const
	{
		// Example hash. Beware: As zneak remarked in the comments* to this post,
		// it is very possible that this may lead to undefined behaviour
		// since the hash of a key is assumed to be constant, but will change
		// when the weak_ptr expires
		auto sp = wp.lock();
		return std::hash<decltype(sp)>()(sp);
	}
};

template<typename T>
struct WeakPtrEqual : public std::unary_function<std::weak_ptr<T>, bool> {

	bool operator()(const std::weak_ptr<T>& left, const std::weak_ptr<T>& right) const
	{
		return !left.owner_before(right) && !right.owner_before(left);
	}
};

#endif