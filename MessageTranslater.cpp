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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*���ܣ�������ϢЭ�飬����Ϣ�ַ����н�����CMemoryMessage��ʽ����Ϣ��
*���룺��Ϣ�ַ�����ַ
*�����CMemoryMessage������Ϣ��
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
CMemoryMessage CMessageTranslater::FromString(const string* message)
{
	/*��Ϣ��ʽ��֤*/
	if (!IsValid(message))
		throw exception("��Ϣ��ʽ����");

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
*ժҪ����֤������Ϣ�ַ����ĸ�ʽ�Ƿ����Ҫ�󣬰�����ֹ�����������
*���룺����֤����Ϣ�ַ���
*�����true -- ��֤ͨ����false -- ��֤ʧ�ܣ�˵����ʽ������Ҫ��
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool CMessageTranslater::IsValid(const string* message)
{
	if (message == nullptr) return false;   //����Ƿ�������Ч��Ϣ

	if (message->at(0) != START_FLAG) return false; //�����б��躬����ʼ��

	/*��Ѱ��ֹ������ͬʱ��¼��Ѱ�����������ķ��������*/
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

	if (endPos < 0 || gsCount == 0) return false; //���������ֹ��������ֹ��֮�����ٰ���һ����Ϣ

	return true;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*���ܣ�����Ϣ�ַ����н�������Ϣ��
*���룺��Ϣ�ַ�����ַ
*�������Ϣ��
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
*ժҪ������Ϣ�ַ����н�������Ϣ���ݣ�������
*���룺��Ϣ�ַ�����ַ
*�������Ϣ����
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
vector<string> CMessageTranslater::GetMessageArgs(const string* message) 
{
	/*���ַ�������Ѱ���з����λ�ã��Լ���ֹ��λ�ã�
	  ���ڷ����֮�����ڷ��������ֹ��֮�������Ϊһ����Ϣ����*/
	vector<int> ps;//���ڴ�Ÿ������λ�ú���ֹ��λ��
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
*���ܣ�����Ϣ�尴Э���ʽת��Ϊ��Ϣ�ַ���
*���룺CMemoryMessage������Ϣ��
*�����Э���ʽ������Ϣ�ַ���
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
string CMessageTranslater::ToString(const CMemoryMessage* message)
{
	if (message == nullptr || message->Code() < 0)
		throw exception("��Ϣ����Ч");

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
���ܣ������������Ϣ�����Ϣ�����飬��֯ΪCMemoryMessage��ʽ����Ϣ��
���룺
     code -- ��Ϣ��
	 args -- ��Ϣ���������ݣ���
�����CMemoryMessage��ʽ����Ϣ��
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
ժҪ������Ϣ�ṹ���н�������Ϣ�����Ϣ����
���룺
      message -- ��Ϣ��
	  args -- ���ڴ�Ž����õ�����Ϣ������
�������Ϣ��
++*/
int CMessageTranslater::ToCodeAndArgs(const CMemoryMessage& message, vector<string>& args)
{
	if (&message == nullptr) return -1;

	args = message.Args();

	return message.Code();
}
