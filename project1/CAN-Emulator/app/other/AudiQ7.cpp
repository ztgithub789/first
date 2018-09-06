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

#include "AudiQ7.h"
#include "AudiQ7_common.h"
using namespace AudiQ7NS;

AudiQ7::AudiQ7() {
	name = "Q7";
	const static uint32_t idList[] = {0x3c3, 0x6a5, 0x6f4, 0x355,0 };
	pCanFilterIDList = idList;
	maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;
	steerAngle = 0;
	baudRate = 100;
	shiftPos = SHIFT_POS_R;	// ����Ŀǰû�е�λ��Ϣ������һֱ���ڵ���
}

AudiQ7::~AudiQ7() {
}

int AudiQ7::init(void)
{
	return 0;
}

void AudiQ7::_doTask(void *p_arg)
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
				_updateRadar(&msg);
				break;
				
			default:
				break;
			}
		}
	}
}

/*******************************************************************************
 * ���   :	���µ�λ��Ϣ����ʱֻ����R����
 * ����   :
 * ����ֵ :
 * ����   :
 *******************************************************************************/
void AudiQ7::_updateShiftPos(CanTMsg *pmsg)
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
 * ���   :	���·�����ת����Ϣ��
 * 			ע������������˲���ʵ��ʹ�ã������й켣���������󡣿�����ĳЩ��Ч֡������Ч֡�����ˡ�
 * 				�˲�����������2֡(���200ms)֮��ĽǶȲ�ܳ���2�ȣ������֡�����ԡ�
 * ����   :
 * ����ֵ :
 * ����   :
 *******************************************************************************/
void AudiQ7::_updateSteerAngle(CanTMsg *pmsg)
{
	// �˴�����Ϊ��������
		const static int32_t thresh = 100;	// ����2֡�����仯
		static int32_t aOld = 0;
		int32_t aNew;

		int32_t val = (int32_t)((pmsg->msg.Data[1] << 8) | pmsg->msg.Data[0]);
		if (CAN_STEER_LEFT_BEGIN_VAL <= val && val <= CAN_STEER_LEFT_BEGIN_VAL + MAX_ABS_CAN_STEER_ANGLE) {
			// �����������
			aNew = -(val - CAN_STEER_LEFT_BEGIN_VAL) * MAX_ABS_STEER_ANGLE / MAX_ABS_CAN_STEER_ANGLE;
		} else if (CAN_STEER_RIGHT_BEGIN_VAL <= val && val <= CAN_STEER_RIGHT_BEGIN_VAL + MAX_ABS_CAN_STEER_ANGLE) {
			// ���������Ҳ�
			aNew = (val - CAN_STEER_RIGHT_BEGIN_VAL) * MAX_ABS_STEER_ANGLE / MAX_ABS_CAN_STEER_ANGLE;
		} else {
			// ��������ֵ
			return;
		}

		if (abs(aNew - aOld) < thresh) {
			steerAngle = aNew;
		} else {
			zprintf("angle filted\n");
		}
		aOld = aNew;
}

void AudiQ7::_updatePKeyStatus(CanTMsg *pmsg)
{
	if (pmsg->msg.Data[2] == 0) {
		isPKeyPressed = false;
	} else {
		isPKeyPressed = true;
	}
}

void AudiQ7::_updateRadar(CanTMsg *pmsg)
{
	detectedRadar = true;
	uint8_t dir = pmsg->msg.Data[1];
	if (dir ==0x87) {
		// ���̽ͷ����
		radarVal[4] = calculateGrade(pmsg->msg.Data[3], 0x3c, 4);
		radarVal[5] = calculateGrade(pmsg->msg.Data[4], 0xa0, 11);
		radarVal[6] = calculateGrade(pmsg->msg.Data[5], 0xa0, 11);
		radarVal[7] = calculateGrade(pmsg->msg.Data[6], 0x3c, 4);
	} else if(dir ==0x86) {
		// ǰ��̽ͷ����
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
#ifdef TEST
CanTxMsg *AudiA6_2012::getMsg(uint8_t type)
{

}

uint32_t *AudiA6_2012::getMsgTimeInterval(uint8_t type)
{
	switch
}
#endif
