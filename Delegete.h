#ifndef CDELEGETE_H_
#define CDELEGETE_H_

//��Ϣ����������Ϣ����ʱ�Ļص�����ָ��
typedef void(*CALLBACK_WHEN_MESSAGE_ARRIVAL)(int message_code, vector<string> args);
typedef CALLBACK_WHEN_MESSAGE_ARRIVAL CALLBACK_FUN;
class CDelegete
{
public:
	CDelegete(int min_code, int max_code, CALLBACK_FUN fun);
	~CDelegete();

	bool IsSame(CALLBACK_FUN another) { return _fun == another; }

private:
	int  _minCode;
	int  _maxCode;
	CALLBACK_FUN _fun;

public:
	int MaxCode() { return _maxCode; }
	int MinCode() { return _minCode; }
	CALLBACK_FUN GetDelFun() { return _fun; }
};
#endif

