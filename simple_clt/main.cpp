#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <signal.h>

#include "utils.h"
#include "net_common.h"
#include "ac_connect_handle.h"

static	void	sighandler(int signal)
{
	/* server exit */
	printf("server exit now\n");
	exit(1);
}

static	void	set_signal()
{
	sigset_t	sigset;
	bzero(&sigset,sizeof(sigset_t));

	sigemptyset(&sigset);
	struct	sigaction	siginfo;
	bzero(&siginfo,sizeof(siginfo));
	siginfo.sa_handler	=	sighandler;
	siginfo.sa_mask	=	sigset;
	siginfo.sa_flags	=	SA_RESTART;

	signal(SIGPIPE, SIG_IGN);	//dont auto close

	sigaction(SIGINT, &siginfo, NULL);
	sigaction(SIGTERM, &siginfo, NULL);
}

#include "ac_dispatcher.h"
int main(int argc, char *argv[])
{
	set_signal();

	AcDispatcher dispatcher;
	dispatcher.set_fd_size(1024);
	if(dispatcher.Init() < 0) {
		ERROR_OUT("dispatcher init error");
		return -1;
	}
	dispatcher.Start();

	sockaddr_in addr = {0};
	int fd = new_tcp_client("8.8.8.8",4444,  &addr);
	if( fd < 0 ) {
		ERROR_OUT("Failed to connect server");
		return -1;
	}

	AcConnectHandle *handle = new AcConnectHandle;
	if(!handle) {
		return -1;
	}
	handle->set_dispatcher(&dispatcher);
	handle->set_fd(fd);
	handle->set_address(addr);
	dispatcher.AddEvent(handle, EVENT_READ);


	usleep(200);
	const char command[] = "svr.command:>";
	printf("%s",command);

	char ch;
	std::string str = "";
	char buf[100] = {0};
	memset(buf, 0x35, 100);
	while(ch = getchar(), ch) {
		//printf("%u, %c\n",ch, ch);
		if(ch == 10) {
			if("quit" == str) break;
			printf("%s",command);
			if(str == "") {
				handle->Send(buf,100);
			} else {
				handle->Send((char*)str.c_str(), str.length());
			}
			str = "";
		} else {
			str+=ch;
		}
	}

	return 0;
}

