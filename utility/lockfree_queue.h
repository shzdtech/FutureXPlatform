#ifndef _utility_lockfree_queue_h
#define _utility_lockfree_queue_h

#include <deque>
#include <atomic>

template <class T>
class lockfree_queue
{
public:
	lockfree_queue() = default;

	void push(T&& value)
	{
		lock();
		_queue.push_back(std::move(value));
		unlock();
	}

	void push(const T& value)
	{
		lock();
		_queue.push_back(value);
		unlock();
	}

	bool pop(T& value)
	{
		lock();
		bool hasElement = !_queue.empty();
		if (hasElement)
		{
			value = _queue.front();
			_queue.pop_front();
		}
		unlock();

		return hasElement;
	}

	std::size_t size()
	{
		return _queue.size();
	}

	void clear()
	{
		lock();
		_queue.clear();
		unlock();
	}

	bool empty()
	{
		return _queue.empty();
	}

	inline void lock()
	{
		while (_opState.test_and_set(std::memory_order_acquire));
	}

	inline void unlock()
	{
		_opState.clear(std::memory_order_release);
	}

private:
	std::deque<T> _queue;
	std::atomic_flag _opState;
};


#endif
