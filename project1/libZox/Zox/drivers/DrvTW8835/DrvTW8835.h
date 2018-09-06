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

#ifndef __TW8835_H
#define __TW8835_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
//#include "integer.h"
#include <Driver.h>
#include <DrVI2CCom.h>
#include <libcommon.h>
typedef unsigned char	BYTE;
typedef unsigned long	DWORD;
/* TW8835 I2C Address */
#define TW8835_I2C_ADDRESS          0x8A    /* 8AH */

//==<<PAGE & TWBASE>>=================================================
#define PAGE0_GENERAL		        0x00
#define PAGE0_OUTPUT		        0x00	//0x007::0x00F & 0x01F
#define PAGE0_INPUT			0x00	//0x040::0x046
#define PAGE0_BT656			0x00	//0x047::0x04A
#define PAGE0_DTV			0x00	//0x050::0x05F
#define PAGE0_GPIO			0x00	//0x080::0x09E
#define PAGE0_MBIST			0x00	//0x0A0::0x0A4
#define PAGE0_TOUCH			0x00	//0x0B0::0x0B4
#define PAGE0_LOPOR			0x00	//0x0D4::0x0DF
#define PAGE0_LEDC			0x00	//0x0E0::0x0E6
#define PAGE0_DCDC			0x00	//0x0E8::0x0EC
#define PAGE0_VCOM			0x00	//0x0ED::0x0EE
#define PAGE0_SSPLL			0x00	//0x0F6::0x0FD
#define TW88_FPLL0		0xF8
#define TW88_FPLL1		0xF9
#define TW88_FPLL2		0xFA
#define TW88_SSPLL_ANALOG	0xFD
#define PAGE1_DECODER		0x01	//0x101::0x142
#define PAGE1_VADC			0x01	//0x1C0::0x1E7	include LLPLL
#define PAGE1_LLPLL			0x01	//0x1C0::0x1E7	include LLPLL
#define PAGE2_SCALER		0x02	//0x201::0x21E
#define PAGE2_TCON			0x02	//0x240::0x24E
#define PAGE2_IMAGE			0x02	//0x280::0x2BF
#define PAGE2_GAMMA			0x02	//0x2E0::0x2E3 include LCDC
#define PAGE2_DITHER		0x02	//0x2E4::0x2E4
#define PAGE2_RGBLEVEL		0x02	//0x2F0::0x2F5
#define PAGE2_8BITPANEL		0x02	//0x2F8::0x2F9
#define PAGE3_FOSD			0x03	//0x300::0x354
#define PAGE4_SOSD			0x04	//0x400::0x3BE
#define PAGE4_SPI			0x04	//0x4C0::0x4DA include MCU
#define TWBASE_SPI		    0xC0	//note:Not a page. it is a offset.
#define PAGE4_CLOCK			0x04	//0x4E0::0x4EB
#define PAGE5_MEAS			0x05	//0x500::0x545

/* CLK & PLL */
#define MCUSPI_CLK_27M		0
#define MCUSPI_CLK_32K		1
#define MCUSPI_CLK_PCLK		2
#define CLKPLL_SEL_PCLK		0
#define CLKPLL_SEL_PLL		1
#define CLKPLL_DIV_1P0		0
#define CLKPLL_DIV_1P5		1
#define CLKPLL_DIV_2P0		2
#define CLKPLL_DIV_2P5		3
#define CLKPLL_DIV_5P0		7

/* SPI */
#define SPI_PAGE	0x04

//-----------------------------------------------------------------------------
//		SPI Read Mode
//-----------------------------------------------------------------------------
#define	SPI_READ_SLOW		0
#define SPI_READ_FAST 		1
#define SPI_READ_DUAL	 	2
#define SPI_READ_QUAD  		3
#define SPI_READ_DUALIO		4
#define SPI_READ_QUADIO		5

/* TW8835 Use the QUAD-IO Mode */
#define SPI_READ_MODE		SPI_READ_QUADIO

//--------------------
// SPI FLASH Vendor
//--------------------
#define SFLASH_VENDOR_MX	0	//MX25L12845(C2 20 18) -- defaut
#define SFLASH_VENDOR_EON	1	//EN25Q128 ID(1C 30 18)
#define SFLASH_VENDOR_WB	2
#define SFLASH_VENDOR_NUM	3	//STM(20 BA 18)


extern BYTE DMAMODE_RW_FIX_REG;
extern BYTE DMAMODE_RW_INC_REG;
extern BYTE DMAMODE_R_XDATA;
extern BYTE DMAMODE_W_XDATA;
extern BYTE SPICMD_READ;
extern BYTE SPICMD_PROGRAM;
extern BYTE SPICMD_BYTES;

/* ---------------------------------------------------------------------------*/
#define TW8835_MODE_INTERNAL_MCU     0
#define TW8835_MODE_EXTERNAL_MCU     1
#define TW8835_MODE_ISP              2

u8 TW8835_ReadReg(u8 addr);
u8 TW8835_WriteReg(u8 addr, u8 data);
u8 TW8835_WritePage(u8 page);

int TW8835_WriteRegTable(const u8 *pData);
void TW8835_Write_LUT(const u8 *pData, u16 Length);

void TW8835_SetSpiClk(u8 spi_clk);
BYTE TW8835_SetSpiClkToPclk(BYTE divider, bool needLowCLK=false);
void TW8835_RestoreSpiClk(void);
BYTE TW8835_GetSpiClk(void);

DWORD ClkPllGetFreq(void);

DWORD SpiClkGetFreq(DWORD mcu_clk);
DWORD SspllGetPPF(void);
void SspllSetFreqReg(DWORD FPLL);
DWORD SspllGetFreqReg(void);
void SspllSetAnalogControl(BYTE value);
BYTE SspllGetPost(void);
DWORD SspllFPLL2FREQ(DWORD FPLL, BYTE POST);

void PclkSetDividerReg(BYTE divider);
DWORD PclkGetFreq(DWORD sspll);
DWORD PclkoGetFreq(DWORD pclk);

void TW8835_DumpLut(u16 start, u16 len);

BYTE SPI_SetModeByRegister( void );
void TW8835_SpiInit(void);
BYTE TW8835_SpiQUADInit(void);

u32 TW8835_SPI_LoadData(u32 spiaddr, u8 *data_p, u32 count);

/*----------------------------------------------------------------------------*/
/* TW88xx Code Porting Macro */

#define ReadTW88(x)         TW8835_ReadReg(x)
#define WriteTW88(a, b)     TW8835_WriteReg(a, b)
#define WriteTW88Page(x)    TW8835_WritePage(x)

struct ResTW8835 {
	uint32_t		RCC_APB2Periph_Reset;
	GPIO_TypeDef	*GPIO_Port_Reset;
	uint16_t		GPIO_Pin_Reset;
	uint32_t		RCC_APB2Periph_ISP;
	GPIO_TypeDef	*GPIO_Port_ISP;
	uint16_t		GPIO_Pin_ISP;
	uint32_t		RCC_APB2Periph_McuEn;
	GPIO_TypeDef	*GPIO_Port_McuEn;
	uint16_t		GPIO_Pin_McuEn;
};

class DrvTW8835 : public Driver {
public:
	DrvTW8835(ResTW8835 *pRes, bool needLowCLK=false,DrvI2CCom **_pDrvI2C=NULL);
	int init(void);
	bool detechTw8835(void);
	void selectYin(uint8_t x);
	bool isVDLOSS(void);
	bool isPSTD(void);
	int writeRegTable(const uint8_t *pData);
	bool readPin(uint8_t port, uint8_t pin);
	void setPin(uint8_t port, uint8_t pin, bool isset);
	uint8_t readPortValue(uint8_t port);
	void writePortValue(uint8_t port, uint8_t value);
	void highZFPdata(bool needhigh);
	void enterMode(int mode);
	void setSpiClkToPclk(BYTE divider);
	void setLowCLK(bool enable);
	static DrvI2CCom **pDrvI2C;
private:
	void _enableControlGPIOs(void);
	void _releaseControlGPIOs(void);
	ResTW8835 *_pRes;
	u8 const *_pStoreRegs;
	bool _needLowCLK;
};

#endif
