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

	/*�����ӽ��̣����ڽ�����Ϣ*/
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
	//	cout << "��Ϣ���ս��̴���ʧ�ܣ�" <<&err<< endl;
	//}

	/*������Ϣ*/
	vector<string> ars;
	ars.push_back("test");
	if (!que.Publish(0, ars))
	{
		err = GetLastError();
		std::cout << "����ʧ�ܣ�" << err << endl;
	}
	else
	{
		std::cout << "������Ϣ:" << endl;
		std::cout << "��Ϣ�룺" << 0 << endl;
		std::cout << "��Ϣ���ݣ�test" << endl;
	}

	//TerminateProcess()
	std::cout << "����������˳�..." << endl;
	getchar();

    return 0;
}