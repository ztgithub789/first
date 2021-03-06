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

#ifndef Landrover_DB_COMMON_H
#define Landrover_DB_COMMON_H

namespace Landrover_DB {

static const int32_t MAX_ABS_STEER_ANGLE		= 320;			// 转角绝对值最大值
static const int32_t MAX_ABS_CAN_STEER_ANGLE	= 0x1259;		// CAN帧中转角信息的最大绝对值
static const int32_t CAN_STEER_MINDDLE_VAL	= 0x1e80;		    // 方向盘往左打时CAN帧中转角信息的起始值


static const uint32_t SHIFT_POS_MSG_ID = 0xb8;

static const uint32_t STEERING_WHEER_ANGLE_MSG_ID = 0x4c0;
static const uint32_t P_KEY_STATUS_MSG_ID = 0x4a6;
static const uint32_t RADAR_TURNIDICATOR_MSG_ID	= 0x174;
static const uint32_t PARKTOSIDE_KEY_STATUS_MSG_ID = 0x607;
static const uint32_t TURNILIGHT_LEFT_MSG_ID = 0x504;
static const uint32_t TURNILIGHT_RIGHT_MSG_ID = 0x505;
static const uint32_t TURNILIGHT_BOTH_MSG_ID = 0xe0;
static const uint32_t CAR_TIME_MSG_ID = 0x4d2;
//static const uint32_t SPEED_MSG_ID                 = 0x30b;
}

#endif /* COMMON_H_ */
