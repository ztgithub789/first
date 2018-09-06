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
#include "PEUGEOTTester.h"
#include "PEUGEOT_common.h"


using namespace PEUGEOT;

PEUGEOTTester::PEUGEOTTester() {
	// TODO Auto-generated constructor stub
	pCar = NULL;
}

PEUGEOTTester::~PEUGEOTTester() {
	// TODO Auto-generated destructor stub
	delete pshiftsimTask;
	pshiftsimTask = NULL;

	delete psteeringTask;
	psteeringTask = NULL;

	delete pKeyTask;
	pKeyTask = NULL;

	delete pradarSimTask;
	pradarSimTask = NULL;

	delete pturnSimTask;
	pturnSimTask = NULL;

	delete pParkToSideTask;
	pParkToSideTask = NULL;
        
 	delete ptirekeySimTask;
	ptirekeySimTask = NULL;
	
	//delete pEngineSimTask;
	//pEngineSimTask = NULL;

	delete pACCStatusTask;
	pACCStatusTask = NULL;

	delete pFrontDoorKeyTask;
	pFrontDoorKeyTask = NULL;

}

void PEUGEOTTester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void PEUGEOTTester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void PEUGEOTTester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void PEUGEOTTester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void PEUGEOTTester::togglePKey()
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
void PEUGEOTTester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x5a, 0x74, 0x74, 0x5a, 0x3c, 0xa0, 0xa0, 0x3c};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// 保证在循环周期内有一次0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}

uint8_t PEUGEOTTester::run(CarInfo *pcar)
{
	pCar = pcar;
	pCar->maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;

	pshiftsimTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
	pshiftsimTask->start(pCar);

	psteeringTask = new SteeringWheelSimTask(STEERING_WHEEL_SIM_TASK_PRIO);
	psteeringTask->start(pCar);

	pKeyTask = new PKeySimTask(P_KEY_SIM_TASK_PRIO);
	pKeyTask->start(pCar);

	pradarSimTask = new RadarSimTask(RADAR_SIM_TASK_PRIO);
	pradarSimTask->start(pCar);

	pParkToSideTask = new ParkToSideKeySimTask(TURNING_LIGHT_SIM_TASK_PRIO);
	pParkToSideTask->start(pCar);

	pturnSimTask = new TurnSimTask( TURN_SIM_TASK_PRIO);
	pturnSimTask->start(pCar);
        
   	 ptirekeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO); //TIREKEY_SIM_TASK_PRIO
	ptirekeySimTask->start(pCar); 
	
	//pEngineSimTask = new EngineSimTask(REMOTE_KEY_TASK_PRIO);
	//pEngineSimTask->start(pCar);

	pACCStatusTask = new ACCStatusTask(BACK_DOOR_STATUS_TASK_PRIO);
	pACCStatusTask->start(pCar);

	pFrontDoorKeyTask = new FrontDoorKeyTask(FRONT_DOOR_KEY_TASK_PRIO);
	pFrontDoorKeyTask->start(pCar);

	return 0;
}


//档位
PEUGEOTTester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}

void PEUGEOTTester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = SHIFT_POS_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 3,
	  .Data = {0x00,0x00, 0x00} //00 00 00
	};
	//0x00默认为P档

	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		uint8_t shiftPos = pCar->shiftPos;

		switch(shiftPos) {
		case CarInfo::SHIFT_POS_P:
			//printf("SHIFT_POS_P\n");
			msg.Data[1] = 0x00;
			break;
		case CarInfo::SHIFT_POS_R:
			//printf("SHIFT_POS_R\n");
			msg.Data[1] = 0x10;
			break;
		case CarInfo::SHIFT_POS_N:
			msg.Data[1] = 0x20;
			break;
		case CarInfo::SHIFT_POS_D:
			msg.Data[1] = 0x30;
			break;
		case CarInfo::SHIFT_POS_S:
			msg.Data[1] = 0xc0;
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

//方控
PEUGEOTTester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio) {

}
void PEUGEOTTester::TireKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg1 =
	{
	  .StdId = TIRE_KEY_STATUS_MSD_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 3,
	  .Data = {0x00, 0x00, 0x00}
	};
	
      CanTxMsg msg2 =
	{
	  .StdId = TIRE_KEY_STATUS_MSD_ID2,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 6,
	  .Data = {0x00, 0x00, 0x00, 0x00,0x00,  0xff}
	};
	
	CarInfo *pCar = (CarInfo*)p_arg;
    static uint8_t times = 0, status = 0, oldkey = 0, newkey = 0;   
	while(1) {

		OSTimeDlyHMSM(0,0,0,100);

        newkey = pCar->TireKey;

        
        switch(newkey)
        {
        	//success
        	case 0://没有按任何键
                msg1.Data[0]=0;
                msg1.Data[1]=0;
                msg1.Data[2]=0;
		   msg2.Data[1]=0;
                break;
		 case 1://return
                msg2.Data[1]=0x10;    
		   printf("return\n");
                break;
		case 2://SRC
                msg1.Data[3]= 0;
                msg1.Data[1]=0;
                msg1.Data[2]=0x40;
		    printf("SRC\n");
		   break;
		case 4://list
                msg1.Data[0]=0x01;
                msg1.Data[1]=0;
                msg1.Data[2]=0;
		   printf("list\n");
		  break;
		case 7://ok
                msg1.Data[0]=0;
                msg1.Data[1]=0;
		   msg1.Data[2]=0x80;
    		   printf("ok\n");
		   break;
		case 12://no_voice
                msg1.Data[0]=0;
                msg1.Data[1]=0;
                msg1.Data[2]=0x0c;
			printf("no voice\n");	
		  break;
		
				                      
            default ://
                msg1.Data[0]=0;
                msg1.Data[1]=0;
                msg1.Data[2]=0;
		   msg2.Data[1]=0;
                break;
        }

		gpDrvCAN->transmit(CAN1, &msg1);
		gpDrvCAN->transmit(CAN1, &msg2);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			/* Release any owned resources; */
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}



//转角
PEUGEOTTester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void PEUGEOTTester::SteeringWheelSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = STEERING_WHEER_ANGLE_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 7,
	  .Data = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0} //00 00 00 00 11 60 00 9f
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
		int32_t angle = pCar->steerAngle;

		if (angle <= 0){
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE/MAX_ABS_STEER_ANGLE + 0x0000;
		}else{
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE/MAX_ABS_STEER_ANGLE + 0xfff1;
		}	
		
		msg.Data[0] = (angle & 0xff00) >> 8;
		msg.Data[1] = (angle & 0x00ff) >> 0;

		gpDrvCAN->transmit(CAN1, &msg);  
		//printf("STEER:%d\n",&msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}


PEUGEOTTester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void PEUGEOTTester::PKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = P_KEY_STATUS_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 4,
	  .Data = {0x22, 0x94, 0x00, 0x00}  //22 94 01 01
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,50);

		static bool oldv = false;
		bool newv = pCar->isPKeyPressed;
		if (oldv != newv){
			if (newv) {
				msg.Data[2] = 0x01;
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

PEUGEOTTester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio) {

}

/*******************************************************************************
 * 简介   :	模拟雷达帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void PEUGEOTTester::RadarSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = RADAR_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x00, 0x00, 0x00, 0xfe,0xc2, 0x00}
	};
	
	CarInfo *pCar = (CarInfo*)p_arg;
	
	static bool isLeft = true;
	static bool isMiddle = false;
	 //pCar->isRadaLeft = true;
	//int16_t value1 = (int16_t)((msg.Data[2])|(msg.Data[3]));
	//int16_t value2 = (int16_t)((msg.Data[2])|(msg.Data[4]));

	while(1){

		OSTimeDlyHMSM(0,0,0,100);

		if(isLeft){
			msg.Data[1] = 0x50;
			msg.Data[4] = 0xfc;
			msg.Data[2] = pCar->radarVal[CarInfo::RADAR_LRS];
			msg.Data[3] = pCar->radarVal[CarInfo::RADAR_LRS];
			isLeft=false;
			isMiddle=true;
		} else if(isMiddle){
			msg.Data[1] = 0xd0;
			msg.Data[4] = 0xfc;
			msg.Data[2] = pCar->radarVal[CarInfo::RADAR_LRM];
			msg.Data[3] = pCar->radarVal[CarInfo::RADAR_RRM];
			isMiddle=false;
		}else{
			msg.Data[1] = 0x90;
			msg.Data[3] = 0xfc;
			msg.Data[2] = pCar->radarVal[CarInfo::RADAR_RRS];
			msg.Data[4] = pCar->radarVal[CarInfo::RADAR_RRS];
			isLeft=true;
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

#if 0
//发动机启停功能，接收发送过来的数据
PEUGEOTTester::EngineSimTask::EngineSimTask(uint8_t prio):ZTask(prio){
// TODO Auto-generated constructor stub
    isEngineStart = false;
   
}


void PEUGEOTTester::EngineSimTask::_doTask(void * p_arg)
{
    CanTMsg msg;
    
    while(1){    
        uint8_t err = gpDrvCAN->getData(&msg,0);
        if(err == OS_ERR_NONE){
           if(msg.CANx == CAN2){
                if(msg.msg.IDE == CAN_ID_STD){
                   if(msg.msg.StdId==ENGINE_KEY_ID){
                        if(msg.msg.Data[3]==0x80){
                            isEngineStart = true; 
                        }else{
                            isEngineStart = false;
                        }
                   }
                }
           }
        }

        if(isEngineStart==true){
          
                 gpDrvLED4->on();
        
        }else{
            gpDrvLED4->off();
        }
    }
}
#endif

#if 0
void PEUGEOTTester::EngineSimTask::_doTask(void * p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = ENGINE_KEY_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 5,
	  .Data = {0x00, 0x00, 0x00, 0x40, 0x00}
	};
	
	CarInfo *pCar = (CarInfo*)p_arg;

	 newkey = pCar->TireKey;

	while(1){

		OSTimeDlyHMSM(0,0,0,100);
		switch(newkey){
			case 3:
				msg.Data[3] = 0x80;
				break;
			defualt:
				msg.Data[3] = 0x40;
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
#endif


PEUGEOTTester::ParkToSideKeySimTask::ParkToSideKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void PEUGEOTTester::ParkToSideKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = TIRE_KEY_STATUS_MSD_ID,
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
				msg.Data[1] = 0x0F;
			} else {
				msg.Data[1] = 0x00;
			}
			gpDrvCAN->transmit(CAN1, &msg);
			oldv = newv;
		}
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

//转向灯
PEUGEOTTester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio) {

}
void PEUGEOTTester::TurnSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = TURNIDICATOR_STATUS_MSD_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x88, 0x52, 0x00, 0x1a,0x15, 0x77, 0x77, 0x00}
	};
    
	CarInfo *pCar = (CarInfo*)p_arg;
    
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
        
		switch(pCar->lightPos){
            case 0://none
                msg.Data[7] = 0x20;
                break;
            case 1://left
                msg.Data[7] = 0x22;
		   printf("TURNIDICATOR_LEFT\n");
                break;
            case 2://right
                msg.Data[7] = 0x21;
		   printf("TURNIDICATOR_RIGHT\n");
                break;
             case 3://flash
                msg.Data[7] = 0x23;
		   printf("TURNIDICATOR_FLASH\n");
                break;
            default:
                msg.Data[7] = 0x20;
                break;
        }
        
		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}


//时间
PEUGEOTTester::CarTimeTask::CarTimeTask(uint8_t prio):ZTask(prio){
// TODO Auto-generated constructor stub
}

void PEUGEOTTester::CarTimeTask::_doTask(void * p_arg){
    CanTxMsg msg =
        {
          .StdId = 0x6b6,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 6,
          .Data = {0x11, 0x10, 0x09, 0x0e, 0x22, 0x00}//0x11, 0x10, 0x09, 0x0e, 0x22, 0x00//2016年01月09日14时34分00秒
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
                            mon += 0x10;
                            if(mon>0xc){
                                mon = 0x01;
                                year += 0x01;
                                msg.Data[1] = year;
                            }else{
                                msg.Data[0] = mon;
                            }
                        }else{
                            msg.Data[2] = day;
                        }
                    }else{
                        msg.Data[3] = hour;
                    }
                }else{
                    msg.Data[4] = min;
                }
            }else{
                msg.Data[5] = sec;
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



PEUGEOTTester::FrontDoorKeyTask::FrontDoorKeyTask(uint8_t prio) : ZTask(prio) {

}

void PEUGEOTTester::FrontDoorKeyTask::_doTask(void * p_arg) {
	CanTxMsg msg =
	{
			.StdId = FRONTDOOR_KEY_ID_Q7,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x01, 0x00, 0x04, 0x00, 0x84, 0x00, 0x1f, 0x00}
	};	

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1){
		
		OSTimeDlyHMSM(0,0,0,100);

		if(pCar->isFDTKPressed){
			msg.Data[1] = 0x08;
		}else{
			msg.Data[1] = 0x00;
		}

		gpDrvCAN->transmit(CAN1, &msg);

		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { /* Release any owned resources; */ 		
																  /* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}

	}	
}

PEUGEOTTester::ACCStatusTask::ACCStatusTask(uint8_t prio) : ZTask(prio) {

}

void PEUGEOTTester::ACCStatusTask::_doTask(void * p_arg){
	CanTxMsg msg =
	{
			.StdId = ACC_STATUS_ID,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 7,
			.Data = {0x01, 0x4c, 0x40, 0x00, 0x00, 0x00, 0x04}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1){

		OSTimeDlyHMSM(0,0,0,200);
		
		gpDrvCAN->transmit(CAN1, &msg);	

		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { /* Release any owned resources; */ 		
															      /* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
    	}
	}
}


