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
DrvGPIO		*gpDrvLEDGreen, *gpDrvLEDRed;	// LED和蜂鸣器本质上可以使用一个驱动
DrvCAN		*gpDrvCAN;
DrvTJA1041	*gpDrvTJA1041;
DrvTJA1041	*gpDrvTJA1041_2;

DrvGPIO *gpDrvLED3;
DrvGPIO *gpDrvLED4;
DrvGPIO *gpDrvLED5;
DrvGPIO *gpDrvLED6;
DrvGPIO *gpDrvLED7;
DrvGPIO *gpDrvLED8;
DrvGPIO *gpDrvLED9;
DrvGPIO *gpDrvLED10;
DrvGPIO *gpDrvLED11;

DrvGPIO *gpDrvCOL1;
DrvGPIO *gpDrvCOL2;
DrvGPIO *gpDrvCOL3;
DrvGPIO *gpDrvCOL4;
DrvGPIO *gpDrvCOL5;

DrvGPIO *gpDrvROW1;
DrvGPIO *gpDrvROW2;
DrvGPIO *gpDrvROW3;
DrvGPIO *gpDrvROW4;
DrvGPIO *gpDrvROW5;


#define REVERSE_CTRL_PORT_PIN 0x65
#define CAM_PWR_CTRL_PORT_PIN 0x64
#define SWITCH_1_PORT_PIN	0x63
#define SWITCH_2_PORT_PIN	0x62
#define SWITCH_3_PORT_PIN	0x61
#define SWITCH_4_PORT_PIN	0x60

static ResGPIO resCOL1 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOB,
		.GPIO_Port				= GPIOB,				
		.GPIO_Pin				= GPIO_Pin_0,			
		.Direction				= false,
};

static ResGPIO resCOL2 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOB,
		.GPIO_Port				= GPIOB,				
		.GPIO_Pin				= GPIO_Pin_1,			
		.Direction				= false,
};

static ResGPIO resCOL3 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOB,
		.GPIO_Port				= GPIOB,				
		.GPIO_Pin				= GPIO_Pin_2,			
		.Direction				= false,
};

static ResGPIO resCOL4 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOB,
		.GPIO_Port				= GPIOB,				
		.GPIO_Pin				= GPIO_Pin_5,			
		.Direction				= false,
};

static ResGPIO resCOL5 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOB,
		.GPIO_Port				= GPIOB,				
		.GPIO_Pin				= GPIO_Pin_6,			
		.Direction				= false,
};

static ResGPIO resROW1 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				
		.GPIO_Pin				= GPIO_Pin_11,			
		.Direction				= true,
};

static ResGPIO resROW2 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				
		.GPIO_Pin				= GPIO_Pin_12,			
		.Direction				= true,
};

static ResGPIO resROW3 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				
		.GPIO_Pin				= GPIO_Pin_13,			
		.Direction				= true,
};

static ResGPIO resROW4 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				
		.GPIO_Pin				= GPIO_Pin_14,			
		.Direction				= true,
};

static ResGPIO resROW5 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				
		.GPIO_Pin				= GPIO_Pin_15,			
		.Direction				= true,
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

static ResGPIO resLEDRed = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_0,			// PIN号
		.Direction				= true,
};

static ResGPIO resLEDGreen = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_1,			// PIN号
		.Direction				= true,
};

static ResGPIO resLED3 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_2,			// PIN号
		.Direction				= true,
};

static ResGPIO resLED4 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_3,			// PIN号
		.Direction				= true,
};

static ResGPIO resLED5 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_4,			// PIN号
		.Direction				= true,
};

static ResGPIO resLED6 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_5,			// PIN号
		.Direction				= true,
};

static ResGPIO resLED7 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_6,			// PIN号
		.Direction				= true,
};

static ResGPIO resLED8 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_7,			// PIN号
		.Direction				= true,
};

static ResGPIO resLED9 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_8,			// PIN号
		.Direction				= true,
};

static ResGPIO resLED10 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_9,			// PIN号
		.Direction				= true,
};

static ResGPIO resLED11 = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOC,
		.GPIO_Port				= GPIOC,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_10,			// PIN号
		.Direction				= true,
};

ResTJA1041 resTJA1041 = {
		.RCC_Periph		= RCC_APB2Periph_GPIOA,
		
		.en_GPIO_Port	= GPIOA,
		.en_GPIO_Pin	= GPIO_Pin_2,
		
		.stb_GPIO_Port	= GPIOA,
		.stb_GPIO_Pin	= GPIO_Pin_1,

		.wake_GPIO_Pin	= (uint16_t)-1,

		.err_GPIO_Port	= GPIOA,
		.err_GPIO_Pin	= GPIO_Pin_0,
};
ResTJA1041 resTJA1041_2 = {
		.RCC_Periph		= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC,
		
		.en_GPIO_Port	= GPIOA,
		.en_GPIO_Pin	= GPIO_Pin_3,
		
		.stb_GPIO_Port	= GPIOA,
		.stb_GPIO_Pin	= GPIO_Pin_4,

		.wake_GPIO_Pin	= (uint16_t)-1,

		.err_GPIO_Port	= GPIOB,
		.err_GPIO_Pin	= GPIO_Pin_10,
};
const static struct ResCAN resCAN1 = {
		.RCC_APB2Periph_CANTx= RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA,
		.GPIO_Port_CANTx= GPIOA,
		.GPIO_Pin_CANTx = GPIO_Pin_12,
		.RCC_APB2Periph_CANRx =RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA,
		.GPIO_Port_CANRx = GPIOA,
		.GPIO_Pin_CANRx = GPIO_Pin_11,
		.GPIO_CAN_Remap = 0,
};

const static struct ResCAN resCAN2 = {
		.RCC_APB2Periph_CANTx= RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB,
		.GPIO_Port_CANTx= GPIOB,
		.GPIO_Pin_CANTx = GPIO_Pin_13,
		.RCC_APB2Periph_CANRx =RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB,
		.GPIO_Port_CANRx = GPIOB,
		.GPIO_Pin_CANRx = GPIO_Pin_12,
		.GPIO_CAN_Remap = 0,
};

void bspConstructDrivers (void)
{
	static DrvUSART		drvUSART(&resUSART1, true);
	static DrvCAN		drvCAN(50,(ResCAN*)&resCAN1,(ResCAN*)&resCAN2);
	static DrvTJA1041	drvTJA1041(&resTJA1041,true);
	static DrvTJA1041 	drvTJA1041_2(&resTJA1041_2,true);
	static DrvGPIO		drvLEDGreen(&resLEDGreen, false);
	static DrvGPIO		drvLEDRed(&resLEDRed, false);
	
    static DrvGPIO		drvLED3(&resLED3, false);
    static DrvGPIO		drvLED4(&resLED4, false);
    static DrvGPIO		drvLED5(&resLED5, false);

    static DrvGPIO		drvLED6(&resLED6, false);
    static DrvGPIO		drvLED7(&resLED7, false);
    static DrvGPIO		drvLED8(&resLED8, false);
    static DrvGPIO		drvLED9(&resLED9, false);
    static DrvGPIO		drvLED10(&resLED10, false);
    static DrvGPIO		drvLED11(&resLED11, false);
    
    static DrvGPIO		drvCOL1(&resCOL1, false);
    static DrvGPIO		drvCOL2(&resCOL2, false);

    static DrvGPIO		drvCOL3(&resCOL3, false);
    static DrvGPIO		drvCOL4(&resCOL4, false);
    static DrvGPIO		drvCOL5(&resCOL5, false);
    
    static DrvGPIO		drvROW1(&resROW1);
    static DrvGPIO		drvROW2(&resROW2);
    static DrvGPIO		drvROW3(&resROW3);
    static DrvGPIO		drvROW4(&resROW4);
    static DrvGPIO		drvROW5(&resROW5);    

	gpDrvUSART		= &drvUSART;
	gpDrvLEDGreen	= &drvLEDGreen;
	gpDrvLEDRed		= &drvLEDRed;
	gpDrvCAN		= &drvCAN;
	gpDrvTJA1041	= &drvTJA1041;
	gpDrvTJA1041_2	= &drvTJA1041_2;

    gpDrvLED3		= &drvLED3;
    gpDrvLED4		= &drvLED4;

    gpDrvLED5		= &drvLED5;
    gpDrvLED6		= &drvLED6;
    gpDrvLED7		= &drvLED7;
    gpDrvLED8		= &drvLED8;
    gpDrvLED9		= &drvLED9;
    gpDrvLED10		= &drvLED10;
    gpDrvLED11		= &drvLED11;
    
    gpDrvCOL1       = &drvCOL1;
    gpDrvCOL2       = &drvCOL2;
    gpDrvCOL3       = &drvCOL3;
    gpDrvCOL4       = &drvCOL4;
    gpDrvCOL5       = &drvCOL5;
    
    gpDrvROW1       = &drvROW1;
    gpDrvROW2       = &drvROW2;
    gpDrvROW3       = &drvROW3;
    gpDrvROW4       = &drvROW4;
    gpDrvROW5       = &drvROW5;    
}
