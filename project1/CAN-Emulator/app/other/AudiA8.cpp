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

#include "AudiA8.h"
#include "AudiA8_common.h"

using namespace AudiA8NS;

AudiA8::AudiA8() {
	name = "A8";
	const static uint32_t idList[] = {
			SHIFT_POS_MSG_ID,
			STEERING_WHEER_ANGLE_MSG_ID, 
			RADAR_MSG_ID, 
			P_KEY_STATUS_MSG_ID, 0};
	pCanFilterIDList = idList;
	maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;
	steerAngle = 0;
	baudRate = 500;
	shiftPos = SHIFT_POS_R;
}

AudiA8::~AudiA8() {
}

int AudiA8::init(void)
{
	return 0;
}

void AudiA8::_doTask(void *p_arg)
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

			case P_KEY_STATUS_MSG_ID:
				_updatePKeyStatus(&msg);
				break;
				
			case RADAR_MSG_ID:
				_updateRadarValue(&msg);
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
void AudiA8::_updateShiftPos(CanTMsg *pmsg)
{
	switch (pmsg->msg.Data[4] & 0xf) {
	case 0x6:
		shiftPos = SHIFT_POS_R;
		break;
	default:
		shiftPos = SHIFT_POS_OTHER;
		break;
	}
}

/*******************************************************************************
 * 简介   :	更新方向盘转角信息。
 * 			注：这里加入了滤波：实际使用，发现有轨迹有跳动现象。可能是某些无效帧当成有效帧处理了。
 * 				滤波方法：相邻2帧(间隔200ms)之间的角度差不能超过2度，否则该帧被忽略。
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void AudiA8::_updateSteerAngle(CanTMsg *pmsg)
{
	// 此处必须为带符号数
	const static int32_t thresh = 100;	// 相邻2帧的最大变化
	static int32_t aOld = 0;
	int32_t aNew;

	int32_t val = (int32_t)((pmsg->msg.Data[2] << 8) | pmsg->msg.Data[1]);
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

void AudiA8::_updatePKeyStatus(CanTMsg *pmsg)
{
	if (pmsg->msg.Data[2] == 0) {
		isPKeyPressed = false;
	} else {
		isPKeyPressed = true;
	}
}

void AudiA8::_updateRadarValue(CanTMsg *pmsg)
{
	detectedRadar = true;
	uint8_t dir = pmsg->msg.Data[1];
	if (dir == 0x93) {
		// 后侧探头数据
		radarVal[4] = calculateGrade(pmsg->msg.Data[2], 0x5a, 4);
		radarVal[5] = calculateGrade(pmsg->msg.Data[3], 0x9c, 11);
		radarVal[6] = calculateGrade(pmsg->msg.Data[4], 0x9c, 11);
		radarVal[7] = calculateGrade(pmsg->msg.Data[5], 0x5a, 4);
	} else if(dir == 0x92){
		// 前侧探头数据
		radarVal[0] = calculateGrade(pmsg->msg.Data[2], 0x5a, 6);
		radarVal[1] = calculateGrade(pmsg->msg.Data[3], 0x78, 8);
		radarVal[2] = calculateGrade(pmsg->msg.Data[4], 0x78, 8);
		radarVal[3] = calculateGrade(pmsg->msg.Data[5], 0x5a, 6);
	}
}

#ifdef TEST
CanTxMsg *AudiA8::getMsg(uint8_t type)
{

}

uint32_t *AudiA8::getMsgTimeInterval(uint8_t type)
{
	switch
}
#endif
