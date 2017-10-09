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
	//vector<LPCTSTR> ars;
	//ars.push_back(L"test");
	int i = 0;
	int max = 10000;
	while (i < max)
	{
		if (!que.Publish(i % 10, "test", 5))
		{
			err = GetLastError();
			std::cout <<"��Ϣ"<<i<< "����ʧ�ܣ�" << err << endl;
		}
		else
		{
			std::cout << "������Ϣ:" << endl;
			std::cout << "��Ϣ�룺" << i << endl;
			std::cout << "��Ϣ���ݣ�test" << endl;
		}

		++i;
		Sleep(1000);
	}

	std::cout << "����������˳�..." << endl;
	getchar();

    return 0;
}