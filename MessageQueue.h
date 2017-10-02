#ifndef _CMESSAGE_QUEUE_H_
#define _CMESSAGE_QUEUE_H_

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ժҪ������һ����Ϣ���У��Թ����ڴ淽ʽ��ʹ�������֮��ɽ���
      ��Ϣͨ�š������˷���/����ģʽ�������ߣ����̣�����Ϣ����
	  �м�����Ϣ����Ϣ���У��м����Ҳ�����ࣩ����Ϣ�����߷���
	  ��Ϣ��
��Ȩ��C�� GPL
�޸ļ�¼��
       �½��� 2017��7��7�� ������
	   �������ü������ƣ�ʵ���Զ����ٹ��ܣ� 2017��10��2��
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "stdafx.h"
#include <thread>
#include "MemoryMessage.h"
#include "Delegete.h"

#define MEMORY_MQ_API _declspec(dllexport) 
#define MEMORY_SIZE 10240 //�ڴ泤��

MEMORY_MQ_API class CMessageQueue
{
public:
	MEMORY_MQ_API CMessageQueue();
	MEMORY_MQ_API ~CMessageQueue();

public:
	MEMORY_MQ_API bool Create();   //������Ϣ����
	MEMORY_MQ_API bool Publish(int message_code, vector<string> args); //������Ϣ
	MEMORY_MQ_API bool Subscrible(int min_message_code, int max_message_code, CALLBACK_FUN callback); //�����ض���Χ�ڵ�������Ϣ
	MEMORY_MQ_API bool UnSubscrible(CALLBACK_FUN callback);//ȡ����Ϣ����

private:
	void Destory(); //������Ϣ����

private:
	bool ExtendMapFileSize(HANDLE hFile, int extendSize);

private:
	std::vector<CDelegete> _dels; //ί�ж���
	const LPCWSTR _memName = L"c:\\_mq_mem_"; //�����ڴ�������
	HANDLE _hMap;  //ӳ���ļ����
	HANDLE _hMutexForMessageQueue; //��Ϣ���л�����
	HANDLE _hMutexForDelQueue;  //ί�ж��л�����
	const LPCTSTR _messageQueueMutexName = L"_mq_message_queue_mutex_";
	const LPCTSTR _delegeteQueueMutexName = L"_mq_delegete_queue_mutex_";
	int* _writePos;   //дָ��
	int* _readPos;    //��ָ��
	int* _refers;     //���ü���ָ�룬���ڼ�¼�ж��ٵ����������˸���Ϣ����
	CMemoryMessage* _pHead; //������ʼ��ַ

	unique_ptr<thread> _dispatcher;  //��Ϣ�ַ���
	static void Dispatch(void* pObj); //��Ϣ�ַ��̣߳�ʵ�ʷַ������ɴ˺������

	static bool _isCreated; //�Ƿ��Ѵ���
};

#endif

