

#include <gtest/gtest.h>
#include <iostream>

#include "buffer.h"

TEST(buffer, example)
{
	//char *in_utf8 = (char*)"utf8字符串";
	//char *in_gb2312 = (char*)"\xbe\xb2\xcc\xac\xc4\xa3\xca\xbd";

	Buffer buffer;
	int nint = 1;
	buffer.SerializeFromInt(nint);
	nint = buffer.DeserializeFromInt();
	printf("nint = %d\n", nint);
	unsigned short nshort = 2;
	buffer.SerializeFromUshort(nshort);
	nshort = buffer.DeserializeFromUshort();
	printf("nshort = %d\n", nshort);
	float nfloat = 3.14;
	buffer.SerializeFromFloat(nfloat);
	nfloat = buffer.DeserializeFromFloat();
	printf("nfloat = %f\n", nfloat);
	unsigned char buf[] = "test.";
	unsigned short bufsize = sizeof(buf);
	buffer.SerializeFromBuffer(buf, bufsize);
	buffer.DeserializeFromBuffer(buf, bufsize);
	printf("buf = %s,size = %d \n", buf, bufsize);
}
