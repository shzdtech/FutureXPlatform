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
class SessionContainer : public IMessageSessionEvent, public std::enable_shared_from_this<SessionContainer<K>>
{
private:
	SessionContainer() {}

public:
	static std::shared_ptr<SessionContainer<K>> NewInstancePtr()
	{
		return std::shared_ptr<SessionContainer<K>>(new SessionContainer<K>());
	}

	void foreach(const K& key, std::function<void(IMessageSession*)> func)
	{
		std::shared_lock<std::shared_mutex> read_lock(_mutex);
		auto it = _sessionMap.find(key);
		if (it != _sessionMap.end())
		{
			for (auto pSession : it->second)
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
			for (auto pSession : pair.second)
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
				it->second.erase(pSession);
				if (it->second.empty())
					_sessionMap.erase(it);

				auto rit = _reverseMap.find(pSession);
				if (rit != _reverseMap.end())
				{
					rit->second.erase(key);
					if (rit->second.empty())
						_reverseMap.erase(rit);
				}

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
				auto rit = _reverseMap.find(pSession);
				if (rit != _reverseMap.end())
				{
					rit->second.erase(key);
					if (rit->second.empty())
						_reverseMap.erase(rit);
				}
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
						sit->second.erase(pSession);
						if (sit->second.empty())
							_sessionMap.erase(sit);
					}
				}
				_reverseMap.erase(it);
			}
			ret = 0;
		}
		return ret;
	}

protected:
	void OnSessionClosing(const IMessageSession_Ptr& msgSessionPtr)
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