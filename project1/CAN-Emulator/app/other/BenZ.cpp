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

#include "BenZ.h"
#include "BenZ_common.h"
#include <UsartRadarDecoder/UsartRadarDecoder.h>
using namespace BenZNS;

BenZ::BenZ() {
	name = "BenZ";
	const static uint32_t idList[] = {
			SHIFT_POS_MSG_ID,
			STEERING_WHEER_ANGLE_MSG_ID,
//			P_KEY_STATUS_MSG_ID,
			RADAR_MSG_ID,
			TIRE_KEY_STATUS_MSD_ID,
			TURNIDICATOR_STATUS_MSD_ID,
			0 };
	pCanFilterIDList = idList;
	maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;
	steerAngle = 0;
	baudRate = 125;
	shiftPos = SHIFT_POS_R;	// 由于目前没有档位信息，让其一直处于倒档
	isParkToSideKeyPressed = false;
	isPKeyPressed = false;
	detectedRadar = false;
}

BenZ::~BenZ() {
}

int BenZ::init(void)
{
	return 0;
}

void BenZ::_doTask(void *p_arg)
{
	CanTMsg msg;

//	UsartRadarDecoder::getInstance()->bindCarAndID(this,RADAR_MSG_ID,500);
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

//			case P_KEY_STATUS_MSG_ID:
//				_updatePKeyStatus(&msg);
//				break;

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
void BenZ::_updateShiftPos(CanTMsg *pmsg)
{
	if(pmsg->msg.IDE != CAN_ID_STD) return;
	switch (pmsg->msg.Data[0] & 0xf) {
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
void BenZ::_updateSteerAngle(CanTMsg *pmsg)
{
	if(pmsg->msg.IDE != CAN_ID_STD) return;
	// 此处必须为带符号数
	const static int32_t thresh = 100;	// 相邻2帧的最大变化
	static int32_t aOld = 0;
	int32_t aNew;
	int32_t val = (int32_t)((pmsg->msg.Data[0] << 8) | pmsg->msg.Data[1]);
	aNew = (val - CAN_MID_BEGIN_VAL ) * MAX_ABS_STEER_ANGLE / MAX_ABS_CAN_STEER_ANGLE;
	if (abs(aNew - aOld) < thresh) {
		steerAngle = aNew;
	} else {
		zprintf("angle filted\n");
	}
	aOld = aNew;
}

void BenZ::_updatePKeyStatus(CanTMsg *pmsg)
{
	if ( (pmsg->msg.Data[0]==0) || (pmsg->msg.Data[1]==0) ) {
		isPKeyPressed = false;
	} else {
		isPKeyPressed = true;
	}
}

void BenZ::_updateRadar(CanTMsg *pmsg)
{
	if(pmsg->msg.IDE != CAN_ID_STD) return;
	detectedRadar = true;
		// 前侧探头数据
	int value=0;
	value = (int)10 - (pmsg->msg.Data[3]&0x0f);
	if(value < 8) radarVal[1] = (uint8_t)value;
	else radarVal[1] = 0;
	if((pmsg->msg.Data[4]&0x0f)) radarVal[0] = radarVal[1];
	else radarVal[0] =0;
		
	value = (int)10 - (pmsg->msg.Data[3]>>4) ;
	if(value < 8) radarVal[2] = (uint8_t)value;
	else radarVal[2] = 0;
	if((pmsg->msg.Data[4]>>4)) radarVal[3] = radarVal[2];
	else radarVal[3] =0;
		
	value = (int)10 - (pmsg->msg.Data[6]&0x0f);
	if(value < 8) radarVal[5] = (uint8_t)value;
	else radarVal[5] = 0;
	if((pmsg->msg.Data[7]&0x0f)) radarVal[4] = radarVal[5];
	else radarVal[4] =0;

	value = (int)10 - (pmsg->msg.Data[6]>>4);
	if(value < 8) radarVal[6] = (uint8_t)value;
	else radarVal[6] = 0;
	if((pmsg->msg.Data[7]>>4)) radarVal[7] = radarVal[6];
	else radarVal[7] =0;
}
void BenZ::updateRadar(CanTMsg *pmsg)
{
	_updateRadar(pmsg);
}

void BenZ::_updateParkToSideKeyStatus(CanTMsg *pmsg)
{
	if(pmsg->msg.IDE != CAN_ID_STD) return;
	static bool isVoiceKeyPressed_OLD = false;
	bool isVoiceKeyPressed_NEW =  ((pmsg->msg.Data[5]&0x01)!=0);
	if(isVoiceKeyPressed_OLD != isVoiceKeyPressed_NEW) {
		isVoiceKeyPressed_OLD = isVoiceKeyPressed_NEW;
		if(isVoiceKeyPressed_OLD) isParkToSideKeyPressed = !isParkToSideKeyPressed;
	}
}


void BenZ::_updateTurnIndicatorStatus(CanTMsg *pmsg)
{
	if(pmsg->msg.IDE != CAN_ID_STD) return;
	uint8_t turnIndicator = pmsg->msg.Data[0] & 0xe0;
	if (turnIndicator == 0x00) {
		turnIndicatorStatus = TURN_INDICATOR_NONE;
	} else if (turnIndicator == 0x40){
		turnIndicatorStatus = TURN_INDICATOR_LEFT;
	} else if (turnIndicator == 0x80){
		turnIndicatorStatus = TURN_INDICATOR_RIGHT;
	}
}