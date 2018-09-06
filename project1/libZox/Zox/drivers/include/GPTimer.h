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
#ifndef GPTIMER_H_
#define GPTIMER_H_
#include <stm32f10x.h>
#include <stm32f10x_tim.h>
#include <Driver.h>

/**
 * 进度达到1u的定时器
 */
class DrvGPTimer : public Driver {
public:
	typedef void (*GPTimerCB)(void*);
	DrvGPTimer();
	virtual ~DrvGPTimer();
	int init(void);

	/**
	 * 设置回调函数
	 * @param cb 回调函数指针
	 * @param data 参数
	 * @return
	 */
	int setIrqCB(GPTimerCB cb, void *data)
	{
		_irqCB = cb;
		_data = data;
		return 0;
	}

	void setTimer(int value);
	void stop();
	void start();

	// 直接被中断处理向量调用的回调函数。
	static void irqVectorCB(void*);
private:
	GPTimerCB _irqCB;
	void *_data;
	//void configure(void);
};

#endif /* GPTIMER_H_ */
