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
#include "Cadillac_XT5Tester.h"
#include "Cadillac_XT5_common.h"

using namespace Cadillac_XT5;

Cadillac_XT5Tester::Cadillac_XT5Tester() {
	// TODO Auto-generated constructor stub
	/*pCar->isPKeyPressed = false;
	pCar->steerAngle = 0;
	pCar->shiftPos = SHIFT_POS_P;
	pCar->isRadaBack = false;
	pCar->isRadaFront = false;*/
	pCar=NULL;
}

Cadillac_XT5Tester::~Cadillac_XT5Tester() {
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

void Cadillac_XT5Tester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void Cadillac_XT5Tester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void Cadillac_XT5Tester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void Cadillac_XT5Tester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void Cadillac_XT5Tester::togglePKey()
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
void Cadillac_XT5Tester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x5a, 0x78, 0x78, 0x5a, 0x5a, 0x9c, 0x9c, 0x5a};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// 保证在循环周期内有一次0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}

uint8_t Cadillac_XT5Tester::run(CarInfo *pcar)
{
	pCar = pcar;
	pCar->maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;

	pshiftTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
	pshiftTask->start(pCar);

	psteeringTask = new SteeringWheelSimTask(STEERING_WHEEL_SIM_TASK_PRIO);
	psteeringTask->start(pCar);

	//pKeyTask = new PKeySimTask(P_KEY_SIM_TASK_PRIO);
	//pKeyTask->start(pCar);

	//pradarSimTask = new RadarSimTask(RADAR_SIM_TASK_PRIO);
	//pradarSimTask->start(pCar);
	
	//pParkToSideTask = new ParkToSideKeySimTask(TURNING_LIGHT_SIM_TASK_PRIO);
	//pParkToSideTask->start(pCar);

	pturnSimTask = new TurnSimTask(TURN_SIM_TASK_PRIO);
	pturnSimTask->start(pCar);
        
	//ptirekeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO);
	//ptirekeySimTask->start(pCar);

	//pCarTimeTask = new CarTimeTask(CARTIME_TASK_PRIO);
	//pCarTimeTask->start(pCar);
	
	pCarSpeedTask = new CarSpeedTask(CAR_SPEED_SIM_TASK_PRIO);
    	pCarSpeedTask->start(pCar);

    	//pCarLightTask = new CarLightTask(CAR_LIGHT_TASK_PRIO);
    	//pCarLightTask->start(pCar);
        
	return 0;
}

Cadillac_XT5Tester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}

void Cadillac_XT5Tester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x135,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x18, 0x14, 0x52, 0x55, 0x10, 0x10, 0x2e}
	};

	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		OSTimeDlyHMSM(0,0,0,40);

		uint8_t shiftPos = pCar->shiftPos;

		switch(shiftPos) {
		case CarInfo::SHIFT_POS_P:
			msg.Data[0] = 0x00;
			break;
		case CarInfo::SHIFT_POS_R:
			msg.Data[0] = 0x03;
			break;
		case CarInfo::SHIFT_POS_N:
			msg.Data[0] = 0x01;
			break;
		case CarInfo::SHIFT_POS_D:
			msg.Data[0] = 0x02;
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

Cadillac_XT5Tester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio) {

}
void Cadillac_XT5Tester::TurnSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x140,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 3,
	  .Data = {0x52, 0x02, 0x11}
	};
	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
        switch(pCar->lightPos)
        {
        case 0://none
            msg.Data[2] = 0x11;
            break;
        case 1://left
            msg.Data[2] = 0x15;
            break;
        case 2://right
            msg.Data[2] = 0x19;
            break;
        default:
            msg.Data[2] = 0x11;
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


Cadillac_XT5Tester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void Cadillac_XT5Tester::SteeringWheelSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x1e5,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x5c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
		int32_t angle = pCar->steerAngle;

		if (angle <= 0) {
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + 0x0000;
		} else {
			angle =-angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + 0x0000;
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

Cadillac_XT5Tester::CarSpeedTask::CarSpeedTask(uint8_t prio) : ZTask(prio){

}

void Cadillac_XT5Tester::CarSpeedTask::_doTask(void * p_arg){
    CanTxMsg msg = {
	  .StdId = SPEED_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 5,
	  .Data = {0x00, 0x00, 0x00, 0x00, 0x1b}
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

        msg.Data[1] = speedHigh;
        msg.Data[0] = speedLow;

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

Cadillac_XT5Tester::CarTimeTask::CarTimeTask(uint8_t prio) : ZTask(prio) {
	
}

void Cadillac_XT5Tester::CarTimeTask::_doTask(void * p_arg) {
		
	 CanTxMsg msg =
        {
          .StdId = 0x135,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 8,
          .Data = {0x00, 0x00, 0x01, 0x00, 0xc7, 0x10, 0x00, 0x37}// 2016年7月1日
        };

    uint8_t year = 0;
    uint8_t mon = 1;
    uint8_t day = 1;
    uint8_t hour = 0;
    uint8_t min = 0;
    uint8_t sec = 0;
    uint8_t count = 0;
    
    while(1){
        OSTimeDlyHMSM(0,0,0,200);

        count++;
        if(5==count){
            count = 0;
            sec += 0x01;
            if(sec>0x3b){
                sec = 0;
                min += 0x01;
                if(min>0x3b){
                    min =0;
                    hour += 0x01;
                    if(hour>0x17){
                        hour = 0;
                        day += 0x01;
                        if(day>0x1e){
                            day = 0x01;
                            mon += 0x01;
                            if(mon>0xc){
                                mon = 0x01;
                                year += 0x01;
                                if(year>0xff){
                                    year = 0;
                                }else{
                                    msg.Data[5] = year;
                                }
                            }else{
                                msg.Data[4] = mon && 0xc0;
                            }
                        }else{
                            msg.Data[2] = day;
                        }
                    }else{
                        msg.Data[3] = hour;
                    }
                }else{
                    msg.Data[6] = min;
                }
            }else{
                msg.Data[7] = sec;
            }
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

Cadillac_XT5Tester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void Cadillac_XT5Tester::PKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = CAR_TIME_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x01, 0x00, 0xc7, 0x10, 0x00, 0x37}// 2016年7月1日
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

Cadillac_XT5Tester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio) {

}

/*******************************************************************************
 * 简介   :	模拟雷达帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void Cadillac_XT5Tester::RadarSimTask::_doTask(void *p_arg)
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


Cadillac_XT5Tester::ParkToSideKeySimTask::ParkToSideKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void Cadillac_XT5Tester::ParkToSideKeySimTask::_doTask(void *p_arg)
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



Cadillac_XT5Tester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio) {

}
void Cadillac_XT5Tester::TireKeySimTask::_doTask(void *p_arg)
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




Cadillac_XT5Tester::CarLightTask::CarLightTask(uint8_t prio) : ZTask(prio){

}

void Cadillac_XT5Tester::CarLightTask::_doTask(void * p_arg){
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
