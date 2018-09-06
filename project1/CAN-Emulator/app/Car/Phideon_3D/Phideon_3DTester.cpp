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
#include "Phideon_3DTester.h"
#include "Phideon_3D_common.h"

using namespace Phideon_3D;

Phideon_3DTester::Phideon_3DTester() {
	// TODO Auto-generated constructor stub
	pCar = NULL;
}

Phideon_3DTester::~Phideon_3DTester() {
	// TODO Auto-generated destructor stub
	delete pshiftsimTask;
	pshiftsimTask = NULL;

}

void Phideon_3DTester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void Phideon_3DTester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void Phideon_3DTester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void Phideon_3DTester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void Phideon_3DTester::togglePKey()
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
void Phideon_3DTester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x5a, 0x74, 0x74, 0x5a, 0x3c, 0xa0, 0xa0, 0x3c};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// 保证在循环周期内有一次0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}

uint8_t Phideon_3DTester::run(CarInfo *pcar)
{
	pCar = pcar;
	pCar->maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;
	//const uint32_t filterList[] = {0x17330b00, 0x0};
        //gpDrvCAN->addFilters(filterList);
	pshiftsimTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
	pshiftsimTask->start(pCar);



	return 0;
}

Phideon_3DTester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}

void Phideon_3DTester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg[22] ={
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 3,
		  .Data = {0x32, 0xcf, 0x00} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 3,
		  .Data = {0x32, 0xd0, 0x00} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 4,
		  .Data = {0x32, 0xd1, 0x02, 0x01} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = 0,
		  .RTR = 0,
		  .DLC = 4,
		  .Data = {0x32, 0xd2, 0x02, 0x07} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 4,
		  .Data = {0x32, 0xd3, 0x02, 0x04} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 4,
		  .Data = {0x32, 0xd4, 0x02, 0x09} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 3,
		  .Data = {0x32, 0xd5, 0x04} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 3,
		  .Data = {0x32, 0xd6, 0x08} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 4,
		  .Data = {0x32, 0xd7, 0x35, 0x01} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 4,
		  .Data = {0x32, 0xd8, 0x33, 0x01} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 4,
		  .Data = {0x32, 0xd9, 0x36, 0x01} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 6,
		  .Data = {0x32, 0xda, 0x00, 0x00, 0x00, 0x00} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 5,
		  .Data = {0x32, 0xde, 0x03, 0x08, 0x00} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 6,
		  .Data = {0x32, 0xdf, 0x01, 0x01, 0x00, 0x01} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 4,
		  .Data = {0x32, 0xe2, 0x00, 0x01} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 8,
		  .Data = {0x32, 0xe5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 8,
		  .Data = {0x32, 0xc2, 0x03, 0x01, 0x0b, 0x00, 0x05, 0x05} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 8,
		  .Data = {0x80, 0x08, 0x32, 0xc3, 0x38, 0x07, 0xff, 0xe3} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 5,
		  .Data = {0xc0, 0x24, 0x00, 0x00, 0x00} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 3,
		  .Data = {0x32, 0xc4, 0x0a} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 3,
		  .Data = {0x32, 0xcd, 0x10} 
		},
		{
		  .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 8,
		  .Data = {0x32, 0xce, 0x3f, 0x1f, 0x0f, 0x07, 0x0e, 0x14} 
		},
	};
	//0x80默认为P档

	CanTxMsg msg1[4]={
		{ .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 6,
		  .Data = {0x42, 0xda, 0x10, 0x00, 0x00, 0x00} 
		},
		{ .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 6,
		  .Data = {0x42, 0xda, 0x10, 0x00, 0x00, 0x00} 
		},
		{ .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 6,
		  .Data = {0x42, 0xda, 0xf1, 0x01, 0x04, 0x09} 
		},
		{ .StdId = 0,
		  .ExtId = 0x17330b10,
		  .IDE = CAN_ID_EXT,
		  .RTR = 0,
		  .DLC = 6,
		  .Data = {0x42, 0xda, 0x11, 0x01, 0x04, 0x09} 
		},
	};
	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		//OSTimeDlyHMSM(0,0,1,0);
		
		uint8_t shiftPos = pCar->shiftPos;
		#if 0
		switch(shiftPos) {
		case CarInfo::SHIFT_POS_P:
			//msg.Data[1] = 0x80;
			gpDrvCAN->transmit(CAN1, &msg1[0]);
			gpDrvCAN->transmit(CAN1, &msg1[1]);
			break;
		case CarInfo::SHIFT_POS_R:
			gpDrvCAN->transmit(CAN1, &msg1[2]);
			gpDrvCAN->transmit(CAN1, &msg1[3]);
			break;
		default:
			break;
		}
		#endif
		
		
		for(int i=0;i<22;i++){
			gpDrvCAN->transmit(CAN1, &msg[i]);
			#if 0
			if(shiftPos == CarInfo::SHIFT_POS_P){
				gpDrvCAN->transmit(CAN1, &msg1[0]);
				gpDrvCAN->transmit(CAN1, &msg1[1]);
			}else if(shiftPos == CarInfo::SHIFT_POS_R){
				gpDrvCAN->transmit(CAN1, &msg1[2]);
				gpDrvCAN->transmit(CAN1, &msg1[3]);
			}
			#endif
			
			OSTimeDlyHMSM(0,0,1,0);
		}

		//gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}




