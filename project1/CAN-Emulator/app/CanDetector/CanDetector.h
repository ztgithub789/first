/*
 * CanDetector.h
 *
 *  Created on: 2013-7-30
 *      Author: Administrator
 */

#ifndef CANDETECTOR_H_
#define CANDETECTOR_H_

#include "includes.h"

class CanDetector {
public:
	static CanDetector *getInstance(void);
	void start(void);
	void stop(void);

private:
	CanDetector(void);
	~CanDetector(void);
	static void _turnOnRedLED(OS_TMR *ptmr,void *param);
	static void _gotoSleep(OS_TMR *ptmr,void *param);
	static void _msgReceiveHeadler(CanTMsg *msg,void *param);

	INT8U _refreshTimer(OS_TMR *ptmr);
	INT8U _initOneShotTimer(OS_TMR **pptmr,u32 dly, OS_TMR_CALLBACK func);

	OS_TMR *_gpRedLEDTimer;
	OS_TMR *_gpSleepTimer;
};


#endif /* CANDETECTOR_H_ */
