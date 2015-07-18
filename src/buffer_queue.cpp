#include "buffer_queue.h"

#include "buffer.h"
#include "mutex_lock.h"

#include <iterator>
#include <assert.h>


//constructor
BufferQueue::BufferQueue(void)
{
}

//destructor
BufferQueue::~BufferQueue(void)
{
	//Clear();
}

void	BufferQueue::Clear()
{
	{
		MutexLockGuard(&lock_);
		BufferList::const_iterator pos	= buffer_list_.begin();
		BufferList::const_iterator end_pos	= buffer_list_.end();
		printf("queue:");
		for(; pos != end_pos; ++pos) {
			//printf("%x,", (unsigned int)*pos);
			delete	(*pos);
		}
		printf("//\n");
		buffer_list_.clear();
	}
}

Buffer*	BufferQueue::PopBuffer()
{
	if(buffer_list_.empty()) {
		return NULL;
	}

	Buffer	*buffer;
	{
		MutexLockGuard(&lock_);
		buffer	= buffer_list_.front();
		buffer_list_.pop_front();
	}
	return buffer;
}

Buffer* BufferQueue::FrontBuffer()
{
	if(buffer_list_.empty()) {
		return NULL;
	}

	Buffer	*buffer;
	{
		MutexLockGuard(&lock_);
		buffer	= buffer_list_.front();
	}
	return buffer;
}

void	BufferQueue::PushBuffer(Buffer	*buffer)
{
	assert(buffer);
	{
		MutexLockGuard(&lock_);
		buffer_list_.push_back(buffer);
	}
}

void	BufferQueue::PushFrontBuffer(Buffer *buffer)
{
	assert(buffer);
	{
		MutexLockGuard(&lock_);
		buffer_list_.push_front(buffer);
	}
}

