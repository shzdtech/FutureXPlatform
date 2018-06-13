#ifndef _utility_lockfree_set_h
#define _utility_lockfree_set_h

#include <unordered_set>
#include <vector>
#include <atomic>

template 
<	class Key,                        // unordered_set::key_type/value_type
	class Hash = std::hash<Key>,           // unordered_set::hasher
	class Pred = std::equal_to<Key>,       // unordered_set::key_equal
	class Alloc = std::allocator<Key>      // unordered_set::allocator_type
>
class lockfree_set
{
public:
	typedef std::unordered_set<Key, Hash, Pred, Alloc> _MySet;

	lockfree_set() = default;

	void emplace(Key&& value)
	{
		lock();
		_set.emplace(std::move(value));
		unlock();
	}

	void emplace(const Key& value)
	{
		lock();
		_set.emplace(value);
		unlock();
	}

	bool pop(Key& value)
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

	bool pop(Key&& value)
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

	void to_vector(std::vector<Key>& vec)
	{
		lock();
		for (auto& val : _set)
			vec.push_back(val);
		unlock();
	}


	bool find(const Key& value)
	{
		lock();
		bool hasElement = _set.find(value) != _set.end();
		unlock();

		return hasElement;
	}

	size_t erase(const Key& value)
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

	_MySet& rawset()
	{
		return _set;
	}

private:
	_MySet _set;
	std::atomic_flag _opState;
};


#endif
