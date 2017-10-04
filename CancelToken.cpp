#include "stdafx.h"
#include "CancelToken.h"


CCancelToken::CCancelToken():_cancelRequest(false)
{
}


CCancelToken::~CCancelToken()
{
}

bool CCancelToken::IsCancelRequest()
{
	return _cancelRequest;
}

void CCancelToken::Cancel()
{
	if (!_cancelRequest)
		_cancelRequest = true;
}