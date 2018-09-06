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
#ifndef HONDAVEZEL_COMMON_H
#define HONDAVEZEL_COMMON_H

namespace HONDAVEZEL_2017 {
	static const uint32_t STEERING_WHEER_ANGLE_MSG_ID = 0x156;
	static const uint32_t SHIFT_POS_MSG_ID = 0x1a3;

	static const int32_t MAX_ABS_STEER_ANGLE		= 320;			// ת�Ǿ���ֵ���ֵ
	static const int32_t MAX_ABS_CAN_STEER_ANGLE	= 0x1100;		// CAN֡��ת����Ϣ��������ֵ
	static const int32_t CAN_STEER_LEFT_BEGIN_VAL	= 0x0000;		// �����������ʱCAN֡��ת����Ϣ����ʼֵ
	static const int32_t CAN_STEER_RIGHT_BEGIN_VAL	= 0xffff;		// ���������Ҵ�ʱCAN֡��ת����Ϣ����ʼֵ
}

#endif