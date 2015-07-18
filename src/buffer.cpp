#include "buffer.h"
#include <memory.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "net_common.h"
//#include "utils.h"
#include "svr_define.h"


template <typename T>
static void	DeserializeBufferToValue(Buffer *buffer, T value, const unsigned int &size)
{
	assert(size > 0);
	assert(size <= buffer->size());

	memcpy((unsigned char*)value, buffer->read_content(), size);
	buffer->increase_read_index(size);
}

template <typename T>
static void	SerializeBufferFromValue(Buffer *buffer, T value, const unsigned int &size)
{
	assert(size > 0);

	memcpy((char*)buffer->write_content(), value, size);
	buffer->increase_write_index(size);
	buffer->set_socket_buffer_size();
}

//constructor
Buffer::Buffer(void):
	read_index_(0),
	write_index_(0),
	socket_buffer_size_(0),
	is_ready_(false),
	cmd_(0)
{
	buffer_.reserve(kBufferSize);
	//SerializeFromUint(0);
}

//destructor
Buffer::~Buffer(void)
{
	buffer_.clear();
}

void	Buffer::Resize(unsigned int resize) 
{
	if(resize < kBufferSize) {
		resize = kBufferSize;
	}
	buffer_.resize(buffer_.capacity() + resize);
	cmd_ = 0;
}

int	Buffer::BufferCopy(char* buf, unsigned int size)
{
	assert(buf);
	assert(size > 0);

	unsigned int	total = size + write_index_;
	if(total > buffer_.capacity()) {
		Resize(total);
	}
	memcpy(write_content(), buf, size);

	write_index_ += size;
	set_socket_buffer_size();
	return 0;
}

int	Buffer::RecvShortBufferSize(int fd)
{
	int	ret = 0;
	unsigned short value;
	unsigned int index = 0;
	while(index < g_socket_buf_size ) {
		ret = net_recvn(fd, (char*) &value + index, g_socket_buf_size - index);
		if(ret < 0) {
			ERROR_OUT("Failed to recv packet size, fd:%d", fd);
			return -1;
		}
		index += ret;
	}

	socket_buffer_size_ = value;
	/* cmd */
	while(index < g_socket_buf_size+2) {
		ret = net_recvn(fd, (char*) &value, g_socket_buf_size + 2 - index);
		if(ret < 0) {
			ERROR_OUT("Failed to recv packet size, fd:%d", fd);
			return -1;
		}
		index += ret;
	}
	cmd_ = value;
	//socket_buffer_size_ = ntohs(value);
	ready_ok();
	return index;
}

int Buffer::SendShortBufferSize(int fd)
{
	int ret = 0;
	//unsigned short value = htons(socket_buffer_size_);
	unsigned short value = socket_buffer_size_;
	unsigned int index = 0;
	while(index < g_socket_buf_size) {
		ret = net_sendn(fd, (char*) &value + index, g_socket_buf_size - index);
		if(ret < 0) {
			ERROR_OUT("Failed to recv packet size, fd:%d", fd);
			return -1;
		}
		index += ret;
	}

	if(cmd_ > 0) {
		value = cmd_;
		while(index < g_socket_buf_size + 2) {
			ret = net_sendn(fd, (char*) &value, g_socket_buf_size + 2 - index);
			if(ret < 0) {
				ERROR_OUT("Failed to recv packet size, fd:%d", fd);
				return -1;
			}
			index += ret;
		}
	}
	ready_ok();
	return index;
}

int	Buffer::Recv(int fd)
{
	if(!is_ready()) {
		int local_ret = RecvShortBufferSize(fd);
		if(local_ret < 1) {
			return -1;
		}
		if(/*socket_buffer_size_ < 1 ||*/
			socket_buffer_size_ > MAX_BUFFER_SOCKET_SIZE) {
			ERROR_OUT("packet size < 1 or > MAX_BUFFER_SIZE, fd:%d, size %u", fd, socket_buffer_size_);
			return -1;
		}
		if(socket_buffer_size_ > (buffer_.capacity() - write_index_)) {
			Resize(socket_buffer_size_);
		}
	}

	int to_recv = socket_buffer_size_ - write_index_;
	DEBUG_MSG("recv continue to_recv = %d, write_index_= %d, socket_buffer_size= %d, buf=%s", to_recv, write_index_, socket_buffer_size_, &buffer_[0]);

	if(to_recv < 1) {
		ERROR_OUT("to recv = 0");
		//return 0;
	}

	int	ret = net_recvn(fd, write_content(), to_recv);
	if(ret < 0) {
		ERROR_OUT("fd: %d recv error, ret:%d", fd, ret);
		return -1;
	}
	write_index_ += ret;

	return ret;
}

int	Buffer::Send(int fd)
{
	if(!is_ready()) {
		if(SendShortBufferSize(fd) < 1) {
			ERROR_OUT("send socket buf size < 1, fd:%d, size %u", fd, socket_buffer_size_);
			return -1;
		}
	}

	int ret;//, send_size = 0;
	DEBUG_MSG("socket buffer size %u, read_index %d", write_index_, read_index_);
	int	size = write_index_ - read_index_;
	//assert(size > 0);
	if(size == 0) {
		DEBUG_MSG("write_index_ - read_index_ = 0?");
		return 0;
	}
	ret = net_sendn(fd, read_content(), size);
	if(ret < 0) {
		ERROR_OUT("fd: %d send error", fd);
		return ret;
	}
	read_index_ += ret;
	//send_size	+= ret;
	//return send_size;
	
	return ret;
}

/* buffer */
void	Buffer::SerializeFromBuffer(unsigned char *buf, int len)
{
	SerializeBufferFromValue(this, buf, len);
}
void	Buffer::DeserializeFromBuffer(unsigned char *buf, int len)
{
	DeserializeBufferToValue(this, buf, len);
}

/* byte */
void	Buffer::SerializeFromByte(unsigned char ch)
{
	SerializeBufferFromValue(this, &ch, 1);
}
unsigned char	Buffer::DeserializeFromByte()
{
	unsigned char ch;
	DeserializeBufferToValue(this, &ch, 1);
	return ch;
}

/* uint */
void	Buffer::SerializeFromUint(unsigned int value)
{
	//value	= htonl(value);
	SerializeBufferFromValue(this, &value, g_uint_size);
}
unsigned int	Buffer::DeserializeFromUint()
{
	unsigned int	value;
	DeserializeBufferToValue(this, &value, g_uint_size);
	return value;
	//return ntohl(value);
}

/* ushort */
void Buffer::SerializeFromUshort(unsigned short value)
{
	//value = htons(value);
	SerializeBufferFromValue(this, &value, g_ushort_size);
}
unsigned short	Buffer::DeserializeFromUshort()
{
	unsigned short	value;
	DeserializeBufferToValue(this, &value, g_ushort_size);
	return value;
	//return ntohs(value);
}

/* int */
void	Buffer::SerializeFromInt(int value)
{
	//value	= htonl(value);
	SerializeBufferFromValue(this, &value, g_int_size);
}
int		Buffer::DeserializeFromInt()
{
	int	value;
	DeserializeBufferToValue(this, &value, g_int_size);
	return value;
}

/* float */
void	Buffer::SerializeFromFloat(float value)
{
	SerializeBufferFromValue(this, &value, g_float_size);
}
float	Buffer::DeserializeFromFloat()
{
	float value;
	DeserializeBufferToValue(this, &value, g_float_size);
	return value;
}

