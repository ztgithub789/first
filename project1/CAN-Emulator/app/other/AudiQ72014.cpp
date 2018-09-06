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

#include "AudiQ72014.h"
#include "AudiQ72014_common.h"
#include <UsartRadarDecoder/UsartRadarDecoder.h>
using namespace AudiQ72014;

AudiQ7_2014::AudiQ7_2014() {
	name = "Q7";
	const static uint32_t idList[] = {
			SHIFT_POS_MSG_ID,
			STEERING_WHEER_ANGLE_MSG_ID,
			RADAR_MSG_ID,
			P_KEY_STATUS_MSG_ID,
			TIRE_KEY_STATUS_MSD_ID,
			TURNIDICATOR_STATUS_MSD_ID,
			RADAR_MSG_ID2,
			0 };
	pCanFilterIDList = idList;
	maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;
	steerAngle = 0;
	baudRate = 500;
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
    
	havePKey = true;

}

AudiQ7_2014::~AudiQ7_2014() {
}

int AudiQ7_2014::init(void)
{
	return 0;
}

void AudiQ7_2014::_doTask(void *p_arg)
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
			case RADAR_MSG_ID2:
				_updateRadar2(&msg);
				break;

			case TIRE_KEY_STATUS_MSD_ID:
				_updateParkToSideKeyStatus(&msg);
				_updateTireKeyStatus(&msg);
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
void AudiQ7_2014::_updateShiftPos(CanTMsg *pmsg)
{
	switch (pmsg->msg.Data[1] & 0xf0) {
	case 0x70:
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
void AudiQ7_2014::_updateSteerAngle(CanTMsg *pmsg)
{
	// 此处必须为带符号数
		const static int32_t thresh = 100;	// 相邻2帧的最大变化
		static int32_t aOld = 0;
		int32_t aNew;

		int32_t val = (int32_t)(((pmsg->msg.Data[1] << 8) & 0x3f00) | pmsg->msg.Data[0] );
		if (pmsg->msg.Data[1] & 0x80) {
			// 方向盘在r侧
			aNew = (val - CAN_STEER_BEGIN_VAL) * MAX_ABS_STEER_ANGLE / MAX_ABS_CAN_STEER_ANGLE;
		} else  {
			// 方向盘在l侧
			aNew = -(val - CAN_STEER_BEGIN_VAL) * MAX_ABS_STEER_ANGLE / MAX_ABS_CAN_STEER_ANGLE;
		}

		if (abs(aNew - aOld) < thresh) {
			steerAngle = aNew;
		} else {
			zprintf("angle filted\n");
		}
		aOld = aNew;
}

void AudiQ7_2014::_updatePKeyStatus(CanTMsg *pmsg)
{
	isPKeyDetected = true;
	if ( (pmsg->msg.Data[2]==0) || (pmsg->msg.Data[3]==0) ) {
		isPKeyPressed = false;
	} else {
		isPKeyPressed = true;
	}
}

void AudiQ7_2014::_updateRadar(CanTMsg *pmsg)
{
	detectedRadar = true;
	uint8_t dir = pmsg->msg.Data[1];
	if (dir==0x93) {
		// 后侧探头数据
		radarVal[4] = calculateGrade(pmsg->msg.Data[2], 0x5a, 6);
		radarVal[5] = calculateGrade(pmsg->msg.Data[3], 0xa0, 11);
		radarVal[6] = calculateGrade(pmsg->msg.Data[4], 0xa0, 11);
		radarVal[7] = calculateGrade(pmsg->msg.Data[5], 0x5a, 6);
	} else if (dir == 0x92) {
		// 前侧探头数据
		radarVal[0] = calculateGrade(pmsg->msg.Data[2], 0x5a, 6);
		radarVal[1] = calculateGrade(pmsg->msg.Data[3], 0x78, 8);
		radarVal[2] = calculateGrade(pmsg->msg.Data[4], 0x78, 8);
		radarVal[3] = calculateGrade(pmsg->msg.Data[5], 0x5a, 6);
	}
}

void AudiQ7_2014::_updateRadar2(CanTMsg *pmsg)
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
}

void AudiQ7_2014::updateRadar(CanTMsg *pmsg)
{
	_updateRadar(pmsg);
}

void AudiQ7_2014::_updateTireKeyStatus(CanTMsg *pmsg)
{
	if( (pmsg->msg.Data[3] &0x10) !=0) {
		if( (pmsg->msg.Data[3] &0x10) == 0x40) isNAVIKeyHolded = true;
		else {
			isNAVIKeyPressed = true;
			isNAVIKeyHolded = false;
		}
	} else {
		isNAVIKeyPressed = false;
		isNAVIKeyHolded = false;
	}
}

void AudiQ7_2014::_updateParkToSideKeyStatus(CanTMsg *pmsg)
{
	static bool isVoiceKeyPressed_OLD = false;
	bool isVoiceKeyPressed_NEW =  ((pmsg->msg.Data[1]&0x0f)!=0);
	if(isVoiceKeyPressed_OLD != isVoiceKeyPressed_NEW) {
		isVoiceKeyPressed_OLD = isVoiceKeyPressed_NEW;
		if(isVoiceKeyPressed_OLD) isParkToSideKeyPressed = !isParkToSideKeyPressed;
	}
}


void AudiQ7_2014::_updateTurnIndicatorStatus(CanTMsg *pmsg)
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
