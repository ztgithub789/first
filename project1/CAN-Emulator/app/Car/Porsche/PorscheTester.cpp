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
#include "PorscheTester.h"
#include "Porsche_common.h"

using namespace Porsche;

PorscheTester::PorscheTester() {
	// TODO Auto-generated constructor stub
	pCar = NULL;
}

PorscheTester::~PorscheTester() {
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
	ptirekeySimTask =NULL;  

	delete pRemoteKeyTask;
	pRemoteKeyTask = NULL;

	delete pBackDoorStatusTask;
	pBackDoorStatusTask = NULL;

	delete pFrontDoorKeyTask;
	pFrontDoorKeyTask = NULL;

	
}

void PorscheTester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void PorscheTester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void PorscheTester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void PorscheTester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void PorscheTester::togglePKey()
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
void PorscheTester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x54, 0x81, 0x81, 0x54, 0x3c, 0x99, 0x99, 0x3c};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// 保证在循环周期内有一次0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}


uint8_t PorscheTester::run(CarInfo *pcar)
{
	pCar = pcar;
	pCar->maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;

	pshiftTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
	pshiftTask->start(pCar);

	psteeringTask = new SteeringWheelSimTask(STEERING_WHEEL_SIM_TASK_PRIO);
	psteeringTask->start(pCar);

	//pKeyTask = new PKeySimTask(P_KEY_SIM_TASK_PRIO);
	//pKeyTask->start(pCar);

	//pradarSimTask = new RadarSimTask(RADAR_SIM_TASK_PRIO);
	//pradarSimTask->start(pCar);

	//pParkToSideTask = new ParkToSideKeySimTask(TURNING_LIGHT_SIM_TASK_PRIO);
	//pParkToSideTask->start(pCar);

	pturnSimTask = new TurnSimTask(TURN_SIM_TASK_PRIO);
	pturnSimTask->start(pCar);
        
       ptirekeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO);
	ptirekeySimTask->start(pCar);

	//pRemoteKeyTask = new RemoteKeyTask(REMOTE_KEY_TASK_PRIO);
	//pRemoteKeyTask->start(pCar);

	//pBackDoorStatusTask = new BackDoorStatusTask(BACK_DOOR_STATUS_TASK_PRIO);
	//pBackDoorStatusTask->start(pCar);

	//pFrontDoorKeyTask = new FrontDoorKeyTask(FRONT_DOOR_KEY_TASK_PRIO);
	//pFrontDoorKeyTask->start(pCar);


	return 0;
}

PorscheTester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}

void PorscheTester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x390,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 4,
	  .Data = {0x02, 0x02, 0x00, 0x08}
	};

	CanTxMsg msg2 =
	{
	  .StdId = 0x393,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x0c, 0x02, 0x00, 0x00, 0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		uint8_t shiftPos = pCar->shiftPos;

		switch(shiftPos) {
    		case CarInfo::SHIFT_POS_P:
    			msg.Data[1] = 0x02;
			msg2.Data[3] = 0x02;
    			break;
    		case CarInfo::SHIFT_POS_R:
    			msg.Data[1] = 0x03;
			msg2.Data[3] = 0x03;
    			break;
    		case CarInfo::SHIFT_POS_N:
    			msg.Data[1] = 0x04;
			msg2.Data[3] = 0x04;
    			break;
    		case CarInfo::SHIFT_POS_D:
    			msg.Data[1] = 0x25;
			msg2.Data[3] = 0x25;
    			break;
    		default:
    			break;
		}

		
		gpDrvCAN->transmit(CAN1, &msg);
		gpDrvCAN->transmit(CAN1, &msg2);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

PorscheTester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void PorscheTester::SteeringWheelSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x086,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,25);
		int32_t angle = pCar->steerAngle;

		//int32_t val = (int32_t)(((msg.Data[5] <<8)& 0x300) | (msg.Data[6]));

		if (angle<=0) {
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE/MAX_ABS_STEER_ANGLE + 0x0400;
			//msg.Data[5] = 1;
		} else {
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE/MAX_ABS_STEER_ANGLE + 0x03ff;
			//msg.Data[5] = 0 ;
		}
		msg.Data[5] = angle & 0x0700 >> 8;
		msg.Data[6] = angle & 0x00ff >> 0;

		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}



PorscheTester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio) {

}
void PorscheTester::TurnSimTask::_doTask(void *p_arg)
{
	
	
	CanTxMsg msg =
	{
	  .StdId = 0x363,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 4,
	  .Data = {0x00, 0x00, 0x10, 0x00}
	};

	
	CarInfo *pCar = (CarInfo*)p_arg;
	
	while(1) {
		OSTimeDlyHMSM(0,0,0,50);
		 switch(pCar->lightPos)
	        {
		        case 0://none
		            msg.Data[1] = 0x00;
		            break;
		        case 1://left
		            msg.Data[1] = 0x01;
		            break;
		        case 2://right
		            msg.Data[1] = 0x02;
		            break;
		        case 3://flash
		            msg.Data[1] = 0x04;
		            break;
		        default:
		            msg.Data[1] = 0x00;
		          break;
	        }
		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

PorscheTester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio) {

}

void PorscheTester::TireKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x5bf,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 4,
	  .Data = {0x00, 0x00, 0x00, 0x0f}
	};
	CarInfo *pCar = (CarInfo*)p_arg;
    static uint8_t times = 0, status = 0, oldkey = 0, newkey = 0; 
    
	while(1) {
        OSTimeDlyHMSM(0,0,0,1000);

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
                        if(status >=6)
                            status = 6;
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
                msg.Data[2]=0;
                break;
            case 1://*(Mode)返回键
                msg.Data[0]=0x08;
                msg.Data[1]=0;
                msg.Data[2]=status;
                msg.Data[3]=0;                        
                break;
	      case 5://channel_up
                msg.Data[0]=0x06;
                msg.Data[2]=0x01;                    
                break; 
            case 6://channel_down
                msg.Data[0]=0x06;
                msg.Data[2]=0x0f;
                break;
            case 7://ok
                msg.Data[0]=0x07;
                msg.Data[2]=status;
		   break;
            case 10://voice_up
                msg.Data[0]=0x12;
                msg.Data[2]=0x10;
                break;  
            case 11://voice_down
                msg.Data[0]=0x12;
                msg.Data[2]=0x0f;
                break;			
            case 2://phone_on             
                msg.Data[0]=0x1e;
                msg.Data[2]=status;
                break;
	     case 12://phone_off
                msg.Data[0]=0x1d;
                msg.Data[2]=status;
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

PorscheTester::RemoteKeyTask::RemoteKeyTask(uint8_t prio) : ZTask(prio) {

}

void PorscheTester::RemoteKeyTask::_doTask(void * p_arg){
	CanTxMsg msg =
	{
			.StdId = REMOTE_KEY_ID_Q5,
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

PorscheTester::FrontDoorKeyTask::FrontDoorKeyTask(uint8_t prio) : ZTask(prio) {

}

void PorscheTester::FrontDoorKeyTask::_doTask(void * p_arg) {
	CanTxMsg msg =
	{
			.StdId = FRONTDOOR_KEY_ID_Q5,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x01, 0x80, 0x04, 0x00, 0x8c, 0x00, 0x00, 0x00}
	};	

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1){
		
		OSTimeDlyHMSM(0,0,0,100);

		if(pCar->isFDTKPressed){
			msg.Data[1] = 0x88;
		}else{
			msg.Data[1] = 0x80;
		}

		gpDrvCAN->transmit(CAN1, &msg);

		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { /* Release any owned resources; */ 		
																  /* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}

	}	
}

PorscheTester::BackDoorStatusTask::BackDoorStatusTask(uint8_t prio) : ZTask(prio) {

}

void PorscheTester::BackDoorStatusTask::_doTask(void * p_arg){
	CanTxMsg msg =
	{
			.StdId = TRUNK_STATUS_ID_Q5,
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


PorscheTester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void PorscheTester::PKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x603,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 4,
	  .Data = {0x22, 0x94, 0x00, 0xff}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,50);

		static bool oldv = false;
		bool newv = pCar->isPKeyPressed;
		if (oldv != newv) {
			if (newv) {
				msg.Data[2] = 0x01;
				//msg.Data[3] = 0x01;
			} else {
				msg.Data[2] = 0x0;
				//msg.Data[3] = 0x0;
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


PorscheTester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio) {

}

/*******************************************************************************
 * 简介   :	模拟雷达帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void PorscheTester::RadarSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x634,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 6,
	  .Data = {0x42, 0x92, 0x00, 0x00, 0x00, 0x00}
	};

	// dir表示当前帧的方向。
	// 	true：前
	//	false: 后
	static bool dir = true;

    //bool sendradar = false;
    static uint8_t oldRadar[8];
    
	CarInfo *pCar = (CarInfo*)p_arg;
    
    for(uint8_t i = 0; i <8; i++)
    {
      oldRadar[i] = pCar->radarVal[i];
    }
    
    //uint8_t timer =0;
    
	while(1) {
		OSTimeDlyHMSM(0,0,0,50);
/*        
        for(uint8_t i = 0; i <8; i++)
        {
          if(oldRadar[i] != pCar->radarVal[i])
          {
            oldRadar[i] = pCar->radarVal[i];
            sendradar = true;
            timer = 0;
          }else{
            
            sendradar = false;
          }
        }
*/        
        if (dir == true) {
          // 前帧
          msg.Data[1] = 0x92;
          msg.Data[2] = pCar->radarVal[CarInfo::RADAR_LFS];
          msg.Data[3] = pCar->radarVal[CarInfo::RADAR_LFM];
          msg.Data[4] = pCar->radarVal[CarInfo::RADAR_RFM];
          msg.Data[5] = pCar->radarVal[CarInfo::RADAR_RFS];
        } else {
          // 后帧
          msg.Data[1] = 0x93;
          msg.Data[2] = pCar->radarVal[CarInfo::RADAR_LRS];
          msg.Data[3] = pCar->radarVal[CarInfo::RADAR_LRM];
          msg.Data[4] = pCar->radarVal[CarInfo::RADAR_RRM];
          msg.Data[5] = pCar->radarVal[CarInfo::RADAR_RRS];
        }

        dir = !dir;	// 每发送一帧后反向
        //        if(sendradar || timer < 200)
        //        {
        gpDrvCAN->transmit(CAN1, &msg);
        //          timer ++;
        //        }
        
        if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
          /* Release any owned resources; */ 
          /* De-allocate any dynamic memory; */
          OSTaskDel(OS_PRIO_SELF);
          break;
        }       
	}
}



PorscheTester::ParkToSideKeySimTask::ParkToSideKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void PorscheTester::ParkToSideKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = TIRE_KEY_MSG_ID,
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