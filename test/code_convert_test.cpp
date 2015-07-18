
#include <gtest/gtest.h>
#include <iostream>

#include "code_convert.h"

TEST(code_convert, example)
{
#define OUTLEN 255
	char *in_utf8 = (char*)"utf8字符串";
	char *in_gb2312 = (char*)"\xbe\xb2\xcc\xac\xc4\xa3\xca\xbd";

	char out[OUTLEN] = {0};

	utf2gb(in_utf8, strlen(in_utf8), out, OUTLEN);
	printf("unicode-->gb2312 out=%s\n",out);

	gb2utf(in_gb2312, strlen(in_gb2312), out, OUTLEN);
	printf("gb2312-->unicode out=%s\n",out);
}
