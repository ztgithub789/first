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

#ifndef BorgWard_BX5_COMMON_H_
#define BorgWard_BX5_COMMON_H_

#include <stdint.h>
#include <ucos_ii.h>

namespace BorgWard_BX52017 {

static const int32_t SHIFT_POS_MSG_ID				= 0x326;
static const int32_t STEERING_WHEER_ANGLE_MSG_ID	= 0x3a;
static const int32_t RADAR_MSG_ID					= 0x441;

static const int32_t TIME_MSG_ID				= 0x476;


static const uint32_t TURNIDICATOR_STATUS_MSD_ID 	= 0x360;
static const uint32_t TURNIDICATOR_DOUBLE_STATUS_MSD_ID 	= 0x650;
static const int32_t MAX_ABS_STEER_ANGLE		= 300;			// 转角绝对值最大值
static const int32_t MAX_ABS_CAN_STEER_ANGLE	= 0x5653;		// CAN帧中转角信息的最大绝对值
static const int32_t CAN_STEER_BEGIN_VAL	= 0x0000;		// 方向盘往左打时CAN帧中转角信息的起始值


}

#endif /* COMMON_H_ */
