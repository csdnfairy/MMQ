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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*功能：根据消息协议，从消息字符串中解析除CMemoryMessage格式的消息体
*输入：消息字符串地址
*输出：CMemoryMessage类型消息体
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
CMemoryMessage CMessageTranslater::FromString(const string* message)
{
	/*消息格式验证*/
	if (!IsValid(message))
		throw exception("消息格式错误");

	int code = GetMessageCode(message);
	CMemoryMessage newMessage(code);
	vector<string> args = GetMessageArgs(message);
	vector<string>::iterator iter = begin(args);
	while (iter < end(args))
	{
		newMessage.AddArg(*iter++);
	}

	return newMessage;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*摘要：验证输入消息字符串的格式是否符合要求，包括起止符、分组符等
*输入：待验证的消息字符串
*输出：true -- 验证通过，false -- 验证失败，说明格式不符合要求
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool CMessageTranslater::IsValid(const string* message)
{
	if (message == nullptr) return false;   //检查是否输入无效消息

	if (message->at(0) != START_FLAG) return false; //输入中必需含有起始符

	/*搜寻终止符，并同时记录搜寻过程中遇到的分组符个数*/
	int gsCount = 0;
	int endPos = -1;
	for (int i = 0; i < message->size(); ++i)
	{
		char tempChar = message->at(i);
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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*功能：从消息字符串中解析出消息码
*输入：消息字符串地址
*输出：消息码
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int CMessageTranslater::GetMessageCode(const string* message)
{
	int code = -1;
	int startPos = -1;
	int endPos = -1;
	for (int i = 0; i < message->size(); ++i)
	{
		if (message->at(i) == START_FLAG)
			startPos = i + 1;

		if (startPos >= 0 && message->at(i) == GROUP_SPILT)
		{
			endPos = i - 1;
			break;
		}
	}

	if (startPos >= 0 && endPos >= startPos)
	{
		int len = endPos - startPos + 1;
		//char* buf = new char[len];
		//memcpy_s(buf, len, message + startPos, len);
		//string tempStr(buf);
		//code = stoi(tempStr);
		/*delete buf;*/
		code = stoi(message->substr(startPos, endPos - startPos + 1));
	}

	return code;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*摘要：从消息字符串中解析出消息内容（参数）
*输入：消息字符串地址
*输出：消息内容
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
vector<string> CMessageTranslater::GetMessageArgs(const string* message) 
{
	/*从字符串中搜寻所有分组符位置，以及终止符位置，
	  夹在分组符之间或夹在分组符与终止符之间的内容为一个消息参数*/
	vector<int> ps;//用于存放各分组符位置和终止符位置
	int sz = message->size();
	for (int i = 0; i < sz; ++i)
	{
		char current = message->at(i);

		if (current == END_FLAG)
		{
			ps.push_back(i);
			break;
		}
		
		if (current == GROUP_SPILT)
		{
			ps.push_back(i);
		}
	}

	vector<string> args;
	vector<int>::iterator iter = begin(ps);
	while (iter < end(ps) - 1)
	{
		int len = *(iter + 1) - *iter - 1;
		//char* buf = new char[len];
		//memcpy_s(buf, len, message + *iter + 1, len);
		args.push_back(message->substr(*iter + 1, len));
		iter++;
		//delete buf;
	}

	return args;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*功能：将消息体按协议格式转换为消息字符串
*输入：CMemoryMessage类型消息体
*输出：协议格式化的消息字符串
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
string CMessageTranslater::ToString(const CMemoryMessage* message)
{
	if (message == nullptr || message->Code() < 0)
		throw exception("消息体无效");

	string strMessage;
	strMessage.append(1, START_FLAG);
	strMessage.append(std::to_string(message->Code()));
	auto args = message->Args();
	vector<string>::iterator iter = begin(args);
	while (iter < end(args))
	{
		strMessage.append(1, GROUP_SPILT);
		strMessage.append(*iter++);
	}

	strMessage.append(1, END_FLAG);

	return strMessage;
}

/*++
功能：根据输入的消息码和消息参数组，组织为CMemoryMessage格式的消息体
输入：
     code -- 消息码
	 args -- 消息参数（内容）组
输出：CMemoryMessage格式的消息体
++*/
CMemoryMessage CMessageTranslater::FromCodeAndArgs(int code, const vector<string>& args)
{
	CMemoryMessage message(code);
	vector<string>::const_iterator iter = begin(args);
	while (iter < end(args))
	{
		message.AddArg(*iter);
		iter++;
	}

	return message;
}

/*++
摘要：从消息结构体中解析出消息码和消息参数
输入：
      message -- 消息体
	  args -- 用于存放解析得到的消息参数组
输出：消息码
++*/
int CMessageTranslater::ToCodeAndArgs(const CMemoryMessage& message, vector<string>& args)
{
	if (&message == nullptr) return -1;

	args = message.Args();

	return message.Code();
}
