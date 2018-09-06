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
#include "AudiQ3_2013_domesticTester.h"
#include "AudiQ3_2013_domestic_common.h"

using namespace AudiQ32013domestic;

AudiQ3_2013_domesticTester::AudiQ3_2013_domesticTester(u8 baudrate) {
	// TODO Auto-generated constructor stub
	pCar = NULL;
	bd = baudrate;
}

AudiQ3_2013_domesticTester::~AudiQ3_2013_domesticTester() {
	// TODO Auto-generated destructor stub
	delete pshiftTask;
	pshiftTask = NULL;

	delete psteeringTask;
	psteeringTask = NULL;

	delete pturnSimTask;
	pturnSimTask = NULL;

	delete pParkToSideTask;
	pParkToSideTask = NULL;
    
    delete pKeyTask;
	pKeyTask = NULL;
    
    
	delete pradarSimTask;
	pradarSimTask = NULL;
    
    delete ptirekeySimTask;
	ptirekeySimTask =NULL;  

	delete pRemoteKeyTask;
	pRemoteKeyTask = NULL;

	delete pBackDoorStatusTask;
	pBackDoorStatusTask = NULL;


}

void AudiQ3_2013_domesticTester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void AudiQ3_2013_domesticTester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void AudiQ3_2013_domesticTester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void AudiQ3_2013_domesticTester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void AudiQ3_2013_domesticTester::togglePKey()
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
void AudiQ3_2013_domesticTester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x5a, 0x74, 0x74, 0x5a, 0x3c, 0xa0, 0xa0, 0x3c};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// 保证在循环周期内有一次0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}

uint8_t AudiQ3_2013_domesticTester::run(CarInfo *pcar)
{
	pCar = pcar;
	pCar->baudRate = bd;
	pCar->maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;

	pshiftTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
	pshiftTask->start(pCar);

	psteeringTask = new SteeringWheelSimTask(STEERING_WHEEL_SIM_TASK_PRIO);
	psteeringTask->start(pCar);

	pParkToSideTask = new ParkToSideKeySimTask(TURNING_LIGHT_SIM_TASK_PRIO);
	pParkToSideTask->start(pCar);

	pturnSimTask = new TurnSimTask(TURN_SIM_TASK_PRIO);
	pturnSimTask->start(pCar);
    
    	pKeyTask = new PKeySimTask(P_KEY_SIM_TASK_PRIO);
	pKeyTask->start(pCar);
    
    	pradarSimTask = new RadarSimTask(RADAR_SIM_TASK_PRIO);
	pradarSimTask->start(pCar);
    
    	ptirekeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO);
	ptirekeySimTask->start(pCar);

	pRemoteKeyTask = new RemoteKeyTask(REMOTE_KEY_TASK_PRIO);
	pRemoteKeyTask->start(pCar);

	pBackDoorStatusTask = new BackDoorStatusTask(BACK_DOOR_STATUS_TASK_PRIO);
	pBackDoorStatusTask->start(pCar);

	return 0;
}

AudiQ3_2013_domesticTester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}

void AudiQ3_2013_domesticTester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = SHIFT_POS_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x00, 0x14, 0x05, 0x00, 0x00, 0x0D}
	};

	CarInfo *pCar = (CarInfo*)p_arg;
	if(pCar->baudRate == 100) {
		msg.StdId = SHIFT_POS_MSG_ID_100K;
	} else {
		msg.StdId = SHIFT_POS_MSG_ID;
	}

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		uint8_t shiftPos = pCar->shiftPos;
        msg.StdId = SHIFT_POS_MSG_ID_100K;
		switch(shiftPos) {
		case CarInfo::SHIFT_POS_P:
			msg.Data[1] = 0x81;
			break;
		case CarInfo::SHIFT_POS_R:
			msg.Data[1] = 0x71;
			break;
		case CarInfo::SHIFT_POS_N:
			msg.Data[1] = 0x61;
			break;
		case CarInfo::SHIFT_POS_D:
			msg.Data[1] = 0x51;
			break;
		case CarInfo::SHIFT_POS_S:
			msg.Data[1] = 0xc1;
			break;
		default:
			break;
		}
		msg.Data[7] = msg.Data[1];

	    gpDrvCAN->transmit(CAN1, &msg);
        
        msg.StdId = SHIFT_POS_MSG_ID;
        gpDrvCAN->transmit(CAN1, &msg);
    
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

AudiQ3_2013_domesticTester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void AudiQ3_2013_domesticTester::SteeringWheelSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = STEERING_WHEER_ANGLE_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0xf0, 0x0f, 0x0, 0x02, 0x00, 0x02, 0x08, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;
	if(pCar->baudRate == 100) {
		msg.StdId = STEERING_WHEER_ANGLE_MSG_ID_100K;
	} else {
		msg.StdId = STEERING_WHEER_ANGLE_MSG_ID;
	}

	while(1) {
		OSTimeDlyHMSM(0,0,0,10);
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
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}


AudiQ3_2013_domesticTester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void AudiQ3_2013_domesticTester::PKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x6f4,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x00, 0x00,0x00,0x00,0x00,0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,50);
        
        //CanTxMsg msg;
        
		static bool oldv = false;
		bool newv = pCar->isPKeyPressed;
		if (oldv != newv){
          
              msg.StdId = 0x6f4;
              msg.ExtId = 0;
              msg.IDE = 0;
              msg.RTR = 0;
              msg.DLC = 8;
              //msg.Data = {0x8a, 0x48, 0x01, 0x00,0x00,0x00,0x00,0x00};
                      
          
			if (newv) {
				msg.Data[3] = 0x08;
			} else {
				msg.Data[3] = 0x0;
			}
			gpDrvCAN->transmit(CAN1, &msg);
            
            

              msg.StdId = 0x67a;
              msg.ExtId = 0;
              msg.IDE = 0;
              msg.RTR = 0;
              msg.DLC = 4;
              //msg.Data = {0x22, 0x95, 0x00, 0x00};
          
			if (newv) {
                msg.Data[2] = 0x11;
				msg.Data[3] = 0x01;
			} else {
                msg.Data[2] = 0x0;
				msg.Data[3] = 0x0;
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

AudiQ3_2013_domesticTester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio) {

}

/*******************************************************************************
 * 简介   :	模拟雷达帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void AudiQ3_2013_domesticTester::RadarSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x6da,
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
		#if 0
		msg.StdId = 0x6a5;
		if(dir==true ){	
			// 前帧
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
        gpDrvCAN->transmit(CAN1, &msg);

		#endif
        
        //msg.StdId = 0x6da;
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
        gpDrvCAN->transmit(CAN1, &msg);
		dir = !dir;
		
        
        
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

AudiQ3_2013_domesticTester::ParkToSideKeySimTask::ParkToSideKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void AudiQ3_2013_domesticTester::ParkToSideKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = TIRE_STATUS_KEY_ID,
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
				msg.Data[0] = 0x19;
			} else {
				msg.Data[0] = 0x00;
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

AudiQ3_2013_domesticTester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio) {

}
void AudiQ3_2013_domesticTester::TurnSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = TURNIDICATOR_STATUS_MSD_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x2b, 0x00, 0x58, 0x08, 0x40, 0x02, 0x00, 0x00} //2b 00 58 08 40 02 00 00
	};
	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
		msg.Data[0] = pCar->lightPos;
        
        switch(pCar->lightPos)
        {
            case 0 ://none
                msg.Data[4] = 0x40;
                break;
            case 1://left
                msg.Data[4] = 0x44;
                break;
            case 2://right
                msg.Data[4] = 0x48;
                break;
            default:
                msg.Data[4] = 0x40;
                break;      
        }
        
		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

AudiQ3_2013_domesticTester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio) {

}
void AudiQ3_2013_domesticTester::TireKeySimTask::_doTask(void *p_arg)
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
            case 5://channel up
                msg.Data[0]=0x06;
                msg.Data[2]=0x01;
                break;
            case 6://channel down
                msg.Data[0]=0x06;
                msg.Data[2]=0x0f;
                break;
            case 7://ok
                msg.Data[0]=0x07;
                msg.Data[2]=status;
                break;
            case 8://left
                msg.Data[0]=0x03;
                msg.Data[2]=status;
                break;
            case 9://right
                msg.Data[0]=0x02;
                msg.Data[2]=status;
                break;
            case 10://voice up
                msg.Data[0]=0x12;
                msg.Data[2]=0x0f;
                break; 
            case 11://voice down
                msg.Data[0]=0x12;
                msg.Data[2]=0x01;
                break; 
            case 12://静音
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


AudiQ3_2013_domesticTester::RemoteKeyTask::RemoteKeyTask(uint8_t prio) : ZTask(prio) {

}

void AudiQ3_2013_domesticTester::RemoteKeyTask::_doTask(void * p_arg){
	CanTxMsg msg =
	{
			.StdId = REMOTE_KEY_ID_Q3,
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
				
				msg.Data[0] = 0x11;
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
				
				msg.Data[0] = 0x11;
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


AudiQ3_2013_domesticTester::BackDoorStatusTask::BackDoorStatusTask(uint8_t prio) : ZTask(prio) {

}

void AudiQ3_2013_domesticTester::BackDoorStatusTask::_doTask(void * p_arg){
	CanTxMsg msg =
	{
			.StdId = TRUNK_STATUS_ID_Q3,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 3,
			.Data = {0xc2, 0x01, 0xc8}
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