#ifndef  __UTILS_H__
#define  __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

void WriteLog(const char* context, const char* filename = 0);

#ifndef	NDEBUG
#define ERROR_OUT(format,...) {\
	fprintf(stderr, "[ERROR|%s:%d>%s()<<"#format"]\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
}
#else
#define ERROR_OUT(format,...) {\
	char buf[1024] = {0}; \
	snprintf(buf, 1024, "[ERROR|%s:%d>%s()<<"#format"]\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
	fprintf(stderr, "%s", buf);\
	WriteLog(buf); \
}
#endif

#define PRINT_MSG(format,...) {\
	fprintf(stderr,"["format"]\n",##__VA_ARGS__);\
}

#ifndef	NDEBUG
#define  DEBUG_MSG(format,...){\
	fprintf(stderr, "[DEBUG|%s:%d>%s()<<"#format"]\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
}
#else
#define  DEBUG_MSG(format,...)
#endif

#define  EXIT_APP(format,...) {\
	fprintf(stderr, "[ERROR|%s:%d>%s()<<"#format"]\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
	exit(-1);	\
}


#define ERROR_LOG(fname,format,...) {\
	char buf[1024] = {0}; \
	snprintf(buf, 1024, "[ERROR|%s:%d>%s()<<"#format"]\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
	fprintf(stderr, "%s", buf);\
	WriteLog(buf, (fname)); \
}
#define PRINT_LOG(fname,format,...) {\
	char buf[1024] = {0}; \
	snprintf(buf, 1024, "[%s:%d>%s()<<"#format"]\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
	fprintf(stderr, "%s", buf);\
	WriteLog(buf, (fname)); \
}
const unsigned int g_short_size = sizeof(short);
const unsigned int g_ushort_size = sizeof(unsigned short);
const unsigned int g_int_size = sizeof(int);
const unsigned int g_uint_size = sizeof(unsigned int);
const unsigned int g_float_size = sizeof(float);
const unsigned int g_double_size = sizeof(double);
const unsigned int g_long_size = sizeof(long);
const unsigned int g_ulong_size = sizeof(unsigned long);


#endif  /*__UTILS_H__*/


