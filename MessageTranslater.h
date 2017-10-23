/*++++++++++++++++++++++++++++++++++++++++++++++++++++++
*摘要：消息转换器，用于将字符串或其它类型的信息转为
*      CMemoryMessage结构；也可以实现反向转换，即，将
*      CMemoryMessage消息结构解析为字符串或其它类型信息
*版权：GPL
*修改记录：
*      新建， 20171013
++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef _CMESSAGE_TRANSLATER_H_
#define _CMESSAGE_TRANSLATER_H_

#include <string>
#include "MemoryMessage.h"

static class CMessageTranslater
{
public:
	CMessageTranslater();
	~CMessageTranslater();

	static CMemoryMessage FromString(const std::string* message);
	static string ToString(const CMemoryMessage* message);

	static CMemoryMessage FromCodeAndArgs(int code, const vector<string>& args);
	static int ToCodeAndArgs(const CMemoryMessage& message, vector<string>& argsBuf);

private:
	static bool IsValid(const string* message);
	static int GetMessageCode(const string* message);
	static vector<string> GetMessageArgs(const string* message);
};
#endif

