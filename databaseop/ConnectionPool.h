#ifndef _DATABASEOP_CONNECTION__poolH_INCLUDED
#define _DATABASEOP_CONNECTION__poolH_INCLUDED

#include <mutex>
#include <atomic>
#include <chrono>
#include <vector>
#include <deque>
//#include <glog/logging.h>
#include "../utility/semaphore.h"

template<typename CONNTYPE>
class managedsession
{
public:
	managedsession() = default;

	managedsession(std::shared_ptr<CONNTYPE> conn) : _conn(conn)
	{}

	managedsession(const managedsession&) = delete;

	std::shared_ptr<CONNTYPE> getConnection()
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
	std::mutex _opmutex;
	Semaphore _semaphore;
	std::deque <int> _activeconns;
	std::vector<std::pair<volatile bool, std::shared_ptr<CONNTYPE>>> _statedconns;

public:
	class managedsession4pool : public managedsession<CONNTYPE>
	{
		friend class connection_pool;
	private:
		managedsession4pool(connection_pool* pool,
			std::chrono::milliseconds millisec = std::chrono::milliseconds(0)
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

	connection_pool(std::vector<std::shared_ptr<CONNTYPE>>& connections) :
		_poolsize(connections.size()),
		_semaphore(_poolsize)
	{
		if (_poolsize <= 0)
		{
			throw std::runtime_error("Pool size must be greater than zero.");
		}

		_statedconns.resize(_poolsize);
		for (int i = 0; i < _poolsize; ++i)
		{
			_statedconns[i].first = true;
			_statedconns[i].second = connections[i];
			_activeconns.push_back(i);
		}
	}

	~connection_pool()
	{
	}

	std::shared_ptr<CONNTYPE> at(int pos)
	{
		if (pos < 0 || pos >= _poolsize)
		{
			throw std::runtime_error("Invalid pool position");
		}

		return _statedconns[pos].second;
	}

	managedsession_ptr lease(
		std::chrono::milliseconds millisecs = std::chrono::milliseconds(0))
	{
		return managedsession_ptr(new managedsession4pool(this, millisecs));
	}

	managedsession_ptr lease_at(int pos)
	{
		return managedsession_ptr(new managedsession4pool(this, pos));
	}

	std::shared_ptr<CONNTYPE> try_lease_at(int pos)
	{
		std::shared_ptr<CONNTYPE> ret;
		std::lock_guard<std::mutex> oplock(_opmutex);
		if (_statedconns[pos].first)
		{
			_semaphore.Wait();
			for (auto it = _activeconns.cbegin(); it != _activeconns.cend(); it++)
			{
				if (*it == pos)
				{
					_activeconns.erase(it);
					_statedconns[pos].first = false;
					ret = _statedconns[pos].second;
					break;
				}
			}
		}
		return ret;
	}

	std::shared_ptr<CONNTYPE> try_lease(int& pos,
		std::chrono::milliseconds millisecs = std::chrono::milliseconds(0))
	{
		pos = -1;
		bool waited = true;
		std::shared_ptr<CONNTYPE> conn;
		if (millisecs == std::chrono::milliseconds(0))
			_semaphore.Wait();
		else
			waited = _semaphore.Wait(millisecs);
		if (waited)
		{
			std::lock_guard<std::mutex> oplock(_opmutex);
			pos = _activeconns.front();
			_activeconns.pop_front();
			if (!_statedconns[pos].first)
			{
				throw std::runtime_error("sync check error.");
			}
			_statedconns[pos].first = false;
			conn = _statedconns[pos].second;
		}

		return conn;
	}

	void release(int pos)
	{
		if (!_statedconns[pos].first)
		{
			std::lock_guard<std::mutex> oplock(_opmutex);
			if (!_statedconns[pos].first)
			{
				//_statedconns[pos].second->reconnect();
				_activeconns.push_back(pos);
				_statedconns[pos].first = true;
				//DLOG(INFO) << "released connection at: " << pos << ", pool size: " << _activeconns.size();
				//Release semaphore
				_semaphore.Signal();
			}
		}
	}
};

#endif