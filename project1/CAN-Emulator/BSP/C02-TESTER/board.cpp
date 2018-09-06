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
DrvGPIO		*gpDrvLEDGreen, *gpDrvLEDRed;	// LED和蜂鸣器本质上可以使用一个驱动
DrvCAN		*gpDrvCAN;
DrvTJA1041	*gpDrvTJA1041;
DrvTJA1041	*gpDrvTJA1041_2;


DrvGPIO		*gpDrvReverseSig;
DrvGPIO		*gpDrvTurnLeftSig;
DrvGPIO		*gpDrvTurnRightSig;
DrvGPIO		*gpDrvPKeyPessedSig;


const static ResGPIO resReverseSig = {
	.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOC,
		.GPIO_Port	= GPIOC,
		.GPIO_Pin	= GPIO_Pin_12,
		.Direction	= false, //input
};

const static ResGPIO resTurnLeftSig = {
	.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOC,
		.GPIO_Port	= GPIOC,
		.GPIO_Pin	= GPIO_Pin_6,
		.Direction	= false, //input
};

const static ResGPIO resTurnRightSig = {
	.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOC,
		.GPIO_Port	= GPIOC,
		.GPIO_Pin	= GPIO_Pin_7,
		.Direction	= false, //input
};

const static ResGPIO resPKeyPessedSig = {
	.RCC_APBPeriph_NUM = RCC_APB2Periph_GPIOD,
		.GPIO_Port	= GPIOD,
		.GPIO_Pin	= GPIO_Pin_2,
		.Direction	= false, //input
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
				.USART_BaudRate = 57600,
				.USART_WordLength = USART_WordLength_8b,
				.USART_StopBits = USART_StopBits_1,
				.USART_Parity = USART_Parity_No,
				.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
				.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
		},
};

const static ResGPIO resLEDGreen = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOB,
		.GPIO_Port				= GPIOB,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_9,			// PIN号
		.Direction				= true,
};

const static ResGPIO resLEDRed = {
		.RCC_APBPeriph_NUM		= RCC_APB2Periph_GPIOB,
		.GPIO_Port				= GPIOB,				// GPIO口
		.GPIO_Pin				= GPIO_Pin_8,			// PIN号
		.Direction				= true,
};

const static ResTJA1041 resTJA1041 = {
		.RCC_Periph		= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC,
		
		.en_GPIO_Port	= GPIOC,
		.en_GPIO_Pin	= GPIO_Pin_11,
		
		.stb_GPIO_Port	= GPIOC,
		.stb_GPIO_Pin	= GPIO_Pin_9,

		.wake_GPIO_Pin	= (uint16_t)-1,

		.err_GPIO_Port	= GPIOA,
		.err_GPIO_Pin	= GPIO_Pin_8,
};

const static ResTJA1041 resTJA1041_2 = {
		.RCC_Periph		= RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOB,

		.en_GPIO_Port	= GPIOB,
		.en_GPIO_Pin	= GPIO_Pin_14,

		.stb_GPIO_Port	= GPIOB,
		.stb_GPIO_Pin	= GPIO_Pin_10,

		.wake_GPIO_Pin	= (uint16_t)-1,

		.err_GPIO_Port	= GPIOB,
		.err_GPIO_Pin	= GPIO_Pin_11,
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
	static DrvUSART		drvUSART((ResUSART*)&resUSART1, true);
	static DrvCAN		drvCAN(50,(ResCAN*)&resCAN1,(ResCAN*)&resCAN2);
	static DrvTJA1041	drvTJA1041((ResTJA1041*)&resTJA1041);
	static DrvTJA1041 	drvTJA1041_2((ResTJA1041*)&resTJA1041_2);        

	static DrvGPIO		drvLEDGreen((ResGPIO*)&resLEDGreen, false);
	static DrvGPIO		drvLEDRed((ResGPIO*)&resLEDRed, false);
	static DrvGPIO 		drvReverseSig((ResGPIO*)&resReverseSig,false);
	static DrvGPIO 		drvTurnLeftSig((ResGPIO*)&resTurnLeftSig,false);
	static DrvGPIO 		drvTurnRightSig((ResGPIO*)&resTurnRightSig,false);
    static DrvGPIO 		drvPKeyPessedSig((ResGPIO*)&resPKeyPessedSig,false);

	
	gpDrvUSART		= &drvUSART;
	gpDrvLEDGreen	= &drvLEDGreen;
	gpDrvLEDRed		= &drvLEDRed;
	gpDrvCAN		= &drvCAN;
	gpDrvTJA1041	= &drvTJA1041;
        gpDrvTJA1041_2	= &drvTJA1041_2;
	gpDrvReverseSig	= &drvReverseSig;
	gpDrvTurnLeftSig = &drvTurnLeftSig;
	gpDrvTurnRightSig = &drvTurnRightSig;
        gpDrvPKeyPessedSig = &drvPKeyPessedSig;
}
