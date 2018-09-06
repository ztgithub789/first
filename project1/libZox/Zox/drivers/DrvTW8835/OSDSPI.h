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


#ifndef __SPIOSD_H
#define __SPIOSD_H

#include "OSD.h"


#define SOSD_WIN0       0
#define SOSD_WIN1       1
#define SOSD_WIN2       2
#define SOSD_WIN3       3
#define SOSD_WIN4       4
#define SOSD_WIN5       5
#define SOSD_WIN6       6
#define SOSD_WIN7       7
#define SOSD_WIN8       8



void SpiOsdSetDeValue(void);
void SpiOsdEnable(BYTE en);

void SpiOsdWinHWEnable		(BYTE winno, BYTE en);
void SpiOsdWinBuffEnable	(BYTE winno, BYTE en);
void SpiOsdWinHWOffAll(BYTE wait);
void SpiOsdWinBuffSynchEnable(void);
void SpiOsdWinBuffOffAll(void);
void SpiOsdWinHWOff(BYTE start, BYTE end);


void SpiOsdWinImageLoc		(BYTE winno, DWORD start);
void SpiOsdWinImageLocBit	(BYTE winno, BYTE start);
void SpiOsdWinImageSizeW	(BYTE winno, WORD w);
void SpiOsdWinImageSizeWH 	(BYTE winno, WORD x, WORD y);
void SpiOsdWinScreen		(BYTE winno, WORD x, WORD y, WORD w, WORD h);
void SpiOsdWinLutOffset		(BYTE winno, WORD table_offset );
void SpiOsdWinPixelWidth	(BYTE winno, BYTE bpp);
void SpiOsdWinFillColor		(BYTE winno, BYTE color );
void SpiOsdWinGlobalAlpha	(BYTE winno, BYTE alpha );
void SpiOsdWinPixelAlpha	(BYTE winno, BYTE alpha );

void SpiOsdWin0SetPriority	(BYTE high);
void SpiOsdWin0Animation	(BYTE mode, BYTE FrameH, BYTE FrameV, BYTE Duration);
void SpiOsdWin0ImageOffsetXY(WORD x, WORD y);



#define LUTTYPE_BYTE	1
#define LUTTYPE_ADDR	0

#define ALPHA_WINDOW    1
#define ALPHA_PIXEL     2
#define ALPHA_NONE      0

void SpiOsdIoLoadLUT(BYTE type, WORD LutOffset, WORD size, DWORD spiaddr);
void SpiOsdLoadLUT(BYTE type, WORD LutOffset, WORD size, DWORD address);
void SpiOsdPixelAlphaAttr(WORD lutloc, BYTE value);

void SpiOsdRLC(BYTE winno,BYTE dcnt, BYTE ccnt);
void SpiOsdResetRLC(BYTE reset);
void SpiOsdEnableRLC(BYTE en);

void SOsdHwBuffClean(void);
void SOsdHwBuffSetLut(BYTE win, /*BYTE type,*/  WORD LutOffset, WORD size, DWORD address);
void SOsdHwBuffSetRle(BYTE win, BYTE bpp, BYTE count);
void SOsdHwBuffSetAlpha(WORD alpha);
void SOsdHwBuffWrite2Hw(void);

void SOsdWinBuffClean(BYTE hw);
void SOsdWinBuffWrite2Hw(BYTE start, BYTE end);

/*----------------------------------------------------------------------------*/
void TW8835_SpiOsdInit(void);
void TW8835_SpiOsdDrawImage(u8 winno, osd_image_info_t *image, u16 dx, u16 dy);
void TW8835_SpiOsdDrawImageWithoutShow(u8 winno, osd_image_info_t *image, u16 dx, u16 dy);
void TW8835_SpiOsdWindowReposition(u8 winno, u16 dx, u16 dy);

#endif
