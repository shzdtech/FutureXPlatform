/***********************************************************************
 * Module:  SessionContainer.h
 * Author:  milk
 * Modified: 2015年10月31日 11:01:25
 * Purpose: Declaration of the class SessionContainer
 ***********************************************************************/

#if !defined(__message_SessionContainer_h)
#define __message_SessionContainer_h
#include <set>
#include <memory>
#include "../utility/autofillmap.h"
#include "../utility/entrywisemutex.h"
#include "IMessageSession.h"


template <typename K>
class SessionContainer : public IMessageSessionEvent, public std::enable_shared_from_this < SessionContainer<K> >
{
private:
	SessionContainer() {}

public:
	static std::shared_ptr<SessionContainer<K>> NewInstance()
	{
		return std::shared_ptr<SessionContainer<K>>(new SessionContainer<K>());
	}

	void foreach(const K& key, std::function<void(IMessageSession*)> func)
	{
		std::shared_lock<std::shared_mutex> read_lock(_mutex);
		auto it = _sessionMap.find(key);
		if (it != _sessionMap.end())
		{
			auto& sessionSet = it->second;
			for (auto pSession : sessionSet)
			{
				func(pSession);
			}
		}
	}

	void forall(std::function<void(IMessageSession*)> func)
	{
		std::shared_lock<std::shared_mutex> read_lock(_mutex);
		for (auto& pair : _sessionMap)
		{
			auto& sessionSet = pair.second;
			for (auto pSession : sessionSet)
			{
				func(pSession);
			}
		}
	}

	int add(const K& key, IMessageSession* pSession)
	{
		int ret = -1;
		if (pSession)
		{
			std::lock_guard<std::shared_mutex> write_lock(_mutex);
			auto& sessionSet = _sessionMap.getorfill(key);
			if (sessionSet.find(pSession) == sessionSet.end())
			{
				_reverseMap.getorfill(pSession).insert(key);
				sessionSet.insert(pSession);
				pSession->addListener(shared_from_this());
			}
			ret = 0;
		}
		return ret;
	}

	int remove(const K& key, IMessageSession* pSession)
	{
		int ret = -1;
		if (pSession)
		{
			std::lock_guard<std::shared_mutex> write_lock(_mutex);
			auto it = _sessionMap.find(key);
			if (it != _sessionMap.end())
			{
				auto& sessionSet = it->second;
				auto sit = sessionSet.find(pSession);
				if (sessionSet.erase(sit) == sessionSet.end())
					_sessionMap.erase(key);

				auto& keySet = _reverseMap.at(pSession);
				auto keyit = keySet.find(key);
				if (keySet.erase(keyit) == keySet.end())
					_reverseMap.erase(pSession);

				ret = 0;
			}
		}
		return ret;
	}

	int removekey(K& key)
	{
		int ret = -1;
		std::lock_guard<std::shared_mutex> write_lock(_mutex);
		auto it = _sessionMap.find(key);
		if (it != _sessionMap.end())
		{
			auto& sessionSet = it->second;
			for (auto pSession : sessionSet)
			{
				auto& keySet = _reverseMap.at(pSession);
				auto keyit = keySet.find(key);
				if (keySet.erase(keyit) == keySet.end())
					_reverseMap.erase(pSession);
			}
			_sessionMap.erase(it);
			ret = 0;
		}
		return ret;
	}

	int removesession(IMessageSession* pSession)
	{
		int ret = -1;
		if (pSession)
		{
			std::lock_guard<std::shared_mutex> write_lock(_mutex);
			auto it = _reverseMap.find(pSession);
			if (it != _reverseMap.end())
			{
				for (auto& key : it->second)
				{
					auto sit = _sessionMap.find(key);
					if (sit != _sessionMap.end())
					{
						auto& sessionSet = sit->second;
						auto sit = sessionSet.find(pSession);
						if (sessionSet.erase(sit) == sessionSet.end())
							_sessionMap.erase(key);
					}
				}
				_reverseMap.erase(it);
			}
			ret = 0;
		}
		return ret;
	}

protected:
	void OnSessionClosing(IMessageSession_Ptr msgSessionPtr)
	{
		removesession(msgSessionPtr.get());
	}

private:
	autofillmap<K, std::set<IMessageSession*>> _sessionMap;
	autofillmap<IMessageSession*, std::set<K>> _reverseMap;
	std::shared_mutex _mutex;
};

template <typename T>
using SessionContainer_Ptr = typename std::shared_ptr<SessionContainer<T>>;

#endif