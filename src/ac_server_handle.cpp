#include "ac_server_handle.h"
#include "net_common.h"
#include "utils.h"
//#include "ac_handle_manager.h"
#include "ac_dispatcher.h"
#include "ac_connect_handle.h"

#include <assert.h>

//constructor
AcServerHandle::AcServerHandle(void)
{
}

//destructor
AcServerHandle::~AcServerHandle(void)
{
}

int		AcServerHandle::Recv()
{
	return Accept();
}

int		AcServerHandle::Send()
{
	return -1;
}

int		AcServerHandle::Accept()
{
	//static socklen_t	s_addr_len = sizeof(sockaddr_in);
	int client_fd;
	sockaddr_in	addr;
	client_fd = net_accept(fd_, &addr);
	if(client_fd < 1) {
		ERROR_OUT("Failed to accept");
		return -1;
	}

	DEBUG_MSG("Accept fd: %d", client_fd);
	AcHandle	*handle	= new AcConnectHandle;
	if(NULL == handle) {
		ERROR_OUT("Failed to allocate memory");
		close(client_fd);
		return -1;
	}

	assert(dispatcher_);

	handle->set_fd(client_fd);
	handle->set_address(addr);
	handle->set_dispatcher(dispatcher_);

	dispatcher_->AddEvent(handle,EVENT_READ);

	return 0;
}
int		AcServerHandle::Close()
{
	/* TODO:insert del_epoll_event after */
	assert(dispatcher_);
	dispatcher_->DelEvent(fd_);
	shutdown(fd_, SHUT_WR);
	close(fd_);
	return 0;
}


