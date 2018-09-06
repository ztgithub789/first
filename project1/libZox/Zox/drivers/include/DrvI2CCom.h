/*
 * RoadPassion CONFIDENTIAL
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
#ifndef DRVI2CCOM_H_
#define DRVI2CCOM_H_

#include <Driver.h>
#include <ucos_ii.h>
#include "cpal_i2c.h"

class DrvI2CCom : public Driver {
public:
	DrvI2CCom(uint32_t clock=400000);
	virtual ~DrvI2CCom();
	virtual int init(void);
	uint8_t read(uint8_t chipId, const uint8_t addr, uint8_t *pBuf);
	uint8_t write(uint8_t chipId, const uint8_t addr, uint8_t val);
private:
	void _pend(void);
	void _post(void);
	OS_EVENT *_pSem;
	uint32_t _clock;
protected:
	CPAL_TransferTypeDef  _txTransferStruct;
	CPAL_TransferTypeDef  _rxTransferStruct;
	CPAL_InitTypeDef *_pDevInitStruct;
};

#endif /* DRVI2C_H_ */