#include "packet_protocol.h"

#include "utils.h"
#include "svr_define.h"

#include <assert.h>

//constructor
PacketProtocol::PacketProtocol(void)
{
}

//destructor
PacketProtocol::~PacketProtocol(void)
{
}

bool PacketProtocol::SetCmd(unsigned short value)
{
	//SerializeFromUshort(value);
	set_cmd(value);
	return true;
}

bool PacketProtocol::GetCmd(unsigned short &value)
{
	value = cmd();
	return true;
}

bool PacketProtocol::InsertInt(int value)
{
	SerializeFromByte(PB_INT);
	SerializeFromInt(value);
	return true;
}
bool PacketProtocol::InsertUint(unsigned int value)
{
	SerializeFromByte(PB_UINT);
	SerializeFromUint(value);
	return true;
}
bool PacketProtocol::InsertFloat(float value)
{
	SerializeFromByte(PB_FLOAT);
	SerializeFromFloat(value);
	return true;
}
bool PacketProtocol::InsertString(char* buf, unsigned short len)
{
	SerializeFromByte(PB_STRING);
	SerializeFromUshort(len);
	SerializeFromBuffer((unsigned char*)buf, len);
	return true;
}
bool PacketProtocol::InsertString(const std::string &str)
{
	SerializeFromByte(PB_STRING);
	SerializeFromUshort((unsigned short)str.length());
	SerializeFromBuffer((unsigned char*)str.c_str(), str.length());
	return true;
}
bool PacketProtocol::InsertIntArray(int *array, unsigned short count)
{
	SerializeFromByte(PB_ARRAY);
	SerializeFromByte(PB_INT);
	SerializeFromUshort(count);
	SerializeFromBuffer((unsigned char*)array, count * g_int_size);
	return true;
}
bool PacketProtocol::InsertUintArray(unsigned int *array, unsigned short count)
{
	SerializeFromByte(PB_ARRAY);
	SerializeFromByte(PB_UINT);
	SerializeFromUshort(count);
	SerializeFromBuffer((unsigned char*)array, count * g_uint_size);
	return true;
}
bool PacketProtocol::InsertFloatArray(float *array, unsigned short count)
{
	SerializeFromByte(PB_ARRAY);
	SerializeFromByte(PB_FLOAT);
	SerializeFromUshort(count);
	SerializeFromBuffer((unsigned char*)array, count * g_float_size);
	return true;
}

bool PacketProtocol::GetInt(int &value)
{
	unsigned char type = DeserializeFromByte();
	if(PB_INT != type) return false;
	value = DeserializeFromInt();
	return true;
}
bool PacketProtocol::GetUint(unsigned int &value)
{
	unsigned char type = DeserializeFromByte();
	if(PB_UINT != type) return false;
	value = DeserializeFromUint();
	return true;
}
bool PacketProtocol::GetFloat(float &value)
{
	unsigned char type = DeserializeFromByte();
	if(PB_FLOAT != type) return false;
	value = DeserializeFromFloat();
	return true;
}
bool PacketProtocol::GetString(char *buf, unsigned short &len)
{
	assert(buf);
	unsigned char type = DeserializeFromByte();
	if(PB_STRING != type) return false;
	unsigned short count = DeserializeFromUshort();
	if(len < count) return false;
	len = count;
	DeserializeFromBuffer((unsigned char*)buf, count);
	return true;
}
bool PacketProtocol::GetString(std::string &str)
{
	unsigned char type = DeserializeFromByte();
	if(PB_STRING != type) return false;
	unsigned short count = DeserializeFromUshort();
	char buf[MAX_BUFFER_SIZE] = {0};
	DeserializeFromBuffer((unsigned char*)buf, count);
	str = buf;
	return true;
}

bool PacketProtocol::GetIntArray(int *array, unsigned short &count)
{
	assert(array);
	unsigned char type = DeserializeFromByte();
	if(PB_ARRAY != type) return false;
	type = DeserializeFromByte();
	if(PB_INT != type) return false;
	unsigned short _count = DeserializeFromUshort();
	if(count < _count) return false;
	count = _count;
	DeserializeFromBuffer((unsigned char*)array, _count * g_int_size);
	return true;
}
bool PacketProtocol::GetUintArray(unsigned int *array, unsigned short &count)
{
	assert(array);
	unsigned char type = DeserializeFromByte();
	if(PB_ARRAY != type) return false;
	type = DeserializeFromByte();
	if(PB_UINT != type) return false;
	unsigned short _count = DeserializeFromUshort();
	if(count < _count) return false;
	count = _count;
	DeserializeFromBuffer((unsigned char*)array, _count * g_uint_size);
	return true;
}
bool PacketProtocol::GetFloatArray(float *array, unsigned short &count)
{
	assert(array);
	unsigned char type = DeserializeFromByte();
	if(PB_ARRAY != type) return false;
	type = DeserializeFromByte();
	if(PB_FLOAT != type) return false;
	unsigned short _count = DeserializeFromUshort();
	if(count < _count) return false;
	count = _count;
	DeserializeFromBuffer((unsigned char*)array, _count * g_float_size);
	return true;
}

