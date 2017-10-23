#include "stdafx.h"
#include "MemoryMessage.h"


CMemoryMessage::CMemoryMessage(int code):_code(code)
{

}


CMemoryMessage::~CMemoryMessage()
{
}

void CMemoryMessage::AddArg(string arg)
{
	if (!arg.empty())
	{
		_args.push_back(arg);
	}
}
