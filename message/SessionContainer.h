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
#include "../utility/ElementMutex.h"
#include "IMessageSession.h"

template <typename K>
class SessionContainer : public IMessageSessionEvent, public std::enable_shared_from_this < SessionContainer<K> >
{
public:
	void foreach(const K& key, std::function<void(IMessageSession*)> func)
	{
		auto it = _sessionMap.find(key);
		if (it != _sessionMap.end())
		{
			std::shared_lock<std::shared_mutex> read_lock(it->second->mutex());
			auto sessionSet = it->second->Element;
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
			_reverseMap.getorfill(pSession).insert(key);
			auto& elmutex = _sessionMap.getorfillfunc(key,
				[](){ return std::make_shared<ElementMutex<std::set<IMessageSession*>>>(); });
			if (elmutex->Element.find(pSession) == elmutex->Element.end())
			{
				std::lock_guard<std::shared_mutex> write_lock(elmutex->mutex());
				elmutex->Element.insert(pSession);
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
			auto it = _sessionMap.find(key);
			if (it != _sessionMap.end())
			{
				if (auto elmutex_ptr = it->second)
				{
					std::lock_guard<std::shared_mutex> write_lock(elmutex_ptr->mutex());
					auto sit = elmutex_ptr->Element.find(pSession);
					if (elmutex_ptr->Element.erase(sit) == elmutex_ptr->Element.end())
						_sessionMap.erase(key);

					auto& keySet = _reverseMap.at(pSession);
					auto keyit = keySet.find(key);
					if (keySet.erase(keyit) == keySet.end())
						_reverseMap.erase(pSession);

					ret = 0;
				}
			}
		}
		return ret;
	}

	int removekey(K& key)
	{
		int ret = -1;
		std::lock_guard<std::mutex> delete_lock(_delMutex);
		auto it = _sessionMap.find(key);
		if (it != _sessionMap.end())
		{
			if (auto elmutex_ptr = it->second)
			{
				std::lock_guard<std::shared_mutex> write_lock(elmutex_ptr->Element.mutex());
				for (auto pSession : elmutex_ptr->Element)
				{
					auto& keySet = _reverseMap.at(pSession);
					auto keyit = keySet.find(key);
					if (keySet.erase(keyit) == keySet.end())
						_reverseMap.erase(pSession);
				}
				_sessionMap.erase(it);
				ret = 0;
			}
		}
		return ret;
	}

	int removesession(IMessageSession* pSession)
	{
		int ret = -1;
		if (pSession)
		{
			std::lock_guard<std::mutex> delete_lock(_delMutex);
			auto it = _reverseMap.find(pSession);
			if (it != _reverseMap.end())
			{
				for (auto& key : it->second)
				{
					if (auto elmutex_ptr = _sessionMap.at(key))
					{
						std::lock_guard<std::shared_mutex> write_lock(elmutex_ptr->mutex());
						auto sit = elmutex_ptr->Element.find(pSession);
						if (elmutex_ptr->Element.erase(sit) == elmutex_ptr->Element.end())
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
	autofillmap<K, ElementMutex_Ptr< std::set<IMessageSession*>>> _sessionMap;
	autofillmap<IMessageSession*, std::set<K>> _reverseMap;
	std::mutex _delMutex;
};

template <typename T>
using SessionContainer_Ptr = typename std::shared_ptr<SessionContainer<T>>;

#endif