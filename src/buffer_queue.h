#ifndef BUFFER_QUEUE_H
#define BUFFER_QUEUE_H

#include <list>
#include "mutex_lock.h"
#include "noncopyable.h"

//class MutexLock;
class Buffer;

class BufferQueue
{
	NONCOPYABLE(BufferQueue);
public:
	//constructor and destructor
	BufferQueue(void);
	~BufferQueue(void);
	Buffer* FrontBuffer();
	Buffer*	PopBuffer();
	void	PushBuffer(Buffer *buffer);
	void	PushFrontBuffer(Buffer *buffer);
	void	Clear();
	unsigned int size() {
		return buffer_list_.size();
	}
	bool	empty() {
		return buffer_list_.empty();
	}
	MutexLock*	lock() {
		return &lock_;
	}
private:
	typedef	std::list<Buffer*>	BufferList;
	BufferList	buffer_list_;
	MutexLock	lock_;
};


#endif
