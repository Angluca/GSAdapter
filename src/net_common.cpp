/* create by Angluca
 * 05/06/12 11:47:46 */
#include 	"net_common.h"
#include    <unistd.h>
#include    <sys/socket.h>
#include    <arpa/inet.h>
#include    <fcntl.h>
#include    <memory.h>
#include    <errno.h>
#include 	<netdb.h>

#include    "svr_define.h"
#include    "utils.h"

int		net_setnonblock(int	fd)
{
	//int opts=fcntl(fd,F_GETFL);
	//if(opts < 0)
	//{
		//ERROR_OUT("fcntl GETFL failed!\n");
		//return -1;
	//}
	//opts|=O_NONBLOCK;
	//if(fcntl(fd,F_SETFL,opts) < 0)
	//{
		//ERROR_OUT("fcntl SETFL failed!\n");
		//return -1;
	//}
	if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)|O_NONBLOCK) == -1) {
		ERROR_OUT("Failed to set nonblock fd: %d", fd);
		return -1;
	}

	return 0;
}

void	net_setaddress(const char* ip, int port, struct sockaddr_in* addr)
{
	bzero(addr,sizeof(sockaddr_in));
	addr->sin_family=AF_INET;
	inet_pton(AF_INET,ip,&(addr->sin_addr));
	addr->sin_port=htons(port);
}

int		addr_to_string(char* strDec, sockaddr_in* addr)
{
	//if(strDec == NULL)
		//return -1;

	//char ip[128];
	char ip[30];
	inet_ntop(AF_INET,&(addr->sin_addr),ip,sizeof(ip));
	sprintf(strDec,"%s:%d",ip,ntohs(addr->sin_port));

	return	0;
}

int		new_tcp_server(int port, struct sockaddr_in* paddr /* NULL */)
{
	int	fd	=	socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in	addr;
	sockaddr_in *p;
	if(paddr == NULL) p	=	&addr;
	else	p	=	paddr;

	int ok=1;
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&ok,sizeof(ok));

	if(net_setnonblock(fd) < 0)
	{
		close(fd);
		return	-1;
	}

	p->sin_family	=	AF_INET;
	p->sin_port	=	htons(port);
	p->sin_addr.s_addr = INADDR_ANY;   

	if( bind(fd,(struct sockaddr*)p,sizeof(struct sockaddr)) == -1 )
	{
		ERROR_OUT("bind failed");
		return	-1;
	}

	if( listen(fd,MAX_CONNECTION_BACKLOG) == -1 )
	{
		ERROR_OUT("listen failed");
		return	-1;
	}

	return	fd;
}

int		new_tcp_client(const char* ip, int port, struct sockaddr_in* paddr /*= 0*/)
{
	int	fd	=	socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in	addr;
	sockaddr_in *taddr = paddr;
	if(NULL == taddr) {
		taddr = &addr;
	}
	net_setaddress(ip,port,taddr);
	if(net_setnonblock(fd) < 0)
	{
		close(fd);
		return	-1;
	}

	if(connect(fd,(struct sockaddr*)taddr, sizeof(sockaddr_in)) == -1)
	{
		if(errno != EINPROGRESS)
		{
			ERROR_OUT("connect failed");
			return	-1;
		}
	}

	return	fd;
}

int		net_accept(int listen_fd, sockaddr_in * addr)
{
	int	clt_fd, errno_code;
	static	socklen_t	addr_len	=	sizeof(sockaddr_in);

	do {
		clt_fd	=	accept(listen_fd, (sockaddr*)addr, &addr_len);
		if(clt_fd > 0)	break;

		errno_code	=	errno;
		if(clt_fd < 0){

			if(errno_code == EINTR){
				continue;
			}else if(errno_code == EAGAIN){
				return	0;
			}else{
				ERROR_OUT("Failed to accept errno: %s",strerror(errno_code));
				return	-1;
			}

		}
	}while(1);

	if(net_setnonblock(clt_fd) < 0)
	{
		close(clt_fd);
		return	-1;
	}

	return	clt_fd;
}

int		net_sendn(int fd, void* p, int n)
{
	char*	pbuf	=	(char*)p;
	int	ret;
	int	total	=	n;
	int	errno_code;
	while(total > 0)
	{
		ret	=	send(fd,pbuf,total,0);
		if(ret > 0)
		{
			total	-=	ret;
			pbuf	+=	ret;
			continue;
		}
		//if(ret	==	0){
			//DEBUG_MSG("send close: %s", strerror(errno_code));
			//return	0;
			////return	-1;
		//}

		errno_code	=	errno;

		if(errno_code == EINTR)
			continue;
		if(errno_code  ==  EAGAIN ||  errno_code  ==  EWOULDBLOCK) {
			//usleep(100);
			//continue;
			break;
		}

		ERROR_OUT("fd:%d send errno:%d,%s, ret:%d",fd,errno_code,strerror(errno_code),ret);
		return	-1;
	}
	return	(n-total);
}

int		net_recvn(int fd,void* p,int n)
{
	ssize_t	nread;
	size_t	nleft = n;
	char	*ptr = (char*)p;

	//ptr = (char*)p;
	//nleft = n;

	int		errno_code;

	while (nleft > 0 && nleft > 0) {
		nread = read(fd, ptr, nleft);

		if(nread > 0)
		{
			nleft -= nread;
			ptr   += nread;
			continue;
		}
		errno_code	=	errno;

		if (nread == 0){
			DEBUG_MSG("client close: %s", strerror(errno_code));
			//return 0;				[> EOF <]
			return -1;				/* EOF */
			//break;
		}


		if (errno_code  ==  EAGAIN  ||  errno_code  ==  EWOULDBLOCK) {
			break ;
		}

		if (errno_code == EINTR) {
			continue;		/* and call read() again */
		}

		ERROR_OUT("fd:%d recv errno:%d,%s, ret:%d",fd,errno_code,strerror(errno_code),(int)nread);
		return(-1);
	}
	return(n - nleft);		/* return >= 0 */
}

bool	net_host2ip(const char* hostname, char* ip, int len)
{
	struct hostent *host = gethostbyname(hostname);
	if(0 == host) return false;
	snprintf(ip, len, "%d.%d.%d.%d",
			( host->h_addr_list[0][0] & 0xff ),
			( host->h_addr_list[0][1] & 0xff ),
			( host->h_addr_list[0][2] & 0xff ),
			( host->h_addr_list[0][3] & 0xff ));
	return true;
}

