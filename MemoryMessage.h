#ifndef _CMEMORY_MESSAGE_H_
#define _CMEMORY_MESSAGE_H_

class CMemoryMessage
{
public:
	CMemoryMessage(int code);
	~CMemoryMessage();

private:
	int _code;
	vector<std::string> _args;

public:
	void AddArg(std::string arg);
	int Code() { return _code; }
	vector<string> Args() { return _args; }
};

#endif

