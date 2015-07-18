#ifndef PROTOCOL_METHOD_H
#define PROTOCOL_METHOD_H

#include "noncopyable.h"
#include <string>

class SqlController;
class AcConnectHandle;
class PacketProtocol;
class ProtocolMethod
{
	NONCOPYABLE(ProtocolMethod);
public:
	//constructor and destructor
	ProtocolMethod(const std::string &gsid, SqlController *sql);
	~ProtocolMethod(void);

	int Send_CMD_MAN_GET_GS_LOAD(AcConnectHandle *handle);
	void Check_CMD_MAN_GET_GS_LOAD(PacketProtocol *protocol);

	int Send_CMD_MAN_FEED(AcConnectHandle *handle, int userID, const char *username, int feed_amt, const char *order, const char* pk);
	void Check_CMD_MAN_FEED(PacketProtocol *protocol);

	int Send_CMD_CONNECT_SERVER(AcConnectHandle *handle, const char *man_id, const char *username, const char *pwd);
	void Check_CMD_CONNECT_SERVER(AcConnectHandle *handle, PacketProtocol *protocol);

	void CheckCmd(AcConnectHandle *handle, PacketProtocol *protocol);
	const std::string& GSID() {return gs_id_;}
private:
	int UpdateFeedRecord(unsigned int userID, char *order, int result);
	int UpdateGSLoad(int num);
	SqlController *sql_;
	std::string gs_id_;
};

#endif

