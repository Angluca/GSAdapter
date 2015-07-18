#ifndef  __AC_HANDLE_H__
#define  __AC_HANDLE_H__

#include "noncopyable.h"
//#include "ac_dispatcher.h"

#include <arpa/inet.h>
#include <memory.h>

enum EventType {
	EVENT_NON,
	EVENT_READ,
	EVENT_WRITE
};

class AcDispatcher;

//class Packet;
//class AcMethodManager;

class AcHandle
{
	NONCOPYABLE(AcHandle);
public:
	AcHandle(): fd_(0), event_(0),
	dispatcher_(NULL),
	shut_down_(false)
	{}

	virtual	~AcHandle() {}

	virtual	int	Recv() = 0;
	virtual	int	Send() = 0;
	virtual int Close()= 0;

	/* event */
	//inline void	set_event(int event) {event_ = event;}
	//virtual int	event() {return event_;}
	/* dispatcher */
	inline void	set_dispatcher(AcDispatcher *dispatcher) {
		dispatcher_ = dispatcher;
	}
	/* fd */
	inline void	set_fd(int fd) {fd_ = fd;}
	inline int	fd() {return fd_;}
	/* address */
	inline void	set_address(sockaddr_in &address) {
		static unsigned int address_len = sizeof(sockaddr_in);
		memcpy(&address_, &address, address_len);
	}
	inline const sockaddr_in& address() {return address_;}
	/* method manager */

protected:
	int	fd_;
	unsigned int	event_;
	sockaddr_in		address_;

	AcDispatcher	*dispatcher_;
	bool	shut_down_;
};

#endif  /*__AC_HANDLE_H__*/
