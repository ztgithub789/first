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

DrvUSART	*gpDrvUSART;
//DrvGPIO	*gpDrvRelay;
//DrvHiResClock	*gpDrvHiResClock;
DrvGPIO		*gpDrvLEDGreen, *gpDrvLEDRed;	// LED和蜂鸣器本质上可以使用一个驱动
DrvCAN		*gpDrvCAN;
DrvTJA1041	*gpDrvTJA1041;
//DrvTJA1041	*gpDrvTJA1041_2;
//DrvI2C		*gpDrvI2C;
//DrvTW8835	*gpDrvTW8835;

DrvGPIO		*gpDrvReverseSig;
//DrvWatchDog	*gpDrvWatchDog;
//
//DrvTW8835GPIO	*gpDrvCamPowerCon;
//DrvTW8835GPIO	*gpDrvReverseCtrl;
//DrvTW8835GPIO	*gpDrvSwitch1Pin;
//DrvTW8835GPIO	*gpDrvSwitch2Pin;
//DrvTW8835GPIO	*gpDrvSwitch3Pin;
//DrvTW8835GPIO	*gpDrvSwitch4Pin;
//
//#define REVERSE_CTRL_PORT_PIN 0x65
//#define CAM_PWR_CTRL_PORT_PIN 0x64
//#define SWITCH_1_PORT_PIN	0x63
//#define SWITCH_2_PORT_PIN	0x62
//#define SWITCH_3_PORT_PIN	0x61
//#define SWITCH_4_PORT_PIN	0x60

//static ResGPIO resRelay = {
//		.RCC_APBPeriph_NUM	= RCC_APB2Periph_GPIOC,
//		.GPIO_Pin		= GPIO_Pin_10,
//		.GPIO_Port		= GPIOC,
//		.Direction		= true,
//};

static ResGPIO resReverseSig = {
	.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOB,
		.GPIO_Port	= GPIOB,
		.GPIO_Pin	= GPIO_Pin_2,
		.Direction	= false, //input
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
				.USART_BaudRate = 57600,
				.USART_WordLength = USART_WordLength_8b,
				.USART_StopBits = USART_StopBits_1,
				.USART_Parity = USART_Parity_No,
				.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
				.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
		},
};

static ResGPIO resLEDGreen = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOD,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_3,			// PIN号
		.Direction				= true,
};

static ResGPIO resLEDRed = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_2,			// PIN号
		.Direction				= true,
};

ResTJA1041 resTJA1041 = {
		.RCC_Periph		= RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,
		
		.en_GPIO_Port	= GPIOC,
		.en_GPIO_Pin	= GPIO_Pin_11,
		
		.stb_GPIO_Port	= GPIOB,
		.stb_GPIO_Pin	= GPIO_Pin_11,

		.wake_GPIO_Pin	= (uint16_t)-1,

		.err_GPIO_Port	= GPIOB,
		.err_GPIO_Pin	= GPIO_Pin_10,
};
//ResTJA1041 resTJA1041_2 = {
//		.RCC_Periph		= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC,
//
//		.en_GPIO_Port	= GPIOA,
//		.en_GPIO_Pin	= GPIO_Pin_8,
//
//		.stb_GPIO_Port	= GPIOC,
//		.stb_GPIO_Pin	= GPIO_Pin_8,
//
//		.wake_GPIO_Pin	= (uint16_t)-1,
//
//		.err_GPIO_Port	= GPIOC,
//		.err_GPIO_Pin	= GPIO_Pin_9,
//};

//static struct ResTW8835 resTW8835 = {
//		.RCC_APB2Periph_Reset = RCC_APB2Periph_GPIOB,
//		.GPIO_Port_Reset = GPIOB,
//		.GPIO_Pin_Reset = GPIO_Pin_0,
//		.RCC_APB2Periph_ISP = RCC_APB2Periph_GPIOA,
//		.GPIO_Port_ISP = GPIOA,
//		.GPIO_Pin_ISP = GPIO_Pin_1,
//		.RCC_APB2Periph_McuEn = RCC_APB2Periph_GPIOC,
//		.GPIO_Port_McuEn = GPIOC,
//		.GPIO_Pin_McuEn = GPIO_Pin_5,
//};

void bspConstructDrivers (void)
{
	static DrvUSART		drvUSART(&resUSART1, true);
	static DrvCAN		drvCAN;
	static DrvTJA1041	drvTJA1041(&resTJA1041);
//	static DrvTJA1041 	drvTJA1041_2(&resTJA1041_2);
//	static DrvI2C		drvI2C;
//	static DrvTW8835	drvTW8835(&resTW8835);

//	static DrvGPIO		drvRelay(&resRelay);
	static DrvGPIO		drvLEDGreen(&resLEDGreen, false);
	static DrvGPIO		drvLEDRed(&resLEDRed, false);
	static DrvGPIO 		drvReverseSig(&resReverseSig);
	
//	static DrvTW8835GPIO	drvCamPowerCon(CAM_PWR_CTRL_PORT_PIN);
//	static DrvTW8835GPIO	drvReverseCtrl(REVERSE_CTRL_PORT_PIN);
//	static DrvTW8835GPIO	drvSwitch1Pin(SWITCH_1_PORT_PIN,false);
//	static DrvTW8835GPIO	drvSwitch2Pin(SWITCH_2_PORT_PIN,false);
//	static DrvTW8835GPIO	drvSwitch3Pin(SWITCH_3_PORT_PIN,false);
//	static DrvTW8835GPIO	drvSwitch4Pin(SWITCH_4_PORT_PIN,false);

	gpDrvUSART		= &drvUSART;
	gpDrvLEDGreen	= &drvLEDGreen;
	gpDrvLEDRed		= &drvLEDRed;
	gpDrvCAN		= &drvCAN;
	gpDrvTJA1041	= &drvTJA1041;
//	gpDrvTJA1041_2	= &drvTJA1041_2;
//	gpDrvI2C		= &drvI2C;
//	gpDrvTW8835		= &drvTW8835;
//	gpDrvRelay		= &drvRelay;
	gpDrvReverseSig	= &drvReverseSig;

//	gpDrvCamPowerCon = &drvCamPowerCon;
//	gpDrvReverseCtrl = &drvReverseCtrl;
//	gpDrvSwitch1Pin = &drvSwitch1Pin;
//	gpDrvSwitch2Pin = &drvSwitch2Pin;
//	gpDrvSwitch3Pin = &drvSwitch3Pin;
//	gpDrvSwitch4Pin = &drvSwitch4Pin;

}
