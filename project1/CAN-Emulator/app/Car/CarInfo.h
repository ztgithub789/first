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
#ifndef CARINFO_H
#define CARINFO_H
#include <includes.h>

class CarInfo {
public:
	// ����λ���������
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
	// ��ת��Ƶ��������
	enum TurnIndicatorStatus {
		TURN_INDICATOR_NONE,	// ��ת���
		TURN_INDICATOR_LEFT,	// ��ת���
		TURN_INDICATOR_RIGHT,	// ��ת���
	};
	// ���ƹ���������
	enum HighBeamStatus {
		HIGH_BEAM_NONE,			// ��Զ���
		HIGH_BEAM_INNER,		// Զ��ƴ����ڲ�
		HIGH_BEAM_OUTER,		// Զ��ƴ������
	};
	// ���������������
	enum RollingStatus {
		ROLLING_NONE,
		ROLLING_UP,
		ROLLING_DOWN,
		ROLLING_PUSH,
		ROLLING_HOLD,
	};
	
	CarInfo(void);
	~CarInfo(void);

	uint8_t shiftPos;			        // ��λ	
	int32_t steerAngle;			        // ������ת�ǣ���λ��0.1�ȡ����Ϊ��ֵ���ұ�Ϊ��ֵ���м�Ϊ0��
	int32_t maxAbsSteerAngle;	        // ������ת�ǵľ���ֵ�����ֵ����λ��0.1�ȡ�steerAngle��ֵ������[-maxAbsSteerAngle, maxAbsSteerAngle]֮���Ϊ��Ч
	uint8_t radarVal[CarInfo::RADAR_NR];// 8�״�̽ͷ������
	uint32_t baudRate;			        // CAN���ߵĲ�����
	uint8_t lightPos;			        // �� bit0:��ת�� bit1:��ת�� bit2:Զ��ƺ� bit3:Զ���ǰ  
    uint8_t TireKey;	
    uint8_t MMIKey;
	uint8_t speedCtl;
	uint8_t lightCtl;
	uint8_t PageTrunKey;
	const uint32_t *pCanFilterIDList;   // ָ��CAN ID���顣���14��ID�ܱ����ա�������0��β��
	bool isPKeyPressed;		        // P���Ƿ���
	bool isDoorClose[4];
	bool isACCOn;				        // ACC�Ƿ���
	bool isRadaFront;
	bool isRadaBack;
	bool isRTKPressed;					//RTK:Remote Trunk Key
	bool isFDTKPressed;					//FDTK:Front Door Trunk Key
//	bool isTrunLeft;
//	bool isTrunRight;
//	bool isMiddle;
//	uint8_t BackDoorStatus;	// 1:close 0:open
};
#endif