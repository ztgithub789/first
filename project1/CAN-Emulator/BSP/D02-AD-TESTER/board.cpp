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
DrvHiResClock	*gpDrvHiResClock;
DrvDipSwitch	*gpDrvDipSwitch;
DrvLED		*gpDrvLEDGreen, *gpDrvLEDRed;	// LED和蜂鸣器本质上可以使用一个驱动
DrvCAN		*gpDrvCAN;
DrvTJA1041	*gpDrvTJA1041;

DrvGPIO		*gpDrvCamPowerCon;
DrvGPIO		*gpDrvReverseSig;

static ResGPIO resCameraPowerCon = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOB,
		.GPIO_Pin	= GPIO_Pin_15,
		.GPIO_Port	= GPIOB,
		.Direction	= true,
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
				.USART_BaudRate = 57600,    //115200
				.USART_WordLength = USART_WordLength_8b,
				.USART_StopBits = USART_StopBits_1,
				.USART_Parity = USART_Parity_No,
				.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
				.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
		},
};

static uint16_t dipPinArray[] = {
		GPIO_Pin_12,
		GPIO_Pin_13
};
static ResDipSwitch resDipSwitch = {
		.pinNr			= 2,
		.RCC_APB2Periph = RCC_APB2Periph_GPIOB,
		.pGPIO_pinArray = dipPinArray,
		.GPIO_port		= GPIOB,
};


static ResLED resLEDGreen = {
		.RCC_APB2Periph			= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_15,			// PIN号
};

static ResLED resLEDRed = {
		.RCC_APB2Periph			= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_14,			// PIN号
};


ResTJA1041 resTJA1041 = {
		.RCC_Periph		= RCC_APB2Periph_GPIOA,
		.en_GPIO_Pin	= -1,
		.stb_GPIO_Pin	= -1,
		.wake_GPIO_Pin	= -1,
		.err_GPIO_Pin	= -1,
};

static ResGPIO resReverseSig = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOB,
		.GPIO_Port	= GPIOB,
		.GPIO_Pin	= GPIO_Pin_14,
		.Direction	= false, //input
};

void bspConstructDrivers (void)
{
	static DrvUSART		drvUSART(&resUSART1, true);
	static DrvCAN		drvCAN;
	static DrvTJA1041	drvTJA1041(&resTJA1041);
	static DrvDipSwitch		drvDipSwitch(&resDipSwitch);
	static DrvLED		drvLEDGreen(&resLEDGreen, false);
	static DrvLED		drvLEDRed(&resLEDRed, false);

	static DrvGPIO 		drvCamPowerCon(&resCameraPowerCon);
	static DrvGPIO 		drvReverseSig(&resReverseSig);

	gpDrvUSART		= &drvUSART;
	gpDrvDipSwitch	= &drvDipSwitch;
	gpDrvLEDGreen	= &drvLEDGreen;
	gpDrvLEDRed		= &drvLEDRed;
	gpDrvCAN		= &drvCAN;
	gpDrvTJA1041	= &drvTJA1041;

	gpDrvCamPowerCon = &drvCamPowerCon;
	gpDrvReverseSig	= &drvReverseSig;
}
