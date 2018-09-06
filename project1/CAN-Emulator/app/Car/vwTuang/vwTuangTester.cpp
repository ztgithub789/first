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
#include "vwTuangTester.h"
#include "vwTuang_common.h"

using namespace vwTuang;
static u8 BVstatusflag = 0;

u8 BVStatusData[4] = {0x0,0x0,0x0,0x0};   //关闭
u8 BVStatusData1[4] = {0x11,0x1,0x4,0x9}; //全景大环视2D

u8 BVStatusData2[4] = {0x11,0x2,0x2,0x7}; //前视带轨迹2D + 小车
u8 BVStatusData3[4] = {0x11,0x1,0x2,0x7}; //前视带轨迹2D 拉伸单视图
u8 BVStatusData4[4] = {0x11,0x2,0x2,0x1}; //前视潜望2D + 小车
u8 BVStatusData5[4] = {0x11,0x1,0x2,0x1}; //前视潜望2D 拉伸单视图
u8 BVStatusData6[4] = {0x11,0x2,0x2,0x3}; //前视俯视2D + 小车
u8 BVStatusData7[4] = {0x11,0x1,0x2,0x3}; //前视俯视2D 拉伸单视图

u8 BVStatusData8[4] = {0x11,0x2,0x1,0x2}; //后视带轨迹2D + 小车
u8 BVStatusData9[4] = {0x11,0x1,0x1,0x2}; //后视带轨迹2D 拉伸单视图
u8 BVStatusData10[4] = {0x11,0x2,0x1,0x1}; //后视侧方位 + 小车
u8 BVStatusData11[4] = {0x11,0x1,0x1,0x1}; //后视侧方位 拉伸单视图
u8 BVStatusData12[4] = {0x11,0x2,0x1,0x7}; //后视潜望2D + 小车
u8 BVStatusData13[4] = {0x11,0x1,0x1,0x7}; //后视潜望2D 拉伸单视图
u8 BVStatusData14[4] = {0x11,0x2,0x1,0x8}; //后视俯视2D + 小车
u8 BVStatusData15[4] = {0x11,0x1,0x1,0x8}; //后视俯视2D 拉伸单视图

u8 BVStatusData16[4] = {0x11,0x2,0x3,0x5}; //左视图2D + 小车
u8 BVStatusData17[4] = {0x11,0x1,0x3,0x5}; //左视图2D 拉伸单视图
u8 BVStatusData18[4] = {0x11,0x2,0x3,0x4}; //右视图2D + 小车 
u8 BVStatusData19[4] = {0x11,0x1,0x3,0x4}; //右视图2D 拉伸单视图
u8 BVStatusData20[4] = {0x11,0x2,0x3,0x6}; //左右双视图2D + 小车
u8 BVStatusData21[4] = {0x11,0x1,0x3,0x6}; //左右双视图2D 拉伸单视图

u8 BVStatusData22[4] = {0x01,0x1,0x0,0x0}; // 3D 俯视90°

u8 BVStatusData23[4] = {0x02,0x1,0x0,0x0}; // 3D 俯视45°1
u8 BVStatusData24[4] = {0x02,0x2,0x0,0x0}; // 3D 俯视45°2
u8 BVStatusData25[4] = {0x02,0x3,0x0,0x0}; // 3D 俯视45°3
u8 BVStatusData26[4] = {0x02,0x4,0x0,0x0}; // 3D 俯视45°4
u8 BVStatusData27[4] = {0x02,0x5,0x0,0x0}; // 3D 俯视45°5
u8 BVStatusData28[4] = {0x02,0x6,0x0,0x0}; // 3D 俯视45°6
u8 BVStatusData29[4] = {0x02,0x7,0x0,0x0}; // 3D 俯视45°7
u8 BVStatusData30[4] = {0x02,0x8,0x0,0x0}; // 3D 俯视45°8

u8 BVStatusData31[4] = {0x03,0x1,0x0,0x0}; // 3D 平视0° 1
u8 BVStatusData32[4] = {0x03,0x2,0x0,0x0}; // 3D 平视0° 2
u8 BVStatusData33[4] = {0x03,0x3,0x0,0x0}; // 3D 平视0° 3
u8 BVStatusData34[4] = {0x03,0x4,0x0,0x0}; // 3D 平视0° 4
u8 BVStatusData35[4] = {0x03,0x5,0x0,0x0}; // 3D 平视0° 5
u8 BVStatusData36[4] = {0x03,0x6,0x0,0x0}; // 3D 平视0° 6
u8 BVStatusData37[4] = {0x03,0x7,0x0,0x0}; // 3D 平视0° 7
u8 BVStatusData38[4] = {0x03,0x8,0x0,0x0}; // 3D 平视0° 8

static bool speedflag = false;
static bool bothlightflag = false;

vwTuangTester::vwTuangTester() {
// TODO Auto-generated constructor stub
	pCar = NULL;
}

vwTuangTester::~vwTuangTester() {
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

void vwTuangTester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void vwTuangTester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void vwTuangTester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void vwTuangTester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void vwTuangTester::togglePKey()
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
void vwTuangTester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x5a, 0x78, 0x78, 0x5a, 0x5a, 0xa0, 0xa0, 0x5a};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// 保证在循环周期内有一次0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}

uint8_t vwTuangTester::run(CarInfo *pcar)
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
    //pBackLightTask->start(pCar);

	pPageTrunKeyTask = new PageTrunKeyTask(PAGETRUN_SIM_TASK_PRIO);
	//pPageTrunKeyTask->start(pCar);

	return 0;
}

vwTuangTester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
// TODO Auto-generated constructor stub
}

void vwTuangTester::ShiftSimTask::_doTask(void *p_arg)
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

vwTuangTester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void vwTuangTester::SteeringWheelSimTask::_doTask(void *p_arg)
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
	static u16 speed1 = 0;
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

		
		if(speedflag){
			msg.Data[0] = 0x20;
			speed1 = 81;
			msg.Data[3] = (speed1*100)&0xff;
			msg.Data[4] = ((speed1*100)&0xff00) >> 8;
			gpDrvLED11->on();
		}else{
			speed1 = 0;
			msg.Data[3] = 0;
			msg.Data[4] = 0;
			gpDrvLED11->off();
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


vwTuangTester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void vwTuangTester::PKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = P_KEY_STATUS_MSG_ID,
			.ExtId = 0,
			.IDE = CAN_ID_STD,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x0, 0x60, 0x0, 0x0, 0x94, 0x56, 0x0, 0x0},
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,50);

		static bool oldv = false;
		bool newv = pCar->isPKeyPressed;
		if (oldv != newv){
			if (newv) {
				msg.Data[1] = 0x0;
				msg.Data[2] = 0x40;
                msg.Data[7] = 0x08;
			} else {
				msg.Data[1] = 0x60;
				msg.Data[2] = 0x0;
                msg.Data[7] = 0x0;
			}
			oldv = newv;
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

vwTuangTester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio) {

}

/*******************************************************************************
 * 简介   :	模拟雷达帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void vwTuangTester::RadarSimTask::_doTask(void *p_arg)
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


vwTuangTester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio) {

}
void vwTuangTester::TurnSimTask::_doTask(void *p_arg)
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
	static u8 bothlightmotion = 0;
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		switch(pCar->lightPos&0x7)
		{
    		case 0://none
    		case 4:
    			if(bothlightflag)
    			{
					if(bothlightmotion++<3){
    					msg.Data[2]=0x10;
    					msg.Data[3]=0x1e;
    				}else{
						bothlightflag = false;
						bothlightmotion = 0;
    				}
    			}else{
    				msg.Data[2]=0;
    				msg.Data[3]=0;
    			}
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

vwTuangTester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio) {

}

void vwTuangTester::TireKeySimTask::testShowVersion()
{
	CanTxMsg msg =
	{
			.StdId = 0,
			.ExtId = BV_STATUS_MSG_ID,
			.IDE = CAN_ID_EXT,
			.RTR = 0,
			.DLC = 6,
			.Data = {0x22, 0xda, 0x00, 0x00, 0x00, 0x00}//00 00 00 11
	};
	bool flag = false;
	for(u8 i=0;i<12;i++){
		if(flag){
			flag = false;
			msg.Data[2] = BVStatusData17[0];
			msg.Data[3] = BVStatusData17[1];
			msg.Data[4] = BVStatusData17[2];
			msg.Data[5] = BVStatusData17[3];
		}else{
			flag = true;
			msg.Data[2] = BVStatusData19[0];
			msg.Data[3] = BVStatusData19[1];
			msg.Data[4] = BVStatusData19[2];
			msg.Data[5] = BVStatusData19[3];
		}
		gpDrvCAN->transmit(CAN1, &msg);
		OSTimeDlyHMSM(0,0,0,100);
	}

	BVstatusflag = 19;
}


void vwTuangTester::TireKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = 0,
			.ExtId = BV_STATUS_MSG_ID,
			.IDE = CAN_ID_EXT,
			.RTR = 0,
			.DLC = 6,
			.Data = {0x22, 0xda, 0x00, 0x00, 0x00, 0x00}//00 00 00 11
	};
	CarInfo *pCar = (CarInfo*)p_arg;
    static uint8_t oldkey = 0, newkey = 0;

	u8 *pArray[39] = {BVStatusData,BVStatusData1,BVStatusData2,BVStatusData3,
	BVStatusData4,BVStatusData5,BVStatusData6,BVStatusData7,BVStatusData8,
	BVStatusData9,BVStatusData10,BVStatusData11,BVStatusData12,BVStatusData13,
	BVStatusData14,BVStatusData15,BVStatusData16,BVStatusData17,BVStatusData18,
	BVStatusData19,BVStatusData20,BVStatusData21,BVStatusData22,BVStatusData23,
	BVStatusData24,BVStatusData25,BVStatusData26,BVStatusData27,BVStatusData28,
	BVStatusData29,BVStatusData30,BVStatusData31,BVStatusData32,BVStatusData33,
	BVStatusData34,BVStatusData35,BVStatusData36,BVStatusData37,BVStatusData38};

	static bool Auto_Switch = false;
	u8 cnt = 0;
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

        newkey = pCar->TireKey;
        if(oldkey != newkey){  
        	oldkey = newkey;
			switch(newkey) {
				case 0:
	    		case 4://没有按任何键
	    			break;
	    		case 10:
	    			bothlightflag = true;
	    			break;
	    		case 12:
	    			if(speedflag)
	    				speedflag = false;
	    			else
	    				speedflag = true;
	    			break;
	    		case 2:
	    			Auto_Switch = !Auto_Switch;
	    			break;
	            case 5://滚动向上(左滑键)
	    			if(BVstatusflag==0){
						BVstatusflag = 38;
	    			}else{
						BVstatusflag--;
	    			}
	    			Auto_Switch = false;
	    			break;
	    		case 6://滚动向下(左滑键)
	    			if(BVstatusflag>=38){
	    				BVstatusflag = 0;
	    			}else{
						BVstatusflag++;
	    			}
	    			Auto_Switch = false;
	    			break;
	    		case 7:// OK键
					BVstatusflag = 0;
					Auto_Switch = false;
	    			break;
	    		case 1:
					this->testShowVersion();
	    			break;
	    		default ://
	    			//BVstatusflag = 0;
	    			//Auto_Switch = false;
	    			break;
			}
			msg.Data[2] = (pArray[BVstatusflag])[0];
			msg.Data[3] = (pArray[BVstatusflag])[1];
			msg.Data[4] = (pArray[BVstatusflag])[2];
			msg.Data[5] = (pArray[BVstatusflag])[3];
		}

		if(Auto_Switch)
		{
			cnt++;
			if(cnt >= 20){
				cnt = 0;
			
				if(BVstatusflag>=38){
					BVstatusflag = 0;
				}else{
					BVstatusflag++;
				}
				msg.Data[2] = (pArray[BVstatusflag])[0];
				msg.Data[3] = (pArray[BVstatusflag])[1];
				msg.Data[4] = (pArray[BVstatusflag])[2];
				msg.Data[5] = (pArray[BVstatusflag])[3];
			}
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


vwTuangTester::MMIKeyTask::MMIKeyTask(uint8_t prio) : ZTask(prio) {

}

void vwTuangTester::MMIKeyTask::_doTask(void *p_arg)
{
	static u8 cnt = 0;
	CanTxMsg msg1 =
	{
			.StdId = TESTER_MSG_ID1,
			.ExtId = 0,
			.IDE = CAN_ID_STD,
			.RTR = 0,
			.DLC = 6,
			.Data = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6},
	};
	CanTxMsg msg2 =
	{
			.StdId = TESTER_MSG_ID2,
			.ExtId = 0,
			.IDE = CAN_ID_STD,
			.RTR = 0,
			.DLC = 8,
			.Data = {0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x66, 0x99},
	};
	CanTxMsg msg3 =
	{
			.StdId = 0,
			.ExtId = TESTER_MSG_ID3,
			.IDE = CAN_ID_EXT,
			.RTR = 0,
			.DLC = 8,
			.Data = {0xcc, 0x22, 0xdd, 0xdb, 0xac, 0xbd, 0xca, 0xcc},
	};
	
	//CarInfo *pCar = (CarInfo*)p_arg;

	gpDrvTJA1041_2->enterPownMode();
	OSTimeDlyHMSM(0,0,0,200);
	gpDrvTJA1041_2->enterNormalMode();
	
	while(1) {
		OSTimeDlyHMSM(0,0,0,90);
		switch(cnt){
			default:
			case 0:
				cnt = 1;
				gpDrvCAN->transmit(CAN2, &msg1);
				//gpDrvLED7->getValue() ? gpDrvLED7->off() : gpDrvLED7->on();
				break;
			case 1:
				cnt = 2;
				gpDrvCAN->transmit(CAN2, &msg2);
				break;
			case 2:
				cnt = 0;
				gpDrvCAN->transmit(CAN2, &msg3);
				break;
		}
				
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

vwTuangTester::BackLightTask::BackLightTask(uint8_t prio): ZTask(prio){

}

void vwTuangTester::BackLightTask::_doTask(void *p_arg)
{
	
}

void vwTuangTester::BackLightTask::_sendMsg1(void) 
{
	
}

void vwTuangTester::BackLightTask::_sendMsg2(void) 
{
	
}

vwTuangTester::PageTrunKeyTask::PageTrunKeyTask(uint8_t prio) : ZTask(prio) {

}

void vwTuangTester::PageTrunKeyTask::_doTask(void * p_arg) 
{
	
}
