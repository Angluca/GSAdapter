#ifndef AC_DISPATCHER_H
#define AC_DISPATCHER_H

#include "ac_thread.h"
#include "noncopyable.h"

struct	epoll_event;
typedef struct	job {
	void *job_function;
	void *user_data;
}job_t;

#define  EVENT_NUMBER 8
class AcHandle;
class AcDispatcher: public AcThreadWorker
{
	NONCOPYABLE(AcDispatcher);
public:
	//constructor and destructor
	AcDispatcher(void);
	virtual ~AcDispatcher(void);

	int	Init();
	void	Destroy();
	virtual void Run();

	int AddEventEx(AcHandle *handle, int handle_event);
	int	AddEvent(AcHandle *handle, int handle_event);
	int	ModEvent(AcHandle *handle, int handle_event);
	int	DelEvent(AcHandle *handle);
	int	DelEvent(int fd);

	/* fd size */
	inline void	set_fd_size(int n) {fd_size_ = n;}
	inline int	fd_size() {return fd_size_;}

	/* shut_down */
	inline void set_shut_down() {shut_down_ = true;}
	inline bool has_shut_down() {return shut_down_;}

	/* time_out */
	inline void	set_time_out(int time_out) {time_out_ = time_out;}

	/* sem */
protected:
	static void	RecvPerform(job* job);
	static void	SendPerform(job* job);
	typedef	void	(*JobFunc)(job *job);
	int	Work(JobFunc, AcHandle* handle);
	int	epfd_;
	int	fd_size_;
	int	thread_number_;
	epoll_event	*events_;
	bool shut_down_;
	int	time_out_;
};


#endif
