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
#ifndef DRVCH7025_H_
#define DRVCH7025_H_

#include <Driver.h>
#include <libcommon.h>
#include "DrVI2CCom.h"

#define CHIP_ID	(0x76<<1)

#define HTI_HAI_ADDR	0x0f
#define HW_HO_ADDR		0x12
#define VTI_VAI_ADDR	0x15
#define VW_VO_ADDR		0x18

#define HTO_HAO_ADDR	0x1b
#define HWO_HOO_ADDR	0x1e
#define VTO_VAO_ADDR	0x21
#define VWO_VOO_ADDR	0x24

#define VP_ADDR		0x33
#define HP_ADDR		0x35

#define MONOB_ADDR		0x3D

#define HUE_ADDR		0x2E
#define SAT_ADDR		0x2F
#define CTA_ADDR		0x30
#define BRI_ADDR		0x31
#define SHAR_ADDR		0x32

#define DAC0_ADDR		0x37
#define DAC1_ADDR		0x39
#define DAC2_ADDR		0x3b


class DrvCH7025 : public Driver {
public:
	DrvCH7025(DrvI2CCom **pDrvI2C=NULL);
	virtual ~DrvCH7025();
	int init(void);
	uint8_t read(const uint8_t addr, uint8_t *pBuf);
	uint8_t write(const uint8_t addr, uint8_t val);
	void poweron_dac0(bool status);
	
#ifdef ZDEBUG
	void adjust(void);
#endif

private:
	void _readPair(uint8_t addr, uint16_t *p1, uint16_t *p2);
	void _writePair(uint8_t addr, uint16_t p1, uint16_t p2);
	void _read16(uint8_t addr, uint16_t *p);
	void _write16(uint8_t addr, uint16_t p);
	DrvI2CCom **_pDrvI2C;

#ifdef ZDEBUG
	uint16_t HTI, HAI, HW, HO, VTI, VAI, VW, VO;
	uint16_t HTO, HAO, HWO, HOO, VTO, VAO, VWO, VOO;
	uint16_t HP, VP;
	uint16_t MONOB, HUE, SAT, CTA, BRI, DAC0, DAC1, DAC2,SHAR;


	void _readParams(void);
	void _writeParams(void);
	void _printParams(void);
	void _enterTestMode(uint8_t patternNr);
	void _showHelp(void);
#endif
};

#endif /* DRVCH7025_H_ */
