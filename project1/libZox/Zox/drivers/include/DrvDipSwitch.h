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
#ifndef DRVDIPSWITCH_H_
#define DRVDIPSWITCH_H_

#include <Driver.h>

#define DIPSWITCH_MAX_PIN_NR	16

struct ResDipSwitch {
	uint8_t		pinNr;					// ���뿪�ص����Ÿ���
	uint32_t	RCC_APB2Periph;
	uint16_t	*pGPIO_pinArray;		// �������飬����Ԫ�ظ�����pinNrָ��
	GPIO_TypeDef*	GPIO_port;
};

class DrvDipSwitch : public Driver {
public:
	DrvDipSwitch(ResDipSwitch *pRes);
	virtual ~DrvDipSwitch();
	int init(void);
	uint16_t getVal(void);

private:
	ResDipSwitch *_pRes;
};

#endif /* DRVDIPSWITCH_H_ */