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

#include "AudiQ3_2012_imported.h"
#include "AudiQ3_2012_imported_common.h"

using namespace AudiQ32012imported;

AudiQ3_2012_imported::AudiQ3_2012_imported() {
	name = "Audi Q3";
	const static uint32_t idList[] = {SHIFT_POS_MSG_ID,
			STEERING_WHEER_ANGLE_MSG_ID,
			//P_KEY_STATUS_MSG_ID,
			RADAR_MSG_ID,0 };
	pCanFilterIDList = idList;
	maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;
	steerAngle = 0;
	baudRate = 500;
	shiftPos = SHIFT_POS_R;	// 由于目前没有档位信息，让其一直处于倒档

	rollingStatus = ROLLING_NONE;
	isAirConditionOn = false;
	isNAVIKeyPressed = false;
	isVoiceKeyPressed = false;
	isNAVIKeyHolded = false;
	isACCOn = true;
	isPKeyDetected = false;
	isParkToSideKeyPressed = false;
}

AudiQ3_2012_imported::~AudiQ3_2012_imported() {
}

int AudiQ3_2012_imported::init(void)
{
    return 0;
}

void AudiQ3_2012_imported::_doTask(void *p_arg)
{
	CanTMsg msg;

	while(1) {
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
void AudiQ3_2012_imported::_updateShiftPos(CanTMsg *pmsg)
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
void AudiQ3_2012_imported::_updateSteerAngle(CanTMsg *pmsg)
{
	// 此处必须为带符号数
		const static int32_t thresh = 100;	// 相邻2帧的最大变化
		static int32_t aOld = 0;
		int32_t aNew;

		int32_t val = (int32_t)((pmsg->msg.Data[3] << 8) | pmsg->msg.Data[2]);
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

void AudiQ3_2012_imported::_updatePKeyStatus(CanTMsg *pmsg)
{
	if (pmsg->msg.Data[2] == 0) {
		isPKeyPressed = false;
	} else {
		isPKeyPressed = true;
	}
}
///*******************************************************************************
// * 简介   :	计算雷达的级数
// * 参数   :	v		探头的数值
// * 			maxV	探头能检测到的的最大数值
// * 			gradeNr	探头级数，不包括不显示的情况。例如，奥迪A4左后侧的探头最多有4级，则gradeNr为4。
// * 返回值 :	当前探头的级数。符合Radar类的规范。0表示无显示，1表示显示最近一级，级数越大，弧线越远。
// * 描述   :
// *******************************************************************************/
//static int calculateGrade(unsigned char v, unsigned char maxV, unsigned int gradeNr)
//{
//	if (v == 0xff)
//		return 0;
//
//	unsigned int g = v * gradeNr / maxV + 1;
//	if (g > gradeNr)
//		g = gradeNr;
//
//	//printf("v: 0x%x, maxv: 0x%x, gradeNr: %d, v*gradeNr: %x, g:%d\n", v, maxV, gradeNr, v * gradeNr, g);
//
//	return g;
//}

void AudiQ3_2012_imported::_updateRadar(CanTMsg *pmsg)
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
		zprintf("%02x ", radarVal[i]);
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
