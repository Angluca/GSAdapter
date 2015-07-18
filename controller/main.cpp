#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include "errno.h"

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

bool CheckRuning(char *proc)
{
    FILE *pp;

    pid_t pid, me;
    char  cmd[128], buf[128], comm[128];

    if( proc == NULL || *proc == 0 ) return(false);

    sprintf(cmd, "ps -eo comm,pid|grep ^%.100s", proc);

    if( (pp = popen(cmd, "r")) != NULL )
    {
        me = getpid();

        while( ! feof(pp) )
        {
            pid = 0;

            if( fgets(buf, sizeof(buf), pp) == NULL ) break;
            sscanf(buf, "%100s%d", comm, &pid);

            if( (strcmp(comm, proc) == 0) && (pid != me) )
            {
                pclose(pp);
                return(true);
            }
        }
        pclose(pp);
    }

    return(false);
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

#include <string>

int main(int argc, char *argv[])
{
	OS_Daemon();

	if(argc < 4) {
		printf("Usage: app <ip> <db_user> <db_pwd>");
		sleep(3);
		return -1;
	}
	std::string db_ip = trim(argv[1]);
	std::string db_user = trim(argv[2]);
	std::string db_pwd = trim(argv[3]);

	bool bRun; 
	char elf_name[100] = {0};
	std::string app_name = "test_GSAdaptor";
	snprintf(elf_name, 100, "../simple/%s %s %s %s", app_name.c_str(), db_ip.c_str(), db_user.c_str(), db_pwd.c_str());
	FILE *pp;
	while(1) {
		bRun = CheckRuning((char*)app_name.c_str());	
		//printf("running ? %d\n", bRun);
		if(!bRun) {
			if( (pp = popen(elf_name, "r")) != NULL ) {
				pclose(pp);
			}
		}
		sleep(1);
	}
	return 0;
}

