// MMQTest.cpp : 定义控制台应用程序的入口点。
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

	std::cout << "开始创建消息..." << endl;

	if (!que.Create())
	{
		err = GetLastError();
		std::cout << "消息队列创建失败：" << err << endl;
	}
	else
	{
		std::cout << "消息队列创建成功..." << endl;
	}

	/*发布消息*/
	//vector<LPCTSTR> ars;
	//ars.push_back(L"test");
	int i = 0;
	int max = 10000;
	while (i < max)
	{
		if (!que.Publish(i % 10, "test", 5))
		{
			err = GetLastError();
			std::cout <<"消息"<<i<< "发布失败：" << err << endl;
		}
		else
		{
			std::cout << "发布消息:" << endl;
			std::cout << "消息码：" << i << endl;
			std::cout << "消息内容：test" << endl;
		}

		++i;
		Sleep(1000);
	}

	std::cout << "单击任意键退出..." << endl;
	getchar();

    return 0;
}