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

	/*创建子进程，用于接收消息*/
	//LPTSTR currentDir = new TCHAR[MAX_PATH];
	//::GetCurrentDirectory(MAX_PATH, currentDir);
	//wcscat_s(currentDir, MAX_PATH, L"\\MQTest_Subcribler.exe");
	//STARTUPINFO si;
	//PROCESS_INFORMATION pi;
	//ZeroMemory(&si, sizeof(si));
	//ZeroMemory(&pi, sizeof(pi));
	//if (CreateProcess(NULL, currentDir, NULL, NULL, false, 0, NULL, NULL, &si, &pi) <= 0)
	//{
	//	int err = GetLastError();
	//	cout << "消息接收进程创建失败：" <<&err<< endl;
	//}

	/*发布消息*/
	vector<string> ars;
	ars.push_back("test");
	if (!que.Publish(0, ars))
	{
		err = GetLastError();
		std::cout << "发布失败：" << err << endl;
	}
	else
	{
		std::cout << "发布消息:" << endl;
		std::cout << "消息码：" << 0 << endl;
		std::cout << "消息内容：test" << endl;
	}

	//TerminateProcess()
	std::cout << "单击任意键退出..." << endl;
	getchar();

    return 0;
}