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

#ifndef __OSD_H
#define __OSD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

struct _RLE_HEADER {
    u8 id[2];     			// id='TW' 
    u8 c;  				// LUT | bits/pixel
    u8 cp; 				// compression info = data bits | count bits      
    u16 w;                		// width (little endian)
    u16 h;				// height (little endian)
    u32 size;				// size (little endian)
    const u8 *bmp;   		        //
} ;

typedef struct {
    u8 bpp;
    u8 type;        // type: 0:LUT+image, 1:header+lut+image 2:image_only 3: lut_only 4:header_only
    u8 alpha_type;  // alpha 0:no alpha, 1:window alpha 2:pixel alpha
    u8 alpha;       // alpha level: 0 - 0x7F
    u8 lut_type;    //1:luts(byte pointer method.default on TW8835MENU), 0:lut(address method)
    u8 lut_update;  //1:nedd to reload the lut 0:don't need to load lut
    u16 lut_offset; // LUT offset in TW8835 table
    u16 lut_size;   // LUT size, in bytes
    u32 lut_loc;    // lut location in SPI flash
    u8 rle;         //upper:bpp bottom:rle_counter.
    u16 w;          // width
    u16 h;          // height
    u32 loc;        // Absolute Location in SPI flash
    u32 size;       // image size
} osd_image_info_t;

//=============================================================================
//		OSD Register Definition
//=============================================================================

#define OSD_ST				0x00
#define WIN0_ST				0x80
#define WIN1_ST				0xa0
#define WIN4_ST				0xc0

#define OSD_MODE			0x00
#define OSD_START			0x01
#define OSD_DATAPORT		0x02
#define OSD_SPECIAL			0x03
#define OSD_RLECTRL			0x04
#define OSD_RLEDATACNT		0x05
#define OSD_BITBLTCOLOR		0x0e

#define	OSD_SOURCECOLOR		0x40
#define	OSD_TARGETCOLOR		0x48
#define	OSD_NOWRITECOLOR	0x50

#define OSD_BITBLTSOURCE	0x60
#define OSD_BITBLTSOURCEW64	0x63
#define OSD_BITBLTSOURCEXY	0x64	
#define OSD_BITBLTSIZE		0x68
#define OSD_BITBLTDEST		0x70
#define OSD_BITBLTDESTW64	0x73
#define OSD_BITBLTDESTXY	0x74

#define OSD_LUT_WINNO		0x7a
#define OSD_LUT_INDEX		0x7b
#define OSD_LUT_ATTR		0x7c		
#define OSD_LUT_R			0x7d
#define OSD_LUT_G			0x7e
#define OSD_LUT_B			0x7f

#define	SPI_OSDWIN_ENABLE		0x00

#define	SPI_OSD_ST				0x00
#define SPI_WIN0_ST				0x20
#define SPI_WIN1_ST				0x40
#define SPI_WIN2_ST				0x50
#define SPI_WIN3_ST				0x60
#define SPI_WIN4_ST				0x70
#define SPI_WIN5_ST				0x80
#define SPI_WIN6_ST				0x90
#define SPI_WIN7_ST				0xA0
#define SPI_WIN8_ST				0xB0


#define SPI_OSDWIN_SCREEN			0x01
#define	SPI_OSDWIN_BUFFERSTART			0x07
#define SPI_OSDWIN_BUFFERSTART_BIT		0x0A
#define SPI_OSDWIN_DISPSIZE			0x0A
#define SPI_OSDWIN_DISPOFFSET			0x0D
#define SPI_OSDWIN_ALPHA			0x0C	//430(42C+4), 44C
#define SPI_OSDWIN_LUT_PTR			0x0D	//431(42D+4), 44D
#define SPI_OSDWIN_FILLCOLOR			0x0E	//436(42E+8), 44E
#define	SPI_OSDWIN_ANIMATION			0x12    //432


#define OFF     0
#define ON      1

typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned long	DWORD;

void WaitVBlank(BYTE cnt);

#endif
