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
#include "volvoTester.h"
#include "volvo_common.h"

using namespace VOLVO_NS;

VolvoTester::VolvoTester(void){
// TODO Auto-generated constructor stub
    pCar = NULL;
}

VolvoTester::~VolvoTester(void){
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
}

uint8_t VolvoTester::run(CarInfo * _pCar){
    pCar = _pCar;
    pCar->maxAbsSteerAngle =MAX_ABS_STEER_ANGLE;

    pShiftSimTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
    pShiftSimTask->start(pCar);

    pSteeringWheelSimTask = new SteeringWheelSimTask(STEERING_WHEEL_SIM_TASK_PRIO);
    pSteeringWheelSimTask->start(pCar);

    pPKeySimTask = new PKeySimTask(P_KEY_SIM_TASK_PRIO);
    //pPKeySimTask->start(pCar);
    
    pRadarSimTask = new RadarSimTask(RADAR_SIM_TASK_PRIO);
    //pRadarSimTask->start(pCar);
    
    pTireKeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO);
    pTireKeySimTask->start(pCar);

    pTurnSimTask = new TurnSimTask(TURN_SIM_TASK_PRIO);
    pTurnSimTask->start(pCar);

    pCarTimeTask = new CarTimeTask(CARTIME_TASK_PRIO);
    pCarTimeTask->start(pCar);

    return 0;
}

void VolvoTester::steerLeft(void){
    if (pCar->steerAngle > -MAX_ABS_CAN_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void VolvoTester::steerRight(void){
    if (pCar->steerAngle < MAX_ABS_CAN_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void VolvoTester::increaseRadarVal(int nr,int dv){
    uint8_t maxRadarVal[(CarInfo::RADAR_NR)/2] = {0x5a, 0x9c, 0x9c, 0x5a};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// 保证在循环周期内有一次0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}

VolvoTester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void VolvoTester::ShiftSimTask::_doTask(void * p_arg){
    CanTxMsg msg =
	{
	  .StdId = SHIFT_POS_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x10, 0x00}//00 00 00 00 23 00 10 00
	};

    CarInfo *pCar = (CarInfo *)p_arg;

    while(1){
        OSTimeDlyHMSM(0,0,0,40);

        switch(pCar->shiftPos){
            case CarInfo::SHIFT_POS_P:
                msg.Data[6] = 0x10;
                break;
            case CarInfo::SHIFT_POS_R:
                msg.Data[6] = 0x12;
                break;
            case CarInfo::SHIFT_POS_N:
                msg.Data[6] = 0x14;
                break;
            case CarInfo::SHIFT_POS_D:
                msg.Data[6] = 0x16;
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

VolvoTester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void VolvoTester::SteeringWheelSimTask::_doTask(void * p_arg){ 
    CanTxMsg msg =
        {
          .StdId = STEERING_WHEER_ANGLE_MSG_ID,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 8,
          .Data = {0x12, 0x07, 0x51, 0x00, 0x1a, 0x92, 0x80, 0x38}//12 07 51 00 1a 92 80 38
        };

    CarInfo *pCar = (CarInfo *)p_arg;
    int32_t angle = 0;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
		int32_t angle = pCar->steerAngle;

		if (angle <= 0) {
			msg.Data[2] = 0x51;
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + CAN_STEER_MINDDLE_VAL;
		} else {
			msg.Data[2] = 0x41;
			angle = angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + CAN_STEER_MINDDLE_VAL;
		}
		
        //angle = abs(pCar->steerAngle)+CAN_STEER_MINDDLE_VAL;

		msg.Data[6] = (angle & 0xff00) >> 8;
		msg.Data[7] = (angle & 0x00ff) >> 0;

		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

VolvoTester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void VolvoTester::PKeySimTask::_doTask(void * p_arg){

    while(1){
        OSTimeDlyHMSM(0,0,0,60);
    }
}

VolvoTester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void VolvoTester::RadarSimTask::_doTask(void * p_arg){

    while(1){
        OSTimeDlyHMSM(0,0,0,60);
    }
}

VolvoTester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void VolvoTester::TireKeySimTask::_doTask(void * p_arg){
    CanTxMsg msg =
        {
          .StdId = TIRE_KEY_STATUS_MSD_ID,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 8,
          .Data = {0x40, 0x10, 0xa4, 0x60, 0x00, 0x00, 0x09, 0x24}//40 10 a4 60 00 00 09 24
        };

	CanTxMsg msg2 =
        {
          .StdId = TIRE_KEY_STATUS_MSD_ID2,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 8,
          .Data = {0x30, 0x00, 0xc0, 0x09, 0x7e, 0x98, 0x01, 0xa8}//40 10 a4 60 00 00 09 24
        };

    CarInfo *pCar = (CarInfo *)p_arg;
    
    while(1){
        OSTimeDlyHMSM(0,0,0,70);
                                           
        switch(pCar->TireKey)
        {
            case 0://没有按任何键
                msg.Data[6]=0x09;
                msg.Data[7]=0x24;
		   msg2.Data[6] = 0x01;
                break;
//            case 1://*
//                break;
//            case 2://导航             
//                break;
//            case 3://语音
//                break;
//            case 4://Menu
//                break;              
//            case 5://channel_up
//                break; 
//            case 6://channel_down                
//                break; 
//            case 7://ok
//                break;
	     case 1:
		 	msg2.Data[6] = 0x41;
			break;
            case 8://MovePrevious
                msg.Data[7] = 0x25;
                break;
            case 9://MoveNext
                msg.Data[7] = 0x2c;
                break;
            case 10://voice_up
                msg.Data[6] = 0x0b;
                break;
            case 11://voice_down
                msg.Data[7] = 0x64;
                break;
//            case 12:
//                break;
            default ://
                msg.Data[6]=0x09;
                msg.Data[7]=0x24;
		   msg2.Data[6] = 0x01;
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

VolvoTester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void VolvoTester::TurnSimTask::_doTask(void * p_arg){
    CanTxMsg msg =
        {
          .StdId = TURNIDICATOR_STATUS_MSG_ID,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 8,
          .Data = {0x00, 0x28, 0x00, 0x37, 0xff, 0x31, 0xb5, 0x2f}//00 28 00 37 ff 31 b5 2f
        };
   
    CarInfo *pCar = (CarInfo *)p_arg;

    while(1){
        OSTimeDlyHMSM(0,0,0,1000);

        switch(pCar->lightPos){
            case 0://none
                msg.Data[3] = 0x37;
                break;
            case 1://left
                msg.Data[3] = 0x77;
                break;
            case 2://right
                msg.Data[3] = 0xb7;
                break;
            case 3://flash
                msg.Data[3] = 0xf7;
                break;
            default:
                msg.Data[3] = 0x37;            
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

VolvoTester::CarTimeTask::CarTimeTask(uint8_t prio):ZTask(prio){
// TODO Auto-generated constructor stub
}

void VolvoTester::CarTimeTask::_doTask(void * p_arg){
    CanTxMsg msg =
        {
          .StdId = CAR_TIME_MSG_ID,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 8,
          .Data = {0x90, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0b, 0x0f}//90 00 00 00 00 07 0b 0f //2015年11月07日0时0分0秒
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
                                msg.Data[7] = year;
                            }else{
                                msg.Data[6] = mon;
                            }
                        }else{
                            msg.Data[5] = day;
                        }
                    }else{
                        msg.Data[4] = hour;
                    }
                }else{
                    msg.Data[3] = min;
                }
            }else{
                msg.Data[2] = sec;
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