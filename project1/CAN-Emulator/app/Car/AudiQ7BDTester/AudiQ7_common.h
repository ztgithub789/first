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

#ifndef AudiQ7_COMMON_H_
#define AudiQ7_COMMON_H_

#include <stdint.h>
#include <ucos_ii.h>

namespace AudiQ7NS {
static const uint32_t REMOTE_KEY_ID_Q5           = 0x1F1;  	//后门的ID是0x171(C6) 1F1(Q5)
static const uint32_t TRUNK_STATUS_ID_Q5         = 0x582;	// 尾门状态
static const uint32_t FRONTDOOR_KEY_ID_Q5        = 0x3D0;//前侧门的尾门控制开关

static const uint32_t REMOTE_KEY_ID_Q7           = 0x171;  	//后门的ID是0x171(C6) 1F1(Q5)
static const uint32_t TRUNK_STATUS_ID_Q7         = 0x59F;	// 尾门状态
}

#endif /* COMMON_H_ */
