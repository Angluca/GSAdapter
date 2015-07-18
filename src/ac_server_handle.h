#ifndef AC_SERVER_HANDLE_H
#define AC_SERVER_HANDLE_H

#include "ac_handle.h"
#include "noncopyable.h"
//#include "ac_handle_manager.h"

class AcHandleManager;

class AcServerHandle : public AcHandle
{
	NONCOPYABLE(AcServerHandle);
public:
	//constructor and destructor
	AcServerHandle(void);
	~AcServerHandle(void);

	virtual int	Recv();
	virtual int	Send();
	virtual int Close();
	//void	set_connect_manager(AcHandleManager *manager) {
		//connect_manager_	= manager;
	//}
private:
	int		Accept();

	//AcHandleManager	*connect_manager_;
};

#endif
