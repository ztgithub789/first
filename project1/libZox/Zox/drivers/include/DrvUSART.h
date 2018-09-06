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

#ifndef DRVUSART_H_
#define DRVUSART_H_

#include <ucos_ii.h>
#include "stm32f10x.h"
#include "Driver.h"

/*
 * 串口状态定义。中断处理程序和__read()函数配合使用。
 */
enum USART_CHAR_STATUS {
	USART_CHAR_READY,		/* 字符在串口中准备就绪 */
	USART_CHAR_CONSUMED,    /* 串口中的字符已被__read()消费掉 */
};

struct ResUSART {
	USART_TypeDef	*USARTx;
	uint32_t		RCC_APBPeriph_USARTx;
	uint32_t		RCC_APBPeriph_rx;
	GPIO_TypeDef	*GPIO_rx;
	uint16_t		GPIO_Pin_rx;
	uint32_t		RCC_APBPeriph_tx;
	GPIO_TypeDef	*GPIO_tx;
	uint16_t		GPIO_Pin_tx;
	uint8_t			NVIC_IRQChannel;
	uint8_t			NVIC_IRQChannelSubPriority;
	USART_InitTypeDef	initStruct;
};

class DrvUSART: public Driver {
public:
	DrvUSART(ResUSART *res, bool isStdioUSART = false);
	virtual ~DrvUSART();
	int init(void);
	static void irqVectorCB(void *);
	void configure(USART_InitTypeDef *pInitStruct);
	void putChar(char ch);
	void puts(const char *ch);
	char readCharNonBlock(char *c);
	char getChar(void);
	void clearUnreadChar(void);
	void setBaudRate(uint32_t baud);
	void defConfigure(void);

	void releaseGPIO(void);
	void reinitGPIO(void);

	OS_EVENT               *usartRxMbox;
	int overflowCount;

private:
	void initGPIO(void);
	ResUSART *_res;
};

#endif /* DRVUSART_H_ */

