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
	_hMap = ::OpenFileMapping(FILE_MAP_WRITE, false, _memName);

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
			
		_pBeginPos = (CMemoryMessage*)::MapViewOfFile(_hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0); //���ļ�ӳ�������̵�ַ�ռ�
		if (_pBeginPos == nullptr) return false;
			

		/*���������Ϣ��������ί�ж�����*/
		_hMutexForMessageQueue = ::CreateMutex(nullptr, false, _messageQueueMutexName);
		_hMutexForDelQueue = ::CreateMutex(nullptr, false, _delegeteQueueMutexName);

		::CloseHandle(hFile);
	}
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
		*(_pBeginPos + _writePos % MEMORY_SIZE) = message;
		_writePos++;
	}
	//::ReleaseMutex(_hMutexForMessageQueue);

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

			pQue->UnSubscrible(match->GetDelFun()); //ɾ��ί�ж����е�ί��
		}

		Sleep(10);
	}
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
