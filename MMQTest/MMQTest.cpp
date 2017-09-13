// MMQTest.cpp : 定义控制台应用程序的入口点。
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

	vector<string> ars;
	ars.push_back("test");

	if (!que.Publish(0, ars))
	{
		err = GetLastError();
		std::cout << "发布失败：" << err << endl;
	}

	std::cout << "单击任意键退出..." << endl;
	getchar();

    return 0;
}

