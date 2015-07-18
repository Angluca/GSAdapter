#ifndef CODE_CONVERT_H
#define CODE_CONVERT_H
#include <iconv.h>
#include <map>
#include <string>
#include "noncopyable.h"

#define  G_CodeConvertInstance CodeConvert::Instance()
class CodeConvert
{
	NONCOPYABLE(CodeConvert);
	CodeConvert(void);
	typedef std::map<std::string, iconv_t> CharsetMap;
	CharsetMap charset_map_;
public:
	//constructor and destructor
	~CodeConvert(void);
	static CodeConvert& Instance();

	bool RegisterCode(const std::string &from, const std::string &to, const std::string &md = "@");
	bool Convert(const std::string &type, char *inBuf, int inSize, char *outBuf, int outSize);

};

/* utf <> gb2312 */
bool utf2gb(char *inBuf, int inSize, char *outBuf, int outSize);
bool gb2utf(char *inBuf, int inSize, char *outBuf, int outSize);

/* utf <> gbk */
bool utf2gbk(char *inBuf, int inSize, char *outBuf, int outSize);
bool gbk2utf(char *inBuf, int inSize, char *outBuf, int outSize);

#endif
