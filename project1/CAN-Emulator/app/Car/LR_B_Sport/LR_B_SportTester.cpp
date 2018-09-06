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
#include "LR_B_SportTester.h"
#include "LR_B_Sport_common.h"

using namespace LANDROVER_NS;

LR_B_SportTester::LR_B_SportTester(void){
// TODO Auto-generated constructor stub
    pCar = NULL;
}

LR_B_SportTester::~LR_B_SportTester(void){
// TODO Auto-generated constructor stub
    delete pShiftSimTask;
    pShiftSimTask = NULL;

    delete pSteeringWheelSimTask;
    pSteeringWheelSimTask = NULL;

    delete pPKeySimTask;
    pPKeySimTask = NULL;

    delete pRadarSimTask;
    pRadarSimTask = NULL;

    delete pACCSimTask;
    pACCSimTask = NULL;

    delete pTurnSimTask;
    pTurnSimTask = NULL;

    delete pCarTimeTask;
    pCarTimeTask = NULL;

    delete pScreenStatusTask;
    pScreenStatusTask = NULL;

    delete pEnabledCANTask;
    pEnabledCANTask = NULL;
}

uint8_t LR_B_SportTester::run(CarInfo * _pCar){
    pCar = _pCar;
    pCar->maxAbsSteerAngle =MAX_ABS_CAN_STEER_ANGLE;

    pShiftSimTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
    pShiftSimTask->start(pCar);//档位、转角

    //pSteeringWheelSimTask = new SteeringWheelSimTask(STEERING_WHEEL_SIM_TASK_PRIO);
    //pSteeringWheelSimTask->start(pCar);

    pPKeySimTask = new PKeySimTask(P_KEY_SIM_TASK_PRIO);
    pPKeySimTask->start(pCar);
    
    pRadarSimTask = new RadarSimTask(RADAR_SIM_TASK_PRIO);
    pRadarSimTask->start(pCar);//雷达、转向灯
    
    pACCSimTask = new ACCSimTask(TIREKEYT_SIM_TASK_PRIO);
    pACCSimTask->start(pCar);

    //pTurnSimTask = new TurnSimTask(TURN_SIM_TASK_PRIO);
    //pTurnSimTask->start(pCar);

    pCarTimeTask = new CarTimeTask(CARTIME_TASK_PRIO);
    pCarTimeTask->start(pCar);

    //pScreenStatusTask = new ScreenStatusTask(BACKLIGHT_SIM_TASK_PRIO);
    //pScreenStatusTask->start(pCar);

    //pEnabledCANTask = new EnabledCANTask(ENABLED_CAN_TASK_PRIO);
    //pEnabledCANTask->start();

    return 0;
}

void LR_B_SportTester::steerLeft(void){
    if (pCar->steerAngle > -MAX_ABS_CAN_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void LR_B_SportTester::steerRight(void){
    if (pCar->steerAngle < MAX_ABS_CAN_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

LR_B_SportTester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void LR_B_SportTester::ShiftSimTask::_doTask(void * p_arg){
    CanTxMsg msg1 =
	{
	  .StdId = SHIFT_POS_MSG_ID1,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0xfe, 0xc0, 0x34, 0x1c, 0x1e, 0x80, 0x3f, 0xff}//a0 e7 f8 00 03 1c 19 fd
	};
	CanTxMsg msg2 =
	{
	  .StdId = SHIFT_POS_MSG_ID2,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00}//a0 e7 f8 00 03 1c 19 fd
	};

    CarInfo *pCar = (CarInfo *)p_arg;
    int32_t angle = 0;

    while(1){
        OSTimeDlyHMSM(0,0,0,100);

        switch(pCar->shiftPos){
            case CarInfo::SHIFT_POS_P:
                msg1.Data[3] = 0x1c;
		   msg2.Data[0] = 0x00;
                break;
            case CarInfo::SHIFT_POS_R:
                msg1.Data[3] = 0x3c;
		   msg2.Data[0] = 0x01;
                break;
            case CarInfo::SHIFT_POS_N:
                msg1.Data[3] = 0x5c;
		   msg2.Data[0] = 0x02;
                break;
            case CarInfo::SHIFT_POS_D:
                msg1.Data[3] = 0x7c;
		   msg2.Data[0] = 0x04;
                break;
            default:
                break;
        }

		angle = abs(pCar->steerAngle)+CAN_STEER_MINDDLE_VAL;

		msg1.Data[4] = (angle & 0xff00) >> 8;
		msg1.Data[5] = (angle & 0x00ff) >> 0;

        gpDrvCAN->transmit(CAN1,&msg1);
	 gpDrvCAN->transmit(CAN1,&msg2);
        if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ){ 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
    }
}


LR_B_SportTester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void LR_B_SportTester::PKeySimTask::_doTask(void * p_arg){

    CanTxMsg msg =
	{
	  .StdId = 0x17d,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x00, 0x00,0x00,0x00,0x00,0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
		
		static bool oldv = false;
		bool newv = pCar->isPKeyPressed;
		if (oldv != newv){
			if (newv) {
				msg.Data[5] = 0x54;
			} else {
				msg.Data[5] = 0x0;
			}
			
			oldv = newv;
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

LR_B_SportTester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void LR_B_SportTester::RadarSimTask::_doTask(void * p_arg){

    CanTxMsg msg =
	{
	  .StdId = 0x174,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}
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
			msg.Data[5] = pCar->radarVal[CarInfo::RADAR_LFS];
			msg.Data[3] = pCar->radarVal[CarInfo::RADAR_LFM];
			msg.Data[4] = pCar->radarVal[CarInfo::RADAR_RFM];
			msg.Data[6] = pCar->radarVal[CarInfo::RADAR_RFS];
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
			msg2.Data[5] = pCar->radarVal[CarInfo::RADAR_LRS];
			msg2.Data[3] = pCar->radarVal[CarInfo::RADAR_LRM];
			msg2.Data[4] = pCar->radarVal[CarInfo::RADAR_RRM];
			msg2.Data[6] = pCar->radarVal[CarInfo::RADAR_RRS];
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

		switch(pCar->lightCtl){
		  	case 0:
				msg.Data[0] = 0x00;
				break;
			case 1:
				msg.Data[0] = 0x01;
				break;
			case 2:
				msg.Data[0] = 0x01;
				break;
			case 3:
				msg.Data[0] = 0x01;
				break;
			default:
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

LR_B_SportTester::ACCSimTask::ACCSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void LR_B_SportTester::ACCSimTask::_doTask(void * p_arg){

	CanTxMsg msg =
	{
	  .StdId = 0xba,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88}//00 00 00 11
	};
    
	CarInfo *pCar = (CarInfo*)p_arg;

    while(1){
        OSTimeDlyHMSM(0,0,0,100);

	  	static bool oldv = false;
		bool newv = pCar->isACCOn;
		if (oldv != newv){
			if (newv) {
				msg.Data[5] = 0x10;
			} else {
				msg.Data[5] = 0x0;
			}
			oldv = newv;
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


LR_B_SportTester::CarTimeTask::CarTimeTask(uint8_t prio):ZTask(prio){
// TODO Auto-generated constructor stub
}

void LR_B_SportTester::CarTimeTask::_doTask(void * p_arg){
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
		
		OSTimeDlyHMSM(0,0,0,100);
		
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

LR_B_SportTester::ScreenStatusTask::ScreenStatusTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void LR_B_SportTester::ScreenStatusTask::_doTask(void * p_arg){
    CanTxMsg msg =
        {
          .StdId = LCD_BACK_LIGHT_MSG_ID,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 8,
          .Data = {0x01, 0x14, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00}//00 00 00 00 00 00 00 00
        };

    u8 i = 0x01;

    while(1){
        OSTimeDlyHMSM(0,0,0,150);

	  if(i > 0x25){
		i = 0x01;
	  }

	  msg.Data[0] = i;
	  
	  switch(i){
		case 0x01:
			msg.Data[1] = 0x14;
			break;
		case 0x02:
		case 0x04:
		case 0x08:
		case 0x0a:
		case 0x0f:
		case 0x11:
		case 0x12:
		case 0x17:
		case 0x20:
		case 0x23:
		case 0x24:
			msg.Data[1] = 0x01;
			break;
		case 0x03:
		case 0x05:
		case 0x09:
		case 0x0d:
		case 0x1f:
			msg.Data[1] = 0x02;
			break;
		case 0x15:
		case 0x1a:
			msg.Data[1] = 0x03;
			break;
		case 0x13:
			msg.Data[1] = 0x12;
			break;
		case 0x06:
		case 0x07:
		case 0x0b:
		case 0x0c:
		case 0x0e:
		case 0x10:
		case 0x14:
		case 0x16:
		case 0x18:
		case 0x19:
		case 0x1b:
		case 0x1c:
		case 0x1d:
		case 0x1e:
		case 0x21:
		case 0x22:
			msg.Data[1] = 0x00;
			break;
		default:
			break;			
	}
	  
        gpDrvCAN->transmit(CAN1, &msg);
	
	i++;
		
	if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
		/* Release any owned resources; */ 
		/* De-allocate any dynamic memory; */
		OSTaskDel(OS_PRIO_SELF);
		break;
	}
    }
}

LR_B_SportTester::EnabledCANTask::EnabledCANTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
    isModifiedShift = false;
    isModifiedBackLight = false;
    isAlternate = true;
}

void LR_B_SportTester::EnabledCANTask::_doTask(void * p_arg){
    CanTMsg msg;
    
    while(1){    
        uint8_t err = gpDrvCAN->getData(&msg,0);
        if(err == OS_ERR_NONE){
           if(msg.CANx == CAN2){
                if(msg.msg.IDE == CAN_ID_STD){
                   if(msg.msg.StdId==SHIFT_POS_MSG_ID1){
                        if(msg.msg.Data[3]==0x3c){
                            isModifiedShift = true; 
                        }else{
                            isModifiedShift = false;
                        }
                   }else if(msg.msg.StdId==LCD_BACK_LIGHT_MSG_ID){
                        if(msg.msg.Data[0]==0x0f){
				  msg.msg.Data[1]==0x02;
                             isModifiedBackLight = true;
                        }else{
                             isModifiedBackLight = false;
                        }
                   }
                }
           }
        }

        if((isModifiedShift==true) && (isModifiedBackLight==true)){
            if(isAlternate){
                isAlternate = false;
                 gpDrvLED4->off();
            }else{
                isAlternate = true;
                 gpDrvLED4->on();
            }  
        }else{
            gpDrvLED4->off();
        }
    }
}


LR_B_SportTester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void LR_B_SportTester::SteeringWheelSimTask::_doTask(void * p_arg){ 
    CanTxMsg msg =
        {
          .StdId = 0x40a,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 8,
          .Data = {0xfe, 0xc0, 0x34, 0x1c, 0x1e, 0x80, 0x3f, 0xff}//6e 10 00 01 0a 80 22 82
        };

    CarInfo *pCar = (CarInfo *)p_arg;
    int32_t angle = 0;

	while(1) {
		OSTimeDlyHMSM(0,0,0,60);

        angle = abs(pCar->steerAngle)+CAN_STEER_MINDDLE_VAL;

		msg.Data[5] = (angle & 0xff00) >> 8;
		msg.Data[6] = (angle & 0x00ff) >> 0;

		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}


LR_B_SportTester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void LR_B_SportTester::TurnSimTask::_doTask(void * p_arg){
    CanTxMsg msg = {
	  .StdId = 0x174,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}//00 11 00 00 00 00 00 07
	};

    CarInfo *_pCar = (CarInfo *)p_arg;

    while(1){      
        switch(_pCar->lightPos){
            case 0:
                msg.Data[1] = 0x00;
                break;
            case 1:
                msg.Data[1] = 0x80;
                break;
            case 2:
                msg.Data[1] = 0x40;
                break;
            case 3:
                msg.Data[1] = 0xc0;
                break;
            default:
		   msg.Data[1] = 0x00;	
                break;
        }

	  switch(_pCar->lightCtl){
	  	case 0:
			msg.Data[0] = 0x00;
			break;
		case 1:
			msg.Data[0] = 0x01;
			break;
		case 2:
			msg.Data[0] = 0x01;
			break;
		case 3:
			msg.Data[0] = 0x01;
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

