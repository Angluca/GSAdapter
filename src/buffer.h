#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include "noncopyable.h"
#include <arpa/inet.h>
#include <memory.h>

#include "utils.h"

const unsigned int g_socket_buf_size = sizeof(unsigned short);
class Buffer
{
	NONCOPYABLE(Buffer);
	static const unsigned int	kBufferSize = 1024;
public:
	//constructor and destructor
	Buffer(void);
	virtual ~Buffer(void);

	inline void	Clear() {
		std::vector<char>	tmpVec(kBufferSize);
		buffer_.swap(tmpVec);
		read_index_ = write_index_ = 0;
		socket_buffer_size_ = 0;
		is_ready_ = false;
	}
	inline char*	content() {
		return	&(buffer_[0]);
	}
	inline char*	read_content() {
		return	&(buffer_[read_index_]);
	}
	inline char*	write_content() {
		return	&(buffer_[write_index_]);
	}
	unsigned int	size() {
		return	write_index_ - read_index_;
	}
	inline bool	is_read_complete() {
		return	read_index_ == write_index_;
	}

	inline void	increase_read_index(int size) {
		read_index_ += size;
	}
	inline void	increase_write_index(int size) {
		write_index_ += size;
	}
	inline unsigned int read_index() {
		return read_index_;
	}
	inline unsigned int write_index() {
		return write_index_;
	}

	int	BufferCopy(char* buf, unsigned int size);

	int	Recv(int fd);
	int	Send(int fd);

	void	Resize(unsigned int resize);

	/* ushort */
	void	SerializeFromUshort(unsigned short value);
	unsigned short	DeserializeFromUshort();

	/* int */
	void	SerializeFromInt(int value);
	int		DeserializeFromInt();

	/* float */
	void	SerializeFromFloat(float value);
	float	DeserializeFromFloat();

	/* uint */
	void	SerializeFromUint(unsigned int value);
	unsigned int	DeserializeFromUint();

	/* buffer */
	void	SerializeFromBuffer(unsigned char *buf, int len);
	void	DeserializeFromBuffer(unsigned char *buf, int len);

	/* byte */
	void	SerializeFromByte(unsigned char ch);
	unsigned char	DeserializeFromByte();

	inline unsigned int	socket_buffer_size() {
		return	socket_buffer_size_;
	}
	inline int	is_write_complete() {
		if(socket_buffer_size_ == write_index_) {
			return 1;
		} else if(socket_buffer_size_ < write_index_){
			ERROR_OUT("is write complete failed");
			return -1;
		}
		return	0;
	}
	inline void	set_socket_buffer_size() {
		set_socket_buffer_size(write_index_);
	}
	int	RecvShortBufferSize(int fd);
	int SendShortBufferSize(int fd);

	void set_cmd(unsigned short cmd) {cmd_ = cmd;}
	unsigned short cmd() {return cmd_;}
private:
	inline void ready_not() { is_ready_ = false; }
	inline void ready_ok() { is_ready_ = true; }
	inline bool is_ready() { return is_ready_; }

	inline void	set_socket_buffer_size(unsigned int value) {
		socket_buffer_size_	= value;
		//memcpy((char*)&buffer_[0], &value, g_socket_buf_size);
	}
	std::vector<char>	buffer_;
	unsigned int	read_index_;
	unsigned int	write_index_;
	unsigned int socket_buffer_size_;
	bool is_ready_;

	unsigned short cmd_;
};


#endif
