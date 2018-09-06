/*
 * RoadPassion CONFIDENTIAL
 *
 * Copyright 2013 RoadPassion Electronics Co., Ltd.
 *
 * DO NOT COPY AND/OR REDISTRIBUTE WITHOUT PERMISSION.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <includes.h>
#include "AudiA6_2012Tester.h"
#include "AudiA6_2012_common.h"

using namespace AudiA6_2012;

AudiA6_2012Tester::AudiA6_2012Tester() {
	// TODO Auto-generated constructor stub
	/*pCar->isPKeyPressed = false;
	pCar->steerAngle = 0;
	pCar->shiftPos = SHIFT_POS_P;
	pCar->isRadaBack = false;
	pCar->isRadaFront = false;*/
	pCar=NULL;
}

AudiA6_2012Tester::~AudiA6_2012Tester() {
	// TODO Auto-generated destructor stub
	
	delete pshiftTask;
	pshiftTask = NULL;

	delete psteeringTask;
	psteeringTask = NULL;
	
	delete pKeyTask;
	pKeyTask = NULL;
	
	delete pradarSimTask;
	pradarSimTask = NULL;
        
    delete pParkToSideTask;
    pParkToSideTask = NULL;

	delete pturnSimTask;
	pturnSimTask = NULL;
        
	delete ptirekeySimTask;
	ptirekeySimTask =NULL;

	delete pCarTimeTask;
	pCarTimeTask = NULL;
	
	delete pCarSpeedTask;
    	pCarSpeedTask = NULL;

    	delete pCarLightTask;
    	pCarLightTask = NULL;
	
	
}

void AudiA6_2012Tester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void AudiA6_2012Tester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void AudiA6_2012Tester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void AudiA6_2012Tester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void AudiA6_2012Tester::togglePKey()
{
	if (pCar->isPKeyPressed)
		pCar->isPKeyPressed = false;
	else
		pCar->isPKeyPressed = true;
}

/*******************************************************************************
 * 简介   :	增加指定雷达探头的数值，如果超过了最大值，自动折回到最小值
 * 参数   :	nr		探头编号。前面从左至右为0, 1, 2, 3，后面从左至右为4, 5, 6, 7
 * 			dv		增量
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void AudiA6_2012Tester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x5a, 0x78, 0x78, 0x5a, 0x5a, 0x9c, 0x9c, 0x5a};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// 保证在循环周期内有一次0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}

uint8_t AudiA6_2012Tester::run(CarInfo *pcar)
{
	pCar = pcar;
	pCar->maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;

	pshiftTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
	pshiftTask->start(pCar);

	psteeringTask = new SteeringWheelSimTask(STEERING_WHEEL_SIM_TASK_PRIO);
	psteeringTask->start(pCar);

	pKeyTask = new PKeySimTask(P_KEY_SIM_TASK_PRIO);
	pKeyTask->start(pCar);

	pradarSimTask = new RadarSimTask(RADAR_SIM_TASK_PRIO);
	pradarSimTask->start(pCar);
	
	pParkToSideTask = new ParkToSideKeySimTask(TURNING_LIGHT_SIM_TASK_PRIO);
	pParkToSideTask->start(pCar);

	pturnSimTask = new TurnSimTask(TURN_SIM_TASK_PRIO);
	pturnSimTask->start(pCar);
        
	ptirekeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO);
	ptirekeySimTask->start(pCar);

	pCarTimeTask = new CarTimeTask(CARTIME_TASK_PRIO);
	pCarTimeTask->start(pCar);
	
	pCarSpeedTask = new CarSpeedTask(CAR_SPEED_SIM_TASK_PRIO);
    	pCarSpeedTask->start(pCar);

    	pCarLightTask = new CarLightTask(CAR_LIGHT_TASK_PRIO);
    	pCarLightTask->start(pCar);
        
	return 0;
}

AudiA6_2012Tester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}

void AudiA6_2012Tester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x3c3,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x00, 0x14, 0x15, 0x00, 0x00, 0x0D}
	};

	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		uint8_t shiftPos = pCar->shiftPos;
	CarInfo *_pCar = (CarInfo *)p_arg;
       static uint8_t speedHigh = 0x00;
       static uint8_t speedLow  = 0x00;

		switch(shiftPos) {
		case CarInfo::SHIFT_POS_P:
			msg.Data[4] = 0x15;
			break;
		case CarInfo::SHIFT_POS_R:
			msg.Data[4] = 0x16;
			break;
		case CarInfo::SHIFT_POS_N:
			msg.Data[4] = 0x17;
			break;
		case CarInfo::SHIFT_POS_D:
			msg.Data[4] = 0x19;
			break;
		case CarInfo::SHIFT_POS_S:
			msg.Data[4] = 0x1d;
			break;
		default:
			break;
		}
#if 0
		switch(_pCar->speedCtl){
            case 0:
                if(speedHigh != 0x00){
                    speedLow--;
                    if(speedLow == 0x00) speedHigh--;
                    //speedHigh--;
                }
                break;
            case 1:
                if(speedHigh != 0xff){
			speedLow++;
                    if(speedLow == 0xff) speedHigh++;
                   // speedHigh++;
                }
                break;
            default:
                break;
        }
        
        msg.Data[2] = speedHigh;
        msg.Data[1] = speedLow;
#endif

		gpDrvCAN->transmit(CAN1, &msg);
		
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

AudiA6_2012Tester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void AudiA6_2012Tester::SteeringWheelSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x525,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0xf0, 0x0f, 0x0, 0x02, 0x00, 0x02, 0x08, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
		int32_t angle = pCar->steerAngle;

		if (angle <= 0) {
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + 0x4000;
		} else {
			angle = angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + 0x6000;
		}

		msg.Data[2] = (angle & 0xff00) >> 8;
		msg.Data[1] = (angle & 0x00ff) >> 0;

		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}


AudiA6_2012Tester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void AudiA6_2012Tester::PKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x603,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 4,
	  .Data = {0x22, 0x95, 0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,50);

		static bool oldv = false;
		bool newv = pCar->isPKeyPressed;
		if (oldv != newv){
			if (newv) {
				msg.Data[2] = 0x11;
				msg.Data[3] = 0x01;
			} else {
				msg.Data[2] = 0x0;
				msg.Data[3] = 0x0;
			}
			gpDrvCAN->transmit(CAN1, &msg);
			oldv = newv;
		}
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

AudiA6_2012Tester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio) {

}

/*******************************************************************************
 * 简介   :	模拟雷达帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void AudiA6_2012Tester::RadarSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x634,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 6,
	  .Data = {0x42, 0x92, 0x00, 0x00, 0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;
	//static char time = 39;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
	//	time++;
	//	if( (pCar->isRadaFront==true) ||(time ==40)){
		if(pCar->isRadaFront ==true) {
			// 前帧
			msg.Data[1] = 0x92;
			msg.Data[2] = pCar->radarVal[CarInfo::RADAR_LFS];
			msg.Data[3] = pCar->radarVal[CarInfo::RADAR_LFM];
			msg.Data[4] = pCar->radarVal[CarInfo::RADAR_RFM];
			msg.Data[5] = pCar->radarVal[CarInfo::RADAR_RFS];
			pCar->isRadaFront = false;
	//	} else if( (pCar->isRadaBack==true) ||(time== 48)){
		} else {
			msg.Data[1] = 0x93;
			msg.Data[2] = pCar->radarVal[CarInfo::RADAR_LRS];
			msg.Data[3] = pCar->radarVal[CarInfo::RADAR_LRM];
			msg.Data[4] = pCar->radarVal[CarInfo::RADAR_RRM];
			msg.Data[5] = pCar->radarVal[CarInfo::RADAR_RRS];
			pCar->isRadaFront = true;
		}
	/*	if(time >= 48)
				time=0;
*/
		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			/* Release any owned resources; */
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}


AudiA6_2012Tester::ParkToSideKeySimTask::ParkToSideKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void AudiA6_2012Tester::ParkToSideKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x607,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 6,
	  .Data = {0x22, 0xda, 0x11, 0x01,0x01,0x02}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,50);

		static bool oldv = false;
		bool newv = pCar->isACCOn;
		if (oldv != newv){
			if (newv) {
				msg.Data[5] = 0x01;
			} else {
				msg.Data[5] = 0x02;
			}
			gpDrvCAN->transmit(CAN1, &msg);
			oldv = newv;
		}
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			/* Release any owned resources; */
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

AudiA6_2012Tester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio) {

}
void AudiA6_2012Tester::TurnSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = TURNIDICATOR_STATUS_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 3,
	  .Data = {0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
	};
	CanTxMsg msg2 =
	{
	  .StdId = TURNIDICATOR_STATUS_MSG_ID2,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 3,
	  .Data = {0x00, 0x00, 0x0d, 0x00, 0x00, 0x00}
	};
	
	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
        switch(pCar->lightPos)
        {
        case 0://none
            msg.Data[1] = 0;
	     msg2.Data[0] = 0;
            break;
        case 1://left
            msg.Data[1] = 1;
	     msg2.Data[0] = 1;
            break;
        case 2://right
            msg.Data[1] = 2;
	     msg2.Data[0] = 2;
            break;
        case 3://flash
            msg.Data[1] = 4;
	     msg2.Data[0] = 0;
            break;
        default:
            msg.Data[1] = 0;
	     msg2.Data[0] = 0;
          break;
        }
		 
		gpDrvCAN->transmit(CAN1, &msg);
		gpDrvCAN->transmit(CAN1, &msg2);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			/* Release any owned resources; */
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}



AudiA6_2012Tester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio) {

}
void AudiA6_2012Tester::TireKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x5bf,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 4,
	  .Data = {0x00, 0x00, 0x00, 0x11}//00 00 00 11
	};
    
	CarInfo *pCar = (CarInfo*)p_arg;
    static uint8_t times = 0, status = 0, oldkey = 0, newkey = 0;
              
	while(1) {
        OSTimeDlyHMSM(0,0,0,100);
        newkey = pCar->TireKey;

        if(newkey != 0)
        {
            if((newkey != 5)&&(newkey != 6)&&(newkey != 10)&&(newkey != 11))
            {
                if(newkey != oldkey)
                {
                    oldkey = newkey;
                    times = 0;
                    status = 0x01;

                }else{
                    times++;
                    if(times >= 1)
                    {
                        status++;
                        if(status >=4)
                            status = 4;
                    }else{
                        status = 1;
                    }
                }
            }
        }else{
            times = 0;
            oldkey = 0;
            status = 0;
        }               
                                         
        switch(newkey)
        {
            case 0://没有按任何键
                msg.Data[0]=0;
                msg.Data[2]=0;
                break;
            case 1://*
                msg.Data[0]=0x21;
                msg.Data[2]=status;
                break;
            case 2://导航
                msg.Data[0]=0x1b;
                msg.Data[2]=status;
                break;
            case 3://语音
                msg.Data[0]=0x19;
                msg.Data[2]=status;
                break;
            case 4://MENU
                msg.Data[0]=0x01;
                msg.Data[2]=status;
                break;
            case 5://滑键  向上滑动
                msg.Data[0]=0x06;
                msg.Data[2]=0x01;
                break; 
            case 6://滑键  向下滑动
                msg.Data[0]=0x06;
                msg.Data[2]=0x0f;
                break; 
            case 7://滑键  按ok
                msg.Data[0]=0x07;
                msg.Data[2]=status;
                break;  
            case 8://left
                msg.Data[0]=0x03;
                msg.Data[2]=status;
                break;
            case 9://right
                msg.Data[0]=0x02;
                msg.Data[2]=status;
                break;
            case 10://voice_up
                msg.Data[0]=0x12;
                msg.Data[2]=0x01;
                break;            
            case 11://voice_down
                msg.Data[0]=0x12;
                msg.Data[2]=0x0f;
                break;
            case 12://静音
                msg.Data[0]=0x20;
                msg.Data[2]=status;
                break;
            default ://
                msg.Data[0]=0;
                msg.Data[2]=0;
                break;
        }
		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			/* Release any owned resources; */
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

AudiA6_2012Tester::CarTimeTask::CarTimeTask(uint8_t prio) : ZTask(prio) {
	
}

void AudiA6_2012Tester::CarTimeTask::_doTask(void * p_arg) {
		
	CanTxMsg msg = {
	  .StdId = CAR_TIME_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 6,
	  .Data = {0x93, 0x0f, 0x0b, 0x0, 0x0, 0x0}//2015年9月11日0时0分0秒
	};
	#if 0
	INT8U err;
	OS_TMR *pCarTime = OSTmrCreate( MS_2_TICK(1000),
									0,
									OS_TMR_OPT_ONE_SHOT,
									(OS_TMR_CALLBACK )_ChargeTime,
									&CT,
									(INT8U*)"",
									&err);
	if (err != OS_ERR_NONE) {
		zprintf("Create Timer fail: %d.\n",err);
	}
	
	OSTmrStart(pCarTime,&err);
	if (err != OS_ERR_NONE) {
		zprintf("Cannot  start a timer: %d. \n",err);
	}
	if(!pCarTime) {
		zprintf("create timer failed.\n");
	}
	#endif

	struct CarTime {
		uint8_t year;
		uint8_t mon;
		uint8_t day;
		uint8_t hour;
		uint8_t min;
		uint8_t sec;
    };
	static struct CarTime CT = {
		.year = 0x0f,
		.mon  = 0x09,
		.day  = 0x0b,
		.hour = 0x0,
		.min  = 0x0,
		.sec  = 0x0,
	};
	
	while(1){
		
		OSTimeDlyHMSM(0,0,0,1000);
		
	CT.sec++;

	if(CT.sec > 0x3b){
		CT.min++;
		if(CT.min > 0x3b){
			CT.hour++;
			if(CT.hour > 0x17){
				CT.day++;
				if(CT.day > 0x1e){
					CT.mon++;
					if(CT.mon > 0x0b){
                        CT.mon = 0x01;
						CT.year++;
					}
					CT.day = 0x01;
				}
				CT.hour = 0x0;
			}
			CT.min = 0x0;
		}
		CT.sec = 0x0;
	}
	
		msg.Data[0] = (CT.mon<<4)|0x03;
		msg.Data[1] = CT.year;
		msg.Data[2] = CT.day;
		msg.Data[3] = CT.hour;
		msg.Data[4] = CT.min;
		msg.Data[5] = CT.sec;
		
		gpDrvCAN->transmit(CAN1, &msg);
		
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			/* Release any owned resources; */
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}
#if 0
void AudiA6_2012Tester::CarTimeTask::_ChargeTime(OS_TMR * ptmr,void * param) {
	struct CarTime *pCT = (struct CarTime *)param;

	pCT->sec++;

	if(pCT->sec > 0x3b){
		pCT->min++;
		if(pCT->min > 0x3b){
			pCT->hour++;
			if(pCT->hour > 0x17){
				pCT->day++;
				if(pCT->day > 0x1e){
					pCT->mon++;
					if(pCT->mon > 0x0b){
						pCT->year++;
						if(pCT->year > 0xff){
							pCT->year = 0x0;
						}
						pCT->mon = 0x01;
					}
					pCT->day = 0x01;
				}
				pCT->hour = 0x0;
			}
			pCT->min = 0x0;
		}
		pCT->sec = 0x0;
	}
}
#endif


AudiA6_2012Tester::CarSpeedTask::CarSpeedTask(uint8_t prio) : ZTask(prio){

}

void AudiA6_2012Tester::CarSpeedTask::_doTask(void * p_arg){
    CanTxMsg msg = {
	  .StdId = SPEED_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10}
	};

    CarInfo *_pCar = (CarInfo *)p_arg;
    static uint8_t speedHigh = 0x00;
    static uint8_t speedLow  = 0x00;

    while(1){
        
        switch(_pCar->speedCtl){
            case 0:
                if(speedHigh != 0x00){
//                    speedLow--;
//                    if(speedLow == 0x00) speedHigh--;
                    speedHigh--;
                }
                break;
            case 1:
                if(speedHigh != 0xff){
//                    speedLow++;
//                    if(speedLow == 0xff) speedHigh++;
                    speedHigh++;
                }
                break;
            default:
                break;
        }
        
//        if(_pCar->speedCtl == 0){
//            if(speedHigh != 0x00){
//                speedLow--;
//                if(speedLow == 0x00) speedHigh--;
//            }
//        } else if(_pCar->speedCtl == 1){
//            if(speedHigh != 0xff){
//                speedLow++;
//                if(speedLow == 0xff) speedHigh++;
//            }
//        }

        msg.Data[3] = speedHigh;
        msg.Data[6] = speedLow;

        gpDrvCAN->transmit(CAN1, &msg);
		
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			/* Release any owned resources; */
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}

        OSTimeDlyHMSM(0,0,0,50);
    }
}

AudiA6_2012Tester::CarLightTask::CarLightTask(uint8_t prio) : ZTask(prio){

}

void AudiA6_2012Tester::CarLightTask::_doTask(void * p_arg){
    CanTxMsg msg = {
	  .StdId = 0x662,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07}//00 11 00 00 00 00 00 07
	};

    CarInfo *_pCar = (CarInfo *)p_arg;

    while(1){      
        switch(_pCar->lightCtl){
            case 0:
                msg.Data[2] = 0x00;
                break;
            case 1:
                msg.Data[2] = 0x03;
                break;
            case 2:
                msg.Data[2] = 0x04;
                break;
            case 3:
                msg.Data[2] = 0x13;
                break;
            default:
                break;
        }
        
        gpDrvCAN->transmit(CAN1, &msg);
		
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			/* Release any owned resources; */
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}

        OSTimeDlyHMSM(0,0,0,50);
    }
}