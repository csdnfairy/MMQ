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
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "stdafx.h"
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
	MEMORY_MQ_API void Destory(); //������Ϣ����

private:
	std::vector<CDelegete> _dels; //ί�ж���
	const LPCWSTR _memName = L"_mq_mem_"; //�����ڴ�������
	HANDLE _hMap;  //ӳ���ļ����
	HANDLE _hMutexForMessageQueue; //��Ϣ���л�����
	HANDLE _hMutexForDelQueue;  //ί�ж��л�����
	const LPCTSTR _messageQueueMutexName = L"_mq_message_queue_mutex_";
	const LPCTSTR _delegeteQueueMutexName = L"_mq_delegete_queue_mutex_";
	long _writePos;
	long _readPos;
	CMemoryMessage* _pBeginPos; //������ʼ��ַ

	bool _isDispatchStartted;  //�Ƿ��Ѿ���ʼ��Ϣ�ַ�

	static bool _isCreated; //�Ƿ��Ѵ���

	static void Dispatch(void* pObj);
};

#endif

