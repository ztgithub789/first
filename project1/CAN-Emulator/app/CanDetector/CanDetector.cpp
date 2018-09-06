/*
 * CanDetector.cpp
 *
 *  Created on: 2013-7-30
 *      Author: Administrator
 */
#include "CanDetector.h"

#define SLEEP_TIME 4000 //ms

CanDetector *CanDetector::getInstance(void)
{
	static CanDetector *_this;
	if(!_this) {
		_this = new CanDetector;
	}
	return (_this);
}

CanDetector::CanDetector(void) {
	_gpRedLEDTimer = NULL;
	_gpSleepTimer = NULL;
}

CanDetector::~CanDetector(void) {

}

void CanDetector::_turnOnRedLED(OS_TMR *ptmr,void *param)
{
	gpDrvLEDRed->on();
}

void CanDetector::_gotoSleep(OS_TMR *ptmr,void *param)
{
	CanDetector *_this = (CanDetector *)param;
	zprintf(" goto sleep .\n");
	gpDrvLEDGreen->getValue()? gpDrvLEDGreen->off(): gpDrvLEDGreen->on();
	gpDrvTJA1041->enterGotoSleepCommandMode();
	gpDrvTJA1041->enterPownMode();
	_this->_refreshTimer(_this->_gpSleepTimer);
}

void CanDetector::_msgReceiveHeadler(CanTMsg *msg,void *param)
{
	CanDetector *_this = (CanDetector *)param;
	bool status;
	status  = gpDrvLEDRed->getValue();
	if( 0 == status ){
		gpDrvLEDRed->on();
	}
	else{
		gpDrvLEDRed->off();
	}
	if(_this->_gpRedLEDTimer)
		_this->_refreshTimer(_this->_gpRedLEDTimer);
	if(_this->_gpSleepTimer)
		_this->_refreshTimer(_this->_gpSleepTimer);
}

INT8U CanDetector::_refreshTimer(OS_TMR *ptmr)
{
	INT8U err_tmr;
	OSTmrStop(ptmr,OS_TMR_OPT_NONE,(void*)0,&err_tmr);
	if(err_tmr != OS_ERR_NONE && err_tmr !=OS_ERR_TMR_STOPPED) {
		zprintf("Cannot stop Timer:err_tmr = %d.\n",err_tmr);
	}
	OSTmrStart(ptmr,&err_tmr);
	if (err_tmr != OS_ERR_NONE) {
		zprintf("Cannot  restart Timer :err_tmr = %d.\n",err_tmr);
	}
	return err_tmr;
}

INT8U CanDetector::_initOneShotTimer(OS_TMR **pptmr,u32 dly, OS_TMR_CALLBACK func)
{
	INT8U err_tmr;
	*pptmr = OSTmrCreate(MS_2_TICK(dly),
			0,
			OS_TMR_OPT_ONE_SHOT,
			(OS_TMR_CALLBACK )func,
			this,
			(INT8U*)"",
			&err_tmr);
	if (err_tmr != OS_ERR_NONE) {
		zprintf("Create Timer fail: %d.\n",err_tmr);
	}
	OSTmrStart(*pptmr,&err_tmr);
	if (err_tmr != OS_ERR_NONE) {
		zprintf("Cannot  start a timer: %d. \n",err_tmr);
	}
	if(!*pptmr) {
		zprintf("create timer failed.\n");
	}
	return err_tmr;
}

void CanDetector::start(void)
{
	if(_gpRedLEDTimer ==NULL)
		_initOneShotTimer(&_gpRedLEDTimer,200,(OS_TMR_CALLBACK)_turnOnRedLED);
	if(_gpSleepTimer == NULL)
		_initOneShotTimer(&_gpSleepTimer,SLEEP_TIME,(OS_TMR_CALLBACK)_gotoSleep);
	gpDrvCAN->clearOnMsgReceivedHandler();
	gpDrvCAN->setOnMsgReceivedHandler(_msgReceiveHeadler,this);
}

void CanDetector::stop(void)
{
	gpDrvCAN->clearOnMsgReceivedHandler();

	INT8U err_tmr;
	if(_gpRedLEDTimer) {
		OSTmrDel(_gpRedLEDTimer,&err_tmr);
		if(err_tmr == OS_ERR_NONE)
			_gpRedLEDTimer = NULL;
	}
	if(_gpSleepTimer) {
		OSTmrDel(_gpSleepTimer, &err_tmr);
		if(err_tmr == OS_ERR_NONE)
			_gpSleepTimer = NULL;
	}
}
