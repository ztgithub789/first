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
	// 时钟输入：APB1的时钟（TIMx_SMCR的SMS取缺省值0）: 72M。
	// 该函数设置TIMx，时钟周期1us整。
	//
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	_overflowCounter = 0;
	TIM_SetCounter(HRCLOCK_DEVICE, 0);

	TIM_DeInit(HRCLOCK_DEVICE);                              //复位TIMx定时器
	/* TIMx定时器允许 */
	RCC_APB1PeriphClockCmd(HRCLOCK_APB_PERIPH, ENABLE);

	/* TIMx configuration */
	TIM_TimeBaseStructure.TIM_Period = 0xffff;		// 让其自然溢出
	TIM_TimeBaseStructure.TIM_Prescaler = 72;		// 分频72，周期1us
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// 时钟分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向向上计数
	TIM_TimeBaseInit(HRCLOCK_DEVICE, &TIM_TimeBaseStructure);

	/* 清除TIMx溢出中断标志 */
	TIM_ClearFlag(HRCLOCK_DEVICE, TIM_FLAG_Update);

	/* 注册中断处理函数 */
	irqVectSet(HRCLOCK_DEVICE_IRQ, isr, this);
	/* TIMx溢出中断允许 */
	TIM_ITConfig(HRCLOCK_DEVICE, TIM_IT_Update, ENABLE);

	/* 允许TIMx计数 */
	TIM_Cmd(HRCLOCK_DEVICE, ENABLE);

	/* 设置NVIC */
	NVIC_InitStructure.NVIC_IRQChannel = HRCLOCK_DEVICE_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x08;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	return 0;
}

/****************************************************************************
 * 简介   :	中断处理函数
 * 参数   :
 * 返回值 :
 * 描述   :
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
 * 简介   :	返回当前时钟，单位us
 * 参数   :
 * 返回值 :
 * 描述   :	为了防止读取过程中中断产生，导致硬件计数器和软件实现的溢出计数器之
 *			间的不一致，采用如下算法:
 *			1. 读溢出计数器
 *			2. 读硬件计数器
 *			3. 再次读溢出计数器，并和第1步的值比较
 *			4. 如果相同，处理结果并返回
 *			5. 否则跳转至1
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

