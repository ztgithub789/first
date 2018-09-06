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
#include "RangeRoverTester.h"
#include "RangeRover_common.h"

using namespace RANGEROVER_NS;

RangeRoverTester::RangeRoverTester(void){
// TODO Auto-generated constructor stub
    pCar = NULL;
}

RangeRoverTester::~RangeRoverTester(void){
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

uint8_t RangeRoverTester::run(CarInfo * _pCar){
    pCar = _pCar;
    pCar->maxAbsSteerAngle =MAX_ABS_STEER_ANGLE;

    pShiftSimTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
    pShiftSimTask->start(pCar);

    pSteeringWheelSimTask = new SteeringWheelSimTask(STEERING_WHEEL_SIM_TASK_PRIO);
    pSteeringWheelSimTask->start(pCar);

    pPKeySimTask = new PKeySimTask(P_KEY_SIM_TASK_PRIO);
    //pPKeySimTask->start(pCar);
    
    pRadarSimTask = new RadarSimTask(RADAR_SIM_TASK_PRIO);
    pRadarSimTask->start(pCar);
    
    pTireKeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO);
    //pTireKeySimTask->start(pCar);

    pTurnSimTask = new TurnSimTask(TURN_SIM_TASK_PRIO);
    pTurnSimTask->start(pCar);

    pCarTimeTask = new CarTimeTask(CARTIME_TASK_PRIO);
    pCarTimeTask->start(pCar);

    pScreenStatusTask = new ScreenStatusTask(BACKLIGHT_SIM_TASK_PRIO);
    //pScreenStatusTask->start(pCar);

    pEnabledCANTask = new EnabledCANTask(ENABLED_CAN_TASK_PRIO);
    //pEnabledCANTask->start();

    return 0;
}

void RangeRoverTester::steerLeft(void){
    if (pCar->steerAngle > -MAX_ABS_CAN_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void RangeRoverTester::steerRight(void){
    if (pCar->steerAngle < MAX_ABS_CAN_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

RangeRoverTester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void RangeRoverTester::ShiftSimTask::_doTask(void * p_arg){
    CanTxMsg msg =
	{
	  .StdId = SHIFT_POS_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x80, 0x00, 0xf2, 0x5a, 0xff, 0x00, 0x00}//00 80 00 f2 5a ff 00 00
	};

    CarInfo *pCar = (CarInfo *)p_arg;

    while(1){
        OSTimeDlyHMSM(0,0,0,60);

        switch(pCar->shiftPos){
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

        gpDrvCAN->transmit(CAN1,&msg);
        if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ){ 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
    }
}

RangeRoverTester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void RangeRoverTester::SteeringWheelSimTask::_doTask(void * p_arg){ 
    CanTxMsg msg =
        {
          .StdId = STEERING_WHEER_ANGLE_MSG_ID,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 8,
          .Data = {0x38, 0x30, 0xad, 0x49, 0x00, 0x1e, 0x6f, 0x00}//38 30 ad 49 00 1e 6f 00
        };

    CarInfo *pCar = (CarInfo *)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,60);

        //angle = abs(pCar->steerAngle)+CAN_STEER_MINDDLE_VAL;
		int32_t angle = pCar->steerAngle;
		if (angle <= 0) {
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + CAN_STEER_MINDDLE_VAL;
		} else {
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + CAN_STEER_MINDDLE_VAL;
		}

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

RangeRoverTester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void RangeRoverTester::PKeySimTask::_doTask(void * p_arg){

    while(1){
        OSTimeDlyHMSM(0,0,0,60);
    }
}

RangeRoverTester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void RangeRoverTester::RadarSimTask::_doTask(void * p_arg){
		CanTxMsg msg =
		{
		  .StdId = 0x220,
		  .ExtId = 0,
		  .IDE = 0,
		  .RTR = 0,
		  .DLC = 8,
		  .Data = {0xff, 0xff, 0xff, 0x10, 0xff, 0x0c, 0x56, 0x28}//ff ff ff 10 ff 0c 56 28
		};
	
		CanTxMsg msg2 =
		{
		  .StdId = 0x234,
		  .ExtId = 0,
		  .IDE = 0,
		  .RTR = 0,
		  .DLC = 8,
		  .Data = {0xff, 0xff, 0xff, 0xff, 0x54, 0xff, 0x55, 0xc8}//ff ff ff ff 54 ff 55 c8
		};
	
		CarInfo *pCar = (CarInfo*)p_arg;
		//static char time = 39;
	
		while(1) {
			OSTimeDlyHMSM(0,0,0,100);
		//	time++;
		//	if( (pCar->isRadaFront==true) ||(time ==40)){
			if(pCar->isRadaFront ==true) {
				// 前帧
				msg2.Data[4] = pCar->radarVal[0];
				msg2.Data[2] = pCar->radarVal[1];
				msg2.Data[0] = pCar->radarVal[2];
				msg2.Data[1] = pCar->radarVal[3];
				msg2.Data[3] = pCar->radarVal[4];
				msg2.Data[5] = pCar->radarVal[5];
				pCar->isRadaFront = false;
		//	} else if( (pCar->isRadaBack==true) ||(time== 48)){
			} else {
				msg.Data[4] = pCar->radarVal[2];
				msg.Data[2] = pCar->radarVal[3];
				msg.Data[0] = pCar->radarVal[4];
				msg.Data[1] = pCar->radarVal[5];
				msg.Data[3] = pCar->radarVal[6];
				msg.Data[5] = pCar->radarVal[7];
				pCar->isRadaFront = true;
			}
		/*	if(time >= 48)
					time=0;
	*/
			 /*switch(pCar->lightPos)
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
				}*/
	
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

RangeRoverTester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void RangeRoverTester::TireKeySimTask::_doTask(void * p_arg){

    while(1){
        OSTimeDlyHMSM(0,0,0,60);
    }
}

RangeRoverTester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void RangeRoverTester::TurnSimTask::_doTask(void * p_arg){
    CanTxMsg msg1 =
        {
          .StdId = TURNILIGHT_LEFT_MSG,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 8,
          .Data = {0x20, 0xf7, 0x32, 0xf7, 0x40, 0x00, 0x04, 0x00}//20 f7 32 f7 40 00 04 00
        };

    CarInfo *pCar = (CarInfo *)p_arg;

    while(1){
        OSTimeDlyHMSM(0,0,0,50);

        switch(pCar->lightPos){
            case 0://none
                msg1.Data[0] = 0x20;
                break;
            case 1://left
                msg1.Data[0] = 0x24;
                break;
            case 2://right
                msg1.Data[0] = 0x28;
                break;
            case 3://flash
                msg1.Data[0] = 0x2c;
                break;
            default:
                msg1.Data[0] = 0x20;
                break;
        }

        gpDrvCAN->transmit(CAN1, &msg1);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
    }
}

RangeRoverTester::CarTimeTask::CarTimeTask(uint8_t prio):ZTask(prio){
// TODO Auto-generated constructor stub
}

void RangeRoverTester::CarTimeTask::_doTask(void * p_arg){
    CanTxMsg msg =
        {
          .StdId = CAR_TIME_MSG_ID,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 8,
          .Data = {0x11, 0x05, 0x19, 0x09, 0x11, 0x00, 0x00, 0x00}//00 00 0f 09 11 00 00 00 //2015年9月17日0时0分0秒
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
                                    msg.Data[0] = year;
                                }
                            }else{
                                msg.Data[1] = mon;
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

RangeRoverTester::ScreenStatusTask::ScreenStatusTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void RangeRoverTester::ScreenStatusTask::_doTask(void * p_arg){
    while(1){
       OSTimeDlyHMSM(0,0,0,60);
    }
}

RangeRoverTester::EnabledCANTask::EnabledCANTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
    isModifiedShift = false;
    isModifiedBackLight = false;
    isAlternate = true;
}

void RangeRoverTester::EnabledCANTask::_doTask(void * p_arg){
    CanTMsg msg;
    
    while(1){    
      OSTimeDlyHMSM(0,0,0,60);
    }
}

