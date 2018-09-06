/**************** (C) COPYRIGHT 2011 BroadVision.Co.Ltd ************************
 *
 * 作者       : Administrator
 * 版本       : V0.1
 * 创建日期   : 2012-6-12
 * 描述       : 
 *
 *******************************************************************************/

#ifndef ZTASK_H_
#define ZTASK_H_

#include <libcommon.h>

class ZTask {
public:
	ZTask(uint8_t prio = OS_LOWEST_PRIO);
	uint8_t start(void *p_arg = 0);
	~ZTask();
	
private:
	static void _taskStub(void *p_arg);
    void _stop();
	/*******************************************************************************
	 * 简介   :	子类中需要重载该函数。注意参数为start()传入的参数，不要将此参数误认为是this指针。
	 * 参数   :
	 * 返回值 :
	 * 描述   :
	 *******************************************************************************/
	virtual void _doTask(void *p_arg) = 0;
	OS_STK _stk[COMMON_TASK_STK_SIZE];
	uint8_t _prio;
	void *_p_arg;
};

#endif /* ZTASK_H_ */
