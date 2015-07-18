#ifndef ACCONNECTHANDLE_H
#define ACCONNECTHANDLE_H

#include "noncopyable.h"
#include "ac_handle.h"
#include "buffer_queue.h"
#include "buffer.h"
#include "packet_protocol.h"

#include <string>
//#include "mutex_lock.h"

class SqlController;
class ProtocolMethod;
class Buffer;
class AcConnectHandle: public AcHandle
{
	NONCOPYABLE(AcConnectHandle);
public:
	//constructor and destructor
	AcConnectHandle();
	~AcConnectHandle();

	virtual int	Recv();
	virtual int Send();
	virtual int	Close();
	int	Send(char* buf, int len);
	int Send(Buffer *buffer);

	int connect_result() {return connect_result_;}
	void set_connect_result(int value) {connect_result_ = value;}
	void set_connect() {connect_result_ = 1;}
	void set_unconnect() {connect_result_ = 0;}

	void set_protocol_method(ProtocolMethod *method) {protocol_ = method;}

	std::string gs_id_;
	std::string gs_name_;
	std::string gs_pkey_;
	std::string source_id_;
	std::string ip_;
	int port_;
	int best_;
	int auto_update_;
	int state_;
	std::string man_id_;
	std::string man_user_;
	std::string man_pwd_;
	std::string man_pkey_;

	void Send_CMD_CONNECT_SERVER();
	void Send_CMD_MAN_GET_GS_LOAD();
	//void Send_CMD_MAN_FEED();
	void Send_CMD_MAN_FEED(int userid, const char* username, int amt, const char* order);

	int Run(unsigned long loop);
	int Connect();
	void set_sql(SqlController *sql) {sql_ = sql;}
private:
	//typedef	TemplateMap<uint32_t, Callback*>CallbackMap;
	PacketProtocol	recv_buffer_;
	BufferQueue	buffer_queue_;
	int connect_result_;
	//CallbackMap	callback_map_;
	MutexLock	handle_lock_;

	ProtocolMethod *protocol_;
	SqlController *sql_;
};

#endif

