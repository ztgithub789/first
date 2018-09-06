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

DrvGPIO 	*gpDrvReverseSigDetector;
DrvGPIO		*gpDrvNavPwrOnSelector;
DrvGPIO		*gpDrvIRDataPin;

DrvTW8835GPIO	*gpDrvCamPwrOnPin;
DrvTW8835GPIO	*gpDrvAV1PwrOnPin;
DrvTW8835GPIO	*gpDrvAV2PwrOnPin;
DrvTW8835GPIO 	*gpDrvAVSwitch1Pin;
DrvTW8835GPIO	*gpDrvAVSwitch2Pin;
DrvTW8835GPIO	*gpDrvAudioSelPin;
DrvTW8835GPIO	*gpDrvNavAsdSelPin;
DrvTW8835GPIO	*gpDrvLvdsSelPin;
DrvTW8835GPIO	*gpDrvTrfbSelPin;
DrvTW8835GPIO	*gpDrvDsEnPin;
DrvTW8835GPIO	*gpDrvVodSelPin;

DrvExti		*gpDrvExti_VSYNC;
DrvExti		*gpDrvExti_DE;
DrvGPIO		*gpDrvBackLightDataPin;

#define CAM_PWRON_PORT_PIN	0x60
#define AV1_PWRON_PORT_PIN	0x62
#define AV2_PWRON_PORT_PIN	0x61
#define AV_SWITCH1_PORT_PIN	0x64
#define AV_SWITCH2_PORT_PIN	0x65
#define AUDIO_SEL_PORT_PIN	0x63
#define NAV_ASD_PORT_PIN	0x57
#define LVDS_SEL_PORT_PIN	0x45
#define DS_TRFB_PORT_PIN	0x44
#define DS_EN_PORT_PIN		0x43
#define DS_VODSEL_PORT_PIN	0x42

static ResGPIO resIRDataPin = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOB,
		.GPIO_Pin	= GPIO_Pin_9,
		.GPIO_Port	= GPIOB,
		.Direction	= true,
};

static ResGPIO resNAVPWRONPin = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOA,
		.GPIO_Pin	= GPIO_Pin_0,
		.GPIO_Port	= GPIOA,
		.Direction	= true,
};

static ResGPIO resReverseSig = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOA,
		.GPIO_Port	= GPIOA,
		.GPIO_Pin	= GPIO_Pin_8,
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
		.GPIO_Port				= GPIOC,				// GPIO¿Ú
		.GPIO_Pin				= GPIO_Pin_15,			// PINºÅ
};

static ResLED resLEDRed = {
		.RCC_APB2Periph			= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO¿Ú
		.GPIO_Pin				= GPIO_Pin_14,			// PINºÅ
};

ResTJA1041 resTJA1041 = {
		.RCC_Periph		= RCC_APB2Periph_GPIOA,

		.en_GPIO_Pin	= (uint16_t)-1,
		.stb_GPIO_Pin	= (uint16_t)-1,
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
	static DrvGPIO		drvNAVPwrOnPin(&resNAVPWRONPin);
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
	static DrvTW8835GPIO	drvTrfbSelPin(DS_TRFB_PORT_PIN);
	static DrvTW8835GPIO	drvDsEnPin(DS_EN_PORT_PIN);
	static DrvTW8835GPIO	drvVodSelPin(DS_VODSEL_PORT_PIN);

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
	//gpDrvRelay		= &drvRelay;
	gpDrvNavPwrOnSelector = &drvNAVPwrOnPin;
    gpDrvReverseSigDetector = &drvReverseSigDetector;
    gpDrvTimer		= &drvTimer;
    gpDrvIRDataPin	= &drvIRDataPin;

    gpDrvCamPwrOnPin	= &drvCamPwrOnPin;
    gpDrvAV1PwrOnPin 	= &drvAV1PwrOnPin;
    gpDrvAV2PwrOnPin 	= &drvAV2PwrOnPin;
    gpDrvAVSwitch1Pin	= &drvAVSwitch1Pin;
    gpDrvAVSwitch2Pin	= &drvAVSwitch2Pin;
    gpDrvAudioSelPin	= &drvAudioSelPin;
    gpDrvNavAsdSelPin	= &drvNavAsdSelPin;
    gpDrvLvdsSelPin	= &drvLvdsSelPin;
    gpDrvTrfbSelPin	= &drvTrfbSelPin;
    gpDrvDsEnPin		= &drvDsEnPin;
    gpDrvVodSelPin		= &drvVodSelPin;
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
	return gpDrvReverseSigDetector->getValue();
}
