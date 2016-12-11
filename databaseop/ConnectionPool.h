#ifndef _DATABASEOP_connection_pool_INCLUDED
#define _DATABASEOP_connection_pool_INCLUDED

#include <mutex>
#include <atomic>
#include <chrono>
#include <vector>
#include <deque>
//#include <glog/logging.h>
#include "../utility/semaphore.h"

static const std::chrono::milliseconds INFINITE_TIMEOUT(0);

template<typename CONNTYPE>
class managedsession
{
public:
	managedsession() = default;

	managedsession(const std::shared_ptr<CONNTYPE>& conn) : _conn(conn)
	{}

	managedsession(const managedsession&) = delete;

	std::shared_ptr<CONNTYPE>& getConnection()
	{
		return _conn;
	}

protected:
	std::shared_ptr<CONNTYPE> _conn;
};


template <typename CONNTYPE>
class connection_pool
{
private:
	int _poolsize;

	struct state_connection
	{
		std::shared_ptr<CONNTYPE> connection_ptr;
		std::atomic_flag lock;
	};

	std::vector<state_connection> _statedconns;

public:
	class managedsession4pool : public managedsession<CONNTYPE>
	{
		friend class connection_pool;
	private:
		managedsession4pool(connection_pool* pool,
			std::chrono::milliseconds millisec = INFINITE_TIMEOUT
		) : _pool(pool)
		{
			if (pool)
				_conn = pool->try_lease(_pos, millisec);

		}

		managedsession4pool(connection_pool* pool, int pos) : _pool(pool), _pos(pos)
		{
			if (_conn = pool->try_lease_at(pos))
				_pos = pos;
		}

	public:
		~managedsession4pool()
		{
			if (_pos >= 0)
			{
				_pool->release(_pos);
				_pos = -1;
			}
		}

	private:
		connection_pool* _pool;
		int _pos = -1;
	};

	typedef std::shared_ptr<managedsession<CONNTYPE>> managedsession_ptr;

public:

	connection_pool(const std::vector<std::shared_ptr<CONNTYPE>>& connections) :
		_poolsize(connections.size()), _statedconns(_poolsize)
	{
		if (_poolsize <= 0)
		{
			throw std::runtime_error("Pool size must be greater than zero.");
		}

		for (int i = 0; i < _poolsize; ++i)
		{
			_statedconns[i].lock.clear(std::memory_order_release);
			_statedconns[i].connection_ptr = connections[i];
		}
	}

	~connection_pool()
	{
	}

	std::shared_ptr<CONNTYPE>& at(int pos)
	{
		if (pos < 0 || pos >= _poolsize)
		{
			throw std::runtime_error("Invalid pool position");
		}

		return _statedconns[pos].connection_ptr;
	}

	managedsession_ptr lease(
		std::chrono::milliseconds millisecs = INFINITE_TIMEOUT)
	{
		return managedsession_ptr(new managedsession4pool(this, millisecs));
	}

	managedsession_ptr lease_at(int pos)
	{
		return managedsession_ptr(new managedsession4pool(this, pos));
	}

	std::shared_ptr<CONNTYPE> try_lease_at(int pos)
	{
		if (pos < 0 || pos >= _poolsize)
		{
			throw std::runtime_error("Invalid pool position");
		}

		std::shared_ptr<CONNTYPE> ret;
		if (!_statedconns[pos].lock.test_and_set(std::memory_order_acquire))
		{
			ret = _statedconns[pos].connection_ptr;
		}

		return ret;
	}

	std::shared_ptr<CONNTYPE> try_lease(int& pos,
		std::chrono::milliseconds millisecs = INFINITE_TIMEOUT)
	{
		pos = -1;
		int i = 0;
		std::shared_ptr<CONNTYPE> conn;
		if (millisecs == INFINITE_TIMEOUT)
		{
			for (; ;)
			{
				if (conn = try_lease_at(i))
				{
					pos = i;
					return conn;
				}

				if (++i >= _poolsize)
				{
					i = 0;
				}
			}
		}
		else
		{
			for (long long m = 0; m < millisecs.count(); m++)
			{
				for (;;)
				{
					if (conn = try_lease_at(i))
					{
						pos = i;
						return conn;
					}

					if (++i >= _poolsize)
					{
						i = 0;
					}
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}

		return conn;
	}

	void release(int pos)
	{
		if (_statedconns[pos].lock.test_and_set(std::memory_order_acquire))
		{
			_statedconns[pos].lock.clear(std::memory_order_release);
		}
	}
};

#endif