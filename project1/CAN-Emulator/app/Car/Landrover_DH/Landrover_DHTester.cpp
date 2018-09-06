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
#include "Landrover_DHTester.h"
#include "Landrover_DH_common.h"

using namespace Landrover_DH;

Landrover_DHTester::Landrover_DHTester() {
	// TODO Auto-generated constructor stub
	/*pCar->isPKeyPressed = false;
	pCar->steerAngle = 0;
	pCar->shiftPos = SHIFT_POS_P;
	pCar->isRadaBack = false;
	pCar->isRadaFront = false;*/
	pCar=NULL;
}

Landrover_DHTester::~Landrover_DHTester() {
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


        
	delete ptirekeySimTask;
	ptirekeySimTask =NULL;

	delete pCarTimeTask;
	pCarTimeTask = NULL;
	
	delete pCarSpeedTask;
    	pCarSpeedTask = NULL;

    	delete pCarLightTask;
    	pCarLightTask = NULL;
	
	
}

void Landrover_DHTester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void Landrover_DHTester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void Landrover_DHTester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void Landrover_DHTester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void Landrover_DHTester::togglePKey()
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
void Landrover_DHTester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x5a, 0x78, 0x78, 0x5a, 0x5a, 0x9c, 0x9c, 0x5a};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// 保证在循环周期内有一次0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}

uint8_t Landrover_DHTester::run(CarInfo *pcar)
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
	
	//pParkToSideTask = new ParkToSideKeySimTask(TURNING_LIGHT_SIM_TASK_PRIO);
	//pParkToSideTask->start(pCar);

	//pturnSimTask = new TurnSimTask(TURN_SIM_TASK_PRIO);
	//pturnSimTask->start(pCar);
        
	ptirekeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO);
	ptirekeySimTask->start(pCar);

	pCarTimeTask = new CarTimeTask(CARTIME_TASK_PRIO);
	pCarTimeTask->start(pCar);
	
	//pCarSpeedTask = new CarSpeedTask(CAR_SPEED_SIM_TASK_PRIO);
    	//pCarSpeedTask->start(pCar);

    	//pCarLightTask = new CarLightTask(CAR_LIGHT_TASK_PRIO);
    	//pCarLightTask->start(pCar);
        
	return 0;
}

Landrover_DHTester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}

void Landrover_DHTester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x125,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x80, 0xa2, 0x00, 0x00, 0x00, 0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		uint8_t shiftPos = pCar->shiftPos;

		switch(shiftPos) {
		case CarInfo::SHIFT_POS_P:
			msg.Data[0] = 0x00;
			break;
		case CarInfo::SHIFT_POS_R:
			msg.Data[0] = 0x01;
			break;
		case CarInfo::SHIFT_POS_N:
			msg.Data[0] = 0x02;
			break;
		case CarInfo::SHIFT_POS_D:
			msg.Data[0] = 0x04;
			break;
		case CarInfo::SHIFT_POS_S:
			msg.Data[0] = 0x08;
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

Landrover_DHTester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void Landrover_DHTester::SteeringWheelSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x4a,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0xfe, 0x00, 0x34, 0x1c, 0x1e, 0x80, 0x3f, 0xff}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
		int32_t angle = pCar->steerAngle;

		if (angle <= 0) {
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + CAN_STEER_MINDDLE_VAL;
		} else {
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + CAN_STEER_MINDDLE_VAL;
		}

		msg.Data[4] = (angle & 0xff00) >> 8;
		msg.Data[5] = (angle & 0x00ff) >> 0;

		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}


Landrover_DHTester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void Landrover_DHTester::PKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = P_KEY_STATUS_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,50);

		static bool oldv = false;
		bool newv = pCar->isPKeyPressed;
		if (oldv != newv){
			if (newv) {
				msg.Data[5] = 0x54;
				msg.Data[6] = 0x54;
			} else {
				msg.Data[5] = 0x0;
				msg.Data[6] = 0x0;
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

Landrover_DHTester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio) {

}

/*******************************************************************************
 * 简介   :	模拟雷达帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void Landrover_DHTester::RadarSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x174,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x80, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}
	};

	CanTxMsg msg2 =
	{
	  .StdId = 0x17f,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x80, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}
	};

	CarInfo *pCar = (CarInfo*)p_arg;
	//static char time = 39;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
	//	time++;
	//	if( (pCar->isRadaFront==true) ||(time ==40)){
		if(pCar->isRadaFront ==true) {
			// 前帧
			msg2.Data[5] = pCar->radarVal[CarInfo::RADAR_LFS];
			msg2.Data[3] = pCar->radarVal[CarInfo::RADAR_LFM];
			msg2.Data[4] = pCar->radarVal[CarInfo::RADAR_RFM];
			msg2.Data[6] = pCar->radarVal[CarInfo::RADAR_RFS];
			pCar->isRadaFront = false;
	//	} else if( (pCar->isRadaBack==true) ||(time== 48)){
		} else {
			msg.Data[5] = pCar->radarVal[CarInfo::RADAR_LRS];
			msg.Data[3] = pCar->radarVal[CarInfo::RADAR_LRM];
			msg.Data[4] = pCar->radarVal[CarInfo::RADAR_RRM];
			msg.Data[6] = pCar->radarVal[CarInfo::RADAR_RRS];
			pCar->isRadaFront = true;
		}
	/*	if(time >= 48)
				time=0;
*/
		 switch(pCar->lightPos)
	        {
		        case 0://none
		            msg.Data[1] = 0x00;
		            break;
		        case 1://left
		            msg.Data[1] = 0x40;
		            break;
		        case 2://right
		            msg.Data[1] = 0x80;
		            break;
		        case 3://flash
		            msg.Data[1] = 0xc0;
		            break;
		        default:
		            msg.Data[1] = 0x00;
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


Landrover_DHTester::ParkToSideKeySimTask::ParkToSideKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void Landrover_DHTester::ParkToSideKeySimTask::_doTask(void *p_arg)
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



Landrover_DHTester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio) {

}
void Landrover_DHTester::TireKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x180,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}//00 00 00 11
	};
    
	CarInfo *pCar = (CarInfo*)p_arg;
    static uint8_t times = 0, status = 0x00, oldkey = 0, newkey = 0,statusup_down = 0x00;
              
	while(1) {
        OSTimeDlyHMSM(0,0,0,100);
        newkey = pCar->TireKey;

        if(newkey != 0)
        {
           
                if(newkey != oldkey)
                {
                    oldkey = newkey;
                    times = 0;
                    status = 0x01;
                    statusup_down = 0x10;

                }else{
                    times++;
                    if(times >= 1)
                    {
                        status++;
                        statusup_down+=0x10;
                        if(status >=0x0f)
                            status = 0x0f;
                        if(statusup_down >= 0xf0)
                            statusup_down = 0xf0;
                    }else{
                        status = 0x01;
                        statusup_down=0x10;
                    }
                }
         
        }else{
            times = 0;
            oldkey = 0;
            status = 0;
            statusup_down = 0;
        }               
                                         
        switch(newkey)
        {
         case 0://没有按任何键
                msg.Data[5]=0;
                msg.Data[6]=0;
		        msg.Data[7]=0;
                break;
	     case 7://滑键  按ok
                msg.Data[6]=(0x80+status);
                break; 
	     case 4://MENU
                msg.Data[6]=(0x80+status);
                break;
	     case 10://voice_up
                msg.Data[5]=status;
                break; 
	     case 11://voice_down
                msg.Data[7]=(0xa0+status);
                break;
	     case 5://滑键  向上滑动
                msg.Data[7]=statusup_down;
                break; 
            case 6://滑键  向下滑动
                msg.Data[6]=statusup_down;
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

Landrover_DHTester::CarTimeTask::CarTimeTask(uint8_t prio) : ZTask(prio) {
	
}

void Landrover_DHTester::CarTimeTask::_doTask(void * p_arg) {
		
	CanTxMsg msg = {
	  .StdId = CAR_TIME_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x20, 0x00, 0x10, 0x03, 0x11, 0x00, 0x00, 0x00}
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
		.day  = 0x11,
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
	
		msg.Data[3] = CT.mon;
		msg.Data[2] = CT.year;
		msg.Data[4] = CT.day;
		msg.Data[5] = CT.hour;
		msg.Data[6] = CT.min;
		msg.Data[7] = CT.sec;
		
		gpDrvCAN->transmit(CAN1, &msg);
		
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			/* Release any owned resources; */
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}


Landrover_DHTester::CarSpeedTask::CarSpeedTask(uint8_t prio) : ZTask(prio){

}

void Landrover_DHTester::CarSpeedTask::_doTask(void * p_arg){
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

Landrover_DHTester::CarLightTask::CarLightTask(uint8_t prio) : ZTask(prio){

}

void Landrover_DHTester::CarLightTask::_doTask(void * p_arg){
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
