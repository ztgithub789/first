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
#include "AudiA3_2014Tester.h"
#include "AudiA3_2014_common.h"

using namespace AudiA32014;

AudiA3_2014Tester::AudiA3_2014Tester() {
// TODO Auto-generated constructor stub
	pCar = NULL;
}

AudiA3_2014Tester::~AudiA3_2014Tester() {
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

	delete ptirekeySimTask;
	ptirekeySimTask = NULL;

	delete pmmiKeyTask;
	pmmiKeyTask =NULL;
        
	delete pBackLightTask;
	pBackLightTask =NULL; 

	delete pPageTrunKeyTask;
	pPageTrunKeyTask = NULL;
              
}

void AudiA3_2014Tester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void AudiA3_2014Tester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void AudiA3_2014Tester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void AudiA3_2014Tester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void AudiA3_2014Tester::togglePKey()
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
void AudiA3_2014Tester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x5a, 0x78, 0x78, 0x5a, 0x5a, 0xa0, 0xa0, 0x5a};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// ��֤��ѭ����������һ��0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}

uint8_t AudiA3_2014Tester::run(CarInfo *pcar)
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

	pturnSimTask = new TurnSimTask(TURN_SIM_TASK_PRIO);
	pturnSimTask->start(pCar);

	ptirekeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO);
	ptirekeySimTask->start(pCar);

	pmmiKeyTask = new MMIKeyTask(MMIKEY_SIM_TASK_PRIO);
	pmmiKeyTask->start(pCar);
        
    pBackLightTask = new BackLightTask(BACKLIGHT_SIM_TASK_PRIO);
    pBackLightTask->start(pCar);

	pPageTrunKeyTask = new PageTrunKeyTask(PAGETRUN_SIM_TASK_PRIO);
	//pPageTrunKeyTask->start(pCar);

	return 0;
}

AudiA3_2014Tester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
// TODO Auto-generated constructor stub
}

void AudiA3_2014Tester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = 0x3dc,
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
			msg.Data[5] = 0x05;
			break;
		case CarInfo::SHIFT_POS_R:
			msg.Data[5] = 0x06;
			break;
		case CarInfo::SHIFT_POS_N:
			msg.Data[5] = 0x07;
			break;
		case CarInfo::SHIFT_POS_D:
			msg.Data[5] = 0x08;
			break;
		case CarInfo::SHIFT_POS_S:
			msg.Data[5] = 0x08;
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

AudiA3_2014Tester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void AudiA3_2014Tester::SteeringWheelSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = 0x3da,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x20, 0x0a, 0xe6, 0x00, 0x00, 0x00, 0x00, 0x00}
	};
	//20 0a e6 00 00 8c 99 00
	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
		int32_t angle = pCar->steerAngle;

		int32_t value = abs(angle) * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE ;
		value <<= 4;
		value &= 0xfff0;
		if (angle <= 0) {
			value |= 0x00;
		} else {
			value |= 0x04;
		}

		msg.Data[6] = (value & 0xff00) >> 8;
		msg.Data[5] = (value & 0x00ff) >> 0;

		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}


AudiA3_2014Tester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * ���   :	����P����ע��ƽʱû��P��֡��ֻ�е�����P����Ż���P��֡
 * ����   :
 * ����ֵ :
 * ����   :
 *******************************************************************************/
void AudiA3_2014Tester::PKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = 0,
			.ExtId = P_KEY_STATUS_MSG_ID,
			.IDE = CAN_ID_EXT,
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

AudiA3_2014Tester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio) {

}

/*******************************************************************************
 * ���   :	ģ���״�֡
 * ����   :
 * ����ֵ :
 * ����   :
 *******************************************************************************/
void AudiA3_2014Tester::RadarSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = 0,
			.ExtId = RADAR_MSG_ID,
			.IDE = CAN_ID_EXT,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x42, 0x93, 0x00, 0x00, 0x00, 0x00,0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	static bool dir = true;

	while(1) {
		OSTimeDlyHMSM(0,0,0,50);

		if(dir==true ){	
			// ǰ֡
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


AudiA3_2014Tester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio) {

}
void AudiA3_2014Tester::TurnSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = TURNIDICATOR_STATUS_MSD_ID,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
	};
	CarInfo *pCar = (CarInfo*)p_arg;
	
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		switch(pCar->lightPos&0x3)
		{
    		case 0://none
    			msg.Data[2]=0;
    			msg.Data[3]=0;
    			break;
    		case 1://left
    			msg.Data[2]=0x80;
    			msg.Data[3]=0x0a;
    			break;
    		case 2://right
    			msg.Data[2]=0x00;
    			msg.Data[3]=0x15;
    			break;
    		case 3://flash
    			msg.Data[2]=0x10;
    			msg.Data[3]=0x1e;
    			break;
    		default :
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

AudiA3_2014Tester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio) {

}
void AudiA3_2014Tester::TireKeySimTask::_doTask(void *p_arg)
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
                                
		switch(newkey) {
    		case 0://û�а��κμ�
    			msg.Data[0]=0;
    			msg.Data[2]=0;
    			break;
    		case 1://*
    			msg.Data[0]=0x21;
    			msg.Data[2]=status;
    			break;
    		case 2://����
    			msg.Data[0]=0x1b;
    			msg.Data[2]=status;
    			break;
    		case 3://����
    			msg.Data[0]=0x19;
    			msg.Data[2]=status;
    			break;
            case 4://Menu
    			msg.Data[0]=0x01;
    			msg.Data[2]=status;
    			break;
            case 5://��������(�󻬼�)
    			msg.Data[0]=0x06;
    			msg.Data[2]=0x01;
    			break;
    		case 6://��������(�󻬼�)
    			msg.Data[0]=0x06;
    			msg.Data[2]=0x0f;
    			break;
            case 7://OK(�󻬼�)
    			msg.Data[0]=0x07;
    			msg.Data[2]=status;
    			break;
            case 8://��һ��\���
    			msg.Data[0]=0x03;
    			msg.Data[2]=status;
    			break;
    		case 9://��һ��\�Ҽ�
    			msg.Data[0]=0x02;
    			msg.Data[2]=status;
    			break;    		
    		case 10://����+(�һ���)
    			msg.Data[0]=0x12;
    			msg.Data[2]=0x01;
    			break; 
            case 11://����-(�һ���)
    			msg.Data[0]=0x12;
    			msg.Data[2]=0x0f;
    			break;
            case 12://������
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


AudiA3_2014Tester::MMIKeyTask::MMIKeyTask(uint8_t prio) : ZTask(prio) {

}
void AudiA3_2014Tester::MMIKeyTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = 0,
			.ExtId = 0x17f8f073,
			.IDE = CAN_ID_EXT,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x03, 0x30, 0x00, 0x00, 0xaa, 0xaa, 0xaa, 0xaa}//00 00 00 11
	};
	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		switch(pCar->MMIKey) {
		case 0://û�а��κμ�
			msg.Data[2]=0;
			msg.Data[3]=0;
			break;
		case 1://TONE��
			msg.Data[1]=0x30;
			msg.Data[2]=0x0a;
			msg.Data[3]=0x01;
			break;
		case 2://CAR��
			msg.Data[1]=0x30;
			msg.Data[2]=0x0b;
			msg.Data[3]=0x01;
			break;
		case 3://RADIO��
			msg.Data[1]=0x30;
			msg.Data[2]=0x0d;
			msg.Data[3]=0x01;
			break;
		case 4://MEDIA��
			msg.Data[1]=0x30;
			msg.Data[2]=0x0e;
			msg.Data[3]=0x0f;
			break;
		case 5://MENU��
			msg.Data[1]=0x30;
			msg.Data[2]=0x1a;
			msg.Data[3]=0x01;
			break;
		case 6://BACK��
			msg.Data[1]=0x30;
			msg.Data[2]=0x0f;
			msg.Data[3]=0x01;
			break;
		case 7://��ť��ת
			msg.Data[1]=0x48;
			msg.Data[2]=0x02;
			msg.Data[3]=0xff;
			break;
		case 8://��ť��ת
			msg.Data[1]=0x48;
			msg.Data[2]=0x02;
			msg.Data[3]=0x01;
			break;
		case 9://��ť��ť����
			msg.Data[1]=0x30;
			msg.Data[2]=0x01;
			msg.Data[3]=0x01;
			break;
		default ://
			msg.Data[2]=0;
			msg.Data[3]=0;
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

AudiA3_2014Tester::BackLightTask::BackLightTask(uint8_t prio): ZTask(prio){

}

void AudiA3_2014Tester::BackLightTask::_doTask(void *p_arg)
{
	uint8_t kk=0;
	while(1) {
		_sendMsg1();
		if(kk<5) {
			kk++;
		} else {
                        kk=0;
			_sendMsg2();
		}
		OSTimeDlyHMSM(0,0,0,200);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

void AudiA3_2014Tester::BackLightTask::_sendMsg1(void) {
	CanTxMsg msg = {
			.StdId = 0,
			.ExtId = 0x1b000073,
			.IDE = CAN_ID_EXT,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x73, 0x00, 0x04, 0x82, 0x20, 0x08, 0x00, 0x00}
	};
	gpDrvCAN->transmit(CAN2, &msg);
}

void AudiA3_2014Tester::BackLightTask::_sendMsg2(void) {
	CanTxMsg msg = {
			.StdId = 0x474,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x49, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00}
	};
	gpDrvCAN->transmit(CAN2, &msg);
}

AudiA3_2014Tester::PageTrunKeyTask::PageTrunKeyTask(uint8_t prio) : ZTask(prio) {

}

void AudiA3_2014Tester::PageTrunKeyTask::_doTask(void * p_arg) {
	CanTxMsg msg =
	{
			.StdId = STEERING_WHEEL_SIDE_KEY_MSG_ID,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 4,
			.Data = {0x00, 0x00, 0x00, 0x40}//00 00 00 40
	};
	
	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		
		OSTimeDlyHMSM(0,0,0,100);

		switch(pCar->PageTrunKey) {
            case 0:
                msg.Data[0] = 0;
				msg.Data[2] = 0;
				break;
			case 5://Page up
				msg.Data[0] = 0x03;
				msg.Data[2] = 0x01;
				break;
			case 6://Page down
				msg.Data[0] = 0x02;
				msg.Data[2] = 0x01;
				break;
			default:
				msg.Data[0] = 0x0;
				msg.Data[2] = 0x0;
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