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
#include "TouaregTester.h"
#include "Touareg_common.h"
using namespace TouaregNS;

TouaregTester::TouaregTester() {
	// TODO Auto-generated constructor stub
	pCar = NULL;
}

TouaregTester::~TouaregTester() {
	// TODO Auto-generated destructor stub
	delete pshiftTask;
	pshiftTask = NULL;

	delete psteeringTask;
	psteeringTask = NULL;

	delete pradarSimTask;
	pradarSimTask = NULL;
        
    delete ptirekeySimTask;
	ptirekeySimTask =NULL; 
    
	delete pKeyTask;
	pKeyTask = NULL;    

}

void TouaregTester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void TouaregTester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void TouaregTester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void TouaregTester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void TouaregTester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x40, 0x7c, 0x7c, 0x40, 0x40, 0xa5, 0xa5, 0x40};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// 保证在循环周期内有一次0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}

void TouaregTester::togglePKey()
{
	if (pCar->isPKeyPressed)
		pCar->isPKeyPressed = false;
	else
		pCar->isPKeyPressed = true;
}

uint8_t TouaregTester::run(CarInfo *pcar)
{
	pCar = pcar;
	pCar->maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;

	pshiftTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
	pshiftTask->start(pCar);

	psteeringTask = new SteeringWheelSimTask(STEERING_WHEEL_SIM_TASK_PRIO);
	psteeringTask->start(pCar);

	pradarSimTask = new RadarSimTask(RADAR_SIM_TASK_PRIO);
	pradarSimTask->start(pCar);
        
    ptirekeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO); //TIREKEY_SIM_TASK_PRIO
	ptirekeySimTask->start(pCar);  
    
	pKeyTask = new PKeySimTask(P_KEY_SIM_TASK_PRIO);
	pKeyTask->start(pCar);    

	return 0;
}

TouaregTester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}
void  TouaregTester::ShiftSimTask::_doTask(void *p_arg)
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
			msg.Data[4] = 0x15;
			break;
		case CarInfo::SHIFT_POS_R:
			msg.Data[4] = 0x16;
			break;
		case CarInfo::SHIFT_POS_N:
			msg.Data[4] = 0x17;
			break;
		case CarInfo::SHIFT_POS_D:
			msg.Data[4] = 0x19;
			break;
		case CarInfo::SHIFT_POS_S:
			msg.Data[4] = 0x1d;
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

TouaregTester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void TouaregTester::SteeringWheelSimTask::_doTask(void *p_arg)
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
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + CAN_STEER_LEFT_BEGIN_VAL;
		} else {
			angle = angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + CAN_STEER_RIGHT_BEGIN_VAL;
		}

		msg.Data[3] = (angle & 0xff00) >> 8;
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

TouaregTester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio) {

}

/*******************************************************************************
 * 简介   :	模拟雷达帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void TouaregTester::RadarSimTask::_doTask(void *p_arg)
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

	CarInfo *pCar = (CarInfo*)p_arg;
	//static char time = 39;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
		//	time++;
		//	if( (pCar->isRadaFront==true) ||(time ==40)){
		if(pCar->isRadaFront ==true) {
			// 前帧
			msg.Data[1] = 0x92;
			msg.Data[2] = pCar->radarVal[CarInfo::RADAR_LFS];
			msg.Data[3] = pCar->radarVal[CarInfo::RADAR_LFM];
			msg.Data[4] = pCar->radarVal[CarInfo::RADAR_RFM];
			msg.Data[5] = pCar->radarVal[CarInfo::RADAR_RFS];
			pCar->isRadaFront = false;
			//	} else if( (pCar->isRadaBack==true) ||(time== 48)){
		} else {
			msg.Data[1] = 0x93;
			msg.Data[2] = pCar->radarVal[CarInfo::RADAR_LRS];
			msg.Data[3] = pCar->radarVal[CarInfo::RADAR_LRM];
			msg.Data[4] = pCar->radarVal[CarInfo::RADAR_RRM];
			msg.Data[5] = pCar->radarVal[CarInfo::RADAR_RRS];
			pCar->isRadaFront = true;
		}
		/*	if(time >= 48)
					time=0;
		 */
		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}


TouaregTester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio) {

}
void TouaregTester::TireKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x5bf,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 4,
	  .Data = {0x00, 0x00, 0x00, 0x11}//00 00 00 11
	};
    
	CarInfo *pCar = (CarInfo*)p_arg;     
    static int8_t times = 0, status = 0, oldkey = 0, newkey = 0;
                
	while(1) {
        OSTimeDlyHMSM(0,0,0,100);
        newkey = pCar->TireKey;

        if(newkey != 0)
        {
            if((newkey != 6)&&(newkey != 5))
            {
                if(newkey != oldkey)
                {
                    oldkey = newkey;
                    times = 0;
                    status = 0x01;
                }else{
                    times++;
                    if(times >= 1)
                    {
                        status++;
                        if(status >=4)
                            status = 4;
                    }else{
                        status = 1;
                    }
                }
            }
        }else{
            times = 0;
            oldkey = 0;
            status = 0;
        }               
                                
        switch(newkey)
        {
            case 0://没有按任何键
                msg.Data[0]=0;
                msg.Data[2]=status;
                break;
            case 1://*
                msg.Data[0]=0x21;
                msg.Data[2]=status;
                break;
            case 2://导航
                msg.Data[0]=0x1b;
                msg.Data[2]=status;
                break;
            case 3://语音
                msg.Data[0]=0x19;
                msg.Data[2]=status;
                break; 
            case 4://Menu
                msg.Data[0]=0x01;
                msg.Data[2]=status;
                break;      
            case 5://向上滑动
                msg.Data[0]=0x12;
                msg.Data[2]=0x0f;
                break; 
            case 6://向下滑动
                msg.Data[0]=0x12;
                msg.Data[2]=0x01;
                break; 
            case 7://ok
                msg.Data[0]=0x20;
                msg.Data[2]=status;
                break;                                     
            default ://
                msg.Data[0]=0;
                msg.Data[2]=0;
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

TouaregTester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}
void TouaregTester::PKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x603,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 4,
	  .Data = {0x22, 0x94, 0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,50);

		static bool oldv = false;
		bool newv = pCar->isPKeyPressed;
		if (oldv != newv){
			if (newv) {
				msg.Data[2] = 0x01;
				msg.Data[3] = 0x01;
			} else {
				msg.Data[2] = 0x0;
				msg.Data[3] = 0x0;
			}
			gpDrvCAN->transmit(CAN1, &msg);
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

