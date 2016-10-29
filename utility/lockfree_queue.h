#ifndef _utility_lockfree_queue_h
#define _utility_lockfree_queue_h

#include <queue>
#include <atomic>

template <class T>
class lockfree_queue
{
public:
	lockfree_queue() { _opState.clear(); }

	void push(T&& value)
	{
		lock();
		_queue.push(std::move(value));
		unlock();
	}

	void push(const T& value)
	{
		lock();
		_queue.push(value);
		unlock();
	}

	bool pop(T& value)
	{
		lock();
		bool hasElement = !_queue.empty();
		if (hasElement)
		{
			value = _queue.front();
			_queue.pop();
		}
		unlock();

		return hasElement;
	}

	std::size_t size()
	{
		lock();
		std::size_t sz = _queue.size();
		unlock();

		return sz;
	}

	inline void lock()
	{
		while (!_opState.test_and_set());
	}

	inline void unlock()
	{
		_opState.clear();
	}

private:
	std::queue<T> _queue;
	std::atomic_flag _opState;
};


#endif
