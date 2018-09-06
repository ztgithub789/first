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

#include "Phaeton.h"
#include "Phaeton_common.h"
using namespace PhaetonNS;

Phaeton::Phaeton(bool needScreenControllerTask) {
	name = "Phaeton";
	const static uint32_t idList[] = {
			STEERING_WHEER_ANGLE_MSG_ID,
			SHIFT_POS_MSG_ID,
			};
	pCanFilterIDList = idList;
	maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;
	steerAngle = 0;
	baudRate = 100;
	shiftPos = SHIFT_POS_R;	// 由于目前没有档位信息，让其一直处于倒档
	isACCOn = false;
}

Phaeton::~Phaeton() {
}

int Phaeton::init(void)
{
	return 0;
}

void Phaeton::_doTask(void *p_arg)
{
	CanTMsg msg;
	while(1) {
//		uint8_t err = _pSlot->read(&msg);
		uint8_t err = gpDrvCAN->getData(&msg,0);
		if (err == OS_ERR_NONE) {
			switch(msg.msg.StdId) {

			case SHIFT_POS_MSG_ID:
				_updateShiftPos(&msg);
				_updateACCStatus(&msg);
				break;
			case STEERING_WHEER_ANGLE_MSG_ID:
				_updateSteerAngle(&msg);
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
void Phaeton::_updateShiftPos(CanTMsg *pmsg)
{
	switch (pmsg->msg.Data[7] & 0x0F) {
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
void Phaeton::_updateSteerAngle(CanTMsg *pmsg)
{
	// 此处必须为带符号数
	const static int32_t thresh = 150;// 相邻2帧的最大变化
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

void Phaeton::_updateRadar(CanTMsg *pmsg)
{
	detectedRadar = true;
	radarVal[0] = calculateGrade(pmsg->msg.Data[0], 0x40, 6);
	radarVal[1] = calculateGrade(pmsg->msg.Data[4], 0x7c, 8);
	radarVal[2] = calculateGrade(pmsg->msg.Data[5], 0x7c, 8);
	radarVal[3] = calculateGrade(pmsg->msg.Data[1], 0x40, 6);

	radarVal[4] = calculateGrade(pmsg->msg.Data[2], 0x40, 4);
	radarVal[5] = calculateGrade(pmsg->msg.Data[6], 0xa5, 11);
	radarVal[6] = calculateGrade(pmsg->msg.Data[7], 0xa5, 11);
	radarVal[7] = calculateGrade(pmsg->msg.Data[3], 0x40, 4);
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
