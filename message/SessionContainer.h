/***********************************************************************
 * Module:  SessionContainer.h
 * Author:  milk
 * Modified: 2015年10月31日 11:01:25
 * Purpose: Declaration of the class SessionContainer
 ***********************************************************************/

#if !defined(__message_SessionContainer_h)
#define __message_SessionContainer_h
#include <set>
#include <unordered_set>
#include <memory>
#include "../include/libcuckoo/cuckoohash_map.hh"
#include "../utility/entrywisemutex.h"
#include "IMessageSession.h"


template <typename K, class Hash = std::hash<K>, class Pred = std::equal_to<K>>
class SessionContainer : public IMessageSessionEvent, public std::enable_shared_from_this<SessionContainer<K, Hash, Pred>>
{
private:
	SessionContainer() : _sessionMap(16) {}

public:
	static std::shared_ptr<SessionContainer<K, Hash, Pred>> NewInstancePtr()
	{
		return std::shared_ptr<SessionContainer<K, Hash, Pred>>(new SessionContainer<K, Hash, Pred>());
	}

	bool contains(const K& key)
	{
		return _sessionMap.contains(key);
	}

	size_t size(const K& key)
	{
		size_t sz = 0;
		_sessionMap.update_fn(key, [&sz](std::unordered_set<IMessageSession_Ptr>& sessionSet)
		{
			sz = sessionSet.size();
		});

		return sz;
	}

	void foreach(const K& key, std::function<void(const IMessageSession_Ptr&)> func)
	{
		//std::shared_lock<std::shared_mutex> read_lock(_mutex);
		_sessionMap.update_fn(key, [&func](std::unordered_set<IMessageSession_Ptr>& sessionSet)
		{
			for (auto& sessionPtr : sessionSet)
			{
				func(sessionPtr);
			}
		});
	}

	void forall(std::function<void(const IMessageSession_Ptr&)> func)
	{
		// std::shared_lock<std::shared_mutex> read_lock(_mutex);
		for (auto& pair : _sessionMap.lock_table();)
		{
			foreach(pair.first, func);
		}
	}

	void forfirst(const K& key, std::function<void(const IMessageSession_Ptr&)> func)
	{
		//std::shared_lock<std::shared_mutex> read_lock(_mutex);
		_sessionMap.update_fn(key, [&func](std::unordered_set<IMessageSession_Ptr>& sessionSet)
		{
			auto it = sessionSet.begin();
			if (it != sessionSet.end())
			{
				func(*it);
			}
		});
	}

	int add(const K& key, const IMessageSession_Ptr& sessionPtr)
	{
		int ret = -1;
		if (sessionPtr)
		{
			//std::unique_lock<std::shared_mutex> write_lock(_mutex);
			sessionPtr->addListener(shared_from_this());
			std::unordered_set<IMessageSession_Ptr> singleton{ sessionPtr };
			_sessionMap.upsert(key, [&sessionPtr](std::unordered_set<IMessageSession_Ptr>& sessionSet)
			{
				sessionSet.insert(sessionPtr);
			},
				singleton);

			ret = 0;
		}
		return ret;
	}

	int remove(const K& key, const IMessageSession_Ptr& sessionPtr)
	{
		int ret = -1;
		if (sessionPtr)
		{
			// std::unique_lock<std::shared_mutex> write_lock(_mutex);
			//auto it = _sessionMap.find(key);
			//if (it != _sessionMap.end())
			//{
			//	it->second.erase(sessionPtr);
			//	if (it->second.empty())
			//		_sessionMap.erase(it);

			//	/*auto rit = _reverseMap.find(sessionPtr);
			//	if (rit != _reverseMap.end())
			//	{
			//		rit->second.erase(key);
			//		if (rit->second.empty())
			//			_reverseMap.erase(rit);
			//	}*/

			//	ret = 0;
			//}

			_sessionMap.update_fn(key, [&sessionPtr](std::unordered_set<IMessageSession_Ptr>& sessionSet)
			{
				sessionSet.erase(sessionPtr);
			});
		}
		return ret;
	}

	int removekey(const K& key)
	{
		int ret = -1;
		// std::unique_lock<std::shared_mutex> write_lock(_mutex);
		_sessionMap.erase(key);
		/*auto it = _sessionMap.find(key);
		if (it != _sessionMap.end())
		{
			auto& sessionSet = it->second;
			for (auto sessionPtr : sessionSet)
			{
				auto rit = _reverseMap.find(sessionPtr);
				if (rit != _reverseMap.end())
				{
					rit->second.erase(key);
					if (rit->second.empty())
						_reverseMap.erase(rit);
				}
			}
			_sessionMap.erase(it);*/
		ret = 0;
		//}
		return ret;
	}

	/*int removesession(const IMessageSession_Ptr& sessionPtr)
	{
		int ret = -1;
		if (sessionPtr)
		{
			std::unique_lock<std::shared_mutex> write_lock(_mutex);
			auto it = _reverseMap.find(sessionPtr);
			if (it != _reverseMap.end())
			{
				auto& set = it->second;
				for (auto key = set.begin(); key != set.end(); key++)
				{
					auto sit = _sessionMap.find(*key);
					if (sit != _sessionMap.end())
					{
						sit->second.erase(sessionPtr);
						if (sit->second.empty())
							_sessionMap.erase(sit);
					}
				}
				_reverseMap.erase(sessionPtr);
			}
			ret = 0;
		}
		return ret;
	}*/

	int removesession(const IMessageSession_Ptr& sessionPtr)
	{
		int ret = -1;
		if (sessionPtr)
		{
			//std::unique_lock<std::shared_mutex> write_lock(_mutex);
			for (auto& pair : _sessionMap.lock_table())
			{
				pair.second.erase(sessionPtr);
			}
			ret = 0;
		}
		return ret;
	}

protected:
	void OnSessionClosing(const IMessageSession_Ptr& msgSessionPtr)
	{
		removesession(msgSessionPtr);
	}

private:
	cuckoohash_map<K, std::unordered_set<IMessageSession_Ptr>, Hash, Pred> _sessionMap;
	// autofillmap<IMessageSession_Ptr, std::unordered_set<K>> _reverseMap;
	// std::shared_mutex _mutex;
};

template <typename K, class Hash = std::hash<K>, class Pred = std::equal_to<K>>
using SessionContainer_Ptr = typename std::shared_ptr<SessionContainer<K, Hash, Pred>>;

#endif