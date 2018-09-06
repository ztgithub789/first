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
#include "HondaCity_2014Tester.h"
#include "HondaCity_2014_common.h"

using namespace HONDACITY_2014;

HondaCity_2014Tester::HondaCity_2014Tester(){
	// TODO Auto-generated constructor stub
	pCar = NULL;
}
HondaCity_2014Tester::~HondaCity_2014Tester(){
	// TODO Auto-generated destructor stub
	delete pShiftSimTask;
	pShiftSimTask = NULL;

	delete pSteeringWheelSimTask;
	pSteeringWheelSimTask = NULL;

	delete pTurnSimTask;
	pTurnSimTask = NULL;
}

void HondaCity_2014Tester::shiftDown(){
	
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void HondaCity_2014Tester::shiftUp(){
	
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void HondaCity_2014Tester::steerLeft(){
	
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0xa;
}

void HondaCity_2014Tester::steerRight(){
	
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0xa;
}

void HondaCity_2014Tester::togglePKey(){
	
}

void HondaCity_2014Tester::increaseRadarVal(int nr, int dv){

}

uint8_t HondaCity_2014Tester::run(CarInfo *pcar)
{
	pCar = pcar;
    pCar->maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;

	pShiftSimTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
	pShiftSimTask->start(pCar);

	pSteeringWheelSimTask = new SteeringWheelSimTask(STEERING_WHEEL_SIM_TASK_PRIO);
	pSteeringWheelSimTask->start(pCar);
	
	pTurnSimTask = new TurnSimTask(TURN_SIM_TASK_PRIO);
	pTurnSimTask->start(pCar);

	return 0;
}

HondaCity_2014Tester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}

void HondaCity_2014Tester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0,
	  .ExtId = SHIFT_POS_MSG_EID_HONDACITY_2014,
	  .IDE = CAN_ID_EXT,
	  .RTR = 0,
	  .DLC = 5,
	  .Data = {0x40, 0x03, 0x00, 0x00, 0x60} 
	}; 

	CarInfo *pCar = (CarInfo*)p_arg;
	
	while(1) {
		
		OSTimeDlyHMSM(0,0,0,300);

		uint8_t shiftPos = pCar->shiftPos;

		switch(shiftPos) {
		case CarInfo::SHIFT_POS_P:
			msg.Data[0] = 0x40;
			break;
		case CarInfo::SHIFT_POS_R:
			msg.Data[0] = 0x10;
			break;
		case CarInfo::SHIFT_POS_N:
			msg.Data[0] = 0x04;
			break;
		case CarInfo::SHIFT_POS_D:
			msg.Data[0] = 0x01;
			break;
		case CarInfo::SHIFT_POS_S:
			msg.Data[0] = 0x00;
			msg.Data[1] = 0x43;
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

HondaCity_2014Tester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void HondaCity_2014Tester::SteeringWheelSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0,
	  .ExtId = STEERING_WHEER_ANGLE_MSG_EID_HONDACITY_2014,
	  .IDE = CAN_ID_EXT,
	  .RTR = 0,
	  .DLC = 3,
	  .Data = {0xe0, 0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {		
		
		OSTimeDlyHMSM(0,0,0,300);
		
//		if(pCar->isTrunLeft){
//			if (pCar->steerAngle > -MAX_ABS_CAN_STEER_ANGLE) pCar->steerAngle -= 0xa;
//		}else if(pCar->isMiddle){
//			pCar->steerAngle = 0;
//		}else if(pCar->isTrunRight){
//			if (pCar->steerAngle < MAX_ABS_CAN_STEER_ANGLE) pCar->steerAngle += 0xa;
//		}
		
		int32_t angle = abs(pCar->steerAngle);
		angle &= 0xffff;
		
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

HondaCity_2014Tester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio) {

}
void HondaCity_2014Tester::TurnSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0,
	  .ExtId = TURN_INDICATOR_MSG_EID_HONDACITY_2014,
	  .IDE = CAN_ID_EXT,
	  .RTR = 0,
	  .DLC = 5,
	  .Data = {0x28, 0x00, 0x80, 0x00, 0x02}
	};
	
	CarInfo *pCar = (CarInfo*)p_arg;
	uint8_t cout = 0;
	
	while(1) {
		
		OSTimeDlyHMSM(0,0,0,300);
		
		uint8_t m = pCar->lightPos;

		switch(m){
			case 0://none
				msg.Data[2] = 0x80;
				cout = 0;
				break;
			case 1://left
				msg.Data[2] = 0x90;
				cout = 0;
				break;
			case 2://right
				msg.Data[2] = 0x88;
				cout = 0;
				break;
			case 3://flash
				if(cout==0 || cout==1){
					msg.Data[2] = 0xb8;
				}else if(cout==2 || cout==3){
					msg.Data[2] = 0xa0;
				}								
				cout++;
				
				if(cout == 4) cout = 0;
				break;
			default:
		}
		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}
