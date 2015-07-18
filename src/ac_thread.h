#ifndef  __AC_THREAD_H__
#define  __AC_THREAD_H__

#include <pthread.h>

class AcThread;

class AcThreadWorker {
public:
		AcThreadWorker();
		virtual ~AcThreadWorker();
		virtual void Run() = 0;
		int	Start();
private:
		AcThread *thread_;
};

class AcThread
{
public:
	AcThread(AcThreadWorker* arg);
	~AcThread();
	int Start();
private:
	static void* ThreadFunc(void *pArg);
	pthread_t thread_;
	AcThreadWorker *worker_ptr_;
};

#endif  /*__AC_THREAD_H__*/
