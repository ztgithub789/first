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
#ifndef DRVEXTI_H_
#define DRVEXTI_H_

#include "stm32f10x.h"
#include "Driver.h"

/**
 * 外部中断的驱动程序。
 *
 * 使用方法1：\n
 * 在任务只需要等待一个中断源的时候，可以用如下方法：
 * <pre>
 * 	  ...
 * 	  gpDrvExti->start(true); //中断开始持续工作
 *    ...
 *    gpDrvExti->pend();  // 此处任务被挂起，中断一来便会被唤醒
 *    ...
 *    gpDrvExit->start(true); //中断停止工作
 *    ...
 * </pre>
 *
 * 使用方法2：目前还没这个功能 \n
 * 如果任务需要等待多个中断，可以用getEvent()获取该中断的信号量，然后调用OSEventPendMulti()。
 * <pre>
 *    OS_EVENT *events[3];
 *    OS_EVENT *eventsRdy[3];
 *    void *eventMsgs[3];
 *    uint8_t err;
 *    events[0] = gpDrvExti1->getEvent();
 *    events[1] = gpDrvExti2->getEvent();
 *    events[2] = 0;
 *    OSEventPendMulti(events, eventsRdy, eventMsgs, 0, &err);
 *    ...
 * </pre>
 */
struct ResExti {
		GPIO_TypeDef	*GPIO_Port;			///< GPIO口
		uint16_t		GPIO_Pin;			///< PIN号
		uint32_t		RCC_APB2Periph;		///< RCC外设号
		uint8_t			EXTI_GPIO_PortSource;	///< 外部中断GPIO口
		uint8_t			EXIT_GPIO_PinSource;	///< 外部中断GPIO PIN号
		uint8_t			EXIT_NVIC_IRQChannel;
		EXTI_InitTypeDef   EXTI_InitStructure;	///< EXTI_Init()参数，用于设置中断特性
};

class DrvExti : public Driver {
public:
	
	DrvExti(ResExti *res);
	~DrvExti();
	int init(void);
	void pend(void);
	void start(bool isstart =true);
	OS_EVENT *getEvent(void);
private:
	ResExti *_res;
	OS_EVENT *_pEvent;
	static void isr(void *d);
};

#endif /* DRVEXTI_H_ */
