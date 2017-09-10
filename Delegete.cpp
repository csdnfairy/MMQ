#include "stdafx.h"
#include "Delegete.h"


CDelegete::CDelegete(int min_code, int max_code, CALLBACK_FUN fun):
	_minCode(min_code), _maxCode(max_code), _fun(fun)
{

}


CDelegete::~CDelegete()
{
}
