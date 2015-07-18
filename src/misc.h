#ifndef  __MISC_H__
#define  __MISC_H__

void GetMD5(char *md5buf, char *data, int datasize);

void GetMD5A(char *md5buf, void *data, int datasize);

void GetKey(char *key, unsigned int tm, unsigned int userid,  char *username);

void GetSignKey(char *szUsername, char *szOrderId, int iFeetAmt, unsigned int tm, char *pk, char *outKey);

void GetManKey(unsigned int tm, char *szUsername, char *szPwd, char *outpwdmd5, char *outkey);

#endif  /*__MISC_H__*/
