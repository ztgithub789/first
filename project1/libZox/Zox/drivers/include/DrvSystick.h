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

		/* 设SysTick中断间隔为1 ms。SysTick中断的优先级在此处被设为最低。*/
		if (SysTick_Config(SystemCoreClock / 1000))
		{
			/* Capture error */
			while (1);
		}

		/*
		 * 设置SysTick为比较高的优先级，这样在其他中断处理函数中可以调用以SysTick为基础的延时函数
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

	// 直接被中断处理向量调用的回调函数。
	static void irqVectorCB(void)
	{
		ms ++;
		if (irqCB != 0)
				(*irqCB)();

		if (irqCB2!= 0)
				(*irqCB2)(irqCB2Data);
	}

	/****************************************************************************
	 * 简介   : 获取当前时间。从系统启动开始计时，单位:ms。
	 * 参数   :
	 * 返回值 : 时间值。单位:ms。
	 * 描述   : 记满32位后会溢出
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
