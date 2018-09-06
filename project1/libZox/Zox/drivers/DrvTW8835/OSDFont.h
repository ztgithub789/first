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

#ifndef __OSDFONT_H
#define __OSDFONT_H

#define FOSD_WIN1	0x10
#define FOSD_WIN2	0x20
#define FOSD_WIN3	0x30
#define FOSD_WIN4	0x40

#define BPP2_START			0x60
#define BPP3DIGIT_START		0x6C
#define BPP4PENGUIN_START	0x98

struct FontInfo_s {
	char name[16];	//consolas
	BYTE w,h;
	BYTE bpp;
	BYTE loc;		//fontram location

	BYTE bppm,bpp2,bpp3,bpp4;	//start loc. bppm[3]:bpp4[8] bppm[2]:bpp3[8] bppm[1]:bpp2[8]

	BYTE bpp2_attr;		//for MultiBPP	(LUT index >> 2)
	BYTE bpp3_attr;		//for MultiBPP	(LUT index >> 2)
	BYTE bpp4_attr;		//for MultiBPP	(LUT index >> 2)
};
struct FontWinInfo_s {
	WORD osdram;		//osdram start offset, 0~511.
	WORD sx,sy;			//pixel base
	BYTE column,line;  	//char base
	BYTE x,y;			//last position. char base.
	//BYTE zoom;		//[3:2] Horizontal, [1:0] Vertical
	//BYTE alpha_color; 	//for 1bpp
	//WORD fgcolor,bgcolor;	//for 1bpp


	BYTE bpp1_attr;		//(bgColor << 4) | fgColor
};

#if 1
typedef struct FONT_INFO_s {
	BYTE name[4+2];
	BYTE width,height;
	WORD max;
	WORD bpp2;		//start of 2BPP
	WORD bpp3;		//start of 3BPP
	WORD bpp4;		//start of 4BPP
	BYTE lut2, lut3, lut4;	//(LUT offset*4)
	BYTE dummy;
} FONT_INFO_t;

typedef struct FONT_SPI_INFO_s {
	DWORD loc;
	WORD size;				//curr max 10*1024 = 2800h
	BYTE width,height;
	WORD max;
	WORD bpp2;
	WORD bpp3;
	WORD bpp4;
} FONT_SPI_INFO_t;
#endif

void InitFontRam(WORD dest_font_index, FONT_SPI_INFO_t *font);
void FOsdIndexMsgPrint(const short *, int);
void FOsdMsgSetInfo(BYTE winno, WORD sx,WORD sy);
void FontOsdInit(WORD startx, WORD starty, BYTE w, BYTE h, WORD *pArray, BYTE size, BYTE zoomHV = 0);
void FontOsdSetDeValue(void);
void FontOsdInit3BppAlphaTable(BYTE fCalculate);
void FontOsdWinInit(BYTE winno, WORD OsdRamAddr);
void FontOsdWinAlphaPixel(BYTE winno, BYTE color, BYTE alpha);
void FontOsdWinScreen(BYTE winno, WORD x, WORD y, BYTE w, BYTE h);
void FontOsdWinZoom(BYTE winno, BYTE zoomH, BYTE zoomV);
void FontOsdWinSetOsdRamStart(BYTE winno, WORD addr);
void FontOsdWinEnable(BYTE winno, BYTE en);
void FontOsdSetOsdRamAddrAttr(WORD OsdRamAddr, BYTE attr, BYTE mode);
void FontOsdSetPaletteColorArray(BYTE index, WORD *array, BYTE size);
void DownloadFontByDMA(WORD dest_font_index, DWORD src_loc, WORD size, BYTE width, BYTE height, bool useLowResolution);

#endif
