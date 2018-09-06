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
DrvCH7025	*gpDrvCH7025;
DrvGPIO	*gpDrvRelay;
DrvM3366	*gpDrvM3366;
DrvHiResClock	*gpDrvHiResClock;
DrvDipSwitch	*gpDrvDipSwitch;
DrvGPIO		*gpDrvLEDGreen, *gpDrvLEDRed;	// LED和蜂鸣器本质上可以使用一个驱动
DrvCAN		*gpDrvCAN;
DrvTJA1041	*gpDrvTJA1041;
DrvTJA1041	*gpDrvTJA1041_2;
DrvI2C		*gpDrvI2C;
DrvTW8835	*gpDrvTW8835;
DrvGPIO	*gpDrvACC_CTRL;
DrvGPIO	*gpDrvREVERSE_CTRL;
DrvGPIO	*gpDrvCAM_PWR_CTRL;
DrvGPIO  *gpDrvH_LAMP_ACC;
DrvGPIO *gpDrvReverseSigDetector;
DrvGPIO *gpDrvRecoderExistsDec;
/* GPIO driver */
static ResGPIO resRecoderExistsDec = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_6,// PIN号
		.Direction	 			= false,
};

static ResGPIO resH_LAMP_ACC = {
		.RCC_APBPeriph_NUM	= RCC_APB2Periph_GPIOB,
		.GPIO_Pin		= GPIO_Pin_1,
		.GPIO_Port		= GPIOB,	
		.Direction		= true,
};

static ResGPIO resACC_CTRL = {
		.RCC_APBPeriph_NUM	= RCC_APB2Periph_GPIOA,
		.GPIO_Pin		= GPIO_Pin_8,
		.GPIO_Port		= GPIOA,	
		.Direction		= true,
};

static ResGPIO resREVERSE_CTRL = {
		.RCC_APBPeriph_NUM	= RCC_APB2Periph_GPIOA,
		.GPIO_Pin		= GPIO_Pin_7,
		.GPIO_Port		= GPIOA,	
		.Direction		= true,
};

static ResGPIO resCAM_PWR_CTRL = {
		.RCC_APBPeriph_NUM	= RCC_APB2Periph_GPIOB,
		.GPIO_Pin		= GPIO_Pin_0,
		.GPIO_Port		= GPIOB,	
		.Direction		= true,
};

static ResGPIO resRelay = {
		.RCC_APBPeriph_NUM	= RCC_APB2Periph_GPIOB,
		.GPIO_Pin		= GPIO_Pin_15,
		.GPIO_Port		= GPIOB,
		.Direction		= true,
};

static ResGPIO resLEDGreen = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_15,// PIN号
		.Direction	 			= true,
};

static ResGPIO resLEDRed = {
		.RCC_APBPeriph_NUM			= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_14,			// PIN号
		.Direction				= true,
};

static ResGPIO resReverseSig = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOB,
		.GPIO_Port	= GPIOB,
		.GPIO_Pin	= GPIO_Pin_14,
		.Direction	= false, //input 
};  

/*  拨码开关  */
static uint16_t dipPinArray[] = {
		GPIO_Pin_3,
		GPIO_Pin_4,
		GPIO_Pin_5,
		GPIO_Pin_6,
};
static ResDipSwitch resDipSwitch = {
		.pinNr			= 4,
		.RCC_APB2Periph = RCC_APB2Periph_GPIOA,
		.pGPIO_pinArray = dipPinArray,
		.GPIO_port		= GPIOA,
};

/* USART */
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

/* CAN 芯片 */
ResTJA1041 resTJA1041 = {
		.RCC_Periph		= RCC_APB2Periph_GPIOA,
		.en_GPIO_Port	= GPIOA,
		.en_GPIO_Pin	= GPIO_Pin_0,
		
		.stb_GPIO_Port	= GPIOA,
		.stb_GPIO_Pin	= GPIO_Pin_1,

		.wake_GPIO_Pin	= (uint16_t)-1,

		.err_GPIO_Port	= GPIOA,
		.err_GPIO_Pin	= GPIO_Pin_2,
};

ResTJA1041 resTJA1041_2 = {
		.RCC_Periph		= RCC_APB2Periph_GPIOC,

		.en_GPIO_Port	= GPIOC,
		.en_GPIO_Pin	= GPIO_Pin_0,
		
		.stb_GPIO_Port	= GPIOC,
		.stb_GPIO_Pin	= GPIO_Pin_1,

		.wake_GPIO_Pin	= (uint16_t)-1,

		.err_GPIO_Port	= GPIOC,
		.err_GPIO_Pin	= GPIO_Pin_2,
};

static struct ResTW8835 resTW8835 = {
		.RCC_APB2Periph_Reset = RCC_APB2Periph_GPIOB,
		.GPIO_Port_Reset = GPIOB,
		.GPIO_Pin_Reset = GPIO_Pin_9,
		.RCC_APB2Periph_ISP = RCC_APB2Periph_GPIOB,
		.GPIO_Port_ISP = GPIOB,
		.GPIO_Pin_ISP = GPIO_Pin_5,
		.RCC_APB2Periph_McuEn = RCC_APB2Periph_GPIOB,
		.GPIO_Port_McuEn = GPIOB,
		.GPIO_Pin_McuEn = GPIO_Pin_2,
};

void bspConstructDrivers (void)
{
	static DrvUSART		drvUSART(&resUSART1, true);
	static DrvCAN		drvCAN;
	static DrvTJA1041	drvTJA1041(&resTJA1041);
	static DrvTJA1041	drvTJA1041_2(&resTJA1041_2);
	
	static DrvI2C		drvI2C;
	static DrvTW8835	drvTW8835(&resTW8835);

	static DrvCH7025	drvCH7025;
	static DrvGPIO		drvRelay(&resRelay);
	
	static DrvGPIO		drvACC_CTRL(&resACC_CTRL);
	static DrvGPIO		drvREVERSE_CTRL(&resREVERSE_CTRL);
	static DrvGPIO		drvCAM_PWR_CTRL(&resCAM_PWR_CTRL);
	static DrvGPIO		drvH_LAMP_ACC(&resH_LAMP_ACC);
	
	static DrvGPIO 		drvReverseSigDetector(&resReverseSig,false);		
	static DrvGPIO		drvRecoderExistsDec(&resRecoderExistsDec,false);
	
	static DrvDipSwitch		drvDipSwitch(&resDipSwitch);

	static DrvGPIO		drvLEDGreen(&resLEDGreen,false);
	static DrvGPIO		drvLEDRed(&resLEDRed,false);

	gpDrvUSART		= &drvUSART;
	
	gpDrvDipSwitch	= &drvDipSwitch;
	gpDrvLEDGreen	= &drvLEDGreen;
	gpDrvLEDRed		= &drvLEDRed;
	gpDrvCAN		= &drvCAN;
	gpDrvTJA1041	= &drvTJA1041;
	gpDrvTJA1041_2	= &drvTJA1041_2;

	gpDrvI2C		= &drvI2C;
	gpDrvTW8835		= &drvTW8835;

	gpDrvCH7025		= &drvCH7025;
	gpDrvRelay		= &drvRelay;
	gpDrvACC_CTRL	= &drvACC_CTRL;
	gpDrvREVERSE_CTRL	= &drvREVERSE_CTRL;
	gpDrvCAM_PWR_CTRL	= &drvCAM_PWR_CTRL;
	gpDrvH_LAMP_ACC		= &drvH_LAMP_ACC;
	gpDrvReverseSigDetector = &drvReverseSigDetector;
	gpDrvRecoderExistsDec = &drvRecoderExistsDec;
}
