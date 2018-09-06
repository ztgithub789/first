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
#ifndef DRVTJA1041_H_
#define DRVTJA1041_H_

#include "stm32f10x.h"
#include "Driver.h"
/*
 * 把相应的pin设为-1,表示该引脚已接固定电平,不需要初始化
 */
struct ResTJA1041 {
		uint32_t		RCC_Periph;
		GPIO_TypeDef	*en_GPIO_Port;
		uint16_t		 en_GPIO_Pin;
		GPIO_TypeDef	*stb_GPIO_Port;
		uint16_t		 stb_GPIO_Pin;
		GPIO_TypeDef	*wake_GPIO_Port;
		uint16_t		 wake_GPIO_Pin;
		GPIO_TypeDef	*err_GPIO_Port;
		uint16_t		 err_GPIO_Pin;
};


class DrvTJA1041 : public Driver {
public:
	DrvTJA1041(ResTJA1041  *res,bool enterNormalAtInit=false);
	virtual ~DrvTJA1041();
	int  init(void);
	void enterStandbyMode(void);
	void enterGotoSleepCommandMode(bool needHold = true);
	void enterNormalMode(void);
	void enterPownMode(void);
	bool getErr(void);
protected:
	ResTJA1041 *pRes;
	bool _enterNormalAtInit;
};

#endif /* DRVTJA1041_H_ */
