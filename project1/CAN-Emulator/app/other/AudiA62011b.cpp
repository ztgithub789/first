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


#include "AudiA62011b.h"
#include "AudiA62011b_common.h"
#include "AudiA62011bComKeepingTasks.h"

using namespace AudiA62011_b;

AudiA6_2011_b::AudiA6_2011_b() {
	name = "Audi A6L 2011_b";
	const static uint32_t idList[] = {0x3c3, 0x355, 0x6d9, 0x545, 0x200, 0x4fc, 0x6a5, 0};
	pCanFilterIDList = idList;
	maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;
	steerAngle = 0;
	baudRate = 100;
	shiftPos = SHIFT_POS_OTHER;

	isPKeyDetected = false;
	isPKeyPressed = false;
	isParkToSideKeyPressed = false;
}

AudiA6_2011_b::~AudiA6_2011_b() {
}

OS_EVENT               *App_RearViewCameraMbox;   /* CAN to Uart */

int AudiA6_2011_b::init(void)
{
//	_pSlot = CanHub::getInstance()->getSlot();
//	uint8_t ret;
//
//	if (_pSlot == 0) {
//		zprintf("AudiA6_2011_b getSlot failure.\n");
//		return -1;
//	}
//
//	ret =  _pSlot->addId(SHIFT_POS_MSG_ID);
//	ret |= _pSlot->addId(STEERING_WHEER_ANGLE_MSG_ID);
//	ret |= _pSlot->addId(RADAR_MSG_ID);
//
//	if (ret != 0) {
//		zprintf("AudiA6_2011_b ID failure.\n");
//		return -1;
//	}

#if (NEED_COM_KEEPING_TASK == 1)

	App_RearViewCameraMbox = OSMboxCreate((void *)0);
	if (App_RearViewCameraMbox == 0)
#ifdef ZDEBUG
		zprintf("Audi A6 2011 mbox create failure.\n");
#endif
	static UARTResolutionTask uartResolutionTask(AUDI_A6_2011_USART_RESOLUTION_TASK_PRIO);
	uartResolutionTask.start(0);

	static CanProtocol1Task canProtocol1Task(AUDI_A6_2011_CAN_PROTOCOL1_TASK_PRIO);
	canProtocol1Task.start(0);

	static RearViewCameraTask rearViewCameraTask(AUDI_A6_2011_REAR_VIEW_CAMERA_TASK_PRIO);
	rearViewCameraTask.start(0);
#endif

	return 0;
}

void AudiA6_2011_b::_doTask(void *p_arg)
{
	CanTMsg msg;

	while(1) {
//		uint8_t err = _pSlot->read(&msg);
		uint8_t err = gpDrvCAN->getData(&msg,0);
		if (err == OS_ERR_NONE) {
			switch(msg.msg.StdId) {
			case SHIFT_POS_MSG_ID:
				_updateShiftPos(&msg);
				break;
			case STEERING_WHEER_ANGLE_MSG_ID:
				_updateSteerAngle(&msg);
				break;
			case RADAR_MSG_ID:
				_updateRadar(&msg);
				break;
			default:
				break;
			}
		}
	}
}

/*******************************************************************************
 * 简介   :	更新档位信息（暂时只关心R档）
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void AudiA6_2011_b::_updateShiftPos(CanTMsg *pmsg)
{
	switch (pmsg->msg.Data[4] & 0xf) {
	case R_SHIFT_VAL:
		shiftPos = SHIFT_POS_R;
		break;
	default:
		shiftPos = SHIFT_POS_OTHER;
		break;
	}
}

/*******************************************************************************
 * 简介   :	更新方向盘转角信息
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void AudiA6_2011_b::_updateSteerAngle(CanTMsg *pmsg)
{
	// 此处必须为带符号数
	const static int32_t thresh = 50;	// 相邻2帧的最大变化
	static int32_t aOld = 0;
	int32_t aNew;

	int32_t val = (int32_t)((pmsg->msg.Data[1] << 8) | pmsg->msg.Data[0]);
	if (CAN_STEER_LEFT_BEGIN_VAL <= val && val <= CAN_STEER_LEFT_BEGIN_VAL + MAX_ABS_CAN_STEER_ANGLE) {
		// 方向盘在左侧
		aNew = -(val - CAN_STEER_LEFT_BEGIN_VAL) * MAX_ABS_STEER_ANGLE / MAX_ABS_CAN_STEER_ANGLE;
	} else if (CAN_STEER_RIGHT_BEGIN_VAL <= val && val <= CAN_STEER_RIGHT_BEGIN_VAL + MAX_ABS_CAN_STEER_ANGLE) {
		// 方向盘在右侧
		aNew = (val - CAN_STEER_RIGHT_BEGIN_VAL) * MAX_ABS_STEER_ANGLE / MAX_ABS_CAN_STEER_ANGLE;
	} else {
		// 忽略其他值
		return;
	}

	if (abs(aNew - aOld) < thresh) {
		steerAngle = aNew;
	} else {
		zprintf("angle filted\n");
	}
	aOld = aNew;
}

void AudiA6_2011_b::_updateRadar(CanTMsg *pmsg)
{
	detectedRadar = true;
	uint8_t dir = pmsg->msg.Data[1];
	if (dir ==0x87) {
		// 后侧探头数据
		radarVal[4] = calculateGrade(pmsg->msg.Data[3], 0x3c, 4);
		radarVal[5] = calculateGrade(pmsg->msg.Data[4], 0xa0, 11);
		radarVal[6] = calculateGrade(pmsg->msg.Data[5], 0xa0, 11);
		radarVal[7] = calculateGrade(pmsg->msg.Data[6], 0x3c, 4);
	} else if(dir ==0x86) {
		// 前侧探头数据
		radarVal[0] = calculateGrade(pmsg->msg.Data[3], 0x5a, 6);
		radarVal[1] = calculateGrade(pmsg->msg.Data[4], 0x74, 8);
		radarVal[2] = calculateGrade(pmsg->msg.Data[5], 0x74, 8);
		radarVal[3] = calculateGrade(pmsg->msg.Data[6], 0x5a, 6);
	}
#ifdef ZDEBUG
#ifdef DUMP_RADARVAL
	zprintf("radar val: ");
	for (int i = 0; i<8; i++) {
		zprintf("%02x ", radarVal[i]);
	}
	zprintf("\n");
#endif
#endif
}
