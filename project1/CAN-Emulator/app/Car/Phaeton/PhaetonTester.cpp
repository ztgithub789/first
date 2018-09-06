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
#include "PhaetonTester.h"
#include "Phaeton_common.h"
using namespace PhaetonNS;

PhaetonTester::PhaetonTester() {
	// TODO Auto-generated constructor stub
	pCar = NULL;
}

PhaetonTester::~PhaetonTester() {
	// TODO Auto-generated destructor stub
	delete pshiftTask;
	pshiftTask = NULL;

	delete psteeringTask;
	psteeringTask = NULL;

	delete pradarSimTask;
	pradarSimTask = NULL;
	
	delete pturningSimTask;
	pturningSimTask = NULL;
}

void PhaetonTester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void PhaetonTester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void PhaetonTester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void PhaetonTester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void PhaetonTester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x40, 0x7c, 0x7c, 0x40, 0x40, 0xa5, 0xa5, 0x40};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// ��֤��ѭ����������һ��0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}


uint8_t PhaetonTester::run(CarInfo *pcar)
{
	pCar = pcar;
	pCar->maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;

	pshiftTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
	pshiftTask->start(pCar);

	psteeringTask = new SteeringWheelSimTask(STEERING_WHEEL_SIM_TASK_PRIO);
	psteeringTask->start(pCar);

//	pradarSimTask = new RadarSimTask(RADAR_SIM_TASK_PRIO);
//	pradarSimTask->start(pCar);
	
	return 0;
}

PhaetonTester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}
void  PhaetonTester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = SHIFT_POS_MSG_ID,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x10, 0x00, 0x00, 0x88, 0x07, 0xfe, 0xa9, 0x80}
	};

	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		uint8_t shiftPos = pCar->shiftPos;

		switch(shiftPos) {
		case CarInfo::SHIFT_POS_P:
			msg.Data[7] = 0x08;
			break;
		case CarInfo::SHIFT_POS_R:
			msg.Data[7] = 0x07;
			break;
		case CarInfo::SHIFT_POS_N:
			msg.Data[7] = 0x06;
			break;
		case CarInfo::SHIFT_POS_D:
			msg.Data[7] = 0x05;
			break;
		case CarInfo::SHIFT_POS_S:
			msg.Data[7] = 0x0c;
			break;
		default:
			break;
		}
		
		if(pCar->isACCOn) {
			msg.Data[0] = 0x88;
		} else {
			msg.Data[0] = 0xFF;
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

PhaetonTester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void PhaetonTester::SteeringWheelSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = STEERING_WHEER_ANGLE_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x64, 0x80, 0x00, 0x00, 0x3c, 0xa0, 0x00, 0x7b}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
		int32_t angle = pCar->steerAngle;

		if (angle <= 0) {
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + 0x0000;
		} else {
			angle = angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + 0x8000;
		}

		msg.Data[1] = (angle & 0xff00) >> 8;
		msg.Data[0] = (angle & 0x00ff) >> 0;

		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

PhaetonTester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio) {

}

/*******************************************************************************
 * ���   :	ģ���״�֡
 * ����   :
 * ����ֵ :
 * ����   :
 *******************************************************************************/
void PhaetonTester::RadarSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = RADAR_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x42, 0x92, 0x00, 0x00, 0x00, 0x00}
	};

	// dir��ʾ��ǰ֡�ķ���
	// 	true��ǰ
	//	false: ��
	static bool dir = true;

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		msg.Data[0] = pCar->radarVal[CarInfo::RADAR_LFS];
		msg.Data[1] = pCar->radarVal[CarInfo::RADAR_RFS];
		msg.Data[2] = pCar->radarVal[CarInfo::RADAR_LRS];
		msg.Data[3] = pCar->radarVal[CarInfo::RADAR_RRS];
		msg.Data[4] = pCar->radarVal[CarInfo::RADAR_LFM];
		msg.Data[5] = pCar->radarVal[CarInfo::RADAR_RFM];
		msg.Data[6] = pCar->radarVal[CarInfo::RADAR_LRM];
		msg.Data[7] = pCar->radarVal[CarInfo::RADAR_RRM];


		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

PhaetonTester::TurningLightSimTask::TurningLightSimTask(uint8_t prio) : ZTask(prio) {

}
