#include "stdafx.h"
#include "MessageQueue.h"
#include <algorithm>
#include <thread>
#include <chrono>

//������
CMessageQueue::CMessageQueue():_isDispatching(false)
{
}

//����
CMessageQueue::~CMessageQueue()
{
	WaitForDispatchCancelled(); //�ȴ���Ϣ�ַ�����ȡ��

	int cnt = (*_refers) - 1;
	memcpy_s(_refers, sizeof(int), &cnt, sizeof(int));

	/*û���κε��������øö���ʱ���Զ����ٶ���*/
	if (cnt == 0)
	{
		Destory();
	}	
}

void CMessageQueue::WaitForDispatchCancelled(int waitTimeLimit)
{
	if (_isDispatching)
	{
		_ct.Cancel();
		int counter = 0;
		int waitInterval = 100;
		while (_isDispatching)
		{
			std::this_thread::sleep_for(chrono::milliseconds(waitInterval));
			if (++counter * waitInterval >= waitTimeLimit) break;
		}
	}
}

/*+++++++++++++++++++++++++++++++++++++++
ժҪ���ú��������ڴ��д���һ�鹲������
      ����Ϣ���з����ڸù������ڣ�
	  �����øú���ʱ���������Ѵ�����ɣ�
	  ��˵����Ϣ�����Ѿ��������������ٴ�
	  ����
���룺��
�����true -- �ɹ�
      false -- ʧ��
+++++++++++++++++++++++++++++++++++++++++*/
bool CMessageQueue::Create()
{
	/*���ȣ����Դ�ָ��ӳ���ļ�*/
	_hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, false, _memName);

	/*��ʧ��˵����δΪ�ļ�����ӳ�䣬��Ҫ�ȴ���ӳ��*/
	if (_hMap == NULL)
	{
		/*�����ļ�����*/
		HANDLE hFile = CreateFile(_memName, 
								  GENERIC_READ | GENERIC_WRITE, 
								  FILE_SHARE_READ | FILE_SHARE_WRITE,
								  0,
								  OPEN_ALWAYS,
								  FILE_ATTRIBUTE_NORMAL,
								  0);
		if (hFile == INVALID_HANDLE_VALUE) return false;

		/*����ļ�С�ڹ����ڴ���Ŀ���С�������ļ�*/
		unsigned long fileSizeHigh = 0;
		auto sz = GetFileSize(hFile, &fileSizeHigh);
		if (sz < MEMORY_SIZE)
			ExtendMapFileSize(hFile, MEMORY_SIZE - sz);
				
		_hMap = ::CreateFileMapping(hFile, 0, PAGE_READWRITE, 0, MEMORY_SIZE, 0);  //�����ļ�ӳ��
		if (_hMap == INVALID_HANDLE_VALUE) return false;
			
		_readPos = (int*)::MapViewOfFile(_hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0); //���ļ�ӳ�������̵�ַ�ռ�
		if (_readPos == nullptr) return false;		
		_writePos = _readPos + 1;		
		_refers = _writePos + 1;
		if (*_refers == 0)
		{
			ZeroMemory(_readPos, sizeof(int));
			ZeroMemory(_writePos, sizeof(int));
			ZeroMemory(_refers, sizeof(int));
		}

		/*��������Ϣ����ͷ����ַ*/
		_pHead = (CMemoryMessage*)(_refers + 1);
			
		/*���������Ϣ��������ί�ж�����*/
		_hMutexForMessageQueue = ::CreateMutex(nullptr, false, _messageQueueMutexName);
		_hMutexForDelQueue = ::CreateMutex(nullptr, false, _delegeteQueueMutexName);

		::CloseHandle(hFile);
	}

	int cnt = (*_refers) + 1;  //���ü������ӣ���ʾ���µ����������˱���Ϣ����
	memcpy_s(_refers, sizeof(int), &cnt, sizeof(int));

	return true;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*ժҪ������Ϣ�����м��뷢��һ������Ϣ
*���룺message_code -- ��Ϣ��
*      args -- ��Ϣ����
*�����true -- �����ɹ�
*      false -- ʧ�ܣ� ԭ�������GetLastError
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
*ժҪ��������Ϣ���ɹ����ĺ���Ϣ���н��Զ���ָ����Χ�ڵ���Ϣ
*      ���͸��ö�����
*���룺������Ϣ��Χ����С��Ϣ�룬�����Ϣ�룬�Լ���Ӧ����
*�����true -- ���ĳɹ��� false -- ����ʧ��
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool CMessageQueue::Subscrible(int min_message_code, int max_message_code, CALLBACK_FUN callback)
{
	CDelegete del(min_message_code, max_message_code, callback);
	//::WaitForSingleObject(_hMutexForDelQueue, INFINITE);
	_dels.push_back(del);
	//::ReleaseMutex(_hMutexForDelQueue);

	if (!_isDispatching)
	{
		thread t(Dispatch, this);
		t.detach();
		_isDispatching = true;
	}
				
	return true;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++
*ժҪ��ȡ��ָ���ص�����������Ϣ�����߶Ա����еĶ���
*���룺�����ߵĻص�����ָ��
*�����true -- ȡ�����ĳɹ��� false -- ȡ������ʧ��
+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool CMessageQueue::UnSubscrible(CALLBACK_FUN callback)
{
	::WaitForSingleObject(_hMutexForDelQueue, INFINITE);
	remove_if(_dels.begin(), _dels.end(), [callback](CDelegete d) {return d.IsSame(callback); });
	::ReleaseMutex(_hMutexForDelQueue);

	return false;
}

/*������Ϣ����*/
void CMessageQueue::Destory()
{
	if (_hMap != INVALID_HANDLE_VALUE)
	{
		UnmapViewOfFile(_hMap);
		_hMap = INVALID_HANDLE_VALUE;

		CloseHandle(_hMutexForMessageQueue);
		CloseHandle(_hMutexForDelQueue);
	}
}

/*+++++++++++++++++++++++++++++++++++++++++++++++
*ժҪ����Ϣ�Զ��ַ�����
*���룺��Ϣ���ж���ָ��
*�������
+++++++++++++++++++++++++++++++++++++++++++++++++*/
void CMessageQueue::Dispatch(void* pObj)
{
	CMessageQueue* pQue = (CMessageQueue*)pObj;
    
	while (!(pQue->_ct.IsCancelRequest()))
	{
		if (*(pQue->_readPos) < *(pQue->_writePos))
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

				pQue->UnSubscrible(match->GetDelFun()); //ɾ��ί�ж����е�ί��
			}
		}

		Sleep(10);
	}

	pQue->_isDispatching = false;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*ժҪ����չӳ���ļ��������㹲���ڴ�����������
*      �������Զ��ӵ�ǰ�ļ�ĩ�˲��롰\0��
*���룺hFile -- �ļ����
*      extendSize -- �ļ��ߴ�������
*���أ�true -- ���ݳɹ��� fasle -- ����ʧ�ܣ�ʧ��ԭ��ɵ���GetLastError
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool CMessageQueue::ExtendMapFileSize(HANDLE hFile, int extendSize)
{
	if (hFile == INVALID_HANDLE_VALUE) return false;

	int offset = GetFileSize(hFile, NULL);
	SetFilePointer(hFile, offset, 0, 0); //��λ���ļ�ĩβ
	char* pBuf = new char[extendSize];
	for (int i = 0; i < extendSize; ++i)
		pBuf[i] = '\0';

	unsigned long writtenBytes = 0;
	return WriteFile(hFile, (LPCVOID)pBuf, extendSize, &writtenBytes, NULL);
}
