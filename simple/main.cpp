
#include "utils.h"
#include "net_common.h"
#include "ac_server_handle.h"
#include "sql_controller.h"
#include "svr_define.h"
#include "ac_dispatcher.h"
#include "misc.h"
#include "protocol_method.h"
#include "ac_connect_handle.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <signal.h>
#include <errno.h>
#include <vector>

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

void OS_Daemon()
{
    pid_t pid;

    if ((pid = fork()) < 0)
    {
        fprintf(stderr, "fork1 failed: %d\n", errno);
        exit(-1);
    }

    if (pid)
    {
        exit(0);
    }

    setsid();

//    signal(SIGCLD, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    if ((pid = fork()) < 0)
    {
        fprintf(stderr, "fork2 failed: %d\n", errno);
        exit(-1);
    }

    if (pid)
    {
        exit(0);
    }

/*
    chdir("/");
    umask(0);
*/
}

std::string trim(const std::string& str)
{
	std::string::size_type pos = str.find_first_not_of(' ');
    if (pos == std::string::npos)
    {
        return str;
    }
	std::string::size_type pos2 = str.find_last_not_of(' ');
    if (pos2 != std::string::npos)
    {
        return str.substr(pos, pos2 - pos + 1);
    }
    return str.substr(pos);
}

struct TableCaGS {
	std::string gsid;
	std::string source_id;
	std::string name;
	std::string ip;
	int port;
	std::string pkey;
	int best;
	int auto_update;
	int state;

	std::string man_pkey;
	std::string man_user;
	std::string man_pwd;
};

int main(int argc, char *argv[])
{
	set_signal();
//OS_Daemon();

	if(argc < 4) {
		PRINT_MSG("Usage: app <ip> <db_user> <db_pwd>");
		sleep(3);
		return -1;
	}
	std::string db_ip = trim(argv[1]);
	std::string db_user = trim(argv[2]);
	std::string db_pwd = trim(argv[3]);

	//std::string gs_id = "91S001";
	//std::string _ip = "iwars91.s001.qiliyi.com";
	//int _port = 889;
	//int _autoupdate = 0;
	//std::string _username = "iwars_b";
	//std::string _pwd = "e647c2109914";
	//std::string _pkey = "8DLE1W878UP7SG4OZG7RU0L2";

	std::vector<AcConnectHandle*> handle_vec;
	//char ip_buf[20] = {0};
	//net_host2ip(_ip.c_str(), ip_buf, 20);
	//_ip = ip_buf;
	//DEBUG_MSG("ip = %s", _ip.c_str());

	AcDispatcher dispatcher;
	dispatcher.set_fd_size(1024);
	if(dispatcher.Init() < 0) {
		ERROR_OUT("dispatcher init error");
		return -1;
	}
	dispatcher.Start();

	SqlController *sql = new SqlController;
	//if(!sql->Open("x.x.x.x", "ca_server", "ssdAFmEAFZQSBVY9","crystal_age_db")) {
	if(!sql->Open(db_ip.c_str(), db_user.c_str(), db_pwd.c_str(), "crystal_age_db")) {
		ERROR_OUT("Connect mysql failed");
		return -1;
	} else {
		PRINT_MSG("Connect mysql success.");
	}
	char sql_buf[MAX_BUFFER_SIZE] = {0};

	snprintf(sql_buf, MAX_BUFFER_SIZE,
			"select gsid, source_id, name, ip, port, pkey, best, auto_update, state, man_pkey, man_user, man_pwd from ca_gs");

	if(!sql->Query(sql_buf)) {
		ERROR_OUT("Exec sql error: %s", sql->GetErrorMsg().c_str());
		return -1;
	} else {
		MYSQL_ROW row;
		AcConnectHandle *handle;
		char is_web[2] = {0};
		while( row = sql->NextRow() , row) {

			//gs_id = row[0];
			//DEBUG_MSG("gsid : %s", gs_id.c_str());

			handle = new AcConnectHandle;
			if(!handle) {
				EXIT_APP("Failed to allocate memory");
				return -1;
			}
			handle->set_dispatcher(&dispatcher);
			handle->set_sql(sql);

			handle->gs_id_ = row[0];
			handle->source_id_ = row[1];
			handle->gs_name_ = row[2];
			handle->ip_ = trim(row[3]);
			is_web[0] = row[3][0];
			if(!atoi(is_web)) {
				char ip_buf[20] = {0};
				if(!net_host2ip(handle->ip_.c_str(), ip_buf, 20)) {
					ERROR_OUT("host to ip failed");
					return -1;
				}
				handle->ip_ = ip_buf;
			}
			handle->port_ = atoi(row[4]);
			handle->gs_pkey_ = row[5];
			handle->best_ = atoi(row[6]);
			handle->auto_update_ = atoi(row[7]);
			handle->state_ = atoi(row[8]);
			handle->man_pkey_ = row[9];
			handle->man_user_ = row[10];
			handle->man_pwd_ = row[11];

			handle->man_id_ = handle->gs_id_ + "_MAN";
			//handle->set_fd(fd);
			//handle->set_address(addr);
			//dispatcher.AddEvent(handle, EVENT_READ);

			ProtocolMethod *protocol = new ProtocolMethod(row[0], sql);
			if(!protocol) {
				EXIT_APP("Failed to allocate memory");
				return -1;
			}

			//std::string gs_man = gs_id + "_MAN";

			//handle->ip_ = _ip;
			//handle->gsid_ = gs_id;
			//handle->manid_ = gs_man;
			//handle->port_ = _port;
			//handle->username_ = _username;
			//handle->pwd_ = _pwd;
			//handle->pkey_ = _pkey;

			handle->set_protocol_method(protocol);
			handle->Connect();
			handle_vec.push_back(handle);
		}

		sql->ReleaseRes();
	}
		//handle->Connect();

	//memset(sql_buf, 0, MAX_BUFFER_SIZE);
	//snprintf(sql_buf, MAX_BUFFER_SIZE, "select ip, port, auto_update, man_user, man_pwd, man_pkey from ca_gs");

	//if(!sql->Query(sql_buf)) {
		//ERROR_OUT("Exec sql error: %s", sql->GetErrorMsg().c_str());
		//return -1;
	//} else {
		//MYSQL_ROW row = sql->NextRow();
		//if(NULL == row) {
			//ERROR_OUT("query error");
			//return -1;
		//}
		//_ip = row[0];
		//_port = atoi(row[1]);
		//_autoupdate = atoi(row[2]);
		//_username = row[3];
		//_pwd = row[4];
		//_pkey = row[5];
		//DEBUG_MSG("ip:%s, port:%d, auto:%d, user:%s, pwd:%s, pk:%s",
				//_ip.c_str(), _port, _autoupdate, _username.c_str(),
				//_pwd.c_str(), _pkey.c_str());
		//sql->ReleaseRes();
	//}

	//char ip_buf[20] = {0};
	//if(!net_host2ip(_ip.c_str(), ip_buf, 20)) {
		//ERROR_OUT("host to ip failed");
		//return -1;
	//}
	//_ip = ip_buf;



	/* client */
	//sockaddr_in addr = {0};
	//int fd = new_tcp_client(_ip.c_str(),_port,  &addr);
	//if( fd < 0 ) {
		//ERROR_OUT("Failed to connect GS[%s:%d]server", _ip.c_str(), _port);
		//return -1;
	//}


	//handle->Send_CMD_CONNECT_SERVER();

	unsigned long loop = 0;
	int i=0;
	int n = handle_vec.size();

	if(n < 1) {
		ERROR_OUT("has not server open!");
		return -1;
	}
	//while(!connect_all) {
		//for(i = 0; i < n; ++i) {
			//handle_vec.at(i)->Connect();
		//}

	//}
	//while(handle->connect_result() < 1) {
		//++loop;
		//if(loop > 5) {
			//ERROR_OUT("connect server error");
			//loop = 0;
			//handle->Connect();
		//}
		//sleep(1);
	//}

	sleep(3);
	PRINT_MSG("GSAdaptor running.");

	while(1) {
		++loop;
		for(i = 0; i < n; ++i) {
			handle_vec.at(i)->Run(loop);
			//usleep(20);
		}
		//handle_vec.at(0)->Run(loop);
		//handle->Run(loop);
		sleep(1);
		//if(loop % 600) {
			//handle->Send_CMD_MAN_FEED("HL179770443", 179770443, "CAOI2XWO15ZVJH1348641545");
		//}
	}

	//protocol->Send_CMD_CONNECT_SERVER(handle, gs_man.c_str(), _username.c_str(), _pwd.c_str());
	/* server */
	//sockaddr_in addr = {0};
	//int fd = new_tcp_server(4444, &addr);
	//if( fd < 0 ) {
		//ERROR_OUT("Failed to create server");
	//}
	//AcServerHandle *handle = new AcServerHandle;
	//if(!handle) {
		//return -1;
	//}
	//handle->set_dispatcher(&dispatcher);
	//handle->set_fd(fd);
	//handle->set_address(addr);
	//dispatcher.AddEventEx(handle, EVENT_READ);


	//usleep(200);
	//const char command[] = "svr.command:>";
	//printf("%s",command);

	//char buf[100];
	//GetMD5(buf, buf, 100);
	//char ch;
	//std::string str = "";
	//while(ch = getchar(), ch) {
		////printf("%u, %c\n",ch, ch);
		//if(ch == 10) {
			//if("quit" == str) break;
			//printf("%s",command);
			//str = "";
		//} else {
			//str+=ch;
		//}
	//}

	//handle->Close();
	dispatcher.set_shut_down();
	usleep(1000);
	delete sql;
	//delete handle;

	return 0;
}

