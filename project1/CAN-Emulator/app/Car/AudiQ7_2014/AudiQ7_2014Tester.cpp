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
#include "AudiQ7_2014Tester.h"
#include "AudiQ7_2014_common.h"

using namespace AudiQ7_2014;

AudiQ7_2014Tester::AudiQ7_2014Tester() {
	// TODO Auto-generated constructor stub
	pCar = NULL;
}

AudiQ7_2014Tester::~AudiQ7_2014Tester() {
	// TODO Auto-generated destructor stub
	delete pshiftsimTask;
	pshiftsimTask = NULL;

	delete psteeringTask;
	psteeringTask = NULL;

	delete pKeyTask;
	pKeyTask = NULL;

	delete pradarSimTask;
	pradarSimTask = NULL;

	delete pturnSimTask;
	pturnSimTask = NULL;

	delete pParkToSideTask;
	pParkToSideTask = NULL;
        
 	//delete ptirekeySimTask;
	//ptirekeySimTask = NULL;
	
	delete pRemoteKeyTask;
	pRemoteKeyTask = NULL;

	delete pBackDoorStatusTask;
	pBackDoorStatusTask = NULL;

	delete pFrontDoorKeyTask;
	pFrontDoorKeyTask = NULL;

}

void AudiQ7_2014Tester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void AudiQ7_2014Tester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void AudiQ7_2014Tester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void AudiQ7_2014Tester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void AudiQ7_2014Tester::togglePKey()
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
void AudiQ7_2014Tester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x5a, 0x74, 0x74, 0x5a, 0x3c, 0xa0, 0xa0, 0x3c};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// 保证在循环周期内有一次0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}

uint8_t AudiQ7_2014Tester::run(CarInfo *pcar)
{
	pCar = pcar;
	pCar->maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;

	pshiftsimTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
	pshiftsimTask->start(pCar);

	psteeringTask = new SteeringWheelSimTask(STEERING_WHEEL_SIM_TASK_PRIO);
	psteeringTask->start(pCar);

	pKeyTask = new PKeySimTask(P_KEY_SIM_TASK_PRIO);
	pKeyTask->start(pCar);

	pradarSimTask = new RadarSimTask(RADAR_SIM_TASK_PRIO);
	pradarSimTask->start(pCar);

	pParkToSideTask = new ParkToSideKeySimTask(TURNING_LIGHT_SIM_TASK_PRIO);
	pParkToSideTask->start(pCar);

	pturnSimTask = new TurnSimTask(TURN_SIM_TASK_PRIO);
	pturnSimTask->start(pCar);
        
    //ptirekeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO); //TIREKEY_SIM_TASK_PRIO
	//ptirekeySimTask->start(pCar); 
	
	pRemoteKeyTask = new RemoteKeyTask(REMOTE_KEY_TASK_PRIO);
	pRemoteKeyTask->start(pCar);

	pBackDoorStatusTask = new BackDoorStatusTask(BACK_DOOR_STATUS_TASK_PRIO);
	pBackDoorStatusTask->start(pCar);

	pFrontDoorKeyTask = new FrontDoorKeyTask(FRONT_DOOR_KEY_TASK_PRIO);
	pFrontDoorKeyTask->start(pCar);

	return 0;
}

AudiQ7_2014Tester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}

void AudiQ7_2014Tester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x440,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x80, 0x00, 0xfe, 0x80, 0x00, 0x48, 0x0D} //00 80 00 fe 80 00 48 00
	};

	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		uint8_t shiftPos = pCar->shiftPos;

		switch(shiftPos) {
		case CarInfo::SHIFT_POS_P:
			msg.Data[1] = 0x80;
			break;
		case CarInfo::SHIFT_POS_R:
			msg.Data[1] = 0x77;
			break;
		case CarInfo::SHIFT_POS_N:
			msg.Data[1] = 0x60;
			break;
		case CarInfo::SHIFT_POS_D:
			msg.Data[1] = 0x50;
			break;
		case CarInfo::SHIFT_POS_S:
			msg.Data[1] = 0x1c;
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

AudiQ7_2014Tester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void AudiQ7_2014Tester::SteeringWheelSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x0c2,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x0, 0x00, 0x11, 0x60, 0x00, 0x9f} //00 00 00 00 11 60 00 9f
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
		int32_t angle = pCar->steerAngle;

		int32_t value = abs(angle) * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE ;
		//value <<= 4;
		value &= 0x3fff;
		if (angle <= 0) {
			value |= 0x0000;
		} else {
			value |= 0x8000;

		}

		msg.Data[1] = (value & 0xff00) >> 8;
		msg.Data[0] = (value & 0x00ff) >> 0;

		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}


AudiQ7_2014Tester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void AudiQ7_2014Tester::PKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = P_KEY_STATUS_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 4,
	  .Data = {0x22, 0x94, 0x00, 0x00}  //22 94 01 01
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

AudiQ7_2014Tester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio) {

}

/*******************************************************************************
 * 简介   :	模拟雷达帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void AudiQ7_2014Tester::RadarSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x6da,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x42, 0x92, 0x00, 0x00, 0x00, 0x00,0x00, 0x00}
	};
	
	CarInfo *pCar = (CarInfo*)p_arg;
	
	static bool dir = true;
	
	while(1) {
		OSTimeDlyHMSM(0,0,0,50);
		
		if(dir==true ){	
			// 前帧
			msg.Data[1] = 0x92;
			msg.Data[2] = pCar->radarVal[CarInfo::RADAR_LFS];
			msg.Data[3] = pCar->radarVal[CarInfo::RADAR_LFM];
			msg.Data[4] = pCar->radarVal[CarInfo::RADAR_RFM];
			msg.Data[5] = pCar->radarVal[CarInfo::RADAR_RFS];
			
		} else {		
			msg.Data[1] = 0x93;
			msg.Data[2] = pCar->radarVal[CarInfo::RADAR_LRS];
			msg.Data[3] = pCar->radarVal[CarInfo::RADAR_LRM];
			msg.Data[4] = pCar->radarVal[CarInfo::RADAR_RRM];
			msg.Data[5] = pCar->radarVal[CarInfo::RADAR_RRS];
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

AudiQ7_2014Tester::ParkToSideKeySimTask::ParkToSideKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void AudiQ7_2014Tester::ParkToSideKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = TIRE_KEY_STATUS_MSD_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 6,
	  .Data = {0x22, 0xda, 0x11, 0x01,0x01,0x02}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,50);

		static bool oldv = false;
		bool newv = pCar->isACCOn;
		if (oldv != newv){
			if (newv) {
				msg.Data[1] = 0x0F;
			} else {
				msg.Data[1] = 0x00;
			}
			gpDrvCAN->transmit(CAN1, &msg);
			oldv = newv;
		}
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

AudiQ7_2014Tester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio) {

}
void AudiQ7_2014Tester::TurnSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = TURNIDICATOR_STATUS_MSD_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 4,
	  .Data = {0x00, 0x00, 0x00, 0x00}
	};
    
	CarInfo *pCar = (CarInfo*)p_arg;
    
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
        
		switch(pCar->lightPos){
            case 0://none
                msg.Data[0] = 0;
                break;
            case 1://left
                msg.Data[0] = 1;
                break;
            case 2://right
                msg.Data[0] = 2;
                break;
            default:
                msg.Data[0] = 0;
                break;
        }
        
		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

#if 0
AudiQ7_2014Tester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio) {

}
void AudiQ7_2014Tester::TireKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x5c4,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 4,
	  .Data = {0x00, 0x00, 0x00, 0x0}
	};
    
	CarInfo *pCar = (CarInfo*)p_arg;
    
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

        switch(pCar->TireKey)
        {
            case 0://没有按任何键
                msg.Data[0]=0;
                msg.Data[1]=0;
                msg.Data[2]=0;
                msg.Data[3]=0;
                break;
            case 1://*(Mode)
                msg.Data[0]=0x01;
                msg.Data[1]=0;
                msg.Data[2]=0;
                msg.Data[3]=0;                        
                break;
            case 2://导航
                msg.Data[3]=0x10;
                msg.Data[1]=0;
                msg.Data[2]=0;
                msg.Data[0]=0;
                break;
            case 3://语音
                msg.Data[1]=0x01;
                msg.Data[0]=0;
                msg.Data[2]=0;
                msg.Data[3]=0;                        
                break; 
//          case 4://Menu
//              msg.Data[0]=0x01;
//              msg.Data[2]=0x01;
//              break;
            case 5://channel_up
                msg.Data[2]=0x01;
                msg.Data[1]=0;
                msg.Data[0]=0;
                msg.Data[3]=0;                       
                break; 
            case 6://channel_down
                msg.Data[2]=0x0f;
                msg.Data[1]=0;
                msg.Data[0]=0;
                msg.Data[3]=0;
                break; 
            case 10//voice_up
                msg.Data[2]=0x10;
                msg.Data[1]=0;
                msg.Data[0]=0;
                msg.Data[3]=0;
                break;  
            case 12//voice_push 
                msg.Data[1]=0x10;
                msg.Data[0]=0;
                msg.Data[2]=0;
                msg.Data[3]=0;
               break;                        
            default ://
                msg.Data[0]=0;
                msg.Data[1]=0;
                msg.Data[2]=0;
                msg.Data[3]=0;
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
#endif

AudiQ7_2014Tester::RemoteKeyTask::RemoteKeyTask(uint8_t prio) : ZTask(prio) {

}

void AudiQ7_2014Tester::RemoteKeyTask::_doTask(void * p_arg){
	CanTxMsg msg =
	{
			.StdId = REMOTE_KEY_ID_Q7,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 5,
			.Data = {0x00, 0x00, 0x00, 0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;
	static uint8_t flag = 0;
	
	while(1){
		
		OSTimeDlyHMSM(0,0,0,40);

		static bool lastRTKStatus = false;
		bool currentRTKStatus = pCar->isRTKPressed;

		if(lastRTKStatus != currentRTKStatus){
			if(currentRTKStatus == true){
				
				msg.Data[0] = 0x01;
				msg.Data[2] = 0x02;
				gpDrvCAN->transmit(CAN1, &msg);
	
				flag = 0;
			}else{
				msg.Data[0] = 0x00;
				msg.Data[2] = 0x00;
				gpDrvCAN->transmit(CAN1, &msg);

				flag++;
			 }
		}else{
			if(currentRTKStatus == false){
				if(flag<5){
					msg.Data[0] = 0x00;
					msg.Data[2] = 0x00;
					gpDrvCAN->transmit(CAN1, &msg);
				}
				
				flag++;
			}else{
				
				msg.Data[0] = 0x01;
				msg.Data[2] = 0x02;
				gpDrvCAN->transmit(CAN1, &msg);
				
				flag = 0;
			 }
		 }
		lastRTKStatus = currentRTKStatus;
		
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { /* Release any owned resources; */ 		
																  /* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

AudiQ7_2014Tester::FrontDoorKeyTask::FrontDoorKeyTask(uint8_t prio) : ZTask(prio) {

}

void AudiQ7_2014Tester::FrontDoorKeyTask::_doTask(void * p_arg) {
	CanTxMsg msg =
	{
			.StdId = FRONTDOOR_KEY_ID_Q7,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x01, 0x00, 0x04, 0x00, 0x84, 0x00, 0x1f, 0x00}
	};	

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1){
		
		OSTimeDlyHMSM(0,0,0,100);

		if(pCar->isFDTKPressed){
			msg.Data[1] = 0x08;
		}else{
			msg.Data[1] = 0x00;
		}

		gpDrvCAN->transmit(CAN1, &msg);

		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { /* Release any owned resources; */ 		
																  /* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}

	}	
}

AudiQ7_2014Tester::BackDoorStatusTask::BackDoorStatusTask(uint8_t prio) : ZTask(prio) {

}

void AudiQ7_2014Tester::BackDoorStatusTask::_doTask(void * p_arg){
	CanTxMsg msg =
	{
			.StdId = TRUNK_STATUS_ID_Q7,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 3,
			.Data = {0x01, 0x4c, 0x40}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1){

		OSTimeDlyHMSM(0,0,0,200);
		#if 0
		static uint8_t lastBackDoorStatus = 1;
		uint8_t newBackDoorStatus = pCar->BackDoorStatus;
		
		if(newBackDoorStatus != lastBackDoorStatus){
			if(newBackDoorStatus){
				msg.Data[0] = 0x02;
				msg.Data[1] = 0x00;
				msg.Data[2] = 0x00;
			}else{
				msg.Data[0] = 0x01;
				msg.Data[1] = 0x4c;
				msg.Data[2] = 0x40;
			}
		}
		//if(!pCar->BackDoorStatus)
		gpDrvCAN->transmit(CAN1, &msg);	
		
		lastBackDoorStatus = newBackDoorStatus;
		#endif
		
		gpDrvCAN->transmit(CAN1, &msg);	

		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { /* Release any owned resources; */ 		
															      /* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
    	}
	}
}
