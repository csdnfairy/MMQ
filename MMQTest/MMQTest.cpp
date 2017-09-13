// MMQTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include "../MessageQueue.h"
#pragma comment(lib, "MemoryMQ.lib")

using namespace std;


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

	vector<string> ars;
	ars.push_back("test");

	if (!que.Publish(0, ars))
	{
		err = GetLastError();
		std::cout << "����ʧ�ܣ�" << err << endl;
	}

	std::cout << "����������˳�..." << endl;
	getchar();

    return 0;
}

