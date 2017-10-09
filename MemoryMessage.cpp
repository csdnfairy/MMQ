#include "stdafx.h"
#include "MemoryMessage.h"


CMemoryMessage::CMemoryMessage(int code):_code(code)
{

}


CMemoryMessage::~CMemoryMessage()
{
}

void CMemoryMessage::AddArg(char* arg)
{
	//if (arg != NULL)
	//{
	//	_args.push_back(arg);
	//}

	_args = arg;
}
