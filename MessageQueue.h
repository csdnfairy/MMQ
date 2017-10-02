#ifndef _CMESSAGE_QUEUE_H_
#define _CMESSAGE_QUEUE_H_

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
摘要：这是一个消息队列，以共享内存方式，使多个进程之间可进行
      消息通信。采用了发布/订阅模式，发布者（进程）向消息队列
	  中加入消息；消息队列（中间件，也即本类）向消息订阅者发送
	  消息。
版权（C） GPL
修改记录：
       新建， 2017年7月7日 朱岳江
	   加入引用计数机制，实现自定销毁功能， 2017年10月2日
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "stdafx.h"
#include <thread>
#include "MemoryMessage.h"
#include "Delegete.h"

#define MEMORY_MQ_API _declspec(dllexport) 
#define MEMORY_SIZE 10240 //内存长度

MEMORY_MQ_API class CMessageQueue
{
public:
	MEMORY_MQ_API CMessageQueue();
	MEMORY_MQ_API ~CMessageQueue();

public:
	MEMORY_MQ_API bool Create();   //创建消息队列
	MEMORY_MQ_API bool Publish(int message_code, vector<string> args); //发布消息
	MEMORY_MQ_API bool Subscrible(int min_message_code, int max_message_code, CALLBACK_FUN callback); //订阅特定范围内的所有消息
	MEMORY_MQ_API bool UnSubscrible(CALLBACK_FUN callback);//取消消息订阅

private:
	void Destory(); //销毁消息队列

private:
	bool ExtendMapFileSize(HANDLE hFile, int extendSize);

private:
	std::vector<CDelegete> _dels; //委托队列
	const LPCWSTR _memName = L"c:\\_mq_mem_"; //共享内存区名称
	HANDLE _hMap;  //映射文件句柄
	HANDLE _hMutexForMessageQueue; //消息队列互斥锁
	HANDLE _hMutexForDelQueue;  //委托队列互斥锁
	const LPCTSTR _messageQueueMutexName = L"_mq_message_queue_mutex_";
	const LPCTSTR _delegeteQueueMutexName = L"_mq_delegete_queue_mutex_";
	int* _writePos;   //写指针
	int* _readPos;    //读指针
	int* _refers;     //引用计数指针，用于记录有多少调用者引用了该消息队列
	CMemoryMessage* _pHead; //队列起始地址

	unique_ptr<thread> _dispatcher;  //消息分发器
	static void Dispatch(void* pObj); //消息分发线程，实际分发动作由此函数完成

	static bool _isCreated; //是否已创建
};

#endif

