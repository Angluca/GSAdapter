#include "protocol_method.h"
#include "sql_controller.h"

#include "packet_protocol.h"
#include "ac_connect_handle.h"
#include "misc.h"
#include "svr_define.h"

#include <time.h>

#define CMD_KEEP_ALIVE 0
#define CMD_CONNECT_SERVER 1
#define CMD_MAN_FEED 43400
#define CMD_MAN_GET_GS_LOAD 41201

//constructor
ProtocolMethod::ProtocolMethod(const std::string &gsid, SqlController *sql):
	sql_(sql)
	,gs_id_(gsid)
{
}

//destructor
ProtocolMethod::~ProtocolMethod(void)
{
}

int ProtocolMethod::Send_CMD_MAN_GET_GS_LOAD(AcConnectHandle *handle)
{
	DEBUG_MSG("Send_CMD_MAN_GET_GS_LOAD");
	PacketProtocol *protocol = new(std::nothrow) PacketProtocol;
	if(!protocol) {
		EXIT_APP("Failed to allocate memory");
	}
	protocol->SetCmd(CMD_MAN_GET_GS_LOAD);
	handle->Send(protocol);
	return 0;
}

void ProtocolMethod::Check_CMD_MAN_GET_GS_LOAD(PacketProtocol *protocol)
{
	DEBUG_MSG("Check_CMD_MAN_GET_GS_LOAD");
	int value;
	unsigned int uValue;
	protocol->GetUint(uValue);
	protocol->GetUint(uValue);

	protocol->GetInt(value);
	//UpdateGSLoad(value);
}

int ProtocolMethod::Send_CMD_MAN_FEED(AcConnectHandle *handle, int userID, const char *username, int feed_amt, const char *order, const char* pk)
{
	assert(handle);

	DEBUG_MSG("Send_CMD_MAN_FEED");
	PacketProtocol *protocol = new(std::nothrow) PacketProtocol;
	if(!protocol) {
		EXIT_APP("Failed to allocate memory");
	}
	protocol->SetCmd(CMD_MAN_FEED);
	protocol->InsertUint(userID);
	protocol->InsertString((char*)username, strlen(username));
	protocol->InsertInt(feed_amt);
	protocol->InsertString((char*)order, strlen(order));
	unsigned int tm = time(NULL);
	protocol->InsertUint(tm);

	char signkey[64] = {0};
	GetSignKey((char*)username, (char*)order, feed_amt, tm, (char*)pk, signkey);
	protocol->InsertString(signkey, strlen(signkey));

	handle->Send(protocol);
	return 0;
}

void ProtocolMethod::Check_CMD_MAN_FEED(PacketProtocol *protocol)
{
	DEBUG_MSG("Check_CMD_MAN_FEED");
	unsigned int uValue;
	int value;
	char buf[MAX_BUFFER_SIZE] = {0};
	//char buf_order[MAX_BUFFER_SIZE] = {0};
	unsigned short buf_len = MAX_BUFFER_SIZE;
	//unsigned short buf_order_len = MAX_BUFFER_SIZE;
	/* user id */
	protocol->GetUint(uValue);
	/* user name */
	protocol->GetString(buf, buf_len);
	/* feed amt */
	protocol->GetInt(value);
	/* order id */
	buf_len = MAX_BUFFER_SIZE;
	memset(buf, 0, MAX_BUFFER_SIZE);
	protocol->GetString(buf, buf_len);
	/* result */
	protocol->GetInt(value);
	//if(value == 0) {
		UpdateFeedRecord(uValue, buf, value);
	//}
}

int ProtocolMethod::Send_CMD_CONNECT_SERVER(AcConnectHandle *handle, const char *man_id, const char *username, const char *pwd)
{
	assert(handle);

	DEBUG_MSG("Send_CMD_CONNECT_SERVER");
	PacketProtocol *protocol = new(std::nothrow) PacketProtocol;
	if(!protocol) {
		EXIT_APP("Failed to allocate memory");
	}

	protocol->SetCmd(CMD_CONNECT_SERVER);

	protocol->InsertString((char*)man_id, strlen(man_id));
	char key[64] = {0};
	char pwdmd5[64] = {0};
	unsigned int tm = time(NULL);
	//unsigned int tm = 1234567890;
	GetManKey(tm, (char*)username, (char*)pwd, pwdmd5, key);
	DEBUG_MSG("manid:%s, username:%s, pwd:%s, pwdmd5:%s, key:%s", man_id, username, pwd, pwdmd5, key);
	protocol->InsertString(key, strlen(key));
	protocol->InsertString((char*)username, strlen(username));
	protocol->InsertString(pwdmd5, strlen(pwdmd5));
	protocol->InsertUint(tm);

	handle->Send(protocol);
	return 0;
}

void ProtocolMethod::Check_CMD_CONNECT_SERVER(AcConnectHandle *handle, PacketProtocol *protocol)
{
	DEBUG_MSG("Check_CMD_CONNECT_SERVER");
	int value;
	protocol->GetInt(value);
	DEBUG_MSG("CMD: CMD_CONNECT_SERVER result: %d", value);
	handle->set_connect_result(value);
}

void ProtocolMethod::CheckCmd(AcConnectHandle *handle, PacketProtocol *protocol)
{
	unsigned short cmd;
	protocol->GetCmd(cmd);
	switch(cmd) {
		case CMD_CONNECT_SERVER:
			Check_CMD_CONNECT_SERVER(handle, protocol);
			break;
		case CMD_MAN_FEED:
			Check_CMD_MAN_FEED(protocol);
			break;
		case CMD_MAN_GET_GS_LOAD:
			Check_CMD_MAN_GET_GS_LOAD(protocol);
			break;
		default:
			break;
	}
}
int ProtocolMethod::UpdateFeedRecord(unsigned int userID, char *order, int result)
{
	char buf[MAX_BUFFER_SIZE] = {0};
	switch(result) {
		case 0:
			{
				DEBUG_MSG("userID: %d, order: %s, Feed result: Ok!", userID, order);
				snprintf(buf, MAX_BUFFER_SIZE, "update ca_recharge_record set state = 2, feed_result = 0 where h_userid = %d and feed_result = 200 and orderid = \'%s\'", userID, order);
				
				{
					MutexLockGuard(sql_->get_lock());
					sql_->Excute(buf);
				}
			}
			break;
		default:
			{
				ERROR_LOG(gs_id_.c_str(), "userID : %d, order : %s, Feed result: Error (%d) ! \n", userID, order, result);

				snprintf(buf, MAX_BUFFER_SIZE, "update ca_recharge_record set state = 1, feed_result = 201 where h_userid = %d and orderid = \'%s\'", userID, order);
				{
					MutexLockGuard(sql_->get_lock());
					sql_->Excute(buf);
				}
			}
			break;
	}
	return 0;
}
int ProtocolMethod::UpdateGSLoad(int num)
{
	char buf[MAX_BUFFER_SIZE] = {0};
	int state;
	switch(num) {
		case 0 ... 299:
			state = 1;
			break;
		case 300 ... 799:
			state = 2;
			break;
		default:
			state = 3;
			break;
	}

	snprintf(buf, MAX_BUFFER_SIZE, "update ca_gs set state = %d where upper(gsid) = upper('%s') and auto_update = 1", state, gs_id_.c_str());

	{
		MutexLockGuard(sql_->get_lock());
		if(sql_->Excute(buf)) {
			DEBUG_MSG("[%s] Online number = %d", gs_id_.c_str(), num);
		} else {
			ERROR_LOG(gs_id_.c_str(), "Sql exec error: %s, msg: %s", buf, sql_->GetErrorMsg().c_str());
		}
	}

	return 0;
}

