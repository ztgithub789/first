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
#include "FordTester.h"
#include "Ford_common.h"

using namespace FORD_NS;

FordTester::FordTester() {
// TODO Auto-generated constructor stub
	pCar = NULL;
}

FordTester::~FordTester() {
// TODO Auto-generated destructor stub
	delete pshiftTask;
	pshiftTask = NULL;

	delete psteeringTask;
	psteeringTask = NULL;

	delete pKeyTask;
	pKeyTask = NULL;

	delete pradarSimTask;
	pradarSimTask = NULL;

	delete pturnSimTask;
	pturnSimTask = NULL;

	delete ptirekeySimTask;
	ptirekeySimTask = NULL;
             
}

void FordTester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void FordTester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void FordTester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void FordTester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void FordTester::togglePKey()
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
void FordTester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x5a, 0x78, 0x78, 0x5a, 0x5a, 0xa0, 0xa0, 0x5a};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// 保证在循环周期内有一次0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}

uint8_t FordTester::run(CarInfo *pcar)
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

	pturnSimTask = new TurnSimTask(TURN_SIM_TASK_PRIO);
	pturnSimTask->start(pCar);

	ptirekeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO);
	ptirekeySimTask->start(pCar);

	return 0;
}

FordTester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
// TODO Auto-generated constructor stub
}

void FordTester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = 0x171,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x14, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		uint8_t shiftPos = pCar->shiftPos;

		switch(shiftPos) {
		case CarInfo::SHIFT_POS_P:
			msg.Data[1] = 0x0c;
			break;
		case CarInfo::SHIFT_POS_R:
			msg.Data[1] = 0x2c;
			break;
		case CarInfo::SHIFT_POS_N:
			msg.Data[1] = 0x4c;
			break;
		case CarInfo::SHIFT_POS_D:
			msg.Data[1] = 0x6c;
			break;
		case CarInfo::SHIFT_POS_S:
			msg.Data[1] = 0x8c;
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

FordTester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void FordTester::SteeringWheelSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = 0x3a8,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x01, 0x00, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00}
	};
	//20 0a e6 00 00 8c 99 00
	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
		int32_t angle = pCar->steerAngle;

		int32_t value = angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE ;
		value = CAN_STEER_BEGIN_VAL - value;

		msg.Data[2] = (value & 0xff00) >> 8;
		msg.Data[3] = (value & 0x00ff) >> 0;

		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}


FordTester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void FordTester::PKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = 0x3ab,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x00, 0xff, 0x0f, 0xf0,0x00,0x00,0x00,0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,50);

		static bool oldv = false;
		bool newv = pCar->isPKeyPressed;
		if (oldv != newv){
			if (newv) {
				msg.Data[0] = 0x10;
                msg.Data[4] = 0x00;
			} else {
				msg.Data[0] = 0x12;
                msg.Data[4] = 0x40;
			}
			gpDrvCAN->transmit(CAN2, &msg);
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

FordTester::RadarSimTask::RadarSimTask(uint8_t prio):ZTask(prio){

}

void FordTester::RadarSimTask::_doTask(void * p_arg){
	CanTxMsg msg1 =
	{
			.StdId = 0x3ab,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x00, 0xff, 0x0f, 0xf0,0x00,0x00,0x00,0x00}
	};

	CanTxMsg msg2 =
	{
			.StdId = 0x3aa,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x00, 0xff, 0x0f, 0xf0,0x00,0x00,0x00,0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,50);
		
		msg1.Data[1] = ((pCar->radarVal[CarInfo::RADAR_LRS]&0xf0)>>4)|(pCar->radarVal[CarInfo::RADAR_LRM]&0xf0);
		msg1.Data[2] = (pCar->radarVal[CarInfo::RADAR_RRM]&0xf0)>>4;
		msg1.Data[3] = pCar->radarVal[CarInfo::RADAR_RRS];

		msg2.Data[3] = ((pCar->radarVal[CarInfo::RADAR_LFS]&0xf0)>>4)|(pCar->radarVal[CarInfo::RADAR_LFM]&0xf0);
		msg2.Data[4] = (pCar->radarVal[CarInfo::RADAR_RFM]&0xf0)>>4;
		msg2.Data[5] = pCar->radarVal[CarInfo::RADAR_RFS];	
		
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

FordTester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio) {

}
void FordTester::TurnSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg1 =
	{
			.StdId = 0x83,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x00, 0x20, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00}
	};

	CanTxMsg msg2 =
	{
			.StdId = 0x3b3,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x44, 0x88, 0x74, 0x07, 0x10, 0x00, 0x00, 0x02}
	};
	CarInfo *pCar = (CarInfo*)p_arg;
	
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		switch(pCar->lightPos&0x3)
		{
    		case 0://none
    			msg1.Data[0]=0x00;
				msg2.Data[1]=0x88;
				msg2.Data[7]=0x02;
    			break;
    		case 1://left
    			msg1.Data[0]=0x10;    			
    			break;
    		case 2://right
    			msg1.Data[0]=0x20;
    			break;
    		case 3://flash
    			msg2.Data[1]=0x8a;
				msg2.Data[7]=0x82;
    			break;
    		default :
    			msg1.Data[0]=0x00;
				msg2.Data[1]=0x88;
				msg2.Data[7]=0x02;
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

FordTester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio) {

}
void FordTester::TireKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = 0x2a1,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {0xff, 0xff, 0xff, 0xff,0x00,0x00,0x00,0x00}
	};
	
	CarInfo *pCar = (CarInfo*)p_arg;
 	
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
                                    
		switch(pCar->TireKey) {
    		case 0://没有按任何键
    			msg.Data[0]=0xff;
				msg.Data[4]=0x00;
    			break;
    		case 1://上
    			msg.Data[0]=0x2b;
				msg.Data[4]=0x10;
    			break;
    		case 2://下
    			msg.Data[0]=0x2c;
				msg.Data[4]=0x10;
    			break;
    		case 3://语音
    			msg.Data[0]=0x30;
				msg.Data[4]=0x10;
    			break;
            case 4://音量+
    			msg.Data[0]=0xfd;
				msg.Data[4]=0x10;
    			break;
            case 5://接电话(左滑键)
    			msg.Data[0]=0x61;
				msg.Data[4]=0x10;
    			break;
    		case 6://挂电话(右滑键)
    			msg.Data[0]=0x62;
				msg.Data[4]=0x10;
    			break;
            case 7://OK
    			msg.Data[0]=0x28;
				msg.Data[4]=0x10;
    			break;
            case 8://左
    			msg.Data[0]=0x29;
				msg.Data[4]=0x10;
    			break;
    		case 9://右
    			msg.Data[0]=0x2a;
				msg.Data[4]=0x10;
    			break;    		
    		case 10://
    			
    			break; 
            case 11://音量-
    			msg.Data[0]=0xfe;
				msg.Data[4]=0x10;
    			break;
            case 12://静音键
    			msg.Data[0]=0x44;
				msg.Data[4]=0x10;
    			break;
    		default ://
    			msg.Data[0]=0xff;
				msg.Data[4]=0x00;
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
