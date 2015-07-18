#include	<iostream>

#include	<memory.h>
#include	<signal.h>
#include	<gtest/gtest.h>

void	sighandler(int signal)
{
	/* server exit */
	//LOGIN_SERVER->destroy();
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


int main(int argc, char *argv[])
{
	set_signal();
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

