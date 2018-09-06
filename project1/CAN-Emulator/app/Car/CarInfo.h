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
	// 各档位的索引编号
	enum {
		SHIFT_POS_P = 0,
		SHIFT_POS_R,
		SHIFT_POS_N,
		SHIFT_POS_D,
		SHIFT_POS_S,
		SHIFT_POS_OTHER,
		SHIFT_POS_MAX
	};
	// 各个雷达探头的索引编号
	enum {
		RADAR_LFS = 0,	// 左前侧： left front side
		RADAR_LFM,		// 左前中： left front middle
		RADAR_RFM,		// 右前中
		RADAR_RFS,		// 右前侧
		RADAR_LRS,		// 左后侧： left front side
		RADAR_LRM,		// 左后中： left front middle
		RADAR_RRM,		// 右后中
		RADAR_RRS,		// 右后侧
		RADAR_NR,
	};
	// 各转向灯的索引编号
	enum TurnIndicatorStatus {
		TURN_INDICATOR_NONE,	// 无转向灯
		TURN_INDICATOR_LEFT,	// 左转向灯
		TURN_INDICATOR_RIGHT,	// 右转向灯
	};
	// 各灯光的索引编号
	enum HighBeamStatus {
		HIGH_BEAM_NONE,			// 无远光灯
		HIGH_BEAM_INNER,		// 远光灯打向内侧
		HIGH_BEAM_OUTER,		// 远光灯打向外侧
	};
	// 各滑键的索引编号
	enum RollingStatus {
		ROLLING_NONE,
		ROLLING_UP,
		ROLLING_DOWN,
		ROLLING_PUSH,
		ROLLING_HOLD,
	};
	
	CarInfo(void);
	~CarInfo(void);

	uint8_t shiftPos;			        // 档位	
	int32_t steerAngle;			        // 方向盘转角，单位：0.1度。左边为负值，右边为正值，中间为0。
	int32_t maxAbsSteerAngle;	        // 方向盘转角的绝对值的最大值，单位：0.1度。steerAngle的值必须在[-maxAbsSteerAngle, maxAbsSteerAngle]之间才为有效
	uint8_t radarVal[CarInfo::RADAR_NR];// 8雷达探头的数据
	uint32_t baudRate;			        // CAN总线的波特率
	uint8_t lightPos;			        // 灯 bit0:左转灯 bit1:右转灯 bit2:远光灯后 bit3:远光灯前  
    uint8_t TireKey;	
    uint8_t MMIKey;
	uint8_t speedCtl;
	uint8_t lightCtl;
	uint8_t PageTrunKey;
	const uint32_t *pCanFilterIDList;   // 指向CAN ID数组。最多14个ID能被接收。必须以0结尾。
	bool isPKeyPressed;		        // P键是否按下
	bool isDoorClose[4];
	bool isACCOn;				        // ACC是否开启
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