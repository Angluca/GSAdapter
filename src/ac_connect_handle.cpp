#include "ac_connect_handle.h"
#include "utils.h"
#include "net_common.h"
#include "svr_define.h"
#include "buffer_queue.h"
#include "buffer.h"
#include "ac_dispatcher.h"
#include "protocol_method.h"
#include "sql_controller.h"

//#include <arpa/inet.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <vector>

//constructor
AcConnectHandle::AcConnectHandle():
	connect_result_(0)
	,protocol_(NULL)
	,sql_(NULL)
{
	gs_id_ = "";
	gs_name_ = "";
	gs_pkey_ = "";
	source_id_ = "";
	ip_ = "";
	port_ = 0;
	best_ = 0;
	auto_update_ = 0;
	state_ = 0;
	man_id_ = "";
	man_user_ = "";
	man_pwd_ = "";
	man_pkey_ = "";
}

//destructor
AcConnectHandle::~AcConnectHandle()
{
}

int		AcConnectHandle::Recv()
{
	int	recv_ret = recv_buffer_.Recv(fd_);
	if(recv_ret < 0) {
		ERROR_OUT("recv error");
		return -1;
	} 
	//else if(recv_ret == 0){
		//ERROR_OUT("double send same packet?");
		//return 0;
	//}

	int ret	= recv_buffer_.is_write_complete();
	if(ret > 0) {
		//unsigned int socket_size = recv_buffer_.socket_buffer_size();
		//int recv_buffer_size = recv_buffer_.size();

		//DEBUG_MSG("seccess recv, sk_size=%u, buf_size=%d buf=%s", socket_size, recv_buffer_size, recv_buffer_.read_content());
		//DEBUG_MSG("windex=%u rindex=%u", recv_buffer_.write_index(), recv_buffer_.read_index());

		protocol_->CheckCmd(this, &recv_buffer_);

		recv_buffer_.Clear();
	} else if(ret < 0) {
		recv_buffer_.Clear();
		return -1;
	}

	if(buffer_queue_.empty()) {
		dispatcher_->ModEvent(this, EVENT_READ);
	} else {
		dispatcher_->ModEvent(this, /*EVENT_READ | */EVENT_WRITE);
	}
	return 0;
}

int		AcConnectHandle::Send()
{
	Buffer	*buffer;
	MutexLockGuard(&handle_lock_);
	if(!buffer_queue_.empty()) {
		//MutexLockGuard(&handle_lock_);
		//DEBUG_MSG("buffer queue size has %d", buffer_queue_.size());

		while(buffer = buffer_queue_.FrontBuffer(), buffer) {
			DEBUG_MSG("buffer queue size has %d",buffer_queue_.size());
			if(buffer->Send(fd_) < 0) {
				ERROR_OUT("send error");
				buffer_queue_.PopBuffer();
				delete	buffer;
				return -1;
			}
			if(!buffer->is_read_complete()) {
				dispatcher_->ModEvent(this, /*EVENT_READ | */EVENT_WRITE); 
				return 0;
			}
			buffer_queue_.PopBuffer();
			delete	buffer;
		}

		//while(buffer = buffer_queue_.PopBuffer(), buffer) {
			//if(buffer->Send(fd_) < 0) {
				//ERROR_OUT("send error");
				//delete	buffer;
				//return -1;
			//}
			//if(!buffer->is_read_complete()) {
				//buffer_queue_.PushFrontBuffer(buffer);
				//dispatcher_->ModEvent(this, EVENT_READ | EVENT_WRITE); 
				//return 0;
			//}
			//delete	buffer;
		//}
	}	

	dispatcher_->ModEvent(this, EVENT_READ);
	return 0;
}

int		AcConnectHandle::Send(char* buf, int len)
{
	Buffer	*buffer	=	new(std::nothrow)	Buffer;
	if(NULL == buffer) {
		EXIT_APP("Failed to allocate memory.");
		return -1;
	}
	buffer->BufferCopy(buf, len);
	buffer_queue_.PushBuffer(buffer);

	dispatcher_->ModEvent(this, /*EVENT_READ | */EVENT_WRITE);
	return 0;
}
int AcConnectHandle::Send(Buffer *buffer)
{
	assert(buffer);
	buffer_queue_.PushBuffer(buffer);
	dispatcher_->ModEvent(this, /*EVENT_READ | */EVENT_WRITE);
	return 0;
}

int 	AcConnectHandle::Close()
{
	
	assert(dispatcher_);

	DEBUG_MSG("into close()");
	MutexLockGuard(&handle_lock_);
	if(fd_) {
		set_unconnect();
		shutdown(fd_, SHUT_WR);
		close(fd_);

		dispatcher_->DelEvent(fd_);
		fd_	= 0;

		DEBUG_MSG("into close() will clear");

		recv_buffer_.Clear();
		buffer_queue_.Clear();
	}
	DEBUG_MSG("into close() will crush?");
	return 0;
}

void AcConnectHandle::Send_CMD_CONNECT_SERVER()
{
	protocol_->Send_CMD_CONNECT_SERVER(this, man_id_.c_str(), man_user_.c_str(), man_pwd_.c_str());
}

void AcConnectHandle::Send_CMD_MAN_GET_GS_LOAD()
{
	Connect();
	protocol_->Send_CMD_MAN_GET_GS_LOAD(this);
}
void AcConnectHandle::Send_CMD_MAN_FEED(int userid, const char* username, int amt, const char* order)
{
	Connect();
	protocol_->Send_CMD_MAN_FEED(this, userid, username, amt, order, man_pkey_.c_str());
}

int AcConnectHandle::Run(unsigned long loop)
{
	if(Connect() < 0) {
		 return -1;
	}

	//if(!sql_->KeepConnect()) {
		//return -1;
	//}

	//if(connect_result_ < 1) {
		//DEBUG_MSG("must connect server");
		//return -1;
	//}

	char sql_buf[MAX_BUFFER_SIZE] = {0};
	//if(!(loop%60)) {
		//PRINT_LOG(gs_id_.c_str(), "tms = %lu", loop);
	//}
	if(!(loop % 5)) {
		Send_CMD_MAN_GET_GS_LOAD();
	}
	snprintf(sql_buf, MAX_BUFFER_SIZE, "select h_userid, recharge_amt, orderid from ca_recharge_record where feed_result = 200 and upper(gsid) = upper('%s') order by id", gs_id_.c_str());
	//snprintf(sql_buf, MAX_BUFFER_SIZE, "select userid, recharge_amt, orderid from ca_recharge_record_91 where upper(gsid) = upper('%s') order by id", gs_id_.c_str());
	{
		MutexLockGuard(sql_->get_lock());
		if(!sql_->Query(sql_buf)) {
			PRINT_LOG(gs_id_.c_str(), "Get all unrecharge info error! msg: %s", sql_->GetErrorMsg().c_str());
		} else {
			std::vector<std::string> userid_arr, amt_arr, order_arr;
			int count = sql_->RowCount();
			if(!(loop%60) && count > 0) {
					PRINT_LOG(gs_id_.c_str(), "unrecharge count = %d", count);
			}

			MYSQL_ROW row;
			if(count > 0) {
				while(row = sql_->NextRow(), row) {
					userid_arr.push_back(row[0]);
					amt_arr.push_back(row[1]);
					order_arr.push_back(row[2]);
				}
			}
			sql_->ReleaseRes();

			count = userid_arr.size();
			int userid, amt;
			std::string order;
			//std::string username;
			char username[64] = {0};
			for(int i = 0; i < count; ++i) {
				userid = atoi(userid_arr.at(i).c_str());
				amt = atoi(amt_arr.at(i).c_str());
				order = order_arr.at(i);
				//memset(username, 0, 64);
				snprintf(username, 64, "HL%d", userid);
				Send_CMD_MAN_FEED(userid, username, amt, order.c_str());

				PRINT_LOG(gs_id_.c_str(), "username: %s, userid: %d, order: %s, amt: %d, Send_CMD_MAN_FEED: %d", username, userid, order.c_str(), amt, connect_result_);

				//if(fd_ > 0) {
					////memset(sql_buf, 0, MAX_BUFFER_SIZE);
					//snprintf(sql_buf, MAX_BUFFER_SIZE, "update ca_recharge_record_91 set state = 1 where userid = %d and upper(orderid) = upper('%s')", userid, order.c_str());
					//sql_->Excute(sql_buf);
				//}
			}

		}
	}

	return 0;
}

int AcConnectHandle::Connect()
{
	if(fd_ < 1) {
		sockaddr_in addr = {0};
		int fd = new_tcp_client(ip_.c_str(),port_,  &addr);
		if( fd < 0 ) {
			ERROR_OUT("Failed to connect GS[%s:%d]server", ip_.c_str(), port_);
			return -1;
		}
		set_fd(fd);
		set_address(addr);
		dispatcher_->AddEvent(this, EVENT_READ);
	}

	if(fd_ && connect_result_ < 1) {
		Send_CMD_CONNECT_SERVER();
		usleep(1000);
		return -1;
	}
	return 0;
}

