/***********************************************************************
 * Module:  ElementMutex.h
 * Author:  milk
 * Modified: 2015年10月30日 1:55:55
 * Purpose: Declaration of the class ElementMutex
 ***********************************************************************/

#if !defined(__utility_entrywisemutex_h)
#define __utility_entrywisemutex_h

#include <memory>
#include <mutex>
#include <shared_mutex>

template <typename M>
class movable_mutex
{
public:
	movable_mutex()
		:_mutex_ptr(new M){};

	M& mutex(void)
	{
		return *_mutex_ptr;
	}

protected:
	std::shared_ptr<M> _mutex_ptr;

private:

};

template <typename T, typename M>
struct entrywisemutex : public movable_mutex<M>
{
	T entry;
};

template <typename T, typename M> using entrywisemutex_ptr = typename std::shared_ptr<entrywisemutex<T, M>>;

#endif