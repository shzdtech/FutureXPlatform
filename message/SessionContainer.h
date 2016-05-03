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
#include "../utility/AutoFillMap.h"
#include "../utility/ElementMutex.h"
#include "IMessageSession.h"

struct ScopeLockContext
{
	ElementMutex_Ptr< std::set<IMessageSession*>> ElementMutex_Ptr;
	std::unique_ptr<std::lock_guard<std::mutex>> ScopeLock_Ptr;
};

template <typename K>
class SessionContainer : public IMessageSessionEvent, public std::enable_shared_from_this < SessionContainer<K> >
{
public:
	const std::set<IMessageSession*>* getwithlock(const K& key,
		ScopeLockContext& sclkctx)
	{
		std::set<IMessageSession*>* ret = nullptr;

		auto it = _sessionMap.find(key);
		if (it != _sessionMap.end())
		{
			if (sclkctx.ElementMutex_Ptr = it->second)
			{
				sclkctx.ScopeLock_Ptr.reset(
					new std::lock_guard<std::mutex>(sclkctx.ElementMutex_Ptr->Mutex()));
				ret = &sclkctx.ElementMutex_Ptr->Element;
			}
		}

		return ret;
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
				std::lock_guard<std::mutex> oplock(elmutex->Mutex());
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
					std::lock_guard<std::mutex> oplock(elmutex_ptr->Mutex());
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
		std::lock_guard<std::mutex> removelock(_delMutex);
		auto it = _sessionMap.find(key);
		if (it != _sessionMap.end())
		{
			if (auto elmutex_ptr = it->second)
			{
				std::lock_guard<std::mutex> oplock(elmutex_ptr->Element.Mutex());
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
			std::lock_guard<std::mutex> removelock(_delMutex);
			auto it = _reverseMap.find(pSession);
			if (it != _reverseMap.end())
			{
				for (auto& key : it->second)
				{
					if (auto elmutex_ptr = _sessionMap.at(key))
					{
						std::lock_guard<std::mutex> oplock(elmutex_ptr->Mutex());
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
	AutoFillMap<K, ElementMutex_Ptr< std::set<IMessageSession*>>> _sessionMap;
	AutoFillMap<IMessageSession*, std::set<K>> _reverseMap;
	std::mutex _delMutex;
};

template <typename T>
using SessionContainer_Ptr = typename std::shared_ptr<SessionContainer<T>>;

#endif