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

#ifndef AUDIA8_H_
#define AUDIA8_H_

#include <Car/Car.h>
#include <CanHub/CanHub.h>

class AudiA8 : public Car {
public:

	AudiA8();
	~AudiA8();
	int init(void);
	void _doTask(void *p_arg);

#ifdef TEST
	enum MsgType{
		MSG_STEER_ANGLE,
		MSG_SHIFT_POS,
		MSG_RADAR_VAL,
	};
	CanTxMsg *getMsg(uint8_t type);
	uint32_t *getMsgTimeInterval(uint8_t type);
#endif

private:
#if 0
	static const int32_t SHIFT_POS_MSG_ID				= 0x3c3;
	static const int32_t STEERING_WHEER_ANGLE_MSG_ID	= 0x525;
	static const int32_t P_KEY_STATUS_MSG_ID			= 0x603;

	static const int32_t MAX_ABS_STEER_ANGLE		= 300;			// 转角绝对值最大值
	static const int32_t MAX_ABS_CAN_STEER_ANGLE	= 0x1000;		// CAN帧中转角信息的最大绝对值
	static const int32_t CAN_STEER_LEFT_BEGIN_VAL	= 0x8000;		// 方向盘往左打时CAN帧中转角信息的起始值
	static const int32_t CAN_STEER_RIGHT_BEGIN_VAL	= 0xa000;		// 方向盘往右打时CAN帧中转角信息的起始值
#endif 
	void _updateShiftPos(CanTMsg *pmsg);
	void _updateSteerAngle(CanTMsg *pmsg);
	void _updatePKeyStatus(CanTMsg *pmsg);
	void _updateRadarValue(CanTMsg *pmsg);
	CanHub::Slot *_pSlot;
};

#endif /* AUDIA62012_H_ */
