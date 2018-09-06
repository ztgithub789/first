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



#include "board.h"

//DrvNVIC		*gpDrvNVIC;
DrvUSART	*gpDrvUSART;
DrvUSART	*gpDrvUSART3;
DrvCH7025	*gpDrvCH7025;
DrvRelay	*gpDrvRelay;
DrvM3366	*gpDrvM3366;
DrvHiResClock	*gpDrvHiResClock;
DrvDipSwitch	*gpDrvDipSwitch;
DrvExti		*gpDrvRemoteExti1;	// 头枕1遥控器用的外部中断驱动
DrvExti		*gpDrvRemoteExti2;	// 头枕2遥控器用的外部中断驱动
DrvLED		*gpDrvLEDGreen, *gpDrvLEDRed, *gpDrvBuzzer;	// LED和蜂鸣器本质上可以使用一个驱动
DrvCAN		*gpDrvCAN;
//DrvTJA1041	*gpDrvTJA1041;
DrvPCA82C250	*gpDrvPCA82C250;


static ResRelay resRelay = {
		.RCC_APB2Periph	= RCC_APB2Periph_GPIOA,
		.GPIO_Pin		= GPIO_Pin_2,	// v0.2
		.GPIO_port		= GPIOA,
};

static ResM3366 resM3366 = {
		.RCC_APB2Periph_SW1	= RCC_APB2Periph_GPIOB,
		.GPIO_Pin_SW1		= GPIO_Pin_8,
		.GPIO_port_SW1		= GPIOB,
		.RCC_APB2Periph_SW2	= RCC_APB2Periph_GPIOB,
		.GPIO_Pin_SW2		= GPIO_Pin_9,
		.GPIO_port_SW2		= GPIOB,
};

static ResUSART resUSART1 = {
		.USARTx					= USART1,
		.RCC_APBPeriph_USARTx	= RCC_APB2Periph_USART1,
		.RCC_APBPeriph_rx		= RCC_APB2Periph_GPIOA,
		.GPIO_rx				= GPIOA,
		.GPIO_Pin_rx			= GPIO_Pin_10,
		.RCC_APBPeriph_tx		= RCC_APB2Periph_GPIOA,
		.GPIO_tx				= GPIOA,
		.GPIO_Pin_tx			= GPIO_Pin_9,
		.NVIC_IRQChannel		= USART1_IRQn,
		.NVIC_IRQChannelSubPriority = 0x2,
		.initStruct				= {
				.USART_BaudRate = 115200,
				.USART_WordLength = USART_WordLength_8b,
				.USART_StopBits = USART_StopBits_1,
				.USART_Parity = USART_Parity_No,
				.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
				.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
		},
};

static ResUSART resUSART3 = {
		.USARTx					= USART3,
		.RCC_APBPeriph_USARTx	= RCC_APB1Periph_USART3,
		.RCC_APBPeriph_rx		= RCC_APB2Periph_GPIOB,
		.GPIO_rx				= GPIOB,
		.GPIO_Pin_rx			= GPIO_Pin_11,
		.RCC_APBPeriph_tx		= RCC_APB2Periph_GPIOB,
		.GPIO_tx				= GPIOB,
		.GPIO_Pin_tx			= GPIO_Pin_10,
		.NVIC_IRQChannel		= USART3_IRQn,
		.NVIC_IRQChannelSubPriority = 0x3,
		.initStruct				= {
				.USART_BaudRate = 9600,
				.USART_WordLength = USART_WordLength_9b,
				.USART_StopBits = USART_StopBits_1,
				.USART_Parity = USART_Parity_Even,
				.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
				.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
		},
};

static uint16_t dipPinArray[] = {
		GPIO_Pin_12,
		GPIO_Pin_11
};
static ResDipSwitch resDipSwitch = {
		.pinNr			= 2,
		.RCC_APB2Periph = RCC_APB2Periph_GPIOA,
		.pGPIO_pinArray = dipPinArray,
		.GPIO_port		= GPIOA,
};

static ResExti resRemoteExti1 = {
		.GPIO_Port				= GPIOA,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_3,			// PIN号
		.RCC_APB2Periph			= RCC_APB2Periph_GPIOA,	// RCC外设号
		.EXTI_GPIO_PortSource	= GPIO_PortSourceGPIOA,	// 外部中断GPIO口
		.EXIT_GPIO_PinSource	= GPIO_PinSource3,		// 外部中断GPIO PIN号
		.EXTI_Line				= EXTI_Line3,			// 外部中断线
		.EXIT_NVIC_IRQChannel	= EXTI3_IRQn,			// 外部中断号
};

static ResExti resRemoteExti2 = {
		.GPIO_Port				= GPIOB,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_15,			// PIN号
		.RCC_APB2Periph			= RCC_APB2Periph_GPIOB,	// RCC外设号
		.EXTI_GPIO_PortSource	= GPIO_PortSourceGPIOB,	// 外部中断GPIO口
		.EXIT_GPIO_PinSource	= GPIO_PinSource15,		// 外部中断GPIO PIN号
		.EXTI_Line				= EXTI_Line15,			// 外部中断线
		.EXIT_NVIC_IRQChannel	= EXTI15_10_IRQn,			// 外部中断号
};

static ResLED resLEDGreen = {
		.RCC_APB2Periph			= RCC_APB2Periph_GPIOB,
		.GPIO_Port				= GPIOB,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_12,			// PIN号
};

static ResLED resLEDRed = {
		.RCC_APB2Periph			= RCC_APB2Periph_GPIOB,
		.GPIO_Port				= GPIOB,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_13,			// PIN号
};

static ResLED resBuzzer = {
		.RCC_APB2Periph			= RCC_APB2Periph_GPIOA,
		.GPIO_Port				= GPIOA,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_0,			// PIN号
};

//ResTJA1041 resTJA1041 = {
//		.RCC_Periph		= RCC_APB2Periph_GPIOB,
//		.en_GPIO_Port	= GPIOB,
//		.en_GPIO_Pin	= GPIO_Pin_14,
//		.stb_GPIO_Port	= GPIOB,
//		.stb_GPIO_Pin	= GPIO_Pin_5,
//		.wake_GPIO_Port	= GPIOB,
//		.wake_GPIO_Pin	= GPIO_Pin_13,
//		.err_GPIO_Port	= GPIOB,
//		.err_GPIO_Pin	= GPIO_Pin_12,
//};

ResPCA82C250 resPCA82C250 = {
		.RCC_Periph		= RCC_APB2Periph_GPIOB,
		.rs_GPIO_Port	= GPIOB,
		.rs_GPIO_Pin	= GPIO_Pin_5,
};

void bspConstructDrivers (void)
{
//	static DrvNVIC		drvNVIC;
	static DrvUSART		drvUSART(&resUSART1, true);
	static DrvCAN		drvCAN;
	//static DrvTJA1041	drvTJA1041(&resTJA1041);
	static DrvPCA82C250		drvPCA82C250(&resPCA82C250);
//	static DrvCH7025	drvCH7025;
//	static DrvRelay		drvRelay(&resRelay);
//	static DrvM3366		drvM3366(&resM3366);
//	static DrvHiResClock drvHiResClock;
//	static DrvGPTimer	drvGPTimer;
//	static DrvDipSwitch		drvDipSwitch(&resDipSwitch);
//	static DrvExti		drvRemoteExti1(&resRemoteExti1);
//	static DrvExti		drvRemoteExti2(&resRemoteExti2);
//	static DrvLED		drvLEDGreen(&resLEDGreen, false);
//	static DrvLED		drvLEDRed(&resLEDRed, false);
//	static DrvLED		drvBuzzer(&resBuzzer);

//	gpDrvNVIC		= &drvNVIC;
	gpDrvUSART		= &drvUSART;
//	gpDrvCH7025		= &drvCH7025;
//	gpDrvRelay		= &drvRelay;
//	gpDrvM3366		= &drvM3366;
//	gpDrvHiResClock	= &drvHiResClock;
//	gpDrvDipSwitch	= &drvDipSwitch;
//	gpDrvRemoteExti1	= &drvRemoteExti1;
//	gpDrvRemoteExti2	= &drvRemoteExti2;
//	gpDrvLEDGreen	= &drvLEDGreen;
//	gpDrvLEDRed		= &drvLEDRed;
//	gpDrvBuzzer		= &drvBuzzer;
	gpDrvCAN		= &drvCAN;
	//gpDrvTJA1041	= &drvTJA1041;
	gpDrvPCA82C250	= &drvPCA82C250;
}
