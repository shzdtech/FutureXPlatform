#ifndef _utility_scopeunlock_h
#define _utility_scopeunlock_h


template<class c>
class scopeunlock
{
public:
	scopeunlock(c* plock)
	{
		_plock = plock;
		_plock->lock();
	}

	~scopeunlock()
	{
		_plock->unlock();
	}

private:
	c* _plock;
};

#endif
