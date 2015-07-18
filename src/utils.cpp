#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#define  LOG_FILE_NAME_SIZE 64
#define  LOG_CONTEXT_SIZE 256

void WriteLog(const char* context, const char* filename)
{
	char tmpbuf[LOG_FILE_NAME_SIZE] = {0};
	time_t tt = time(NULL);
	tm *t = localtime(&tt);
	if(filename) {
		snprintf(tmpbuf, LOG_FILE_NAME_SIZE, "%s_%d.%d.%d.log", filename, 
			t->tm_year, t->tm_mon, t->tm_mday);
	} else {
		snprintf(tmpbuf, LOG_FILE_NAME_SIZE, "%d.%d.%d.log", 
			t->tm_year, t->tm_mon, t->tm_mday);
	}
	FILE *fp = fopen(tmpbuf, "a");
	if(fp) {
		char buf[LOG_CONTEXT_SIZE] = {0};
		snprintf(buf, LOG_CONTEXT_SIZE, "%d-%d-%d %d:%d:%d> %s", 
				t->tm_year
				,t->tm_mon
				,t->tm_mday
				,t->tm_hour
				,t->tm_min
				,t->tm_sec 
				,context);
		fwrite(buf, strlen(buf), 1, fp);
		fflush(fp);
		fclose(fp);
	}
}
