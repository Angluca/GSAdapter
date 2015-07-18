#ifndef  __NET_COMMON_H__
#define  __NET_COMMON_H__

/* create by Angluca
 * 05/06/12 11:47:46 */
struct	sockaddr_in;

int		addr_to_string(char* strDec, sockaddr_in* addr);

int		new_tcp_server(int port, struct sockaddr_in* paddr = 0);
int		new_tcp_client(const char* ip, int port, struct sockaddr_in* paddr = 0);

int		net_setnonblock(int	fd);
void	net_setaddress(const char* ip, int port, struct sockaddr_in* addr);

int		net_accept(int listen_fd, sockaddr_in * addr);
int		net_sendn(int fd, void* p, int n);
int		net_recvn(int fd,void* p, int n);
bool	net_host2ip(const char* hostname, char* ip, int len);

#endif  /*__NET_COMMON_H__*/

