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
#include "VolvoS90Tester.h"
#include "VolvoS90_common.h"

using namespace VOLVOS90_NS;

VolvoS90Tester::VolvoS90Tester(void){
// TODO Auto-generated constructor stub
    pCar = NULL;
}

VolvoS90Tester::~VolvoS90Tester(void){
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

uint8_t VolvoS90Tester::run(CarInfo * _pCar){
    pCar = _pCar;
    pCar->maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;

    pSteeringWheelSimTask = new SteeringWheelSimTask(STEERING_WHEEL_SIM_TASK_PRIO);
    pSteeringWheelSimTask->start(pCar);

    return 0;
}

void VolvoS90Tester::increaseRadarVal(int nr,int dv){
    uint8_t maxRadarVal[(CarInfo::RADAR_NR)/2] = {0x5a, 0x9c, 0x9c, 0x5a};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// 保证在循环周期内有一次0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}

VolvoS90Tester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void VolvoS90Tester::ShiftSimTask::_doTask(void * p_arg){

    while(1){
        OSTimeDlyHMSM(0,0,0,40);
    }
}

void VolvoS90Tester::steerLeft(void){
    if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void VolvoS90Tester::steerRight(void){
    if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

VolvoS90Tester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void VolvoS90Tester::SteeringWheelSimTask::_doTask(void * p_arg){ 
    CanTxMsg msg =
        {
          .StdId = STEERING_WHEER_ANGLE_MSG_ID,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 8,
          .Data = {0x00, 0x00, 0xC0, 0x00, 0xF9, 0x60, 0x00, 0x00}
        };

    CarInfo *pCar = (CarInfo *)p_arg;
    int32_t angle = 0;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
		int32_t angle = pCar->steerAngle;

		if (angle <= 0) {
			// left		
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + CAN_STEER_LEFT_BEGIN_VAL;
		} else {
			// right
			angle = (CAN_STEER_RIGHT_BEGIN_VAL - (angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE));
		}
		
        //angle = abs(pCar->steerAngle)+CAN_STEER_MINDDLE_VAL;

		msg.Data[0] = (angle & 0xff00) >> 8;
		msg.Data[1] = (angle & 0x00ff) >> 0;

		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

VolvoS90Tester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void VolvoS90Tester::PKeySimTask::_doTask(void * p_arg){

    while(1){
        OSTimeDlyHMSM(0,0,0,60);
    }
}

VolvoS90Tester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void VolvoS90Tester::RadarSimTask::_doTask(void * p_arg){

    while(1){
        OSTimeDlyHMSM(0,0,0,60);
    }
}

VolvoS90Tester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void VolvoS90Tester::TireKeySimTask::_doTask(void * p_arg){

    CarInfo *pCar = (CarInfo *)p_arg;
    
    while(1){
        OSTimeDlyHMSM(0,0,0,70);
    }
}

VolvoS90Tester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void VolvoS90Tester::TurnSimTask::_doTask(void * p_arg){
 

    while(1){
        OSTimeDlyHMSM(0,0,0,1000);

    }
}

VolvoS90Tester::CarTimeTask::CarTimeTask(uint8_t prio):ZTask(prio){
// TODO Auto-generated constructor stub
}

void VolvoS90Tester::CarTimeTask::_doTask(void * p_arg){

    while(1){
        OSTimeDlyHMSM(0,0,0,200);
    }
}