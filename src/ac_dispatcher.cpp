#include "ac_handle.h"
#include "utils.h"
#include "ac_dispatcher.h"

#include <stdlib.h>
#include <sys/epoll.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>


//constructor
AcDispatcher::AcDispatcher(void):
	fd_size_(0),
	events_(NULL),
	shut_down_(false),
	time_out_(-1)
{
}

//destructor
AcDispatcher::~AcDispatcher(void)
{
	if(events_) {
		delete[]	events_;
	}
}

int AcDispatcher::AddEventEx(AcHandle *handle, int handle_event)
{
	assert(handle);
	int	fd = handle->fd();
	struct	epoll_event ev;
	ev.data.u64 = 0;
	ev.data.ptr = handle;
	ev.events = EPOLLET | EPOLLERR | EPOLLHUP ;

	if(handle_event & EVENT_READ) {
		ev.events |= EPOLLIN;
	} 
	if(handle_event & EVENT_WRITE) {
		ev.events |= EPOLLOUT;
	}
	if(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) < 0) {
		return -1;
	}
	return 0;
}

int	AcDispatcher::AddEvent(AcHandle *handle, int handle_event)
{
	assert(handle);
	int	fd = handle->fd();
	struct	epoll_event ev;
	ev.data.u64 = 0;
	ev.data.ptr = handle;
	ev.events = EPOLLET | EPOLLERR | EPOLLHUP | EPOLLONESHOT;
	//int	handle_event = handle->event();
	if(handle_event & EVENT_READ) {
		ev.events |= EPOLLIN;
	}
	if(handle_event & EVENT_WRITE) {
		ev.events |= EPOLLOUT;
	}
	if(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) < 0) {
		return -1;
	}
	return 0;
}


int	AcDispatcher::ModEvent(AcHandle *handle, int handle_event)
{
	assert(handle);
	int	fd = handle->fd();
	struct	epoll_event ev;
	ev.data.u64 = 0;
	ev.data.ptr = handle;
	ev.events = EPOLLET | EPOLLERR | EPOLLHUP | EPOLLONESHOT;
	//int	handle_event = handle->event();
	if(handle_event & EVENT_READ) {
		ev.events |= EPOLLIN;
	}
	if(handle_event & EVENT_WRITE) {
		ev.events |= EPOLLOUT;
	}
	if(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev) < 0) {
		return -1;
	}
	return 0;
	
}

int	AcDispatcher::DelEvent(AcHandle *handle)
{
	int	fd	= handle->fd();
	return DelEvent(fd);
}

int	AcDispatcher::DelEvent(int fd)
{
	struct	epoll_event ev = {0};
	if(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev) < 0) {
		return -1;
	}
	return 0;
}

int	AcDispatcher::Init()
{
	epfd_	= epoll_create(fd_size_);
	if(epfd_ < 0) {
		ERROR_OUT("create epoll error");
		return -1;
	}
	events_ = (epoll_event*) calloc(EVENT_NUMBER, sizeof(epoll_event));
	if(NULL == events_) {
		Destroy();
		return -1;
	}
	return 0;
}
void	AcDispatcher::Destroy()
{
	//DEBUG_MSG("Dispactcher destroy ");

	if(epfd_ > 0) {
		close(epfd_);
		epfd_ = 0;
		fd_size_ = 0;
	}
	if(events_) {
		free(events_);
		events_ = NULL;
	}
}

void	AcDispatcher::RecvPerform(job *job)
{
	AcHandle	*handle	= (AcHandle*)job->user_data;
	DEBUG_MSG("RecvPerform fd: %d",handle->fd());
	int ret	= handle->Recv();
	if(ret < 0) {
		handle->Close();
	}

}

void	AcDispatcher::SendPerform(job *job)
{
	AcHandle	*handle = (AcHandle*)job->user_data;
	DEBUG_MSG("SendPerform fd: %d",handle->fd());
	int	ret	= handle->Send();
	if(ret < 0) {
		handle->Close();
	}
}

int	AcDispatcher::Work(JobFunc func, AcHandle* handle)
{
	job_t job;
	job.job_function = (void*)func;
	job.user_data	= (void*)handle;
	func(&job);
	return 0;
}

void	AcDispatcher::Run()
{
	int nfds = 0, i;
	unsigned int events;
	//unsigned int events_rw = EPOLLIN |EPOLLOUT;
	AcHandle	*handle;
	int	ret;

	while(!shut_down_) {
		
		nfds	= epoll_wait(epfd_, events_, EVENT_NUMBER, time_out_);
		DEBUG_MSG("nfds:%d",nfds);
		if(nfds < 0) {
			if(errno == EINTR) {
				usleep(10);
				continue;
			}
			ERROR_OUT("epoll wait error, %s", strerror(errno));
			Destroy();
			return;
		}

		for(i=0; i<nfds; ++i) 
		{
			events = events_[i].events;
			handle = (AcHandle*)events_[i].data.ptr;

			if(handle->fd() < 1) {
				DEBUG_MSG("handle close at epoll wait");
				continue;
			}

			//if(events_rw & events) {
				if(events & EPOLLIN) {
					/* test Recv */
					ret = Work(RecvPerform, handle);
					//ret = handle->Recv();
					if(ret < 0) {
						Destroy();
						//continue;
						return;
					}
				} else if(events & EPOLLOUT) {
					/* test Send */
					ret = Work(SendPerform, handle);
					//ret = handle->Send();
					if(ret < 0) {
						Destroy();
					}
				} else {
					handle->Close();
				}
			//} else {
				//handle->Close();
				//return;
			//}
			
		}
	}
}

