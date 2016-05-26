#if !defined(__utility_atomicutil_h)
#define __utility_atomicutil_h

#include <atomic>
#include "epsdouble.h"

class atomicutil
{
public:
	template<class T>
	static T atomic_fetch_add(std::atomic<T>& obj, T arg)
	{
		T expected = obj.load();
		while (!std::atomic_compare_exchange_weak(&obj, &expected, expected + arg));
		return expected;
	};

	template<class T>
	static T atomic_round_add(std::atomic<T>& obj, T arg, T epsilon)
	{
		T curVal = obj.load();
		T tVal = curVal + arg;
		long lVal = std::lround(tVal);
		T residual = tVal - lVal;

		if ((-epsilon < residual) && (residual < epsilon))
		{
			T delta = arg - residual;
			return atomic_fetch_add(obj, delta);
		}
		else
		{
			return atomic_fetch_add(obj, arg);
		}
	};

	template<class T>
	static T round_add(T& curVal, T arg, T epsilon)
	{
		T tVal = curVal + arg;
		long lVal = std::lround(tVal);
		T residual = tVal - lVal;

		curVal = ((-epsilon < residual) && (residual < epsilon)) ? lVal : tVal;
		
		return curVal;
	};
};


#endif
