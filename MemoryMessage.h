#ifndef _CMEMORY_MESSAGE_H_
#define _CMEMORY_MESSAGE_H_

class CMemoryMessage
{
public:
	CMemoryMessage(int code);
	~CMemoryMessage();

private:
	int _code;
	vector<string> _args;
	//char* _args;

public:
	void AddArg(string arg);
	int Code() const { return _code; }
	vector<string> Args() const { return _args; }
};

#endif

