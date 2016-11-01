#ifndef _utility_lockfree_set_h
#define _utility_lockfree_set_h

#include <set>
#include <atomic>

template <class T>
class lockfree_set
{
public:
	lockfree_set() { _opState.clear(); }

	void emplace(T&& value)
	{
		lock();
		_set.emplace(std::move(value));
		unlock();
	}

	void emplace(const T& value)
	{
		lock();
		_set.emplace(value);
		unlock();
	}

	bool pop(T& value)
	{
		lock();
		auto it = _set.begin();
		bool hasElement = it != _set.end();
		if (hasElement)
		{
			value = *it;
			_set.erase(it);
		}
		unlock();

		return hasElement;
	}

	bool pop(T&& value)
	{
		lock();
		auto it = _set.begin();
		bool hasElement = it != _set.end();
		if (hasElement)
		{
			value = std::move(*it);
			_set.erase(it);
		}
		unlock();

		return hasElement;
	}

	void to_vector(std::vector<T>& vec)
	{
		lock();
		for (auto& val : _set)
			vec.push_back(val);
		unlock();
	}


	bool find(const T& value)
	{
		lock();
		bool hasElement = _set.find(value) != _set.end();
		unlock();

		return hasElement;
	}

	size_t erase(const T& value)
	{
		lock();
		size_t nErased = _set.erase(value);
		unlock();

		return nErased;
	}

	std::size_t size()
	{
		return _set.size();
	}

	void clear()
	{
		lock();
		_set.clear();
		unlock();
	}

	bool empty()
	{
		return _set.empty();
	}

	inline void lock()
	{
		while (_opState.test_and_set(std::memory_order_acquire));
	}

	inline void unlock()
	{
		_opState.clear(std::memory_order_release);
	}

	std::set<T>& rawset()
	{
		return _set;
	}

private:
	std::set<T> _set;
	std::atomic_flag _opState;
};


#endif
