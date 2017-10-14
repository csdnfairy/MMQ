#include "stdafx.h"
#include "MessageTranslater.h"

/*+++++++++++++++++++++++++++++++++++++++++++
*消息格式（解包封包规则）：
*起始符：stx(0x02)
*终止符：etx(0x03)
*分组符：gs(0x1d)
*第一组必须表示本条消息的消息码
*样例：stx  1 gs arg1 gs arg02 etx
*表示：消息码为1，包含两个参数“arg1”和“arg02”
++++++++++++++++++++++++++++++++++++++++++++++*/

#define START_FLAG 0x02   //起始标志
#define END_FLAG 0x03     //终止标志
#define GROUP_SPILT 0x1d  //分组符

CMessageTranslater::CMessageTranslater()
{
}


CMessageTranslater::~CMessageTranslater()
{
}

CMemoryMessage CMessageTranslater::FromString(const string* message)
{
	/*消息格式验证*/
	if (!IsValid(message))
		throw exception("消息格式错误");

	int code = GetMessageCode(message);
	CMemoryMessage newMessage(code);
	vector<string> args = GetMessageArgs(message);
	newMessage.
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*摘要：验证输入消息字符串的格式是否符合要求，包括起止符、分组符等
*输入：待验证的消息字符串
*输出：true -- 验证通过，false -- 验证失败，说明格式不符合要求
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool CMessageTranslater::IsValid(const string* message)
{
	if (message == nullptr) return false;   //检查是否输入无效消息

	if (*(char*)message != START_FLAG) return false; //输入中必需含有起始符

	/*搜寻终止符，并同时记录搜寻过程中遇到的分组符个数*/
	int gsCount = 0;
	int endPos = -1;
	for (int i = 0; i < message->size(); ++i)
	{
		char tempChar = *((char*)message + i);
		if (tempChar == END_FLAG)
		{
			endPos = i;
			break;
		}

		if (tempChar == GROUP_SPILT)
			gsCount++;
	}

	if (endPos < 0 || gsCount == 0) return false; //必须存在终止，且在起止符之间至少包含一组信息

	return true;
}

int CMessageTranslater::GetMessageCode(const string* message)
{
	int code = -1;
	int startPos = -1;
	int endPos = -1;
	for (int i = 0; i < message->size(); ++i)
	{
		if (*((char*)message + i) == START_FLAG)
			startPos = i + 1;

		if (startPos >= 0 && *((char*)message + i) == GROUP_SPILT)
		{
			endPos = i - 1;
			break;
		}
	}

	if (startPos >= 0 && endPos > startPos)
	{
		int len = endPos - startPos;
		char* buf = new char[len];
		memcpy_s(buf, len, message + startPos, len);
		string tempStr(buf);
		code = stoi(tempStr);
		delete buf;
	}

	return code;
}

vector<string> CMessageTranslater::GetMessageArgs(const string* message) 
{

}

string CMessageTranslater::ToString(const CMemoryMessage* message)
{

}


