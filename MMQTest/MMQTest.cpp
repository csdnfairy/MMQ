// MMQTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include "../MessageQueue.h"
#pragma comment(lib, "MemoryMQ")

using namespace std;

void OnMessageRecieved(int code, vector<string> args);

int main()
{
	CMessageQueue que;
	int err = 0;

	std::cout << "��ʼ������Ϣ..." << endl;

	if (!que.Create())
	{
		err = GetLastError();
		std::cout << "��Ϣ���д���ʧ�ܣ�" << err << endl;
	}
	else
	{
		std::cout << "��Ϣ���д����ɹ�..." << endl;
	}

	/*������Ϣ*/
	que.Subscrible(0, 0xff, OnMessageRecieved);

	/*������Ϣ*/
	vector<string> ars;
	ars.push_back("test");
	std::cout << "������Ϣ:" << endl;
	std::cout << "��Ϣ�룺" << 0 << endl;
	std::cout << "��Ϣ���ݣ�test" << endl;
	if (!que.Publish(0, ars))
	{
		err = GetLastError();
		std::cout << "����ʧ�ܣ�" << err << endl;
	}

	std::cout << "����������˳�..." << endl;
	getchar();

    return 0;
}

//�ú������ڽ�����Ϣ����Ϣ���н����䴫����Ϣ�����Ϣ�ַ���
void OnMessageRecieved(int messageCode, vector<string> args)
{
	std::cout << "������Ϣ����Ϣ�룺" << messageCode << endl;
	if (args.size() == 0)
	{
		cout << "����Ϣ����" << endl;
	}
	else
	{
		cout << "��Ϣ���ݣ�";
		vector<string>::iterator iter = args.begin();
		while (iter < args.end())
		{
			cout << iter->c_str() << endl;
			iter++;
		}
	}
}