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
#include "volvo_V40Tester.h"
#include "volvo_V40_common.h"

using namespace Volvo_V40_NS;

Volvo_V40Tester::Volvo_V40Tester() {
	// TODO Auto-generated constructor stub
	/*pCar->isPKeyPressed = false;
	pCar->steerAngle = 0;
	pCar->shiftPos = SHIFT_POS_P;
	pCar->isRadaBack = false;
	pCar->isRadaFront = false;*/
	pCar=NULL;
}

Volvo_V40Tester::~Volvo_V40Tester() {
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

void Volvo_V40Tester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void Volvo_V40Tester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void Volvo_V40Tester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void Volvo_V40Tester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void Volvo_V40Tester::togglePKey()
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
void Volvo_V40Tester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x5a, 0x78, 0x78, 0x5a, 0x5a, 0x9c, 0x9c, 0x5a};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// 保证在循环周期内有一次0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}

uint8_t Volvo_V40Tester::run(CarInfo *pcar)
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
	//pParkToSideTask->start(pCar);

	pturnSimTask = new TurnSimTask(TURN_SIM_TASK_PRIO);
	pturnSimTask->start(pCar);
        
	ptirekeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO);
	//ptirekeySimTask->start(pCar);

	pCarTimeTask = new CarTimeTask(CARTIME_TASK_PRIO);
	pCarTimeTask->start(pCar);
	
	pCarSpeedTask = new CarSpeedTask(CAR_SPEED_SIM_TASK_PRIO);
    	pCarSpeedTask->start(pCar);

    	pCarLightTask = new CarLightTask(CAR_LIGHT_TASK_PRIO);
    	//pCarLightTask->start(pCar);
        
	return 0;
}

Volvo_V40Tester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}

void Volvo_V40Tester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x60,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0xc0, 0x94, 0x00, 0xfe, 0x80, 0x00, 0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		uint8_t shiftPos = pCar->shiftPos;

		switch(shiftPos) {
		case CarInfo::SHIFT_POS_P:
			msg.Data[2] = 0x00;
			break;
		case CarInfo::SHIFT_POS_R:
			msg.Data[2] = 0x20;
			break;
		case CarInfo::SHIFT_POS_N:
			msg.Data[2] = 0x40;
			break;
		case CarInfo::SHIFT_POS_D:
			msg.Data[4] = 0x60;
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
	}
}

Volvo_V40Tester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void Volvo_V40Tester::SteeringWheelSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x240,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x07, 0x37, 0x80, 0x28, 0xc3, 0x00, 0x00, 0xff}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
		int32_t angle = pCar->steerAngle;

		if (angle <= 0) {
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + CAN_STEER_MINDDLE_VAL;
		} else {
			angle = angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + CAN_STEER_MINDDLE_VAL;
		}

		msg.Data[1] = (angle & 0xff00) >> 8;
		msg.Data[2] = (angle & 0x00ff) >> 0;

		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}


Volvo_V40Tester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void Volvo_V40Tester::PKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x80,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x81, 0xc4, 0xc2, 0xb8,0x26, 0x28, 0x88, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,50);

		static bool oldv = false;
		bool newv = pCar->isPKeyPressed;
		if (oldv != newv){
			if (newv) {
				msg.Data[1] = 0xc6;
			} else {
				msg.Data[1] = 0xc4;
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

Volvo_V40Tester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio) {

}

/*******************************************************************************
 * 简介   :	模拟雷达帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void Volvo_V40Tester::RadarSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x110,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x00, 0x00, 0x8f, 0x00, 0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;
	//static char time = 39;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
	//	time++;
	//	if( (pCar->isRadaFront==true) ||(time ==40)){
		if(pCar->isRadaFront ==true) {
			// 前帧
			msg.Data[3] = pCar->radarVal[CarInfo::RADAR_LFS];  //左前侧
			msg.Data[2] = ((pCar->radarVal[CarInfo::RADAR_LFM])||(pCar->radarVal[CarInfo::RADAR_RFM])||(pCar->radarVal[CarInfo::RADAR_RFS]));
			msg.Data[1] = pCar->radarVal[CarInfo::RADAR_RFS];
			pCar->isRadaFront = false;
	//	} else if( (pCar->isRadaBack==true) ||(time== 48)){
		} else {
			msg.Data[7] = pCar->radarVal[CarInfo::RADAR_LRS];
			msg.Data[6] = ((pCar->radarVal[CarInfo::RADAR_LRM])||(pCar->radarVal[CarInfo::RADAR_RRS])||(pCar->radarVal[CarInfo::RADAR_RRM]));
			msg.Data[5] = pCar->radarVal[CarInfo::RADAR_RRM];
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


Volvo_V40Tester::ParkToSideKeySimTask::ParkToSideKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void Volvo_V40Tester::ParkToSideKeySimTask::_doTask(void *p_arg)
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

Volvo_V40Tester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio) {

}
void Volvo_V40Tester::TurnSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = TURNIDICATOR_STATUS_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x1c, 0x07, 0x31, 0xb0, 0x2f, 0x27, 0xe7, 0xc0}
	};
	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
        switch(pCar->lightPos)
        {
        case 0://none
            msg.Data[1] = 0x07;
            break;
        case 1://left
            msg.Data[1] = 0x0f;
            break;
        case 2://right
            msg.Data[1] = 0x17;
            break;
        case 3://flash
            msg.Data[1] = 0x1f;
            break;
        default:
            msg.Data[1] = 0x07;
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



Volvo_V40Tester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio) {

}
void Volvo_V40Tester::TireKeySimTask::_doTask(void *p_arg)
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

Volvo_V40Tester::CarTimeTask::CarTimeTask(uint8_t prio) : ZTask(prio) {
	
}

void Volvo_V40Tester::CarTimeTask::_doTask(void * p_arg) {
		
	CanTxMsg msg = {
	  .StdId = CAR_TIME_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x06,0x68,0x34,0x32,0x0e,0x08,0x03,0x10} 
	};

	struct CarTime {
		uint8_t year;
		uint8_t mon;
		uint8_t day;
		uint8_t hour;
		uint8_t min;
		uint8_t sec;
    };
	static struct CarTime CT = {
		.year = 0x10,
		.mon  = 0x03,
		.day  = 0x08,
		.hour = 0x0e,
		.min  = 0x32,
		.sec  = 0x34,
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
	
		msg.Data[6] = CT.mon ;
		msg.Data[7] = CT.year;
		msg.Data[5] = CT.day;
		msg.Data[4] = CT.hour;
		msg.Data[3] = CT.min;
		msg.Data[2] = CT.sec;
		
		gpDrvCAN->transmit(CAN1, &msg);
		
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			/* Release any owned resources; */
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
} 

Volvo_V40Tester::CarSpeedTask::CarSpeedTask(uint8_t prio) : ZTask(prio){

}

void Volvo_V40Tester::CarSpeedTask::_doTask(void * p_arg){
    CanTxMsg msg = {
	  .StdId = SPEED_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x00, 0x00, 0x00, 0xab, 0x06, 0x00}
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

        msg.Data[7] = speedHigh;
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

Volvo_V40Tester::CarLightTask::CarLightTask(uint8_t prio) : ZTask(prio){

}

void Volvo_V40Tester::CarLightTask::_doTask(void * p_arg){
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