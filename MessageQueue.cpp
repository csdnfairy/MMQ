#include "stdafx.h"
#include "MessageQueue.h"
#include <algorithm>
#include <thread>

//构造器
CMessageQueue::CMessageQueue():_writePos(0),_readPos(0), _dispatcher(nullptr)
{
}

//析构
CMessageQueue::~CMessageQueue()
{
}

/*+++++++++++++++++++++++++++++++++++++++
摘要：该函数先在内存中创建一块共享区，
      后将消息队列放置于该共享区内，
	  若调用该函数时，共享区已创建完成，
	  则说明消息队列已经被创建，无需再次
	  创建
输入：无
输出：true -- 成功
      false -- 失败
+++++++++++++++++++++++++++++++++++++++++*/
bool CMessageQueue::Create()
{
	/*首先，尝试打开指定映射文件*/
	_hMap = ::OpenFileMapping(FILE_MAP_WRITE, false, _memName);

	/*打开失败说明尚未为文件创建映射，需要先创建映射*/
	if (_hMap == NULL)
	{
		/*创建文件对象*/
		HANDLE hFile = CreateFile(_memName, 
								  GENERIC_READ | GENERIC_WRITE, 
								  FILE_SHARE_READ | FILE_SHARE_WRITE,
								  0,
								  OPEN_ALWAYS,
								  FILE_ATTRIBUTE_NORMAL,
								  0);
		if (hFile == INVALID_HANDLE_VALUE) return false;

		/*如果文件小于共享内存区目标大小，扩充文件*/
		unsigned long fileSizeHigh = 0;
		auto sz = GetFileSize(hFile, &fileSizeHigh);
		if (sz < MEMORY_SIZE)
			ExtendMapFileSize(hFile, MEMORY_SIZE - sz);
				
		_hMap = ::CreateFileMapping(hFile, 0, PAGE_READWRITE, 0, MEMORY_SIZE, 0);  //创建文件映射
		if (_hMap == INVALID_HANDLE_VALUE) return false;
			
		_readPos = (int*)::MapViewOfFile(_hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0); //将文件映射至进程地址空间
		if (_pHead == nullptr) return false;
		ZeroMemory(_readPos, sizeof(int));
		_writePos = _readPos + 1;
		ZeroMemory(_writePos, sizeof(int));

		/*在起始添加一个计数器，用于记录当前队列中有多少消息*/
		_pHead = (CMemoryMessage*)(_writePos + 1);
			
		/*创建或打开消息队列锁和委托队列锁*/
		_hMutexForMessageQueue = ::CreateMutex(nullptr, false, _messageQueueMutexName);
		_hMutexForDelQueue = ::CreateMutex(nullptr, false, _delegeteQueueMutexName);

		::CloseHandle(hFile);
	}
	return true;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*摘要：向消息队列中加入发布一条新消息
*输入：message_code -- 消息码
*      args -- 消息参数
*输出：true -- 发布成功
*      false -- 失败， 原因请调用GetLastError
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool CMessageQueue::Publish(int message_code, vector<string> args)
{
	CMemoryMessage message(message_code);
	vector<string>::iterator begin = args.begin();
	for (; begin < args.end(); ++begin)
		message.AddArg(*begin);
	
	//::WaitForSingleObject(_hMutexForMessageQueue, INFINITE);
	if (_writePos - _readPos < MEMORY_SIZE)
	{
		memcpy_s(_pHead + *_writePos % MEMORY_SIZE, sizeof(CMemoryMessage), &message, sizeof(message));
		int newWPos = *_writePos + 1;
		memcpy_s(_writePos, sizeof(int), &newWPos, sizeof(int));
	}
	//::ReleaseMutex(_hMutexForMessageQueue);

	return true;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*摘要：订阅消息，成功订阅后，消息队列将自动把指定范围内的消息
*      发送给该订阅者
*输入：订阅消息范围的最小消息码，最大消息码，以及响应函数
*输出：true -- 订阅成功； false -- 订阅失败
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool CMessageQueue::Subscrible(int min_message_code, int max_message_code, CALLBACK_FUN callback)
{
	CDelegete del(min_message_code, max_message_code, callback);
	//::WaitForSingleObject(_hMutexForDelQueue, INFINITE);
	_dels.push_back(del);
	//::ReleaseMutex(_hMutexForDelQueue);

	if (_dispatcher == nullptr)
		_dispatcher.reset(new thread(Dispatch, this));
		
	return true;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++
*摘要：取消指定回调函数对于消息订阅者对本队列的订阅
*输入：订阅者的回调函数指针
*输出：true -- 取消订阅成功； false -- 取消订阅失败
+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool CMessageQueue::UnSubscrible(CALLBACK_FUN callback)
{
	::WaitForSingleObject(_hMutexForDelQueue, INFINITE);
	remove_if(_dels.begin(), _dels.end(), [callback](CDelegete d) {return d.IsSame(callback); });
	::ReleaseMutex(_hMutexForDelQueue);

	return false;
}

void CMessageQueue::Destory()
{

}

void CMessageQueue::Dispatch(void* pObj)
{
	CMessageQueue* pQue = (CMessageQueue*)pObj;
    
	while (true)
	{
		if (pQue->_readPos < pQue->_writePos)
		{
			CMemoryMessage message = *(pQue->_pHead + *(pQue->_readPos) % MEMORY_SIZE);
			int code = message.Code();
			auto match = find_if(pQue->_dels.begin(), pQue->_dels.end(),
				[code](CDelegete d) {return code <= d.MaxCode() && code >= d.MinCode(); });
			if (match >= pQue->_dels.begin() && match < pQue->_dels.end())
			{
				auto fun = match->GetDelFun();
				fun(message.Code(), message.Args());

				::WaitForSingleObject(pQue->_hMutexForMessageQueue, INFINITE);
				int newRPos = *(pQue->_readPos++);
				memcpy_s(pQue->_readPos, sizeof(int), &newRPos, sizeof(int));
				::ReleaseMutex(pQue->_hMutexForMessageQueue);

				pQue->UnSubscrible(match->GetDelFun()); //删除委托队列中的委托
			}
		}

		Sleep(10);
	}
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*摘要：扩展映射文件，以满足共享内存量增长需求
*      函数将自动从当前文件末端插入“\0”
*输入：hFile -- 文件句柄
*      extendSize -- 文件尺寸增加量
*返回：true -- 扩容成功； fasle -- 扩容失败，失败原因可调用GetLastError
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool CMessageQueue::ExtendMapFileSize(HANDLE hFile, int extendSize)
{
	if (hFile == INVALID_HANDLE_VALUE) return false;

	int offset = GetFileSize(hFile, NULL);
	SetFilePointer(hFile, offset, 0, 0); //定位到文件末尾
	char* pBuf = new char[extendSize];
	for (int i = 0; i < extendSize; ++i)
		pBuf[i] = '\0';

	unsigned long writtenBytes = 0;
	return WriteFile(hFile, (LPCVOID)pBuf, extendSize, &writtenBytes, NULL);
}
