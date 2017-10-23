/*++++++++++++++++++++++++++++++++++++++++++++++++++++++
*ժҪ����Ϣת���������ڽ��ַ������������͵���ϢתΪ
*      CMemoryMessage�ṹ��Ҳ����ʵ�ַ���ת����������
*      CMemoryMessage��Ϣ�ṹ����Ϊ�ַ���������������Ϣ
*��Ȩ��GPL
*�޸ļ�¼��
*      �½��� 20171013
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

