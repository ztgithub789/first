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

#ifndef AUDIA6_2012_COMMON_H
#define AUDIA6_2012_COMMON_H

namespace AudiA6_2012 {

static const int32_t MAX_ABS_STEER_ANGLE		= 320;			// 转角绝对值最大值
static const int32_t MAX_ABS_CAN_STEER_ANGLE	= 0x1500;		// CAN帧中转角信息的最大绝对值
static const int32_t CAN_STEER_LEFT_BEGIN_VAL	= 0x4000;		// 方向盘往左打时CAN帧中转角信息的起始值
static const int32_t CAN_STEER_RIGHT_BEGIN_VAL	= 0x6000;		// 方向盘往右打时CAN帧中转角信息的起始值


static const uint32_t SHIFT_POS_MSG_ID = 0x3c3;
static const uint32_t STEERING_WHEER_ANGLE_MSG_ID = 0x525;
static const uint32_t P_KEY_STATUS_MSG_ID = 0x603;
static const uint32_t RADAR_MSG_ID	= 0x634;
static const uint32_t PARKTOSIDE_KEY_STATUS_MSG_ID = 0x607;
static const uint32_t TURNIDICATOR_STATUS_MSG_ID = 0x363;
static const uint32_t TURNIDICATOR_STATUS_MSG_ID2 = 0x3c2;
static const uint32_t CAR_TIME_MSG_ID = 0x6b6;
static const uint32_t SPEED_MSG_ID                 = 0x30b;
}

#endif /* COMMON_H_ */
