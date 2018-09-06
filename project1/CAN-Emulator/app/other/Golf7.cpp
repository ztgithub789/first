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
	shiftPos = SHIFT_POS_R;	// ����Ŀǰû�е�λ��Ϣ������һֱ���ڵ���

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
 * ���   :	���µ�λ��Ϣ����ʱֻ����R����
 * ����   :
 * ����ֵ :
 * ����   :
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
 * ���   :	���·�����ת����Ϣ��
 * 			ע������������˲���ʵ��ʹ�ã������й켣���������󡣿�����ĳЩ��Ч֡������Ч֡�����ˡ�
 * 				�˲�����������2֡(���200ms)֮��ĽǶȲ�ܳ���2�ȣ������֡�����ԡ�
 * ����   :
 * ����ֵ :
 * ����   :
 *******************************************************************************/
void AudiQ3_2012_imported::_updateSteerAngle(CanTMsg *pmsg)
{
	// �˴�����Ϊ��������
		const static int32_t thresh = 100;	// ����2֡�����仯
		static int32_t aOld = 0;
		int32_t aNew;

		int32_t val = (int32_t)((pmsg->msg.Data[3] << 8) | pmsg->msg.Data[2]);
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

void AudiQ3_2012_imported::_updatePKeyStatus(CanTMsg *pmsg)
{
	if (pmsg->msg.Data[2] == 0) {
		isPKeyPressed = false;
	} else {
		isPKeyPressed = true;
	}
}
///*******************************************************************************
// * ���   :	�����״�ļ���
// * ����   :	v		̽ͷ����ֵ
// * 			maxV	̽ͷ�ܼ�⵽�ĵ������ֵ
// * 			gradeNr	̽ͷ����������������ʾ����������磬�µ�A4�����̽ͷ�����4������gradeNrΪ4��
// * ����ֵ :	��ǰ̽ͷ�ļ���������Radar��Ĺ淶��0��ʾ����ʾ��1��ʾ��ʾ���һ��������Խ�󣬻���ԽԶ��
// * ����   :
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
		// ���̽ͷ����
		radarVal[4] = calculateGrade(pmsg->msg.Data[2], 0x5a, 6);
		radarVal[5] = calculateGrade(pmsg->msg.Data[3], 0xa0, 11);
		radarVal[6] = calculateGrade(pmsg->msg.Data[4], 0xa0, 11);
		radarVal[7] = calculateGrade(pmsg->msg.Data[5], 0x5a, 6);
	} else if(dir ==0x92) {
		// ǰ��̽ͷ����
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
