#ifndef _CANCEL_TOKEN_H_
#define _CANCEL_TOKEN_H_

/*===================================================
*摘要：本类可用于结束子线程运行
*版权：gpl
*修改记录：
*      新建， 2017年10月4日
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

