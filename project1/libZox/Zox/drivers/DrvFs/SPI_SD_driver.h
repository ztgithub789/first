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

#ifndef _SPI_SD_DRIVER_H_
#define _SPI_SD_DRIVER_H_

#include "stm32f10x.h"

///* Porting Layer -------------------------------------------------------------*/
#define SD_SPI_PORT         SPI1
/* Private define ------------------------------------------------------------*/
/* SD卡类型定义 */
#define SD_TYPE_MMC     0
#define SD_TYPE_V1      1
#define SD_TYPE_V2      2
#define SD_TYPE_V2HC    4

/* SPI总线速度设置*/
#define SPI_SPEED_LOW   0
#define SPI_SPEED_HIGH  1

/* SD传输数据结束后是否释放总线宏定义 */
#define NO_RELEASE      0
#define RELEASE         1

/* SD卡指令表 */
#define CMD0    0       //卡复位
#define CMD9    9       //命令9 ，读CSD数据
#define CMD10   10      //命令10，读CID数据
#define CMD12   12      //命令12，停止数据传输
#define CMD16   16      //命令16，设置SectorSize 应返回0x00
#define CMD17   17      //命令17，读sector
#define CMD18   18      //命令18，读Multi sector
#define ACMD23  23      //命令23，设置多sector写入前预先擦除N个block
#define CMD24   24      //命令24，写sector
#define CMD25   25      //命令25，写Multi sector
#define ACMD41  41      //命令41，应返回0x00
#define CMD55   55      //命令55，应返回0x01
#define CMD58   58      //命令58，读OCR信息
#define CMD59   59      //命令59，使能/禁止CRC，应返回0x00

struct SPI_SDCard_Device  {
	uint32_t		RCC_APB2Periph_SCK;
	GPIO_TypeDef	*GPIO_Port_SCK;
	uint16_t		GPIO_Pin_SCK;

	uint32_t		RCC_APB2Periph_MOSI;
	GPIO_TypeDef	*GPIO_Port_MOSI;
	uint16_t		GPIO_Pin_MOSI;

	uint32_t		RCC_APB2Periph_MISO;
	GPIO_TypeDef	*GPIO_Port_MISO;
	uint16_t		GPIO_Pin_MISO;

	uint32_t		RCC_APB2Periph_nCS;
	GPIO_TypeDef	*GPIO_Port_nCS;
	uint16_t		GPIO_Pin_nCS;

	uint32_t		RCC_APB2Periph_nCD;
	GPIO_TypeDef	*GPIO_Port_nCD;
	uint16_t		GPIO_Pin_nCD;
};

void SPI_SD_Init(struct SPI_SDCard_Device *pdevice);
void SPI_SD_Release(void);

void SPI_SD_EnableSelect(void);
void SPI_SD_DisableSelect(void);
bool SPI_SD_GetSDCard(void);

void SPI_SetSpeed(u8 SpeedSet);

u8 SPI_ReadWriteByte(u8 TxData);                //SPI总线读写一个字节
u8 SD_WaitReady(void);                          //等待SD卡就绪
u8 SD_SendCommand(u8 cmd, u32 arg, u8 crc);     //SD卡发送一个命令
u8 SD_SendCommand_NoDeassert(u8 cmd, u32 arg, u8 crc);
u8 SD_Init(void);                               //SD卡初始化
                                                //
u8 SD_ReceiveData(u8 *data, u16 len, u8 release);//SD卡读数据
u8 SD_GetCID(u8 *cid_data);                     //读SD卡CID
u8 SD_GetCSD(u8 *csd_data);                     //读SD卡CSD
u32 SD_GetCapacity(void);                       //取SD卡容量

u8 SD_ReadSingleBlock(u32 sector, u8 *buffer);  //读一个sector
u8 SD_WriteSingleBlock(u32 sector, const u8 *buffer); //写一个sector
u8 SD_ReadMultiBlock(u32 sector, u8 *buffer, u8 count); //读多个sector
u8 SD_WriteMultiBlock(u32 sector, const u8 *data, u8 count);  //写多个sector

#endif
