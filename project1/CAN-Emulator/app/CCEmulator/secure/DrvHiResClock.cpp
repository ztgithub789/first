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
#include <irq.h>
#include "DrvHiResClock.h"


DrvHiResClock::DrvHiResClock() {
	// TODO Auto-generated constructor stub

}

DrvHiResClock::~DrvHiResClock() {
	// TODO Auto-generated destructor stub
}

int DrvHiResClock::init(void)
{
	///
	// ʱ�����룺APB1��ʱ�ӣ�TIMx_SMCR��SMSȡȱʡֵ0��: 72M��
	// �ú�������TIMx��ʱ������1us����
	//
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	_overflowCounter = 0;
	TIM_SetCounter(HRCLOCK_DEVICE, 0);

	TIM_DeInit(HRCLOCK_DEVICE);                              //��λTIMx��ʱ��
	/* TIMx��ʱ������ */
	RCC_APB1PeriphClockCmd(HRCLOCK_APB_PERIPH, ENABLE);

	/* TIMx configuration */
	TIM_TimeBaseStructure.TIM_Period = 0xffff;		// ������Ȼ���
	TIM_TimeBaseStructure.TIM_Prescaler = 72;		// ��Ƶ72������1us
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// ʱ�ӷ�Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //�����������ϼ���
	TIM_TimeBaseInit(HRCLOCK_DEVICE, &TIM_TimeBaseStructure);

	/* ���TIMx����жϱ�־ */
	TIM_ClearFlag(HRCLOCK_DEVICE, TIM_FLAG_Update);

	/* ע���жϴ����� */
	irqVectSet(HRCLOCK_DEVICE_IRQ, isr, this);
	/* TIMx����ж����� */
	TIM_ITConfig(HRCLOCK_DEVICE, TIM_IT_Update, ENABLE);

	/* ����TIMx���� */
	TIM_Cmd(HRCLOCK_DEVICE, ENABLE);

	/* ����NVIC */
	NVIC_InitStructure.NVIC_IRQChannel = HRCLOCK_DEVICE_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x08;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	return 0;
}

/****************************************************************************
 * ���   :	�жϴ�����
 * ����   :
 * ����ֵ :
 * ����   :
 ****************************************************************************/
void DrvHiResClock::isr(void* data)
{
	if (TIM_GetITStatus(HRCLOCK_DEVICE, TIM_IT_Update) != RESET) {
		DrvHiResClock *_this = (DrvHiResClock*)data;
		TIM_ClearITPendingBit(HRCLOCK_DEVICE, TIM_IT_Update);
		_this->_overflowCounter++;
	}
}

/****************************************************************************
 * ���   :	���ص�ǰʱ�ӣ���λus
 * ����   :
 * ����ֵ :
 * ����   :	Ϊ�˷�ֹ��ȡ�������жϲ���������Ӳ�������������ʵ�ֵ����������֮
 *			��Ĳ�һ�£����������㷨:
 *			1. �����������
 *			2. ��Ӳ��������
 *			3. �ٴζ���������������͵�1����ֵ�Ƚ�
 *			4. �����ͬ��������������
 *			5. ������ת��1
 *
 ****************************************************************************/
uint64_t DrvHiResClock::getTimeus(void)
{
retry:
	uint64_t ret = _overflowCounter;
	uint16_t countVal = TIM_GetCounter(HRCLOCK_DEVICE);
	if (ret != _overflowCounter)
		goto retry;

	ret = (ret << 16) | countVal;
	return ret;
}

