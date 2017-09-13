// MMQTest.cpp : 定义控制台应用程序的入口点。
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

	/*订阅消息*/
	que.Subscrible(0, 0xff, OnMessageRecieved);

	/*发布消息*/
	vector<string> ars;
	ars.push_back("test");
	std::cout << "发布消息:" << endl;
	std::cout << "消息码：" << 0 << endl;
	std::cout << "消息内容：test" << endl;
	if (!que.Publish(0, ars))
	{
		err = GetLastError();
		std::cout << "发布失败：" << err << endl;
	}

	std::cout << "单击任意键退出..." << endl;
	getchar();

    return 0;
}

//该函数用于接收消息，消息队列将向其传递消息码和消息字符串
void OnMessageRecieved(int messageCode, vector<string> args)
{
	std::cout << "接收消息，消息码：" << messageCode << endl;
	if (args.size() == 0)
	{
		cout << "无消息内容" << endl;
	}
	else
	{
		cout << "消息内容：";
		vector<string>::iterator iter = args.begin();
		while (iter < args.end())
		{
			cout << iter->c_str() << endl;
			iter++;
		}
	}
}