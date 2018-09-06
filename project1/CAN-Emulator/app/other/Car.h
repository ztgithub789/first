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

#ifndef CAR_H_
#define CAR_H_

#include <ZTask.h>
#include <includes.h>
class Car : public ZTask {
public:
	enum {
		SHIFT_POS_P = 0,
		SHIFT_POS_R,
		SHIFT_POS_N,
		SHIFT_POS_D,
		SHIFT_POS_S,
		SHIFT_POS_OTHER,
		SHIFT_POS_MAX
	};

	// �����״�̽ͷ���������
	enum {
		RADAR_LFS = 0,	// ��ǰ�ࣺ left front side
		RADAR_LFM,		// ��ǰ�У� left front middle
		RADAR_RFM,		// ��ǰ��
		RADAR_RFS,		// ��ǰ��
		RADAR_LRS,		// ���ࣺ left front side
		RADAR_LRM,		// ����У� left front middle
		RADAR_RRM,		// �Һ���
		RADAR_RRS,		// �Һ��
		RADAR_NR,
	};

	enum TurnIndicatorStatus {
		TURN_INDICATOR_NONE,	// ��ת���
		TURN_INDICATOR_LEFT,	// ��ת���
		TURN_INDICATOR_RIGHT,	// ��ת���
	};

	enum HighBeamStatus {
		HIGH_BEAM_NONE,			// ��Զ���
		HIGH_BEAM_INNER,		// Զ��ƴ����ڲ�
		HIGH_BEAM_OUTER,		// Զ��ƴ������
	};

	enum RollingStatus {
		ROLLING_NONE,
		ROLLING_UP,
		ROLLING_DOWN,
		ROLLING_PUSH,
		ROLLING_HOLD,
	};

	Car(uint8_t prio = CAR_TASK_PRIO);
	virtual ~Car();
	virtual int init(void) = 0;
	virtual void updateRadar(CanTMsg *pmsg);
	int calculateGrade(unsigned char v, unsigned char maxV, unsigned int gradeNr);

	int32_t steerAngle;			// ������ת�ǣ���λ��0.1�ȡ����Ϊ��ֵ���ұ�Ϊ��ֵ���м�Ϊ0��
	int32_t maxAbsSteerAngle;	// ������ת�ǵľ���ֵ�����ֵ����λ��0.1�ȡ�steerAngle��ֵ������[-maxAbsSteerAngle, maxAbsSteerAngle]֮���Ϊ��Ч
	uint32_t baudRate;			// CAN���ߵĲ�����
	const uint32_t *pCanFilterIDList; // ָ��CAN ID���顣���14��ID�ܱ����ա�������0��β��

	HighBeamStatus highBeamStatus;
	TurnIndicatorStatus turnIndicatorStatus;
	RollingStatus	rollingStatus;

	const char *name;
	uint8_t shiftPos;			// ��λ
	uint8_t radarVal[RADAR_NR];	// 8�״�̽ͷ������
	//uint8_t lightPos;			// �� bit0:��ת�� bit1:��ת�� bit2:Զ��ƺ� bit3:Զ���ǰ
	bool	isPKeyPressed;		// P���Ƿ���
	bool isACCOn;				// ACC�Ƿ���
	bool isRadaFront;
	bool isRadaBack;
	bool detectedRadar;

	bool isVoiceKeyPressed;
	bool isNAVIKeyPressed;
	bool isAirConditionOn;
	bool isNAVIKeyHolded;
	
	bool isPKeyDetected;
	bool isParkToSideKeyPressed;
};

#endif /* CAR_H_ */
