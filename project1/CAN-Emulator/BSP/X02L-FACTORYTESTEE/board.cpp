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
DrvGPIO		*gpDrvLEDGreen, *gpDrvLEDRed;
DrvCAN		*gpDrvCAN;
DrvTJA1041	*gpDrvTJA1041;
DrvI2C		*gpDrvI2C;
DrvTW8835	*gpDrvTW8835;
DrvWM8875	*gpDrvWM8875;
DrvGPTimer 	*gpDrvTimer;

DrvExti		*gpDrvExti_VSYNC;
DrvExti		*gpDrvExti_DE;
//DrvRelay	*gpDrvRelay;

DrvGPIO		*gpDrvBackLightDataPin;
DrvGPIO 	*gpDrvReverseSigDetector;
DrvGPIO		*gpDrvNavPwrOnSelector;
DrvGPIO		*gpDrvIRDataPin;

DrvGPIO		*gpDrvCamPwrOnPin;
DrvGPIO		*gpDrvAV1PwrOnPin;
DrvGPIO		*gpDrvAV2PwrOnPin;
DrvGPIO		*gpDrvLvdsSelPin;
DrvGPIO		*gpDrvDsEnPin;
DrvGPIO		*gpDrvNAVnResetPin;
DrvGPIO		*gpDrvCMMBPwrOnPin;
DrvGPIO		*gpDrvCMMBnResetPin;
DrvGPIO		*gpDrvNavAudioMixPin;

DrvGPIO		*gpDrvUSART2TxPin;
DrvGPIO		*gpDrvUSART2RxPin;

DrvTW8835GPIO 	*gpDrvAVSwitch1Pin;
DrvTW8835GPIO	*gpDrvAVSwitch2Pin;
DrvTW8835GPIO	*gpDrvNavAsdSelPin;
#define AV_SWITCH1_PORT_PIN	0x00
#define AV_SWITCH2_PORT_PIN	0x01
#define NAV_ASD_PORT_PIN	0x62

const static ResGPIO resNavAudioMixPin = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOD,
		.GPIO_Pin	= GPIO_Pin_2,
		.GPIO_Port	= GPIOD,
		.Direction	= false,
};

const static ResGPIO resCamPwrOnPin = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOB,
		.GPIO_Pin	= GPIO_Pin_9,
		.GPIO_Port	= GPIOB,
		.Direction	= true,
};

const static ResGPIO resAV1PwrOnPin = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOB,
		.GPIO_Pin	= GPIO_Pin_5,
		.GPIO_Port	= GPIOB,
		.Direction	= true,
};

const static ResGPIO resAV2PwrOnPin = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOB,
		.GPIO_Pin	= GPIO_Pin_8,
		.GPIO_Port	= GPIOB,
		.Direction	= true,
};

const static ResGPIO resLvdsSelPin = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOC,
		.GPIO_Pin	= GPIO_Pin_3,
		.GPIO_Port	= GPIOC,
		.Direction	= true,
};

const static ResGPIO resDsEnPin = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOB,
		.GPIO_Pin	= GPIO_Pin_0,
		.GPIO_Port	= GPIOB,
		.Direction	= true,
};

const static ResGPIO resNAVnResetPin = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOC,
		.GPIO_Pin	= GPIO_Pin_6,
		.GPIO_Port	= GPIOC,
		.Direction	= true,
};

const static ResGPIO resCMMBPwrOnPin = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOC,
		.GPIO_Pin	= GPIO_Pin_7,
		.GPIO_Port	= GPIOC,
		.Direction	= true,
};

const static ResGPIO resCMMBnResetPin = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOC,
		.GPIO_Pin	= GPIO_Pin_8,
		.GPIO_Port	= GPIOC,
		.Direction	= true,
};

const static ResGPIO resIRDataPin = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOC,
		.GPIO_Pin	= GPIO_Pin_9,
		.GPIO_Port	= GPIOC,
		.Direction	= true,
};

const static ResGPIO resNAVPWRONPin = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOB,
		.GPIO_Pin	= GPIO_Pin_15,
		.GPIO_Port	= GPIOB,
		.Direction	= true,
};

const static ResGPIO resReverseSig = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOC,
		.GPIO_Port	= GPIOC,
		.GPIO_Pin	= GPIO_Pin_13,
		.Direction	= false, //input
};

const static ResGPIO resBackLightDataPin = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOA,
		.GPIO_Pin	= GPIO_Pin_0,
		.GPIO_Port	= GPIOA,
		.Direction	= true,
};

const static ResGPIO resUSART2TxPin = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOA,
		.GPIO_Pin	= GPIO_Pin_2,
		.GPIO_Port	= GPIOA,
		.Direction	= true,
};

const static ResGPIO resUSART2RxPin = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOA,
		.GPIO_Pin	= GPIO_Pin_3,
		.GPIO_Port	= GPIOA,
		.Direction	= true,
};

const static ResExti resExti_VSYNC = {
		.GPIO_Port	= GPIOA,			///< GPIO口
		.GPIO_Pin	= GPIO_Pin_4,			///< PIN号
		.RCC_APB2Periph	= RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,		///< RCC外设号
		.EXTI_GPIO_PortSource = GPIO_PortSourceGPIOA,	///< 外部中断GPIO口
		.EXIT_GPIO_PinSource	= GPIO_PinSource4,	///< 外部中断GPIO PIN号
		.EXIT_NVIC_IRQChannel	= EXTI4_IRQn,
		.EXTI_InitStructure = { //EXTI_Init()参数，用于设置中断特性
				.EXTI_Line = EXTI_Line4,
				.EXTI_Mode = EXTI_Mode_Interrupt, //
				.EXTI_Trigger = EXTI_Trigger_Rising, //
				.EXTI_LineCmd = ENABLE,  
		},
};

const static ResExti resExti_DE = {
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

static uint16_t dipPinArray[] = {
		GPIO_Pin_12,
		GPIO_Pin_11,
		GPIO_Pin_10,
};
static ResDipSwitch resDipSwitch = {
		.pinNr			= 3,
		.RCC_APB2Periph = RCC_APB2Periph_GPIOC,
		.pGPIO_pinArray = dipPinArray,
		.GPIO_port		= GPIOC,
};

const static ResGPIO resLEDGreen = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOC,
		.GPIO_Port	= GPIOC,
		.GPIO_Pin	= GPIO_Pin_2,
		.Direction	= true,
};

const static ResGPIO resLEDRed = {
		.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOC,
		.GPIO_Port	= GPIOC,
		.GPIO_Pin	= GPIO_Pin_1,
		.Direction	= true,
};

const static ResTJA1041 resTJA1041 = {
		.RCC_Periph		= RCC_APB2Periph_GPIOC,

		.en_GPIO_Port	= GPIOC,
		.en_GPIO_Pin	= GPIO_Pin_15,

		.stb_GPIO_Port 	= GPIOC,
		.stb_GPIO_Pin	= GPIO_Pin_14,

		.wake_GPIO_Pin	= (uint16_t)-1,

		.err_GPIO_Port	= GPIOC,
		.err_GPIO_Pin	= GPIO_Pin_0,
};

const static struct ResTW8835 resTW8835 = {
		.RCC_APB2Periph_Reset = RCC_APB2Periph_GPIOB,
		.GPIO_Port_Reset = GPIOB,
		.GPIO_Pin_Reset = GPIO_Pin_2,
		.RCC_APB2Periph_ISP = RCC_APB2Periph_GPIOB,
		.GPIO_Port_ISP = GPIOB,
		.GPIO_Pin_ISP = GPIO_Pin_14,
		.RCC_APB2Periph_McuEn = RCC_APB2Periph_GPIOB,
		.GPIO_Port_McuEn = GPIOB,
		.GPIO_Pin_McuEn = GPIO_Pin_1,
};

void bspConstructDrivers (void)
{
	static DrvUSART		drvUSART((ResUSART*)&resUSART1, true);

	static DrvCAN		drvCAN;
	static DrvTJA1041	drvTJA1041((ResTJA1041*)&resTJA1041);
	static DrvI2C		drvI2C;
	static DrvTW8835	drvTW8835((ResTW8835*)&resTW8835);
	static DrvWM8875	drvWM8875;
	//static DrvDipSwitch		drvDipSwitch((ResDipSwitch*)&resDipSwitch);
	static DrvExti		drvExti_VSYNC((ResExti*)&resExti_VSYNC);
	static DrvExti		drvExti_DE((ResExti*)&resExti_DE);
	static DrvGPIO		drvBackLightDataPin((ResGPIO*)&resBackLightDataPin,true);
//	static DrvRelay		drvRelay(&resRelay);
	static DrvGPIO		drvNAVPwrOnPin((ResGPIO*)&resNAVPWRONPin);

	static DrvGPIO		drvCamPwrOnPin((ResGPIO*)&resCamPwrOnPin);
	static DrvGPIO		drvAV1PwrOnPin((ResGPIO*)&resAV1PwrOnPin);
	static DrvGPIO		drvAV2PwrOnPin((ResGPIO*)&resAV2PwrOnPin);
	static DrvGPIO		drvLvdsSelPin((ResGPIO*)&resLvdsSelPin);
	static DrvGPIO		drvDsEnPin((ResGPIO*)&resDsEnPin);
	static DrvGPIO		drvNAVnResetPin((ResGPIO*)&resNAVnResetPin);
	static DrvGPIO		drvCMMBPwrOnPin((ResGPIO*)&resCMMBPwrOnPin);
	static DrvGPIO		drvCMMBnResetPin((ResGPIO*)&resCMMBnResetPin);

	static DrvGPIO		drvNavAudioMixPin((ResGPIO*)&resNavAudioMixPin,false);

	static DrvGPIO		drvUSART2TxPin((ResGPIO*)&resUSART2TxPin);
	static DrvGPIO		drvUSART2RxPin((ResGPIO*)&resUSART2RxPin);

	static DrvGPIO		drvLEDGreen((ResGPIO*)&resLEDGreen, false);
	static DrvGPIO		drvLEDRed((ResGPIO*)&resLEDRed, false);
	static DrvGPIO 		drvReverseSigDetector((ResGPIO*)&resReverseSig,false);

	static DrvGPIO		drvIRDataPin((ResGPIO*)&resIRDataPin,true);
	static DrvGPTimer 	drvTimer;

	static DrvTW8835GPIO 	drvAVSwitch1Pin(AV_SWITCH1_PORT_PIN);
	static DrvTW8835GPIO	drvAVSwitch2Pin(AV_SWITCH2_PORT_PIN);
	static DrvTW8835GPIO	drvNavAsdSelPin(NAV_ASD_PORT_PIN);

	gpDrvUSART		= &drvUSART;
	//gpDrvDipSwitch	= &drvDipSwitch;
	gpDrvLEDGreen	= &drvLEDGreen;
	gpDrvLEDRed		= &drvLEDRed;
	gpDrvCAN		= &drvCAN;
	gpDrvTJA1041	= &drvTJA1041;
	gpDrvI2C		= &drvI2C;
	gpDrvTW8835		= &drvTW8835;
	gpDrvWM8875		= &drvWM8875;

	gpDrvExti_VSYNC 	= &drvExti_VSYNC;
	gpDrvExti_DE	= &drvExti_DE;
	gpDrvBackLightDataPin	= &drvBackLightDataPin;
	//gpDrvRelay		= &drvRelay;
	gpDrvNavPwrOnSelector	= &drvNAVPwrOnPin;
	gpDrvCamPwrOnPin	= &drvCamPwrOnPin;
	gpDrvAV1PwrOnPin	= &drvAV1PwrOnPin;
	gpDrvAV2PwrOnPin	= &drvAV2PwrOnPin;
	gpDrvLvdsSelPin	= &drvLvdsSelPin;
	gpDrvDsEnPin	= &drvDsEnPin;
	gpDrvNAVnResetPin	= &drvNAVnResetPin;
	gpDrvCMMBPwrOnPin	= &drvCMMBPwrOnPin;
	gpDrvCMMBnResetPin	= &drvCMMBnResetPin;

    gpDrvReverseSigDetector = &drvReverseSigDetector;
    gpDrvTimer		= &drvTimer;
    gpDrvIRDataPin	= &drvIRDataPin;

    gpDrvAVSwitch1Pin	= &drvAVSwitch1Pin;
    gpDrvAVSwitch2Pin	= &drvAVSwitch2Pin;
    gpDrvNavAsdSelPin	= &drvNavAsdSelPin;

    gpDrvNavAudioMixPin = &drvNavAudioMixPin;

    gpDrvUSART2TxPin	= &drvUSART2TxPin;
    gpDrvUSART2RxPin	= &drvUSART2RxPin;
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
