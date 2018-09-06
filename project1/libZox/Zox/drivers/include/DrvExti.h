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
 * �ⲿ�жϵ���������
 *
 * ʹ�÷���1��\n
 * ������ֻ��Ҫ�ȴ�һ���ж�Դ��ʱ�򣬿��������·�����
 * <pre>
 * 	  ...
 * 	  gpDrvExti->start(true); //�жϿ�ʼ��������
 *    ...
 *    gpDrvExti->pend();  // �˴����񱻹����ж�һ����ᱻ����
 *    ...
 *    gpDrvExit->start(true); //�ж�ֹͣ����
 *    ...
 * </pre>
 *
 * ʹ�÷���2��Ŀǰ��û������� \n
 * ���������Ҫ�ȴ�����жϣ�������getEvent()��ȡ���жϵ��ź�����Ȼ�����OSEventPendMulti()��
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
		GPIO_TypeDef	*GPIO_Port;			///< GPIO��
		uint16_t		GPIO_Pin;			///< PIN��
		uint32_t		RCC_APB2Periph;		///< RCC�����
		uint8_t			EXTI_GPIO_PortSource;	///< �ⲿ�ж�GPIO��
		uint8_t			EXIT_GPIO_PinSource;	///< �ⲿ�ж�GPIO PIN��
		uint8_t			EXIT_NVIC_IRQChannel;
		EXTI_InitTypeDef   EXTI_InitStructure;	///< EXTI_Init()���������������ж�����
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
