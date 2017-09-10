#include "stdafx.h"
#include "MessageQueue.h"
#include <algorithm>
#include <thread>


CMessageQueue::CMessageQueue():_writePos(0),_readPos(0)
{
}


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
	int err = 0;
	/*首先，尝试打开指定映射文件*/
	_hMap = ::OpenFileMapping(FILE_MAP_WRITE, false, _memName);


	/*打开失败说明尚未为文件创建映射，需要先创建映射*/
	if (_hMap == NULL)
	{
		err = GetLastError();
		/*创建文件对象*/
		HANDLE hFile = CreateFile(_memName, 
								  GENERIC_READ | GENERIC_WRITE, 
								  FILE_SHARE_READ | FILE_SHARE_WRITE,
								  0,
								  OPEN_ALWAYS,
								  FILE_ATTRIBUTE_NORMAL,
								  0);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			err = GetLastError();
			return false;
		}
				

		_hMap = ::CreateFileMapping(hFile,
				                    0, 
				                    PAGE_READWRITE,
				                    0, 
				                    MEMORY_SIZE,
				                    0);  //创建文件映射
		if (_hMap == INVALID_HANDLE_VALUE)
		{
			err = GetLastError();
			return false;
		}
			

		_pBeginPos = (CMemoryMessage*)::MapViewOfFile(_hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0); //将文件映射至进程地址空间
		if (_pBeginPos == nullptr)
		{
			err = GetLastError();
			return false;
		}
			

		/*创建或打开消息队列锁和委托队列锁*/
		_hMutexForMessageQueue = ::CreateMutex(nullptr, false, _messageQueueMutexName);
		_hMutexForDelQueue = ::CreateMutex(nullptr, false, _delegeteQueueMutexName);

		::CloseHandle(hFile);
	}
	return true;
}

bool CMessageQueue::Publish(int message_code, vector<string> args)
{
	CMemoryMessage message(message_code);
	vector<string>::iterator begin = args.begin();
	for (; begin < args.end(); ++begin)
		message.AddArg(*begin);
	
	::WaitForSingleObject(_hMutexForMessageQueue, INFINITE);
	if (_writePos - _readPos < MEMORY_SIZE)
	{
		CMemoryMessage* ptemp = _pBeginPos + 1;
		int temp_size = sizeof(message);
		*(_pBeginPos + _writePos % MEMORY_SIZE) = message;
		_writePos++;
	}
	::ReleaseMutex(_hMutexForMessageQueue);

	return false;
}

bool CMessageQueue::Subscrible(int min_message_code, int max_message_code, CALLBACK_FUN callback)
{
	CDelegete del(min_message_code, max_message_code, callback);
	::WaitForSingleObject(_hMutexForDelQueue, INFINITE);
	_dels.push_back(del);
	::ReleaseMutex(_hMutexForDelQueue);

	if (!_isDispatchStartted)
	{
		std::thread dispatcher(Dispatch, this);
		_isDispatchStartted = true;
	}

	return true;
}

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
    
	while (pQue->_readPos < pQue->_writePos)
	{
		CMemoryMessage message = *(pQue->_pBeginPos + pQue->_readPos % MEMORY_SIZE);
		int code = message.Code();
		auto match = find_if(pQue->_dels.begin(), pQue->_dels.end(),
			[code](CDelegete d) {return code <= d.MaxCode() && code >= d.MinCode(); });
		if (match >= pQue->_dels.begin() && match < pQue->_dels.end())
		{
			auto fun = match->GetDelFun();
			fun(message.Code(), message.Args());

			::WaitForSingleObject(pQue->_hMutexForMessageQueue, INFINITE);
			pQue->_readPos++;
			::ReleaseMutex(pQue->_hMutexForMessageQueue);

			pQue->UnSubscrible(match->GetDelFun()); //删除委托队列中的委托
		}

		Sleep(10);
	}
}
