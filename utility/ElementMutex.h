/***********************************************************************
 * Module:  ElementMutex.h
 * Author:  milk
 * Modified: 2015年10月30日 1:55:55
 * Purpose: Declaration of the class ElementMutex
 ***********************************************************************/

#if !defined(__utility_ElementMutex_h)
#define __utility_ElementMutex_h

#include <memory>
#include <mutex>

class MovableMutex
{
public:
	MovableMutex()
		:_mutex_ptr(new std::mutex){};

	std::mutex& Mutex(void)
	{
		return *_mutex_ptr;
	}

protected:
	std::shared_ptr<std::mutex> _mutex_ptr;

private:

};

template <typename T>
struct ElementMutex : public MovableMutex
{
	T Element;
};
template <typename T> using ElementMutex_Ptr = typename std::shared_ptr<ElementMutex<T>>;

#endif