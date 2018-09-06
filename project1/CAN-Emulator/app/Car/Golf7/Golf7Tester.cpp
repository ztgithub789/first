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
#include "Golf7Tester.h"
#include "Golf7_common.h"

using namespace Golf7;

Golf7Tester::Golf7Tester() {
	// TODO Auto-generated constructor stub
	pCar = NULL;
}

Golf7Tester::~Golf7Tester() {
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
              
}

void Golf7Tester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void Golf7Tester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void Golf7Tester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void Golf7Tester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void Golf7Tester::togglePKey()
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
void Golf7Tester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x5a, 0x78, 0x78, 0x5a, 0x5a, 0xa0, 0xa0, 0x5a};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// 保证在循环周期内有一次0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}

uint8_t Golf7Tester::run(CarInfo *pcar)
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

	return 0;
}

Golf7Tester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}

void Golf7Tester::ShiftSimTask::_doTask(void *p_arg)
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

Golf7Tester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void Golf7Tester::SteeringWheelSimTask::_doTask(void *p_arg)
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


Golf7Tester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void Golf7Tester::PKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = 0,
			.ExtId = P_KEY_STATUS_MSG_ID,
			.IDE = CAN_ID_EXT,
			.RTR = 0,
			.DLC = 4,
			.Data = {0x62, 0x95, 0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,50);

		static bool oldv = false;
		bool newv = pCar->isPKeyPressed;
		if (oldv != newv){
			if (newv) {
				msg.Data[2] = 0x11;
                msg.Data[2] = 0x03;
			} else {
				msg.Data[2] = 0x0;
                msg.Data[2] = 0x0;
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

Golf7Tester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio) {

}

/*******************************************************************************
 * 简介   :	模拟雷达帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void Golf7Tester::RadarSimTask::_doTask(void *p_arg)
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


Golf7Tester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio) {

}
void Golf7Tester::TurnSimTask::_doTask(void *p_arg)
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
	//int lightpos = 0;
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

Golf7Tester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio) {

}

void Golf7Tester::TireKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = 0x5bf,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 4,
			.Data = {0x00, 0x00, 0x00, 0x40}//00 00 00 11
	};
    
	CarInfo *pCar = (CarInfo*)p_arg;
    static uint8_t times = 0, status = 0, oldkey = 0, newkey = 0;
        
	while(1) {
        OSTimeDlyHMSM(0,0,0,100);

        newkey = pCar->TireKey;

        if(newkey != 0)
        {
            if((newkey != 8)&&(newkey != 9)&&(newkey != 10)&&(newkey != 11))
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
                                
		switch(pCar->TireKey) {
    		case 0://没有按任何键
    			msg.Data[0]=0;
    			msg.Data[2]=0;
    			break;
    		case 1://电话
    			msg.Data[0]=0x1c;
    			msg.Data[2]=status;
    			break;
    		//case 2://导航
    		//	msg.Data[0]=0x1b;
    		//	msg.Data[2]=status;
    		//	break;
    		case 3://语音
    			msg.Data[0]=0x19;
    			msg.Data[2]=status;
    			break;
            case 4://上一曲
    			msg.Data[0]=0x16;
    			msg.Data[2]=status;
    			break;    
            case 5://上
    			msg.Data[0]=0x04;
    			msg.Data[2]=status;
    			break;
    		case 6://下
    			msg.Data[0]=0x05;
    			msg.Data[2]=status;
    			break;
            case 7://OK
    			msg.Data[0]=0x07;
    			msg.Data[2]=status;
    			break;
            case 8://翻页-左
    			msg.Data[0]=0x03;
    			msg.Data[2]=0x01;
    			break;
    		case 9://翻页-右
    			msg.Data[0]=0x02;
    			msg.Data[2]=0x0f;
    			break;
            case 10:// 音量+
    			msg.Data[0]=0x10;
    			msg.Data[2]=0x01;
    			break;
    		case 11:// 音量-
    			msg.Data[0]=0x11;
    			msg.Data[2]=0x01;
    			break;
    		case 12://下一曲
    			msg.Data[0]=0x15;
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


Golf7Tester::MMIKeyTask::MMIKeyTask(uint8_t prio) : ZTask(prio) {

}
void Golf7Tester::MMIKeyTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = 0,
			.ExtId = 0x17f8f173,
			.IDE = CAN_ID_EXT,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x03, 0x30, 0x00, 0x00, 0xaa, 0xaa, 0xaa, 0xaa}//00 00 00 11
	};
    
	CarInfo *pCar = (CarInfo*)p_arg;
    
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		switch(pCar->MMIKey) {
    		case 0://没有按任何键
    			msg.Data[2]=0;
    			msg.Data[3]=0;
    			break;
    		case 1://NAV Q
    			msg.Data[1]=0x30;
    			msg.Data[2]=0x05;
    			msg.Data[3]=0x01;
    			break;
    		case 2://TRAFFIC W
    			msg.Data[1]=0x30;
    			msg.Data[2]=0x06;
    			msg.Data[3]=0x01;
    			break;
    		case 3://CAR E
    			msg.Data[1]=0x30;
    			msg.Data[2]=0x07;
    			msg.Data[3]=0x01;
    			break;
    		case 4://MENU 
    			msg.Data[1]=0x30;
    			msg.Data[2]=0x1a;
    			msg.Data[3]=0x0f;
    			break;
    		case 5://RADIO
    			msg.Data[1]=0x30;
    			msg.Data[2]=0x18;
    			msg.Data[3]=0x01;
    			break;
    		case 6://MEDIA
    			msg.Data[1]=0x30;
    			msg.Data[2]=0x02;
    			msg.Data[3]=0x01;
    			break;
    		case 7://PHONE
    			msg.Data[1]=0x30;
    			msg.Data[2]=0x04;
    			msg.Data[3]=0x01;
    			break;
    		case 8://VOICE
    			msg.Data[1]=0x30;
    			msg.Data[2]=0x1d;
    			msg.Data[3]=0x01;
    			break;
    		case 9://<音量旋钮-左>-音量小
    			msg.Data[1]=0x48;
    			msg.Data[2]=0x01;
    			msg.Data[3]=0xff;
    			break;
    		case 10://<音量旋钮-右>-音量大
    			msg.Data[1]=0x48;
    			msg.Data[2]=0x01;
    			msg.Data[3]=0x01;
    			break; 
    		case 11://音量旋钮-确认：关机
    			msg.Data[1]=0x30;
    			msg.Data[2]=0x38;
    			msg.Data[3]=0xff;
    			break;
    		case 12://<上一曲>:左旋
    			msg.Data[1]=0x48;
    			msg.Data[2]=0x02;
    			msg.Data[3]=0xff;
    			break; 
    		case 13://<下一曲>:右旋
    			msg.Data[1]=0x48;
    			msg.Data[2]=0x02;
    			msg.Data[3]=0x01;
    			break; 
    		case 14://确认
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

Golf7Tester::BackLightTask::BackLightTask(uint8_t prio): ZTask(prio){

}

void Golf7Tester::BackLightTask::_doTask(void *p_arg)
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

void Golf7Tester::BackLightTask::_sendMsg1(void) {
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

void Golf7Tester::BackLightTask::_sendMsg2(void) {
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
