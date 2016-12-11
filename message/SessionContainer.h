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

	void foreach(const K& key, std::function<void(const IMessageSession_Ptr&)> func)
	{
		std::shared_lock<std::shared_mutex> read_lock(_mutex);
		auto it = _sessionMap.find(key);
		if (it != _sessionMap.end())
		{
			for (auto& sessionPtr : it->second)
			{
				func(sessionPtr);
			}
		}
	}

	void forall(std::function<void(const IMessageSession_Ptr&)> func)
	{
		std::shared_lock<std::shared_mutex> read_lock(_mutex);
		for (auto& pair : _sessionMap)
		{
			for (auto& sessionPtr : pair.second)
			{
				func(sessionPtr);
			}
		}
	}

	int add(const K& key, const IMessageSession_Ptr& sessionPtr)
	{
		int ret = -1;
		if (sessionPtr)
		{
			std::unique_lock<std::shared_mutex> write_lock(_mutex);
			auto& sessionSet = _sessionMap.getorfill(key);
			if (sessionSet.find(sessionPtr) == sessionSet.end())
			{
				// _reverseMap.getorfill(sessionPtr).insert(key);
				sessionSet.insert(sessionPtr);
				sessionPtr->addListener(shared_from_this());
			}
			ret = 0;
		}
		return ret;
	}

	int remove(const K& key, const IMessageSession_Ptr& sessionPtr)
	{
		int ret = -1;
		if (sessionPtr)
		{
			std::unique_lock<std::shared_mutex> write_lock(_mutex);
			auto it = _sessionMap.find(key);
			if (it != _sessionMap.end())
			{
				it->second.erase(sessionPtr);
				if (it->second.empty())
					_sessionMap.erase(it);

				/*auto rit = _reverseMap.find(sessionPtr);
				if (rit != _reverseMap.end())
				{
					rit->second.erase(key);
					if (rit->second.empty())
						_reverseMap.erase(rit);
				}*/

				ret = 0;
			}
		}
		return ret;
	}

	int removekey(const K& key)
	{
		int ret = -1;
		std::unique_lock<std::shared_mutex> write_lock(_mutex);
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
			std::unique_lock<std::shared_mutex> write_lock(_mutex);
			for (auto& pair : _sessionMap)
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
	autofillmap<K, std::set<IMessageSession_Ptr>> _sessionMap;
	// autofillmap<IMessageSession_Ptr, std::set<K>> _reverseMap;
	std::shared_mutex _mutex;
};

template <typename T>
using SessionContainer_Ptr = typename std::shared_ptr<SessionContainer<T>>;

#endif