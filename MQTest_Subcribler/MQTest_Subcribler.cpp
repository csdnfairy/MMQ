// MQTest_Subcribler.cpp : 定义控制台应用程序的入口点。
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
	cout << "消息接收程序启动...." << endl;

	unique_ptr<CMessageQueue> que(new CMessageQueue());
	
	do
	{
		if (!que->Create())
		{
			cout << "消息队列打开失败..." << endl;
			break;
		}

		if (!que->Subscrible(MIN_MESSAGE_CODE, MAX_MESSAGE_CODE, OnMessageRecieved))
		{
			cout << "注册消息接收程序失败..." << endl;
			break;
		}

		while (counter >= 0)
		{
			if (--counter == 0)
			{
				que->UnSubscrible(OnMessageRecieved);
				cout << "接收器" << WAIT_TIME << "秒内未接收到任何消息，自动退出..." << endl;
				break;
			}
			Sleep(1000);
		}
	} while (0);

    return 0;
}

//该函数用于接收消息，消息队列将向其传递消息码和消息字符串
void OnMessageRecieved(int messageCode, vector<string> args)
{
	std::cout << "消息码：" << messageCode << endl;
	if (args.size() == 0)
	{
		cout << "无消息内容" << endl;
	}
	else
	{
		vector<string>::iterator iter = args.begin();
		int i = 0;
		while (iter < args.end())
		{
			cout <<"消息参数"<<i++<<":"<< iter++->c_str() << endl;
		}
	}

	counter = WAIT_TIME;
}

