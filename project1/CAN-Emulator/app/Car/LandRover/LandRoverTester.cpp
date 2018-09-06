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
#include "LandRoverTester.h"
#include "LandRover_common.h"

using namespace LANDROVER_NS;

LandRoverTester::LandRoverTester(void){
// TODO Auto-generated constructor stub
    pCar = NULL;
}

LandRoverTester::~LandRoverTester(void){
// TODO Auto-generated constructor stub
    delete pShiftSimTask;
    pShiftSimTask = NULL;

    delete pSteeringWheelSimTask;
    pSteeringWheelSimTask = NULL;

    delete pPKeySimTask;
    pPKeySimTask = NULL;

    delete pRadarSimTask;
    pRadarSimTask = NULL;

    delete pTireKeySimTask;
    pTireKeySimTask = NULL;

    delete pTurnSimTask;
    pTurnSimTask = NULL;

    delete pCarTimeTask;
    pCarTimeTask = NULL;

    delete pScreenStatusTask;
    pScreenStatusTask = NULL;

    delete pEnabledCANTask;
    pEnabledCANTask = NULL;
}

uint8_t LandRoverTester::run(CarInfo * _pCar){
    pCar = _pCar;
    pCar->maxAbsSteerAngle =MAX_ABS_CAN_STEER_ANGLE;

    pShiftSimTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
    pShiftSimTask->start(pCar);

    pSteeringWheelSimTask = new SteeringWheelSimTask(STEERING_WHEEL_SIM_TASK_PRIO);
    pSteeringWheelSimTask->start(pCar);

    pPKeySimTask = new PKeySimTask(P_KEY_SIM_TASK_PRIO);
    //pPKeySimTask->start(pCar);
    
    pRadarSimTask = new RadarSimTask(RADAR_SIM_TASK_PRIO);
    //pRadarSimTask->start(pCar);
    
    pTireKeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO);
    //pTireKeySimTask->start(pCar);

    pTurnSimTask = new TurnSimTask(TURN_SIM_TASK_PRIO);
    pTurnSimTask->start(pCar);

    pCarTimeTask = new CarTimeTask(CARTIME_TASK_PRIO);
    pCarTimeTask->start(pCar);

    pScreenStatusTask = new ScreenStatusTask(BACKLIGHT_SIM_TASK_PRIO);
    pScreenStatusTask->start(pCar);

    pEnabledCANTask = new EnabledCANTask(ENABLED_CAN_TASK_PRIO);
    pEnabledCANTask->start();

    return 0;
}

void LandRoverTester::steerLeft(void){
    if (pCar->steerAngle > -MAX_ABS_CAN_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void LandRoverTester::steerRight(void){
    if (pCar->steerAngle < MAX_ABS_CAN_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

LandRoverTester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void LandRoverTester::ShiftSimTask::_doTask(void * p_arg){
    CanTxMsg msg =
	{
	  .StdId = SHIFT_POS_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0xa0, 0xe7, 0xf8, 0x00, 0x03, 0x1c, 0x19, 0xfd}//a0 e7 f8 00 03 1c 19 fd
	};

    CarInfo *pCar = (CarInfo *)p_arg;

    while(1){
        OSTimeDlyHMSM(0,0,0,60);

        switch(pCar->shiftPos){
            case CarInfo::SHIFT_POS_P:
                msg.Data[4] = 0x03;
                break;
            case CarInfo::SHIFT_POS_R:
                msg.Data[4] = 0x23;
                break;
            case CarInfo::SHIFT_POS_N:
                msg.Data[4] = 0x43;
                break;
            case CarInfo::SHIFT_POS_D:
                msg.Data[4] = 0x63;
                break;
            default:
                break;
        }

        gpDrvCAN->transmit(CAN1,&msg);
        if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ){ 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
    }
}

LandRoverTester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void LandRoverTester::SteeringWheelSimTask::_doTask(void * p_arg){ 
    CanTxMsg msg =
        {
          .StdId = STEERING_WHEER_ANGLE_MSG_ID,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 8,
          .Data = {0x6e, 0x10, 0x00, 0x01, 0x0a, 0x80, 0x22, 0x82}//6e 10 00 01 0a 80 22 82
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

LandRoverTester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void LandRoverTester::PKeySimTask::_doTask(void * p_arg){

    while(1){
        OSTimeDlyHMSM(0,0,0,60);
    }
}

LandRoverTester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void LandRoverTester::RadarSimTask::_doTask(void * p_arg){

    while(1){
        OSTimeDlyHMSM(0,0,0,60);
    }
}

LandRoverTester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void LandRoverTester::TireKeySimTask::_doTask(void * p_arg){

    while(1){
        OSTimeDlyHMSM(0,0,0,60);
    }
}

LandRoverTester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void LandRoverTester::TurnSimTask::_doTask(void * p_arg){
    CanTxMsg msg1 =
        {
          .StdId = TURNILIGHT_LEFT_MSG_ID,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 8,
          .Data = {0x05, 0x02, 0x00, 0x01, 0x04, 0x00, 0x00, 0x00}//05 02 00 01 04 00 00 00
        };
    
    CanTxMsg msg2 =
        {
          .StdId = TURNILIGHT_RIGHT_MSG_ID,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 8,
          .Data = {0x08, 0x02, 0x00, 0x01, 0x05, 0x00, 0x00, 0x00}//08 02 00 01 05 00 00 00
        };

    CarInfo *pCar = (CarInfo *)p_arg;

    while(1){
        OSTimeDlyHMSM(0,0,0,900);

        switch(pCar->lightPos){
            case 0://none
                msg1.Data[2] = 0;
                msg2.Data[2] = 0;
                break;
            case 1://left
                msg1.Data[2] = 4;
                msg2.Data[2] = 0;
                break;
            case 2://right
                msg1.Data[2] = 0;
                msg2.Data[2] = 4;
                break;
            case 3://flash
                msg1.Data[2] = 4;
                msg2.Data[2] = 4;
                break;
            default:
                msg1.Data[2] = 0;
                msg2.Data[2] = 0;
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

LandRoverTester::CarTimeTask::CarTimeTask(uint8_t prio):ZTask(prio){
// TODO Auto-generated constructor stub
}

void LandRoverTester::CarTimeTask::_doTask(void * p_arg){
    CanTxMsg msg =
        {
          .StdId = CAR_TIME_MSG_ID,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 8,
          .Data = {0x00, 0x00, 0x0f, 0x09, 0x11, 0x00, 0x00, 0x00}//00 00 0f 09 11 00 00 00 //2015年9月17日0时0分0秒
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
                                    msg.Data[2] = year;
                                }
                            }else{
                                msg.Data[3] = mon;
                            }
                        }else{
                            msg.Data[4] = day;
                        }
                    }else{
                        msg.Data[5] = hour;
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

LandRoverTester::ScreenStatusTask::ScreenStatusTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void LandRoverTester::ScreenStatusTask::_doTask(void * p_arg){
    CanTxMsg msg =
        {
          .StdId = LCD_BACK_LIGHT_MSG_ID,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 8,
          .Data = {0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00}//00 00 00 00 00 00 00 00
        };

    CarInfo *pCar = (CarInfo *)p_arg;

    while(1){
        OSTimeDlyHMSM(0,0,0,60);

        if(pCar->shiftPos==CarInfo::SHIFT_POS_R){
            msg.Data[7] = 0x80;
        }else{
            msg.Data[7] = 0;
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

LandRoverTester::EnabledCANTask::EnabledCANTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
    isModifiedShift = false;
    isModifiedBackLight = false;
    isAlternate = true;
}

void LandRoverTester::EnabledCANTask::_doTask(void * p_arg){
    CanTMsg msg;
    
    while(1){    
        uint8_t err = gpDrvCAN->getData(&msg,0);
        if(err == OS_ERR_NONE){
           if(msg.CANx == CAN2){
                if(msg.msg.IDE == CAN_ID_STD){
                   if(msg.msg.StdId==SHIFT_POS_MSG_ID){
                        if(msg.msg.Data[4]==0x22){
                            isModifiedShift = true; 
                        }else{
                            isModifiedShift = false;
                        }
                   }else if(msg.msg.StdId==LCD_BACK_LIGHT_MSG_ID){
                        if(msg.msg.Data[7]==0x80){
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