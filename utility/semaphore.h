#ifndef _singleton_semaphore_h
#define _singleton_semaphore_h

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>

class Semaphore
{
private:
	std::atomic_uint m_uiCount;
	std::mutex m_mutex;
	std::condition_variable m_condition;

public:
	inline Semaphore(unsigned int uiCount) { m_uiCount = uiCount; }

	inline void Wait()
	{
		std::unique_lock< std::mutex > lock(m_mutex);
		m_condition.wait(lock, [&]()->bool{ return m_uiCount>0; });
		--m_uiCount;
	}

	bool Wait(const std::chrono::milliseconds& millisec)
	{
		std::unique_lock< std::mutex > lock(m_mutex);
		if (!m_condition.wait_for(lock, millisec, [&]()->bool{ return m_uiCount>0; }))
			return false;
		--m_uiCount;
		return true;
	}

	inline void Signal()
	{
		std::unique_lock< std::mutex > lock(m_mutex);
		++m_uiCount;
		m_condition.notify_one();
	}
};

#endif