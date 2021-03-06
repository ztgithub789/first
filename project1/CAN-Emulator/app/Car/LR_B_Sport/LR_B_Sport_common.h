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
#ifndef LR_B_Sport_COMMON_H
#define LR_B_Sport_COMMON_H

namespace LANDROVER_NS {
	static const int32_t MAX_ABS_CAN_STEER_ANGLE	= 0x1259;		// CAN帧中转角信息的最大绝对值
	static const int32_t CAN_STEER_MINDDLE_VAL	= 0x1e80;		    // 方向盘往左打时CAN帧中转角信息的起始值
	
	/* CAN data message ID */
	static const uint32_t SHIFT_POS_MSG_ID1 = 0x4a;
	static const uint32_t SHIFT_POS_MSG_ID2 = 0x125;
	static const uint32_t STEERING_WHEER_ANGLE_MSG_ID = 0x4c0;
	static const uint32_t LCD_BACK_LIGHT_MSG_ID = 0x400;
	static const uint32_t TURNILIGHT_LEFT_MSG_ID = 0x504;
	static const uint32_t TURNILIGHT_RIGHT_MSG_ID = 0x505;
	static const uint32_t CAR_TIME_MSG_ID = 0x192;
	static const uint32_t P_KEY_STATUS_MSG_ID = 0x17d;
}

#endif /* LandRover_common.h */
