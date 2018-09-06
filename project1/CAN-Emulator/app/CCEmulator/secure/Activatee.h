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

#ifndef ACTIVATION_H_
#define ACTIVATION_H_
#include <stm32f10x.h>
#include <DrvUSART.h>

#define ACTIVATE_REQUEST			"ActiReq\r"
#define ACTIVATE_RESULT_SUCCESS	"success\r\n"
#define ACTIVATE_RESULT_FAILURE	"failure\r\n"
#define ACTIVATE_ID_LEN				12				// 二进制设备ID的长度
#define ACTIVATE_PLAIN_ID_TEXT_LEN		28				// ID明文字符串的长度
#define ACTIVATE_ENCRYPTED_ID_TEXT_LEN	27				// ID密文字符串的长度

class Activatee {
public:

	Activatee(DrvUSART *pDrvUSART, uint32_t flashAddr);

	bool isActivated(void);
	void waiteActivation(void);

private:
	DrvUSART *_pDrvUSART;
	uint32_t _flashAddr;

	void _waitActivationRequest(void);
	int _cmpID(const u32 *id1, const u32 *id2);
	void _readDevID(u32 *PDEVID);
	void _readFlashDevID(u32 *PDEVID);
	u32 _writeFlashDevID(u32 *PDEVID);
    void _32data_to_string(u32 data, char *str, uint8_t startbty );
};

#endif /* ACTIVATION_H_ */
