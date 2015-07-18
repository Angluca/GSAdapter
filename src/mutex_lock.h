#ifndef  __MUTEX_LOCK_H__
#define  __MUTEX_LOCK_H__


#include <assert.h>
#include <pthread.h>
#include "utils.h"

class MutexLock
{
	MutexLock(const MutexLock&);
	MutexLock&	operator=(const	MutexLock&);
public:

	explicit	MutexLock(pthread_mutexattr_t* muterattr=NULL)
	{
		pthread_mutex_init(&m_mutex_, muterattr);
	}

	~MutexLock()
	{
		pthread_mutex_destroy(&m_mutex_);
	}

	inline void lock()
	{
		//DEBUG_MSG("lock %x",(unsigned int)this);
		pthread_mutex_lock(&m_mutex_);
	}

	inline void unlock()
	{
		pthread_mutex_unlock(&m_mutex_);
		//DEBUG_MSG("unlock %x",(unsigned int)this);
	}

	inline bool trylock()
	{
		return (pthread_mutex_trylock(&m_mutex_)==0);
	}

private:
	pthread_mutex_t m_mutex_;
};

class	MutexLockGuard
{
	MutexLockGuard(const MutexLockGuard&);
	MutexLockGuard&	operator=	(const	MutexLockGuard&);
	MutexLock*	m_lock;
public:
	explicit	MutexLockGuard(MutexLock* lock):m_lock(lock) {
		assert(lock && "guard lock = null");
		m_lock->lock();
	}
	~MutexLockGuard()
	{
		m_lock->unlock();
	}
};

#define	 MutexLockGuard(x) \
	MutexLockGuard ____guard(x);

#endif  /*__MUTEX_LOCK_H__*/
