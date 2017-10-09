#ifndef _CMEMORY_MESSAGE_H_
#define _CMEMORY_MESSAGE_H_

class CMemoryMessage
{
public:
	CMemoryMessage(int code);
	~CMemoryMessage();

private:
	int _code;
	//vector<LPCTSTR> _args;
	char* _args;

public:
	void AddArg(char* arg);
	int Code() { return _code; }
	char* Args() { return _args; }
};

#endif

