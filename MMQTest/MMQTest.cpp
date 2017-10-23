// MMQTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include "../MessageQueue.h"
#pragma comment(lib, "MemoryMQ")

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

	/*������Ϣ*/
	vector<string> ars;
	ars.push_back("test_arg1");
	ars.push_back("test_arg2");
	int i = 0;
	int max = 10000;
	while (i < max)
	{
		if (!que.Publish(i, ars))
		{
			err = GetLastError();
			std::cout <<"��Ϣ"<<i<< "����ʧ�ܣ�" << err << endl;
		}
		else
		{
			std::cout << "������Ϣ:" << endl;
			std::cout << "��Ϣ�룺" << i << endl;
			vector<string>::iterator iter = begin(ars);
			while (iter < end(ars))
			{
				std::cout << "��Ϣ����:" << iter++->c_str() << endl;
			}
		}

		++i;
		Sleep(1000);
	}

	std::cout << "����������˳�..." << endl;
	getchar();

    return 0;
}