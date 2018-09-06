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
#include "SUZUKTester.h"
#include "SUZUK_common.h"

using namespace SUZUK_2017;

SUZUKTester::SUZUKTester(){
	// TODO Auto-generated constructor stub
	pCar = NULL;
}
SUZUKTester::~SUZUKTester(){
	// TODO Auto-generated destructor stub
	delete pShiftSimTask;
	pShiftSimTask = NULL;

	delete pSteeringWheelSimTask;
	pSteeringWheelSimTask = NULL;

}

void SUZUKTester::shiftDown(){
	
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void SUZUKTester::shiftUp(){
	
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void SUZUKTester::steerLeft(){
	
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0xa;
}

void SUZUKTester::steerRight(){
	
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0xa;
}

void SUZUKTester::togglePKey(){
	
}

void SUZUKTester::increaseRadarVal(int nr, int dv){

}

uint8_t SUZUKTester::run(CarInfo *pcar)
{
	pCar = pcar;
    pCar->maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;

	pShiftSimTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
	pShiftSimTask->start(pCar);

	pSteeringWheelSimTask = new SteeringWheelSimTask(STEERING_WHEEL_SIM_TASK_PRIO);
	pSteeringWheelSimTask->start(pCar);
	

	return 0;
}

SUZUKTester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}

void SUZUKTester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = SHIFT_POS_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x40, 0x03, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00} 
	}; 

	CarInfo *pCar = (CarInfo*)p_arg;
	
	while(1) {
		
		OSTimeDlyHMSM(0,0,0,300);

		uint8_t shiftPos = pCar->shiftPos;

		switch(shiftPos) {
		case CarInfo::SHIFT_POS_P:
			msg.Data[3] = 0x0a;
			break;
		case CarInfo::SHIFT_POS_R:
			msg.Data[3] = 0x08;
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

SUZUKTester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void SUZUKTester::SteeringWheelSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = STEERING_WHEER_ANGLE_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {		
		
		OSTimeDlyHMSM(0,0,0,100);
		
//		if(pCar->isTrunLeft){
//			if (pCar->steerAngle > -MAX_ABS_CAN_STEER_ANGLE) pCar->steerAngle -= 0x5;
//		}else if(pCar->isMiddle){
//			pCar->steerAngle = 0;
//		}else if(pCar->isTrunRight){
//			if (pCar->steerAngle < MAX_ABS_CAN_STEER_ANGLE) pCar->steerAngle += 0x5;
//		}

		int32_t angle = pCar->steerAngle;
		if (angle <= 0) {
			angle = angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + CAN_STEER_LEFT_BEGIN_VAL;
		} else {
			angle = angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + CAN_STEER_RIGHT_BEGIN_VAL;
		}

		msg.Data[2] = (angle & 0xff00) >> 8;
		msg.Data[3] = (angle & 0x00ff) >> 0;
      
		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}


