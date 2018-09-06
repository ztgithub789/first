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
#ifndef LANDROVER_COMMON_H
#define LANDROVER_COMMON_H

namespace LANDROVER_NS {
	static const int32_t MAX_ABS_CAN_STEER_ANGLE	= 0x29d8;		// CAN帧中转角信息的最大绝对值
	static const int32_t CAN_STEER_MINDDLE_VAL	= 0x8022;		    // 方向盘往左打时CAN帧中转角信息的起始值
	
	/* CAN data message ID */
	static const uint32_t SHIFT_POS_MSG_ID = 0xb8;
	static const uint32_t STEERING_WHEER_ANGLE_MSG_ID = 0x4c0;
	static const uint32_t LCD_BACK_LIGHT_MSG_ID = 0x4a3;
	static const uint32_t TURNILIGHT_LEFT_MSG_ID = 0x504;
	static const uint32_t TURNILIGHT_RIGHT_MSG_ID = 0x505;
	static const uint32_t CAR_TIME_MSG_ID = 0x4d2;
}

#endif /* LandRover_common.h */
