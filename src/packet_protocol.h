#ifndef PACKET_PROTOCOL_H
#define PACKET_PROTOCOL_H


#include "noncopyable.h"
#include "buffer.h"

#include <string>

const unsigned char PB_INT = 0;
const unsigned char PB_UINT = 1;
const unsigned char PB_FLOAT = 2;
const unsigned char PB_STRING = 3;
const unsigned char PB_ARRAY = 9;
//#define  PB_INT 0
//#define  PB_UINT 1
//#define  PB_FLOAT 2
//#define  PB_STRING 3
//#define  PB_ARRAY 9

class PacketProtocol: public Buffer
{
	NONCOPYABLE(PacketProtocol);
public:
	//constructor and destructor
	PacketProtocol(void);
	~PacketProtocol(void);

	bool SetCmd(unsigned short value);
	bool GetCmd(unsigned short &value);

	bool InsertInt(int value);
	bool InsertUint(unsigned int value);
	bool InsertFloat(float value);
	bool InsertString(char* buf, unsigned short len);
	bool InsertString(const std::string &str);

	bool InsertIntArray(int *array, unsigned short count);
	bool InsertUintArray(unsigned int *array, unsigned short count);
	bool InsertFloatArray(float *array, unsigned short count);

	bool GetInt(int &value);
	bool GetUint(unsigned int &value);
	bool GetFloat(float &value);
	bool GetString(char *buf, unsigned short &len);
	bool GetString(std::string &str);

	bool GetIntArray(int *array, unsigned short &count);
	bool GetUintArray(unsigned int *array, unsigned short &count);
	bool GetFloatArray(float *array, unsigned short &count);
};



#endif
