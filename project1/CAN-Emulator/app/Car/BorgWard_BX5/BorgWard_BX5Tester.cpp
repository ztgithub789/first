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
#include "BorgWard_BX5Tester.h"
#include "BorgWard_BX5_common.h"

using namespace BorgWard_BX52017;

BorgWard_BX5Tester::BorgWard_BX5Tester() {
	// TODO Auto-generated constructor stub
	pCar = NULL;
}

BorgWard_BX5Tester::~BorgWard_BX5Tester() {
	// TODO Auto-generated destructor stub
	delete pshiftsimTask;
	pshiftsimTask = NULL;

	delete psteeringTask;
	psteeringTask = NULL;

	delete pradarSimTask;
	pradarSimTask = NULL;

	delete pturnSimTask;
	pturnSimTask = NULL;

	delete pCarTimeTask;
	pCarTimeTask = NULL;
}

void BorgWard_BX5Tester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void BorgWard_BX5Tester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void BorgWard_BX5Tester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void BorgWard_BX5Tester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void BorgWard_BX5Tester::togglePKey()
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
void BorgWard_BX5Tester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x5a, 0x74, 0x74, 0x5a, 0x3c, 0xa0, 0xa0, 0x3c};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// 保证在循环周期内有一次0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}

uint8_t BorgWard_BX5Tester::run(CarInfo *pcar)
{
	pCar = pcar;
	pCar->maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;

	pshiftsimTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
	pshiftsimTask->start(pCar);

	psteeringTask = new SteeringWheelSimTask(STEERING_WHEEL_SIM_TASK_PRIO);
	psteeringTask->start(pCar);

	pradarSimTask = new RadarSimTask(RADAR_SIM_TASK_PRIO);
	pradarSimTask->start(pCar);

	pturnSimTask = new TurnSimTask( TURN_SIM_TASK_PRIO);
	pturnSimTask->start(pCar);

	pCarTimeTask = new CarTimeTask(CARTIME_TASK_PRIO);
	pCarTimeTask->start(pCar);
	
	return 0;
}

BorgWard_BX5Tester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}

void BorgWard_BX5Tester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = SHIFT_POS_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x80, 0x10, 0x00, 0x21, 0x00, 0x00, 0x00} //d7 08 10 01 20 01 70 7d
	};
	//0x80默认为P档

	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		OSTimeDlyHMSM(0,0,0,10);

		uint8_t shiftPos = pCar->shiftPos;

		switch(shiftPos) {
		case CarInfo::SHIFT_POS_P:
			msg.Data[2] = 0x10;
			break;
		case CarInfo::SHIFT_POS_R:
			msg.Data[2] = 0x27;
			break;
		case CarInfo::SHIFT_POS_N:
			msg.Data[2] = 0x30;
			break;
		case CarInfo::SHIFT_POS_D:
			msg.Data[2] = 0x41;
			break;
		case CarInfo::SHIFT_POS_S:
			msg.Data[2] = 0x51;
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



//转角
BorgWard_BX5Tester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void BorgWard_BX5Tester::SteeringWheelSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = STEERING_WHEER_ANGLE_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00} //00 00 00 00 11 60 00 9f
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
		int32_t angle = pCar->steerAngle;

		//int32_t value = (int32_t)(((msg.Data[3]&0x3f)<<8)|(msg.Data[2]));  msg.Data[3]&0x3f)>0x15

		if (angle <= 0) {
			msg.Data[6] = 0x08;
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + CAN_STEER_BEGIN_VAL;
		} else {
			msg.Data[6] = 0x09;
			angle = angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + CAN_STEER_BEGIN_VAL;
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


BorgWard_BX5Tester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio) {

}

/*******************************************************************************
 * 简介   :	模拟雷达帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void BorgWard_BX5Tester::RadarSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = RADAR_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 5,
	  .Data = {0x00, 0x00, 0x00, 0x00, 0x00}
	};
	
	CarInfo *pCar = (CarInfo*)p_arg;
	
	static bool dir = true;
	
	while(1) {
		OSTimeDlyHMSM(0,0,0,50);

		if(dir==true ){	
			// 前帧
			msg.Data[0] = pCar->radarVal[CarInfo::RADAR_LFS];
			msg.Data[1] = pCar->radarVal[CarInfo::RADAR_LFM];
			msg.Data[2] = pCar->radarVal[CarInfo::RADAR_RFM];
			msg.Data[3] = pCar->radarVal[CarInfo::RADAR_RFS];
			
		} else {		
			msg.Data[4] = pCar->radarVal[CarInfo::RADAR_LRS];
			msg.Data[5] = pCar->radarVal[CarInfo::RADAR_LRM];
			msg.Data[6] = pCar->radarVal[CarInfo::RADAR_RRM];
			msg.Data[7] = pCar->radarVal[CarInfo::RADAR_RRS];
			pCar->isRadaBack = false;
		}
		dir = !dir;

		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}



//转向灯
BorgWard_BX5Tester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio) {

}
void BorgWard_BX5Tester::TurnSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = TURNIDICATOR_STATUS_MSD_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x40, 0x00}
	};

	CanTxMsg msg1 =
	{
	  .StdId = TURNIDICATOR_DOUBLE_STATUS_MSD_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x00, 0x00,0x01, 0x00, 0x00, 0x00}
	};
    
	CarInfo *pCar = (CarInfo*)p_arg;
    
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
        
		switch(pCar->lightPos){
            case 0://none
                msg.Data[6] = 0x40;
		   msg1.Data[4] = 0x01;
                break;
            case 1://left
                msg.Data[6] = 0x42;
		   msg1.Data[4] = 0x01;
		   //printf("turn left\n");
                break;
            case 2://right
                msg.Data[6] = 0x41;
		   msg1.Data[4] = 0x01;
		   //printf("turn right\n");
                break;
             case 3://double
                msg.Data[6] = 0x40;
                msg1.Data[4] = 0x09;
		   //printf("turn flash\n");
                break;
            default:
                msg.Data[6] = 0x40;
		   msg1.Data[4] = 0x01;
                break;
        }
        
		gpDrvCAN->transmit(CAN1, &msg);
		gpDrvCAN->transmit(CAN1, &msg1);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}


BorgWard_BX5Tester::CarTimeTask::CarTimeTask(uint8_t prio):ZTask(prio){
// TODO Auto-generated constructor stub
}

void BorgWard_BX5Tester::CarTimeTask::_doTask(void * p_arg){
    CanTxMsg msg =
        {
          .StdId = TIME_MSG_ID,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 7,
          .Data = {0x03, 0xb3, 0x2f, 0x16, 0x39, 0x37, 0x00}//03 b3 2f 16 39 37 00//
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
                                msg.Data[0] = year;
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