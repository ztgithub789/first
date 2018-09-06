/**************** (C) COPYRIGHT 2011 BroadVision.Co.Ltd ************************
 *
 * ����       : Administrator
 * �汾       : V0.1
 * ��������   : 2012-6-12
 * ����       : 
 *
 *******************************************************************************/

#ifndef ZSMALLTASK_H_
#define ZSMALLTASK_H_

#include <libcommon.h>

class ZSmallTask {
public:
	ZSmallTask(uint8_t prio = OS_LOWEST_PRIO);
	uint8_t start(void *p_arg = 0);
	~ZSmallTask();
	
private:
	static void _taskStub(void *p_arg);
    void _stop();
	/*******************************************************************************
	 * ���   :	��������Ҫ���ظú�����ע�����Ϊstart()����Ĳ�������Ҫ���˲�������Ϊ��thisָ�롣
	 * ����   :
	 * ����ֵ :
	 * ����   :
	 *******************************************************************************/
	virtual void _doTask(void *p_arg) = 0;
	OS_STK _stk[SMALL_TASK_STK_SIZE];
	uint8_t _prio;
	void *_p_arg;
};

#endif /* ZSMALLTASK_H_ */
