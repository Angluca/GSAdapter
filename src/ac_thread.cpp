#include "ac_thread.h"
#include <iostream>

AcThreadWorker::AcThreadWorker()
{
	thread_ = new(std::nothrow) AcThread(this);
}
AcThreadWorker::~AcThreadWorker()
{
	delete	thread_;
}
int	AcThreadWorker::Start()
{
	return	thread_->Start();
}

AcThread::AcThread(AcThreadWorker *arg):
	worker_ptr_(arg)
{}

AcThread::~AcThread()
{
}

int AcThread::Start()
{
	pthread_attr_t thread_attr;
	if (pthread_attr_init(&thread_attr) != 0) {
		return -1;
	}
	if (pthread_create(&thread_, &thread_attr, ThreadFunc, this) != 0) {
		return -1;
	}
	return 0;
}

void* AcThread::ThreadFunc(void *pArg)
{
	AcThread *thread = static_cast<AcThread*> (pArg);
	thread->worker_ptr_->Run();
	pthread_exit(&thread->thread_);
	return NULL;
}

