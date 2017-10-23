// MQTest_Subcribler.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <atomic>
#include "MessageQueue.h"
using namespace std;

#define MIN_MESSAGE_CODE 0
#define MAX_MESSAGE_CODE 0xff

#pragma comment(lib, "MemoryMQ")

//static int counter = 10;
#define WAIT_TIME 30
static atomic_int counter(WAIT_TIME);

void OnMessageRecieved(int code, vector<string> args);

int main()
{
	cout << "��Ϣ���ճ�������...." << endl;

	unique_ptr<CMessageQueue> que(new CMessageQueue());
	
	do
	{
		if (!que->Create())
		{
			cout << "��Ϣ���д�ʧ��..." << endl;
			break;
		}

		if (!que->Subscrible(MIN_MESSAGE_CODE, MAX_MESSAGE_CODE, OnMessageRecieved))
		{
			cout << "ע����Ϣ���ճ���ʧ��..." << endl;
			break;
		}

		while (counter >= 0)
		{
			if (--counter == 0)
			{
				que->UnSubscrible(OnMessageRecieved);
				cout << "������" << WAIT_TIME << "����δ���յ��κ���Ϣ���Զ��˳�..." << endl;
				break;
			}
			Sleep(1000);
		}
	} while (0);

    return 0;
}

//�ú������ڽ�����Ϣ����Ϣ���н����䴫����Ϣ�����Ϣ�ַ���
void OnMessageRecieved(int messageCode, vector<string> args)
{
	std::cout << "��Ϣ�룺" << messageCode << endl;
	if (args.size() == 0)
	{
		cout << "����Ϣ����" << endl;
	}
	else
	{
		vector<string>::iterator iter = args.begin();
		int i = 0;
		while (iter < args.end())
		{
			cout <<"��Ϣ����"<<i++<<":"<< iter++->c_str() << endl;
		}
	}

	counter = WAIT_TIME;
}

