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
#ifndef DRVSYSTICK_H_
#define DRVSYSTICK_H_

#include <stm32f10x.h>
#include <Driver.h>

class DrvSysTick : public Driver {
public:
	typedef void (*DrvSysTickCB)(void *);
	DrvSysTick();
	virtual ~DrvSysTick();
	int init(void)
	{

		/* ��SysTick�жϼ��Ϊ1 ms��SysTick�жϵ����ȼ��ڴ˴�����Ϊ��͡�*/
		if (SysTick_Config(SystemCoreClock / 1000))
		{
			/* Capture error */
			while (1);
		}

		/*
		 * ����SysTickΪ�Ƚϸߵ����ȼ��������������жϴ������п��Ե�����SysTickΪ��������ʱ����
		 */
		//NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));

		return 0;
	}

	static int setIrqCB(irqCB_t _cb)
	{
		irqCB = _cb;
		return 0;
	}

	static int setIrqCB2(DrvSysTickCB _cb, void *data)
	{
		irqCB2 = _cb;
		irqCB2Data = data;
		return 0;
	}

	// ֱ�ӱ��жϴ����������õĻص�������
	static void irqVectorCB(void)
	{
		ms ++;
		if (irqCB != 0)
				(*irqCB)();

		if (irqCB2!= 0)
				(*irqCB2)(irqCB2Data);
	}

	/****************************************************************************
	 * ���   : ��ȡ��ǰʱ�䡣��ϵͳ������ʼ��ʱ����λ:ms��
	 * ����   :
	 * ����ֵ : ʱ��ֵ����λ:ms��
	 * ����   : ����32λ������
	 ****************************************************************************/
	static int getTime(void)
	{
		return ms;
	}

private:
	static Driver::irqCB_t irqCB;
	static DrvSysTickCB irqCB2;
	static void *irqCB2Data;
	static int ms;
};

#endif /* DRVSYSTICK_H_ */
