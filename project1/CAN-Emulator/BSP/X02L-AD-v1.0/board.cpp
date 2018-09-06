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

#define TW8835_REVERSE_DET_PORT 0x06
#define TW8835_REVERSE_DET_PIN	0x04

DrvUSART	*gpDrvUSART;
DrvUSART	*gpDrvUSART2;
DrvUSART	*gpDrvUSART3;
DrvM3366	*gpDrvM3366;
DrvHiResClock	*gpDrvHiResClock;
DrvDipSwitch	*gpDrvDipSwitch;
DrvLED		*gpDrvLEDGreen, *gpDrvLEDRed;
DrvCAN		*gpDrvCAN;
DrvTJA1041	*gpDrvTJA1041;
DrvI2C		*gpDrvI2C;
DrvTW8835	*gpDrvTW8835;
DrvGPTimer 	*gpDrvTimer;

DrvExti		*gpDrvExti_VSYNC;
DrvExti		*gpDrvExti_DE;

DrvGPIO		*gpDrvBackLightDataPin;
DrvGPIO 	*gpDrvReverseSigDetector;
DrvGPIO		*gpDrvIRDataPin;

DrvTW8835GPIO	*gpDrvCamPwrOnPin;
DrvTW8835GPIO	*gpDrvAV1PwrOnPin;
DrvTW8835GPIO	*gpDrvAV2PwrOnPin;
DrvTW8835GPIO 	*gpDrvAVSwitch1Pin;
DrvTW8835GPIO	*gpDrvAVSwitch2Pin;
DrvTW8835GPIO	*gpDrvAudioSelPin;
DrvTW8835GPIO	*gpDrvNavAsdSelPin;
DrvTW8835GPIO	*gpDrvLvdsSelPin;
DrvTW8835GPIO	*gpDrvDsEnPin;
DrvTW8835GPIO	*gpDrvNavPwrOnSelector;

#define CAM_PWRON_PORT_PIN	0x61
#define AV1_PWRON_PORT_PIN	0x63
#define AV2_PWRON_PORT_PIN	0x62
#define AV_SWITCH1_PORT_PIN	0x66
#define AV_SWITCH2_PORT_PIN	0x67
#define AUDIO_SEL_PORT_PIN	0x65
#define NAV_ASD_PORT_PIN	0x60
#define LVDS_SEL_PORT_PIN	0x45
#define DS_EN_PORT_PIN		0x44
#define NAV_PWRON_PORT_PIN	0x42

static ResGPIO resIRDataPin = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOB,
		.GPIO_Pin	= GPIO_Pin_9,
		.GPIO_Port	= GPIOB,
		.Direction	= true,
};

//static ResGPIO resNAVPWRONPin = {
//		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOA,
//		.GPIO_Pin	= GPIO_Pin_0,
//		.GPIO_Port	= GPIOA,
//		.Direction	= true,
//};

static ResGPIO resReverseSig = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOA,
		.GPIO_Port	= GPIOA,
		.GPIO_Pin	= GPIO_Pin_8,
		.Direction	= false, //input
};

static ResGPIO resBackLightDataPin = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOA,
		.GPIO_Pin	= GPIO_Pin_4,
		.GPIO_Port	= GPIOA,
		.Direction	= true,
};

static ResExti resExti_VSYNC = {
		.GPIO_Port	= GPIOA,			///< GPIO口
		.GPIO_Pin	= GPIO_Pin_0,			///< PIN号
		.RCC_APB2Periph	= RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,		///< RCC外设号
		.EXTI_GPIO_PortSource = GPIO_PortSourceGPIOA,	///< 外部中断GPIO口
		.EXIT_GPIO_PinSource	= GPIO_PinSource0,	///< 外部中断GPIO PIN号
		.EXIT_NVIC_IRQChannel	= EXTI0_IRQn,
		.EXTI_InitStructure = { //EXTI_Init()参数，用于设置中断特性
				.EXTI_Line = EXTI_Line0,
				.EXTI_Mode = EXTI_Mode_Interrupt, //
				.EXTI_Trigger = EXTI_Trigger_Rising, //
				.EXTI_LineCmd = ENABLE,  
		},
};

static ResExti resExti_DE = {
		.GPIO_Port	= GPIOA,			///< GPIO口
		.GPIO_Pin	= GPIO_Pin_1,			///< PIN号
		.RCC_APB2Periph	= RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,		///< RCC外设号
		.EXTI_GPIO_PortSource = GPIO_PortSourceGPIOA  ,	///< 外部中断GPIO口
		.EXIT_NVIC_IRQChannel	= EXTI1_IRQn,
		.EXIT_GPIO_PinSource	= GPIO_PinSource1,	///< 外部中断GPIO PIN号
		.EXTI_InitStructure = { //EXTI_Init()参数，用于设置中断特性
				.EXTI_Line = EXTI_Line1,
				.EXTI_Mode = EXTI_Mode_Interrupt, //
				.EXTI_Trigger = EXTI_Trigger_Rising, //
				.EXTI_LineCmd = ENABLE,  
		},
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

static ResUSART resUSART2 = {
		.USARTx					= USART2,
		.RCC_APBPeriph_USARTx	= RCC_APB1Periph_USART2,
		.RCC_APBPeriph_rx		= RCC_APB2Periph_GPIOA,
		.GPIO_rx				= GPIOA,
		.GPIO_Pin_rx			= GPIO_Pin_3,
		.RCC_APBPeriph_tx		= RCC_APB2Periph_GPIOA,
		.GPIO_tx				= GPIOA,
		.GPIO_Pin_tx			= GPIO_Pin_2,
		.NVIC_IRQChannel		= USART2_IRQn,
		.NVIC_IRQChannelSubPriority = 0x3,
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
		GPIO_Pin_15,
		GPIO_Pin_14,
		GPIO_Pin_13,
		GPIO_Pin_12
};
static ResDipSwitch resDipSwitch = {
		.pinNr			= 4,
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
		.RCC_Periph		= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,

		.en_GPIO_Port 	= GPIOA,
		.en_GPIO_Pin	= GPIO_Pin_8,

		.stb_GPIO_Port	= GPIOB,
		.stb_GPIO_Pin	= GPIO_Pin_11,

		.wake_GPIO_Pin	= (uint16_t)-1,
		.err_GPIO_Pin	= (uint16_t)-1,
};

static struct ResTW8835 resTW8835 = {
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
	static DrvUSART		drvUSART(&resUSART1, true);
    static DrvUSART		drvUSART2(&resUSART2, false);
	static DrvUSART		drvUSART3(&resUSART3, false);
	static DrvCAN		drvCAN;
	static DrvTJA1041	drvTJA1041(&resTJA1041);
	static DrvI2C		drvI2C;
	static DrvTW8835	drvTW8835(&resTW8835);
	static DrvDipSwitch		drvDipSwitch(&resDipSwitch);
	static DrvExti		drvExti_VSYNC(&resExti_VSYNC);
	static DrvExti		drvExti_DE(&resExti_DE);
	static DrvGPIO		drvBackLightDataPin(&resBackLightDataPin,true);
	static DrvLED		drvLEDGreen(&resLEDGreen, false);
	static DrvLED		drvLEDRed(&resLEDRed, false);
	static DrvGPIO 		drvReverseSigDetector(&resReverseSig,false);

	static DrvGPIO		drvIRDataPin(&resIRDataPin,true);
	static DrvGPTimer 	drvTimer;

	static DrvTW8835GPIO	drvCamPwrOnPin(CAM_PWRON_PORT_PIN);
	static DrvTW8835GPIO	drvAV1PwrOnPin(AV1_PWRON_PORT_PIN);
	static DrvTW8835GPIO	drvAV2PwrOnPin(AV2_PWRON_PORT_PIN);
	static DrvTW8835GPIO 	drvAVSwitch1Pin(AV_SWITCH1_PORT_PIN);
	static DrvTW8835GPIO	drvAVSwitch2Pin(AV_SWITCH2_PORT_PIN);
	static DrvTW8835GPIO	drvAudioSelPin(AUDIO_SEL_PORT_PIN);
	static DrvTW8835GPIO	drvNavAsdSelPin(NAV_ASD_PORT_PIN);
	static DrvTW8835GPIO	drvLvdsSelPin(LVDS_SEL_PORT_PIN);
	static DrvTW8835GPIO	drvDsEnPin(DS_EN_PORT_PIN);
	static DrvTW8835GPIO	DrvNavPwrOnSelector(NAV_PWRON_PORT_PIN);

	gpDrvUSART		= &drvUSART;
	gpDrvUSART2		= &drvUSART2;
	gpDrvUSART3		= &drvUSART3;
	gpDrvDipSwitch	= &drvDipSwitch;
	gpDrvLEDGreen	= &drvLEDGreen;
	gpDrvLEDRed		= &drvLEDRed;
	gpDrvCAN		= &drvCAN;
	gpDrvTJA1041	= &drvTJA1041;
	gpDrvI2C		= &drvI2C;
	gpDrvTW8835		= &drvTW8835;
	gpDrvExti_VSYNC 	= &drvExti_VSYNC;
	gpDrvExti_DE	= &drvExti_DE;
	gpDrvBackLightDataPin	= &drvBackLightDataPin;
    gpDrvTimer		= &drvTimer;
    gpDrvIRDataPin	= &drvIRDataPin;

    gpDrvCamPwrOnPin	= &drvCamPwrOnPin;
    gpDrvAV1PwrOnPin 	= &drvAV1PwrOnPin;
    gpDrvAV2PwrOnPin 	= &drvAV2PwrOnPin;
    gpDrvAVSwitch1Pin	= &drvAVSwitch1Pin;
    gpDrvAVSwitch2Pin	= &drvAVSwitch2Pin;
    gpDrvAudioSelPin	= &drvAudioSelPin;
    gpDrvNavAsdSelPin	= &drvNavAsdSelPin;
    gpDrvLvdsSelPin		= &drvLvdsSelPin;
    gpDrvDsEnPin		= &drvDsEnPin;
    gpDrvNavPwrOnSelector	= &DrvNavPwrOnSelector;
}

ReverseSigDetector* ReverseSigDetector::getInstance(void)
{
	static ReverseSigDetector *pInstance = 0;
	if (pInstance == 0) {
		pInstance = new ReverseSigDetector;
	}
	return pInstance;
}

ReverseSigDetector::~ReverseSigDetector(void){
}

bool ReverseSigDetector::getValue(void)
{
	if(gpDrvTW8835->readPin(TW8835_REVERSE_DET_PORT, TW8835_REVERSE_DET_PIN)) {
		return false;
	} else {
		return true;
	}
}
