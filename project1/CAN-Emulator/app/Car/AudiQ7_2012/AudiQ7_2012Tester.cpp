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
#include "AudiQ7_2012Tester.h"
#include "AudiQ7_2012_common.h"

using namespace AudiQ7_2012;

AudiQ7_2012Tester::AudiQ7_2012Tester() {
	// TODO Auto-generated constructor stub
	pCar = NULL;
}

AudiQ7_2012Tester::~AudiQ7_2012Tester() {
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

	delete pParkToSideTask;
	pParkToSideTask = NULL;
        
 	delete ptirekeySimTask;
	ptirekeySimTask = NULL;   

	delete pRemoteKeyTask;
	pRemoteKeyTask = NULL;

	delete pBackDoorStatusTask;
	pBackDoorStatusTask = NULL;

	delete pFrontDoorKeyTask;
	pFrontDoorKeyTask = NULL;
}

void AudiQ7_2012Tester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void AudiQ7_2012Tester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void AudiQ7_2012Tester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void AudiQ7_2012Tester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void AudiQ7_2012Tester::togglePKey()
{
	if (pCar->isPKeyPressed)
		pCar->isPKeyPressed = false;
	else
		pCar->isPKeyPressed = true;
}

/*******************************************************************************
 * ���   :	����ָ���״�̽ͷ����ֵ��������������ֵ���Զ��ۻص���Сֵ
 * ����   :	nr		̽ͷ��š�ǰ���������Ϊ0, 1, 2, 3�������������Ϊ4, 5, 6, 7
 * 			dv		����
 * ����ֵ :
 * ����   :
 *******************************************************************************/
void AudiQ7_2012Tester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x5a, 0x74, 0x74, 0x5a, 0x3c, 0xa0, 0xa0, 0x3c};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// ��֤��ѭ����������һ��0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}

uint8_t AudiQ7_2012Tester::run(CarInfo *pcar)
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

	pParkToSideTask = new ParkToSideKeySimTask(TURNING_LIGHT_SIM_TASK_PRIO);
	pParkToSideTask->start(pCar);

	pturnSimTask = new TurnSimTask(TURN_SIM_TASK_PRIO);
	pturnSimTask->start(pCar);
        
        ptirekeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO); //TIREKEY_SIM_TASK_PRIO
	ptirekeySimTask->start(pCar);

	pRemoteKeyTask = new RemoteKeyTask(REMOTE_KEY_TASK_PRIO);
	pRemoteKeyTask->start(pCar);

	pBackDoorStatusTask = new BackDoorStatusTask(BACK_DOOR_STATUS_TASK_PRIO);
	pBackDoorStatusTask->start(pCar);

	pFrontDoorKeyTask = new FrontDoorKeyTask(FRONT_DOOR_KEY_TASK_PRIO);
	pFrontDoorKeyTask->start(pCar);
        
	return 0;
}

AudiQ7_2012Tester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}

void AudiQ7_2012Tester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x355,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x00, 0x14, 0x05, 0x00, 0x00, 0x0D}
	};

	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		uint8_t shiftPos = pCar->shiftPos;

		switch(shiftPos) {
		case CarInfo::SHIFT_POS_P:
			msg.Data[4] = 0x18;
			break;
		case CarInfo::SHIFT_POS_R:
			msg.Data[4] = 0x17;
			break;
		case CarInfo::SHIFT_POS_N:
			msg.Data[4] = 0x16;
			break;
		case CarInfo::SHIFT_POS_D:
			msg.Data[4] = 0x15;
			break;
		case CarInfo::SHIFT_POS_S:
			msg.Data[4] = 0x1c;
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

AudiQ7_2012Tester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void AudiQ7_2012Tester::SteeringWheelSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x3c3,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0xf0, 0x0f, 0x0, 0x02, 0x00, 0x02, 0x08, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,200);
		int32_t angle = pCar->steerAngle;

		if (angle <= 0) {
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + CAN_STEER_LEFT_BEGIN_VAL;
		} else {
			angle = angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + CAN_STEER_RIGHT_BEGIN_VAL;
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


AudiQ7_2012Tester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * ���   :	����P����ע��ƽʱû��P��֡��ֻ�е�����P����Ż���P��֡
 * ����   :
 * ����ֵ :
 * ����   :
 *******************************************************************************/
void AudiQ7_2012Tester::PKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = P_KEY_STATUS_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 4,
	  .Data = {0x8a, 0x48, 0x01, 0x00,0x00,0x00,0x00,0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,50);

		static bool oldv = false;
		bool newv = pCar->isPKeyPressed;
		if (oldv != newv){
			if (newv) {
				msg.Data[0] = 0x01;
			} else {
				msg.Data[0] = 0x0;
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

AudiQ7_2012Tester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio) {

}

/*******************************************************************************
 * ���   :	ģ���״�֡
 * ����   :
 * ����ֵ :
 * ����   :
 *******************************************************************************/
void AudiQ7_2012Tester::RadarSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x6a5,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x42, 0x87, 0x00, 0x00, 0x00, 0x00,0x00, 0x00}
	};
	
	CarInfo *pCar = (CarInfo*)p_arg;
	
	static bool dir = true;
	
	while(1) {
		OSTimeDlyHMSM(0,0,0,50);
		
		if(dir==true ){	
			// ǰ֡
			msg.Data[1] = 0x86;
			msg.Data[3] = pCar->radarVal[CarInfo::RADAR_LFS];
			msg.Data[4] = pCar->radarVal[CarInfo::RADAR_LFM];
			msg.Data[5] = pCar->radarVal[CarInfo::RADAR_RFM];
			msg.Data[6] = pCar->radarVal[CarInfo::RADAR_RFS];
			
		} else {		
			msg.Data[1] = 0x87;
			msg.Data[3] = pCar->radarVal[CarInfo::RADAR_LRS];
			msg.Data[4] = pCar->radarVal[CarInfo::RADAR_LRM];
			msg.Data[5] = pCar->radarVal[CarInfo::RADAR_RRM];
			msg.Data[6] = pCar->radarVal[CarInfo::RADAR_RRS];
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

AudiQ7_2012Tester::ParkToSideKeySimTask::ParkToSideKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * ���   :	����P����ע��ƽʱû��P��֡��ֻ�е�����P����Ż���P��֡
 * ����   :
 * ����ֵ :
 * ����   :
 *******************************************************************************/
void AudiQ7_2012Tester::ParkToSideKeySimTask::_doTask(void *p_arg)
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

AudiQ7_2012Tester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio) {

}
void AudiQ7_2012Tester::TurnSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = TURNIDICATOR_STATUS_MSD_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 4,
	  .Data = {0x00, 0x02, 0x09, 0x00}
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

AudiQ7_2012Tester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio) {

}
void AudiQ7_2012Tester::TireKeySimTask::_doTask(void *p_arg)
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
    static uint8_t times = 0, status = 0, oldkey = 0, newkey = 0;   
    
	while(1) {
        OSTimeDlyHMSM(0,0,0,100);

        newkey = pCar->TireKey;

        if(newkey != 0)
        {
            if((newkey != 5)&&(newkey != 6)&&(newkey != 10)&&(newkey != 11))
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
            case 0://û�а��κμ�
                msg.Data[0]=0;
                msg.Data[1]=0;
                msg.Data[2]=0;
                msg.Data[3]=0;
                break;
            case 1://*(Mode)
                msg.Data[0]=status;
                msg.Data[1]=0;
                msg.Data[2]=0;
                msg.Data[3]=0;                        
                break;
            case 2://����
                msg.Data[3]= status<<4;
                msg.Data[1]=0;
                msg.Data[2]=0;
                msg.Data[0]=0;
                break;
            case 3://����
                msg.Data[1]=status;
                msg.Data[0]=0;
                msg.Data[2]=0;
                msg.Data[3]=0;                        
                break; 
//            case 4://Menu
//                msg.Data[0]=0x01;
//                msg.Data[2]=0x01;
//                break;
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
            case 7://ok
                msg.Data[0]=status<<4;
                msg.Data[1]=0;
                msg.Data[2]=0;
                msg.Data[3]=0;
                break;
            case 10://voice_up
                msg.Data[2]=0x10;
                msg.Data[1]=0;
                msg.Data[0]=0;
                msg.Data[3]=0;
                break;
            case 11://voice_down
                msg.Data[2]=0xf0;
                msg.Data[1]=0;
                msg.Data[0]=0;
                msg.Data[3]=0;
                break; 
            case 12://voice_push
                msg.Data[1]= status<<4;
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


AudiQ7_2012Tester::RemoteKeyTask::RemoteKeyTask(uint8_t prio) : ZTask(prio) {

}

void AudiQ7_2012Tester::RemoteKeyTask::_doTask(void * p_arg){
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

AudiQ7_2012Tester::FrontDoorKeyTask::FrontDoorKeyTask(uint8_t prio) : ZTask(prio) {

}

void AudiQ7_2012Tester::FrontDoorKeyTask::_doTask(void * p_arg) {
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

AudiQ7_2012Tester::BackDoorStatusTask::BackDoorStatusTask(uint8_t prio) : ZTask(prio) {

}

void AudiQ7_2012Tester::BackDoorStatusTask::_doTask(void * p_arg){
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

