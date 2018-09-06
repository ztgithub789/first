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
#include "BMWTester.h"
#include "BMW_common.h"

using namespace BMW;

BMWTester::BMWTester() {
	// TODO Auto-generated constructor stub
	/*pCar->isPKeyPressed = false;
	pCar->steerAngle = 0;
	pCar->shiftPos = SHIFT_POS_P;
	pCar->isRadaBack = false;
	pCar->isRadaFront = false;*/
	pCar=NULL;
}

BMWTester::~BMWTester() {
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
                
}

void BMWTester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void BMWTester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void BMWTester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void BMWTester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void BMWTester::togglePKey()
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
void BMWTester::increaseRadarVal(int nr, int dv)
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
                pCar->radarVal[nr] |= 0xf0;    //��ǰ�����������״ﱨ���������ڸ�λ
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
                pCar->radarVal[nr] |= 0x0f;    //��ǰ�����������״ﱨ���������ڸ�λ
              else if((pCar->radarVal[nr]&0xf0)>0x70) 
                pCar->radarVal[nr] |= 0x0e; 
              else{}              
              break;
        default :
              break;
    }      
}

uint8_t BMWTester::run(CarInfo *pcar)
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


	ptirekeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO);
	ptirekeySimTask->start(pCar); 
        
    pkeyboardSimTask = new KeyBoardSimTask(MMIKEY_SIM_TASK_PRIO);
	pkeyboardSimTask->start(pCar);
        
	return 0;
}

BMWTester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}
static int checkP=0x0;  //��¼P��
void BMWTester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x0f3,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x04, 0xc0, 0xf0, 0x00, 0xff, 0xff} //5b 65 04 c0 f0 c2 ff ff
	};

	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		uint8_t shiftPos = pCar->shiftPos;

		switch(shiftPos) {
		case CarInfo::SHIFT_POS_P:
			msg.Data[5] = 0xc4;
                        checkP = 0x01;
			break;
		case CarInfo::SHIFT_POS_R:
			msg.Data[5] = 0xc2;
			break;
		//case CarInfo::SHIFT_POS_N:
		//	msg.Data[5] = 0x16;
		//	break;
		//case CarInfo::SHIFT_POS_D:
		//	msg.Data[5] = 0x15;
		//	break;
		//case CarInfo::SHIFT_POS_S:
		//	msg.Data[5] = 0x1d;
		//	break;
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

BMWTester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void BMWTester::SteeringWheelSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x301,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0xf0, 0x0f, 0x0, 0x02, 0x00, 0x02, 0x08, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
		int32_t angle = pCar->steerAngle;

		angle = angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + 0x1000;

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


BMWTester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * ���   :	����P����ע��ƽʱû��P��֡��ֻ�е�����P����Ż���P��֡
 * ����   :
 * ����ֵ :
 * ����   :
 *******************************************************************************/
void BMWTester::PKeySimTask::_doTask(void *p_arg)
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

BMWTester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio) {

}

/*******************************************************************************
 * ���   :	ģ���״�֡
 * ����   :
 * ����ֵ :
 * ����   :
 *******************************************************************************/
void BMWTester::RadarSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x2ee,
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
	    //time++;
            if(pCar->shiftPos == CarInfo::SHIFT_POS_P)
                {
                  msg.Data[3] = 0x00;
                  msg.Data[4] = 0x00;
                  msg.Data[6] = 0x00;
                  msg.Data[7] = 0x00;                 
                }
                else 
                {
			msg.Data[3] = (pCar->radarVal[0]&0x0f)|(pCar->radarVal[1]&0xf0);
			msg.Data[4] = ((pCar->radarVal[0]&0xf0)>>4)|((pCar->radarVal[1]&0x0f)<<4);
			msg.Data[6] = (pCar->radarVal[2]&0x0f)|(pCar->radarVal[3]&0xf0);
			msg.Data[7] = ((pCar->radarVal[2]&0xf0)>>4)|((pCar->radarVal[3]&0x0f)<<4);                 
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


BMWTester::ParkToSideKeySimTask::ParkToSideKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * ���   :	����P����ע��ƽʱû��P��֡��ֻ�е�����P����Ż���P��֡
 * ����   :
 * ����ֵ :
 * ����   :
 *******************************************************************************/
void BMWTester::ParkToSideKeySimTask::_doTask(void *p_arg)
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

BMWTester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio) {

}
void BMWTester::TurnSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = TURNIDICATOR_STATUS_MSD_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 3,
	  .Data = {0x00, 0xff, 0x00}
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
                msg.Data[2]=0x05;
                msg.Data[0]=0x40;
                break;
            case 2://right
                msg.Data[2]=0x0a;
                msg.Data[0]=0x80;
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

BMWTester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio) {

}

void BMWTester::TireKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = 0x045,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}//00 ff 00 00 00 00 40 10
	};
    
	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);                   
                
		switch(pCar->TireKey) {
    		case 0://û�а��κμ�
    			msg.Data[4]=0;
    			msg.Data[5]=0;
    			break;
    		case 1://return 
    			msg.Data[4]=0x00;
    			msg.Data[5]=0x02;
    			break;
    		case 2://Phone_answer
    			msg.Data[4]=0x40;
    			msg.Data[5]=0x00;
    			break;
    		case 3://����
    			msg.Data[4]=0x00;
    			msg.Data[5]=0x01;
    			break;
            case 4://Menu
    			msg.Data[4]=0x00;
    			msg.Data[5]=0x04;
    			break;
            case 5://��������
    			msg.Data[4]=0x01;
    			msg.Data[5]=0x00;
    			break;
    		case 6://��������
    			msg.Data[4]=0x02;
    			msg.Data[5]=0x00;
    			break;
            case 7://OK
    			msg.Data[4]=0x00;
    			msg.Data[5]=0x08;
    			break;
            case 8://�󰴼�
    			msg.Data[4]=0x08;
    			msg.Data[5]=0x00;
    			break;
    		case 9://�Ұ���
    			msg.Data[4]=0x04;
    			msg.Data[5]=0x00;
    			break;
            case 10://����+
    			msg.Data[4]=0x01;
    			msg.Data[5]=0x00;
    			break;
            case 11://����-
    			msg.Data[4]=0x02;
    			msg.Data[5]=0x00;
    			break;
            case 12://Phone_handoff
    			msg.Data[4]=0x80;
    			msg.Data[5]=0x00;
    			break;   		
      		default ://
    			msg.Data[4]=0;
    			msg.Data[5]=0;
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


BMWTester::KeyBoardSimTask::KeyBoardSimTask(uint8_t prio) : ZTask(prio) {

}
void BMWTester::KeyBoardSimTask::_doTask(void *p_arg)
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
        
        switch(pCar->TireKey) { //  pCar->MMIKey
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

