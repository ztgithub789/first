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
#include "BenZ50_OBDTester.h"
#include "BenZ50_OBD_common.h"

using namespace BenZ50OBD;

BenZ50_OBDTester::BenZ50_OBDTester() {
	// TODO Auto-generated constructor stub
	/*pCar->isPKeyPressed = false;
	pCar->steerAngle = 0;
	pCar->shiftPos = SHIFT_POS_P;
	pCar->isRadaBack = false;
	pCar->isRadaFront = false;*/
	pCar=NULL;
}

BenZ50_OBDTester::~BenZ50_OBDTester() {
	// TODO Auto-generated destructor stub
	
	delete pshiftTask;
	pshiftTask = NULL;

	delete psteeringTask;
	psteeringTask = NULL;
	
	delete pKeyTask;
	pKeyTask = NULL;
	
	delete pradarSimTask;
	pradarSimTask = NULL;

    delete pParkToSideTask;
    pParkToSideTask = NULL;
        
	delete pturnSimTask;
	pturnSimTask = NULL;
        
	delete ptirekeySimTask;
	ptirekeySimTask = NULL; 
        
    delete pkeyboardSimTask;
	pkeyboardSimTask = NULL; 
	
	delete pCarSpeedSimTask;
    	pCarSpeedSimTask = NULL;
                
}

void BenZ50_OBDTester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void BenZ50_OBDTester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void BenZ50_OBDTester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void BenZ50_OBDTester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void BenZ50_OBDTester::togglePKey()
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
void BenZ50_OBDTester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x09, 0x90, 0x09, 0x90};
//00 00 05 22 00 04 29 0f
    switch(nr)
    {
        case 0:
        case 2:  
          pCar->radarVal[nr] = (pCar->radarVal[nr]&0x0f) + dv/10;
          if((pCar->radarVal[nr]&0x0f) > maxRadarVal[nr])
            pCar->radarVal[nr] = 0x02;
          if((pCar->radarVal[nr]&0x0f)>0x08)
            pCar->radarVal[nr] |= 0xf0;    //将前、后左区域雷达报警情况存放在高位
          else if((pCar->radarVal[nr]&0x0f)>0x07) 
            pCar->radarVal[nr] |= 0xe0; 
          else{} 
          break;
        case 1:
        case 3:
          pCar->radarVal[nr] = (pCar->radarVal[nr]&0xf0) + (dv/10)*0x10;
          if((pCar->radarVal[nr]&0xf0) > maxRadarVal[nr])
            pCar->radarVal[nr] = 0x20;
           if((pCar->radarVal[nr]&0xf0)>0x80)
            pCar->radarVal[nr] |= 0x0f;    //将前、后右区域雷达报警情况存放在高位
          else if((pCar->radarVal[nr]&0xf0)>0x70) 
            pCar->radarVal[nr] |= 0x0e; 
          else{}              
          break;
        default :
          break;
    }       
}

uint8_t BenZ50_OBDTester::run(CarInfo *pcar)
{
	pCar = pcar;
	pCar->maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;

	pshiftTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
	pshiftTask->start(pCar);

	psteeringTask = new SteeringWheelSimTask(STEERING_WHEEL_SIM_TASK_PRIO);
	psteeringTask->start(pCar);

	pKeyTask = new PKeySimTask(P_KEY_SIM_TASK_PRIO);
	//pKeyTask->start(pCar);

	pradarSimTask = new RadarSimTask(RADAR_SIM_TASK_PRIO);
	pradarSimTask->start(pCar);
	
	pParkToSideTask = new ParkToSideKeySimTask(TURNING_LIGHT_SIM_TASK_PRIO);
	//pParkToSideTask->start(pCar);

	pturnSimTask = new TurnSimTask(TURN_SIM_TASK_PRIO);
	pturnSimTask->start(pCar);


	ptirekeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO);
	ptirekeySimTask->start(pCar); 
        
    	pkeyboardSimTask = new KeyBoardSimTask(MMIKEY_SIM_TASK_PRIO);
	//pkeyboardSimTask->start(pCar);
	
	pCarSpeedSimTask = new CarSpeedSimTask(CAR_SPEED_SIM_TASK_PRIO);
    	pCarSpeedSimTask->start(pCar);

	return 0;
}


BenZ50_OBDTester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio) {

}
void BenZ50_OBDTester::TireKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg1 =
	{
			.StdId = 0x638,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x02, 0x3e, 0x00, 0x55, 0x55, 0x55, 0x55, 0x55}
	};
      
    
	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,10);                   
                
		switch(pCar->TireKey) {
    		case 0://没有按任何键
    			msg1.Data[0]=0;
    			msg1.Data[1]=0;               
                	msg1.Data[2]=0;    			                   
    			break;
    		case 1://return 
    			
    			break;
             case 2://phone_answer:
    			msg1.Data[1]=0x04;
    			break;            
             case 3://语音
    			msg1.Data[1]=0x10;
    			break;
    		default ://
    		    	msg1.Data[1]=0;               
                	msg1.Data[3]=0;    			                  
    			break;
		}
		
		gpDrvCAN->transmit(CAN1, &msg1);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			/* Release any owned resources; */
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}



BenZ50_OBDTester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}
static int checkP=0x0;  //记录P档
void BenZ50_OBDTester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x2c7,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0xbc, 0x37, 0x00, 0xe9, 0x02, 0x0f, 0x00, 0x1d}      
	};

	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		uint8_t shiftPos = pCar->shiftPos;

		switch(shiftPos) {
		case CarInfo::SHIFT_POS_P:
			msg.Data[2] = 0x50;
            checkP = 0x01;
			break;
		case CarInfo::SHIFT_POS_R:
			msg.Data[2] = 0x52;
			break;
		case CarInfo::SHIFT_POS_N:
			msg.Data[2] = 0x4e;
			break;
		case CarInfo::SHIFT_POS_D:
			msg.Data[2] = 0x44;
			break;
		case CarInfo::SHIFT_POS_S:
			msg.Data[2] = 0x44;
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

BenZ50_OBDTester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void BenZ50_OBDTester::SteeringWheelSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = STEERING_WHEER_ANGLE_MSG_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
//	  .Data = {0xf0, 0x0f, 0x0, 0x02, 0x00, 0x02, 0x08, 0x00}
	  .Data = {0x7F, 0x58, 0xA3, 0x80, 0x8B, 0xBE, 0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

//	static uint8_t angle = 0xC0;
//	static bool angleInc = true;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
		int32_t angle = pCar->steerAngle;

		angle = angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + 0xC0;

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


BenZ50_OBDTester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void BenZ50_OBDTester::PKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x545,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 4,
	  .Data = {0x22, 0x95, 0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,50);

		static bool oldv = false;
		bool newv = pCar->isPKeyPressed;
		if (oldv != newv){
			if (newv) {
				msg.Data[2] = 0x11;
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

BenZ50_OBDTester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio) {

}

/*******************************************************************************
 * 简介   :	模拟雷达帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void BenZ50_OBDTester::RadarSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x433,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x05, 0x22, 0x00, 0x04, 0x22, 0x00}
	};
//00 00 05 22 00 04 29 0f
	CarInfo *pCar = (CarInfo*)p_arg;
	//static char time = 39;
        pCar->radarVal[0]=0x02;
        pCar->radarVal[1]=0x20;
        pCar->radarVal[2]=0x02;
        pCar->radarVal[3]=0x20;       
        
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
	//	time++;
            if(pCar->shiftPos == CarInfo::SHIFT_POS_P)
                {
                  msg.Data[0] = 0x00;
                  msg.Data[1] = 0x00;
                  msg.Data[4] = 0x00;
                  msg.Data[5] = 0x00;                 
                }
                else 
                {
			msg.Data[0] = (pCar->radarVal[0]&0x0f)|(pCar->radarVal[1]&0xf0);
			msg.Data[1] = ((pCar->radarVal[0]&0xf0)>>4)|((pCar->radarVal[1]&0x0f)<<4);
			msg.Data[4] = (pCar->radarVal[2]&0x0f)|(pCar->radarVal[3]&0xf0);
			msg.Data[5] = ((pCar->radarVal[2]&0xf0)>>4)|((pCar->radarVal[3]&0x0f)<<4);                 
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


BenZ50_OBDTester::ParkToSideKeySimTask::ParkToSideKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void BenZ50_OBDTester::ParkToSideKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x607,
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
				msg.Data[5] = 0x01;
			} else {
				msg.Data[5] = 0x02;
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

BenZ50_OBDTester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio) {

}
void BenZ50_OBDTester::TurnSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = TURNIDICATOR_STATUS_MSD_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 2,
	  .Data = {0x00, 0x00}
	};
    
	CarInfo *pCar = (CarInfo*)p_arg;
    
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
        
		msg.Data[0] = pCar->lightPos;
        msg.Data[2] = pCar->lightPos;
        
        switch(pCar->lightPos&0x3)
        {
            case 0://none
                msg.Data[0]=0;
                msg.Data[2]=0;
                break;
            case 1://left
                msg.Data[0]=0x15;
                break;
            case 2://right
                msg.Data[0]=0x2a;
                 break; 
            case 3://flash
                msg.Data[0]=0x7f;
                break;                                      
            default :
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


BenZ50_OBDTester::KeyBoardSimTask::KeyBoardSimTask(uint8_t prio) : ZTask(prio) {

}
void BenZ50_OBDTester::KeyBoardSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = 0x0fd,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 4,
			.Data = {0x00, 0x00, 0x00, 0x00}//00 ff 00 00 00 00 40 10
	};
    
	CarInfo *pCar = (CarInfo*)p_arg;
    
    while(1)
    {
        OSTimeDlyHMSM(0,0,0,100);
        
        switch(pCar->MMIKey) { //  pCar->MMIKey
            case 0:
                msg.Data[0] = 0x00;                  
                msg.Data[1] = 0x00;
                msg.Data[2] = 0x00;
                break;
            case 1:
                msg.Data[0] = 0x00;                  
                msg.Data[1] = 0x00;
                msg.Data[2] = 0x20; //SCROLL_DOWN_LEFT
                break;
            case 2:
                msg.Data[0] = 0x00;                  
                msg.Data[1] = 0x00;
                msg.Data[2] = 0x10; //SCROLL_DOWN
                break;
            case 3:
                msg.Data[0] = 0x00;                  
                msg.Data[1] = 0x00;
                msg.Data[2] = 0x08; //SCROLL_DOWN_RIGHT:
                break;
            case 4:
                msg.Data[0] = 0x00;                  
                msg.Data[1] = 0x00;
                msg.Data[2] = 0x40; //SCROLL_LEFT:
                break;
            case 5:
                msg.Data[0] = 0x00;                  
                msg.Data[1] = 0x80;
                msg.Data[2] = 0x00; //SCROLL_PUSH:
                break;
            case 6:
                msg.Data[0] = 0x00;                  
                msg.Data[1] = 0x00;
                msg.Data[2] = 0x04; //SCROLL_RIGHT:
                break;
            case 7:
                msg.Data[0] = 0x00;                  
                msg.Data[1] = 0x00;
                msg.Data[2] = 0x80; //SCROLL_UP_LEFT:
                break;
            case 8:
                msg.Data[0] = 0x00;                  
                msg.Data[1] = 0x00;
                msg.Data[2] = 0x01; //SCROLL_UP:
                break;
            case 9:
                msg.Data[0] = 0x00;                  
                msg.Data[1] = 0x00;
                msg.Data[2] = 0x02; //SCROLL_UP_RIGHT:
                break;
            case 10:
                msg.Data[0] = 0x02;
                msg.Data[1] = 0x00;
                msg.Data[2] = 0x00;// _backKeyPressed             
                break;
            case 11:
                msg.Data[0] = 0x20;
                msg.Data[1] = 0x00;
                msg.Data[2] = 0x00; //  _cKeyPressed       
                break;
            default:
                msg.Data[0] = 0x00;
                msg.Data[1] = 0x00;
                msg.Data[2] = 0x00;//
                break;
        }
	
        gpDrvCAN->transmit(CAN2, &msg);
        if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
        	/* Release any owned resources; */
        	/* De-allocate any dynamic memory; */
        	OSTaskDel(OS_PRIO_SELF);
        	break;
        }
    }
}


BenZ50_OBDTester::CarSpeedSimTask::CarSpeedSimTask(uint8_t prio) : ZTask(prio){
// TODO Auto-generated constructor stub
}

void BenZ50_OBDTester::CarSpeedSimTask::_doTask(void * p_arg){
    CanTxMsg msg = {
        .StdId = 0x309,
		.ExtId = 0,
		.IDE   = 0,
		.RTR   = 0,
		.DLC   = 8,
		.Data  = {0xed, 0x53, 0x05, 0x76, 0x0c, 0x30, 0x00, 0x80}//ed 53 05 76 0c 30 00 80    
    };

    CarInfo *pCar = (CarInfo *)p_arg;
    static uint8_t speedHighCnt = 0x80;
    static uint8_t speedLowCnt = 0x00;

    while(1){
        OSTimeDlyHMSM(0,0,0,100);

        switch(pCar->speedCtl){
            case 0:
                if(speedHighCnt != 0x80){
                    speedLowCnt--;
                    
                    if(speedLowCnt == 0x00){
                        speedLowCnt--;
                        speedHighCnt--;
                    }
                } else{
                    if(speedLowCnt != 0x00) speedLowCnt--;
                }
                break;
            case 1:
                if(speedHighCnt != 0x8f){
                    speedLowCnt++;
                    
                    if(speedLowCnt == 0xff){
                        speedLowCnt++;
                        speedHighCnt++;
                    }
                } else{
                    if(speedLowCnt != 0xff) speedLowCnt++;
                }
                break;
            default:
                break;
        }

        msg.Data[6] = speedLowCnt;
        msg.Data[7] = speedHighCnt;

        gpDrvCAN->transmit(CAN1, &msg);
        
        if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
        	/* Release any owned resources; */
        	/* De-allocate any dynamic memory; */
        	OSTaskDel(OS_PRIO_SELF);
        	break;
        }
    }
}