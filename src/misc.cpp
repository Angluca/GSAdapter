#include "misc.h"
#include "md5.h"
#include <stdio.h>
#include <string.h>

void GetMD5(char *md5buf, char *data, int datasize)
{
	struct MD5Context md5c;
	char *s = data;
	unsigned char ss[16];
	char tmp[3]={'\0'};
	int i;

	MD5Init( &md5c );
	MD5Update( &md5c, (unsigned char*)s, datasize);
	MD5Final( ss, &md5c );

	for( i=0; i<16; i++ ){
		sprintf(tmp,"%02x", ss[i] );
		strcat(md5buf,tmp);
	}
}

void GetMD5A(char *md5buf, void *data, int datasize)
{
	struct MD5Context md5c;
	unsigned char ss[16];
	char tmp[3]={'\0'};
	int i;

	MD5Init( &md5c );
	MD5Update( &md5c, (unsigned char*)data, datasize);
	MD5Final( ss, &md5c );

	for( i=0; i<16; i++ ){
		sprintf(tmp,"%02x", ss[i] );
		strcat(md5buf,tmp);
	}
}
void GetKey(char *key, unsigned int tm, unsigned int userid,  char *username)
{
	char data[1024] = {0};
	char md5buf[64] = {0};
	sprintf(data, "A0HXZPJJYDJ2YYH86A32YIPX:%u", tm);
	GetMD5(md5buf, data, strlen(data));
	memset(data, 0, sizeof(data));

	sprintf(data, "%suserid=%u&username=%s&time=%u&sourceid=QILIYI&fcm=1", md5buf, userid, username, tm);
	memset(md5buf, 0, sizeof(md5buf));

	GetMD5(key, data, strlen(data));
}

void GetSignKey(char *szUsername, char *szOrderId, int iFeetAmt, unsigned int tm, char *pk, char *outKey)
{
	char data[1024] = {0};
	snprintf(data, 1024, "%s%s%d%u%s", szUsername, szOrderId, iFeetAmt, tm, pk);
	GetMD5(outKey, data, strlen(data));
}

void GetManKey(unsigned int tm, char *szUsername, char *szPwd, char *outpwdmd5, char *outkey)
{
	char data[1024] = {0};
	char md5buf1[64] = {0}, md5buf2[64] = {0};
	sprintf(data, "%u", tm);
	//	GetMD5(md5buf1, data);
	GetMD5A(md5buf1, &tm, sizeof(unsigned int));

	memset(data, 0, sizeof(data));
	sprintf(data, "%s", szUsername);
	GetMD5(md5buf2, data, strlen(data));

	memset(data, 0, sizeof(data));
	sprintf(data, "%s", szPwd);
	GetMD5(outpwdmd5, data, strlen(data));

	memset(data, 0, sizeof(data));
	sprintf(data, "%s%s%s1efca3d3d2287db6eb5a8f2ef29eee62", md5buf1, md5buf2, outpwdmd5);
	GetMD5(outkey, data, strlen(data));
}

