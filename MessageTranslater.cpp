#include "stdafx.h"
#include "MessageTranslater.h"

/*+++++++++++++++++++++++++++++++++++++++++++
*��Ϣ��ʽ�����������򣩣�
*��ʼ����stx(0x02)
*��ֹ����etx(0x03)
*�������gs(0x1d)
*��һ������ʾ������Ϣ����Ϣ��
*������stx  1 gs arg1 gs arg02 etx
*��ʾ����Ϣ��Ϊ1����������������arg1���͡�arg02��
++++++++++++++++++++++++++++++++++++++++++++++*/

#define START_FLAG 0x02   //��ʼ��־
#define END_FLAG 0x03     //��ֹ��־
#define GROUP_SPILT 0x1d  //�����

CMessageTranslater::CMessageTranslater()
{
}


CMessageTranslater::~CMessageTranslater()
{
}

CMemoryMessage CMessageTranslater::FromString(const string* message)
{
	/*��Ϣ��ʽ��֤*/
	if (!IsValid(message))
		throw exception("��Ϣ��ʽ����");

	int code = GetMessageCode(message);
	CMemoryMessage newMessage(code);
	vector<string> args = GetMessageArgs(message);
	newMessage.
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*ժҪ����֤������Ϣ�ַ����ĸ�ʽ�Ƿ����Ҫ�󣬰�����ֹ�����������
*���룺����֤����Ϣ�ַ���
*�����true -- ��֤ͨ����false -- ��֤ʧ�ܣ�˵����ʽ������Ҫ��
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool CMessageTranslater::IsValid(const string* message)
{
	if (message == nullptr) return false;   //����Ƿ�������Ч��Ϣ

	if (*(char*)message != START_FLAG) return false; //�����б��躬����ʼ��

	/*��Ѱ��ֹ������ͬʱ��¼��Ѱ�����������ķ��������*/
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

	if (endPos < 0 || gsCount == 0) return false; //���������ֹ��������ֹ��֮�����ٰ���һ����Ϣ

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


