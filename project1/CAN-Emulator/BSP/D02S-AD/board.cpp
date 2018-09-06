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
DrvUSART	*gpDrvUSART3;
DrvCH7025	*gpDrvCH7025;
DrvGPIO		*gpDrvRelay;
DrvM3366	*gpDrvM3366;
DrvHiResClock	*gpDrvHiResClock;
DrvDipSwitch	*gpDrvDipSwitch;
DrvGPIO		*gpDrvLEDGreen, *gpDrvLEDRed;	// LED和蜂鸣器本质上可以使用一个驱动
DrvCAN		*gpDrvCAN;
DrvTJA1041	*gpDrvTJA1041;
DrvI2C		*gpDrvI2C;
DrvTW8835	*gpDrvTW8835;
DrvDS241	*gpDrvDS241;
DrvGPIO		*gpDrvCamPowerCon;
DrvGPIO		*gpDrvReverseSig;

DrvTW8835GPIO	*gpDrvSwitch3Pin;
DrvTW8835GPIO	*gpDrvSwitch4Pin;

#define SWITCH_3_PORT_PIN	0x60
#define SWITCH_4_PORT_PIN	0x57

const static ResGPIO resCameraPowerCon = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOB,
		.GPIO_Pin	= GPIO_Pin_15,
		.GPIO_Port	= GPIOB,
		.Direction	= true,
};

const static ResGPIO resRelay = {
		.RCC_APBPeriph_NUM	= RCC_APB2Periph_GPIOA,
		.GPIO_Pin		= GPIO_Pin_0,
		.GPIO_Port		= GPIOA,
		.Direction		= true,
};

const static ResUSART resUSART1 = {
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
				.USART_BaudRate = 19200,//9600,
				.USART_WordLength = USART_WordLength_9b,
				.USART_StopBits = USART_StopBits_1,
				.USART_Parity = USART_Parity_Even,
				.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
				.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
		},
};

static uint16_t dipPinArray[] = {
		GPIO_Pin_12,
		GPIO_Pin_13
};
const static ResDipSwitch resDipSwitch = {
		.pinNr			= 2,
		.RCC_APB2Periph = RCC_APB2Periph_GPIOB,
		.pGPIO_pinArray = dipPinArray,
		.GPIO_port		= GPIOB,
};


const static ResGPIO resLEDGreen = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_15,			// PIN号
		.Direction				= true,
};

const static ResGPIO resLEDRed = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_14,			// PIN号
		.Direction				= true,
};

const static ResGPIO resReverseSig = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOB,
		.GPIO_Port	= GPIOB,
		.GPIO_Pin	= GPIO_Pin_14,
		.Direction	= false, //input
};

ResTJA1041 resTJA1041 = {
		.RCC_Periph		= RCC_APB2Periph_GPIOA,

		.en_GPIO_Pin	= (uint16_t)-1,
		.stb_GPIO_Pin	= (uint16_t)-1,
		.wake_GPIO_Pin	= (uint16_t)-1,
		.err_GPIO_Pin	= (uint16_t)-1,
};

ResDS241 resDS241 = {
		.RCC_APB2Periph_EN		= RCC_APB2Periph_GPIOA,
		.GPIO_Port_EN			= GPIOA,
		.GPIO_Pin_EN			= GPIO_Pin_4,

		.RCC_APB2Periph_TRFB	= RCC_APB2Periph_GPIOB,
		.GPIO_Port_TRFB			= GPIOB,
		.GPIO_Pin_TRFB			= GPIO_Pin_9,

		.RCC_APB2Periph_VODSEL	= RCC_APB2Periph_GPIOA,
		.GPIO_Port_VODSEL		= GPIOA,
		.GPIO_Pin_VODSEL		= GPIO_Pin_1,
};

const static struct ResTW8835 resTW8835 = {
		.RCC_APB2Periph_Reset = RCC_APB2Periph_GPIOB,
		.GPIO_Port_Reset = GPIOB,
		.GPIO_Pin_Reset = GPIO_Pin_10,
		.RCC_APB2Periph_ISP = RCC_APB2Periph_GPIOB,
		.GPIO_Port_ISP = GPIOB,
		.GPIO_Pin_ISP = GPIO_Pin_5,
		.RCC_APB2Periph_McuEn = RCC_APB2Periph_GPIOB,
		.GPIO_Port_McuEn = GPIOB,
		.GPIO_Pin_McuEn = GPIO_Pin_2,
};

void bspConstructDrivers (void)
{
	static DrvUSART		drvUSART((ResUSART*)&resUSART1, true);
	static DrvUSART		drvUSART3((ResUSART*)&resUSART3, false);
	static DrvCAN		drvCAN;
	static DrvTJA1041	drvTJA1041((ResTJA1041*)&resTJA1041);
	static DrvI2C		drvI2C;
	static DrvTW8835	drvTW8835((ResTW8835*)&resTW8835);
	static DrvDS241		drvDS241((ResDS241*)&resDS241);
	static DrvGPIO		drvRelay((ResGPIO*)&resRelay);
	static DrvDipSwitch		drvDipSwitch((ResDipSwitch*)&resDipSwitch);
	static DrvGPIO		drvLEDGreen((ResGPIO*)&resLEDGreen, false);
	static DrvGPIO		drvLEDRed((ResGPIO*)&resLEDRed, false);
	static DrvGPIO 		drvDrvCamPowerCon((ResGPIO*)&resCameraPowerCon);
	static DrvGPIO 		drvReverseSig((ResGPIO*)&resReverseSig,false);

	static DrvTW8835GPIO	drvSwitch3Pin(SWITCH_3_PORT_PIN,false);
	static DrvTW8835GPIO	drvSwitch4Pin(SWITCH_4_PORT_PIN,false);

	gpDrvUSART		= &drvUSART;
	gpDrvUSART3		= &drvUSART3;
	gpDrvDipSwitch	= &drvDipSwitch;
	gpDrvLEDGreen	= &drvLEDGreen;
	gpDrvLEDRed		= &drvLEDRed;
	gpDrvReverseSig	= &drvReverseSig;
	gpDrvCAN		= &drvCAN;
	gpDrvTJA1041	= &drvTJA1041;
	gpDrvI2C		= &drvI2C;
	gpDrvTW8835		= &drvTW8835;
	gpDrvDS241		= &drvDS241;
	gpDrvRelay		= &drvRelay;
	gpDrvCamPowerCon = &drvDrvCamPowerCon;

	gpDrvSwitch3Pin = &drvSwitch3Pin;
	gpDrvSwitch4Pin = &drvSwitch4Pin;
}
