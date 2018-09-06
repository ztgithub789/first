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

#include "AudiA4.h"
#include <UsartRadarDecoder/UsartRadarDecoder.h>

using namespace AudiA42012;

AudiA4::AudiA4(){
	name = "Audi A4";
	const static uint32_t idList[] = {
			SHIFT_POS_MSG_ID,
			STEERING_WHEER_ANGLE_MSG_ID,
			RADAR_MSG_ID,
			TIRE_KEY_MSG_ID,
			AIRCONDITION_MSG_ID,
			ACC_STATUS_ID,
			P_KEY_STATUS_MSG_ID,
			TURNIDICATOR_STATUS_MSD_ID,
			0};

	pCanFilterIDList = idList;
	maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;
	steerAngle = 0;
	baudRate = 100;
	shiftPos = SHIFT_POS_OTHER;	// 由于目前没有档位信息，让其一直处于倒档

	rollingStatus = ROLLING_NONE;
	isAirConditionOn = false;
	isNAVIKeyPressed = false;
	isVoiceKeyPressed = false;
	isNAVIKeyHolded = false;
	isACCOn = true;
	isParkToSideKeyPressed = false;
	turnIndicatorStatus = TURN_INDICATOR_NONE;
	isPKeyDetected = false;
	isPKeyPressed = false;
}

AudiA4::~AudiA4() {
}

int AudiA4::init(void)
{
	return 0;
}

void AudiA4::_doTask(void *p_arg)
{
	UsartRadarDecoder::getInstance()->bindCarAndID(this,RADAR_MSG_ID);
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
			case TIRE_KEY_MSG_ID:
				_updateTireKeyStatus(&msg);
				_updateParkToSideKeyStatus(&msg);
				break;
			case AIRCONDITION_MSG_ID:
				_updateAirConditionKeyStatus(&msg);
				break;
			case ACC_STATUS_ID:
				_updateACCStatus(&msg);
			case P_KEY_STATUS_MSG_ID:
				_updatePKeyStatus(&msg);
				break;
			case TURNIDICATOR_STATUS_MSD_ID:
				_updateTurnIndicatorStatus(&msg);
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
void AudiA4::_updateShiftPos(CanTMsg *pmsg)
{
	char v = pmsg->msg.Data[4] & 0xf;
	switch (v) {
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
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void AudiA4::_updateSteerAngle(CanTMsg *pmsg)
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

void AudiA4::_updateRadar(CanTMsg *pmsg)
{
	uint8_t dir = pmsg->msg.Data[1];
	if (dir == 0x93) {
		// 后侧探头数据
		radarVal[4] = calculateGrade(pmsg->msg.Data[2], 0x3c, 4);
		radarVal[5] = calculateGrade(pmsg->msg.Data[3], 0x99, 11);
		radarVal[6] = calculateGrade(pmsg->msg.Data[4], 0x99, 11);
		radarVal[7] = calculateGrade(pmsg->msg.Data[5], 0x3c, 4);
	} else if(dir == 0x92){
		// 前侧探头数据
		radarVal[0] = calculateGrade(pmsg->msg.Data[2], 0x54, 6);
		radarVal[1] = calculateGrade(pmsg->msg.Data[3], 0x81, 8);
		radarVal[2] = calculateGrade(pmsg->msg.Data[4], 0x81, 8);
		radarVal[3] = calculateGrade(pmsg->msg.Data[5], 0x54, 6);
	}
	detectedRadar = true;
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

void AudiA4::updateRadar(CanTMsg *pmsg)
{
	_updateRadar(pmsg);
}

void AudiA4::_updateTireKeyStatus(CanTMsg *pmsg)
{
	if(pmsg->msg.Data[1] & 0x0f) {
		isVoiceKeyPressed = true;
	} else {
		isVoiceKeyPressed = false;
	}

	if(pmsg->msg.Data[3] == 0x10) {
		isNAVIKeyPressed = true;
	} else {
		isNAVIKeyPressed = false;
	}

	if(pmsg->msg.Data[3]== 0x40) {
		isNAVIKeyHolded = true;
	} else {
		isNAVIKeyHolded = false;
	}

	if(pmsg->msg.Data[2]) {
		if (pmsg->msg.Data[2]&0x0c) {
			rollingStatus = ROLLING_DOWN;
		} else if(pmsg->msg.Data[2]&0x03){
			rollingStatus = ROLLING_UP;
		}
	} else if(pmsg->msg.Data[0] & 0xf0) {
		rollingStatus = ROLLING_PUSH;
	} else {
		rollingStatus = ROLLING_NONE;
	}
}

void  AudiA4::_updateAirConditionKeyStatus(CanTMsg *pmsg)
{
	if(pmsg->msg.Data[1]==0x73) {
		if(pmsg->msg.Data[2]==0x01 && pmsg->msg.Data[3]== 0x01 && pmsg->msg.Data[4]== 0x01) {
			isAirConditionOn = true;
		} else if(pmsg->msg.Data[2]==0x00 && pmsg->msg.Data[3]== 0x00 && pmsg->msg.Data[4]== 0x00) {
			isAirConditionOn = false;
		}
	} else if(pmsg->msg.Data[1]==0x57) {
		if(pmsg->msg.Data[2]==0xff && pmsg->msg.Data[3]== 0x01 ) {
			isAirConditionOn = true;
		} else if(pmsg->msg.Data[2]==0xff && pmsg->msg.Data[3]== 0x00) {
			isAirConditionOn = false;
		}
	}
}

void AudiA4::_updateACCStatus(CanTMsg *pmsg)
{
	if(pmsg->msg.Data[2] == 0x00) {
		isACCOn = false;
	} else {
		isACCOn = true;
	}
}

void AudiA4::_updateTurnIndicatorStatus(CanTMsg *pmsg)
{
	uint8_t turnIndicator = pmsg->msg.Data[1] & 0x0f;
	if (turnIndicator == 0x00) {
		turnIndicatorStatus = TURN_INDICATOR_NONE;
	} else if (turnIndicator == 0x01){
		turnIndicatorStatus = TURN_INDICATOR_LEFT;
	} else if (turnIndicator == 0x02){
		turnIndicatorStatus = TURN_INDICATOR_RIGHT;
	}
}

void AudiA4::_updatePKeyStatus(CanTMsg *pmsg)
{
	isPKeyDetected = true;
	if (pmsg->msg.Data[2] == 0) {
		isPKeyPressed = false;
	} else {
		isPKeyPressed = true;
	}
}

void AudiA4::_updateParkToSideKeyStatus(CanTMsg *pmsg)
{
	static bool isVoiceKeyPressed_OLD = false;
	bool isVoiceKeyPressed_NEW =  isVoiceKeyPressed;
	if(isVoiceKeyPressed_OLD != isVoiceKeyPressed_NEW) {
		isVoiceKeyPressed_OLD = isVoiceKeyPressed_NEW;
		if(isVoiceKeyPressed_OLD) isParkToSideKeyPressed = !isParkToSideKeyPressed;
	}
}
