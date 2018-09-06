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

#include "AudiQ72012.h"
#include "AudiQ72012_common.h"
#include <UsartRadarDecoder/UsartRadarDecoder.h>
using namespace AudiQ72012;

AudiQ7_2012::AudiQ7_2012() {
	name = "Q7";
	const static uint32_t idList[] = {0x3c3, 0x6da, 0x6f4, 0x355,P_KEY_STATUS_MSG_ID,TIRE_KEY_STATUS_MSD_ID,TURNIDICATOR_STATUS_MSD_ID,0 };
	pCanFilterIDList = idList;
	maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;
	steerAngle = 0;
	baudRate = 100;
	shiftPos = SHIFT_POS_R;	// 由于目前没有档位信息，让其一直处于倒档
	isParkToSideKeyPressed = false;
	isPKeyPressed = false;
	isPKeyDetected = false;
	turnIndicatorStatus = TURN_INDICATOR_NONE;
}

AudiQ7_2012::~AudiQ7_2012() {
}

int AudiQ7_2012::init(void)
{
	return 0;
}

void AudiQ7_2012::_doTask(void *p_arg)
{
	CanTMsg msg;

	UsartRadarDecoder::getInstance()->bindCarAndID(this,RADAR_MSG_ID,500);
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
				_updateRadar(&msg);
				break;
				
			case TIRE_KEY_STATUS_MSD_ID:
				_updateParkToSideKeyStatus(&msg);
				break;

			case TURNIDICATOR_STATUS_MSD_ID:
				_updateTurnIndicatorStatus(&msg);
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
void AudiQ7_2012::_updateShiftPos(CanTMsg *pmsg)
{
	switch (pmsg->msg.Data[4] & 0xf) {
	case 0x7:
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
void AudiQ7_2012::_updateSteerAngle(CanTMsg *pmsg)
{
	// 此处必须为带符号数
		const static int32_t thresh = 100;	// 相邻2帧的最大变化
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

void AudiQ7_2012::_updatePKeyStatus(CanTMsg *pmsg)
{
	isPKeyDetected = true;
	if ( (pmsg->msg.Data[0]==0) || (pmsg->msg.Data[1]==0) ) {
		isPKeyPressed = false;
	} else {
		isPKeyPressed = true;
	}
}

void AudiQ7_2012::_updateRadar(CanTMsg *pmsg)
{
	detectedRadar = true;
	uint8_t dir = pmsg->msg.Data[1];
	if (dir ==0x93) {
		// 后侧探头数据
		radarVal[4] = calculateGrade(pmsg->msg.Data[2], 0x5a, 6);
		radarVal[5] = calculateGrade(pmsg->msg.Data[3], 0xa0, 11);
		radarVal[6] = calculateGrade(pmsg->msg.Data[4], 0xa0, 11);
		radarVal[7] = calculateGrade(pmsg->msg.Data[5], 0x5a, 6);
	} else if(dir ==0x92) {
		// 前侧探头数据
		radarVal[0] = calculateGrade(pmsg->msg.Data[2], 0x5a, 6);
		radarVal[1] = calculateGrade(pmsg->msg.Data[3], 0x78, 8);
		radarVal[2] = calculateGrade(pmsg->msg.Data[4], 0x78, 8);
		radarVal[3] = calculateGrade(pmsg->msg.Data[5], 0x5a, 6);
	}
#ifdef ZDEBUG
#ifdef DUMP_RADARVAL
	zprintf("radar val: ");
	for (int i = 0; i<8; i++) {
		zprintf("%02x ", _info.radarVal[i]);
	}
	zprintf("\n");
#endif
#endif
}
#ifdef TEST
CanTxMsg *AudiA6_2012::getMsg(uint8_t type)
{

}

uint32_t *AudiA6_2012::getMsgTimeInterval(uint8_t type)
{
	switch
}
#endif
void AudiQ7_2012::updateRadar(CanTMsg *pmsg)
{
	_updateRadar(pmsg);
}


void AudiQ7_2012::_updateParkToSideKeyStatus(CanTMsg *pmsg)
{
	static bool isVoiceKeyPressed_OLD = false;
	bool isVoiceKeyPressed_NEW =  ((pmsg->msg.Data[1]&0x0f)!=0);
	if(isVoiceKeyPressed_OLD != isVoiceKeyPressed_NEW) {
		isVoiceKeyPressed_OLD = isVoiceKeyPressed_NEW;
		if(isVoiceKeyPressed_OLD) isParkToSideKeyPressed = !isParkToSideKeyPressed;
	}
}


void AudiQ7_2012::_updateTurnIndicatorStatus(CanTMsg *pmsg)
{
	uint8_t turnIndicator = pmsg->msg.Data[0] & 0x0f;
	if (turnIndicator == 0x00) {
		turnIndicatorStatus = TURN_INDICATOR_NONE;
	} else if (turnIndicator == 0x01){
		turnIndicatorStatus = TURN_INDICATOR_LEFT;
	} else if (turnIndicator == 0x02){
		turnIndicatorStatus = TURN_INDICATOR_RIGHT;
	}
}
