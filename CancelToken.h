#ifndef _CANCEL_TOKEN_H_
#define _CANCEL_TOKEN_H_

/*===================================================
*ժҪ����������ڽ������߳�����
*��Ȩ��gpl
*�޸ļ�¼��
*      �½��� 2017��10��4��
=====================================================*/
class CCancelToken
{
public:
	CCancelToken();
	~CCancelToken();

	bool IsCancelRequest();
	void Cancel();

private:
	bool _cancelRequest;

};

#endif

