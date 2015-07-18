#include "code_convert.h"
#include "utils.h"

#include <iterator>

//constructor
CodeConvert::CodeConvert(void)
{
	RegisterCode("utf-8", "gb2312");
	RegisterCode("utf-8", "gbk");
}

//destructor
CodeConvert::~CodeConvert(void)
{
	CharsetMap::iterator pos = charset_map_.begin();
	for(; pos!= charset_map_.end(); ++pos) {
		iconv_close(pos->second);
	}
	charset_map_.clear();
}

CodeConvert& CodeConvert::Instance()
{
	static CodeConvert instance;
	return instance;
}

bool CodeConvert::RegisterCode(const std::string &from, const std::string &to, const std::string &md)
{
	std::string type = from + md + to;
	if(charset_map_.find(type) != charset_map_.end()) return true;
	iconv_t code_type = iconv_open(to.c_str(), from.c_str());
	if(code_type == 0) {
		return false;
	}
	iconv_t code_type2 = iconv_open(from.c_str(), to.c_str());
	if(code_type2 == 0) {
		iconv_close(code_type);
		return false;
	}
	charset_map_[type] = code_type;
	type = to + md + from;
	charset_map_[type] = code_type2;
	return true;
}

bool CodeConvert::Convert(const std::string &type, char *inBuf, int inSize, char *outBuf, int outSize)
{
	CharsetMap::iterator pos = charset_map_.find(type);
	if(pos != charset_map_.end()) {
		char **p_in = &inBuf;
		char **p_out = &outBuf;
		if(iconv(pos->second, p_in, (size_t*)&inSize, p_out, (size_t*)&outSize) < 0) {
			return false;
		}
		return true;
	}
	return false;
}

/* convert function */
bool utf2gb(char *inBuf, int inSize, char *outBuf, int outSize)
{
	return G_CodeConvertInstance.Convert("utf-8@gb2312",inBuf, inSize, outBuf, outSize);
}

bool gb2utf(char *inBuf, int inSize, char *outBuf, int outSize)
{
	return G_CodeConvertInstance.Convert("gb2312@utf-8",inBuf, inSize, outBuf, outSize);
}

bool utf2gbk(char *inBuf, int inSize, char *outBuf, int outSize)
{
	return G_CodeConvertInstance.Convert("utf-8@gbk",inBuf, inSize, outBuf, outSize);
}

bool gbk2utf(char *inBuf, int inSize, char *outBuf, int outSize)
{
	return G_CodeConvertInstance.Convert("gbk@utf-8",inBuf, inSize, outBuf, outSize);
}

