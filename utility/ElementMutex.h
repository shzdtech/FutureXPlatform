/***********************************************************************
 * Module:  ElementMutex.h
 * Author:  milk
 * Modified: 2015年10月30日 1:55:55
 * Purpose: Declaration of the class ElementMutex
 ***********************************************************************/

#if !defined(__utility_ElementMutex_h)
#define __utility_ElementMutex_h

#include <memory>
#include <shared_mutex>

class movable_shared_mutex
{
public:
	movable_shared_mutex()
		:_mutex_ptr(new std::shared_mutex){};

	std::shared_mutex& mutex(void)
	{
		return *_mutex_ptr;
	}

protected:
	std::shared_ptr<std::shared_mutex> _mutex_ptr;

private:

};

template <typename T>
struct ElementMutex : public movable_shared_mutex
{
	T Element;
};
template <typename T> using ElementMutex_Ptr = typename std::shared_ptr<ElementMutex<T>>;

#endif