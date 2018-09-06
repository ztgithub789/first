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

#include <string.h>
#include <stdio.h>
#include "Encryption.h"
#include "key.h"
#include "receiveStr.h"
#include "Activatee.h"

// #define SECURE_DEBUG	1

/*******************************************************************************
 * 简介   :	被激活对象的初始化
 * 参数   :	pDrvUSART	使用的串口驱动的指针
 * 			flashAddr	加密后的ID的保存地址
 * 返回值 :
 * 描述   :
 *******************************************************************************/
Activatee::Activatee(DrvUSART *pDrvUSART, uint32_t flashAddr)
{
	_pDrvUSART = pDrvUSART;
	_flashAddr = flashAddr;
}

/*******************************************************************************
 * 简介   :	判断自身是否已经被激活
 * 参数   :	无
 * 返回值 :	true	已被激活
 * 			false	未被激活
 * 描述   :
 *******************************************************************************/
bool Activatee::isActivated(void)
{
	u32 DevID[3];
	u32 EncryptedFLashID[3];
	u32 DecryptedFLashID[3];

	_readDevID(DevID);
	_readFlashDevID(EncryptedFLashID);

	if (Encryption::decrypt(EncryptedFLashID, ACTIVATE_KEY, DecryptedFLashID) != 0) {
		return false;
	}

	if (_cmpID(DevID, DecryptedFLashID) == 0) {
		return true;
	} else {
		return false;
	}
}

void Activatee::_32data_to_string(u32 data, char *str, uint8_t startbty )
{
    char _4bit;
    char i;
    for( i= 7; i>=0; i--)
    {
        _4bit = (data>>(i*4))&0xf;
        
        if(_4bit<0xa)
        {
          str[startbty++] = _4bit+'0';
        }else{
          str[startbty++] = (_4bit-0xa)+'a';
        }
    }
}

/*******************************************************************************
 * 简介   :	等待激活流程。直到激活成功才返回。
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void Activatee::waiteActivation(void)
{
	u32 DevID[3];
	u32 EncryptedFLashID[3];
	char buf[32];
	uint8_t ret;
    uint8_t buf_zise, ID_zise, IDbit_zise;
    
	while (1) {
      
		_waitActivationRequest();

		_readDevID(DevID);
		//sprintf(buf, "%08x %08x %08x\r\n", DevID[0], DevID[1], DevID[2]);	// 激活请求已确认，发送ID明文
        _32data_to_string(DevID[0],buf,0);
        buf[8] = ' ';
        _32data_to_string(DevID[1],buf,9);
        buf[17] = ' ';
        _32data_to_string(DevID[2],buf,18);
        buf[26] = '\r';
        buf[27] = '\n';
        buf[28] = 0; 
		_pDrvUSART->puts(buf);

		ret = receiveStr(buf, ACTIVATE_ENCRYPTED_ID_TEXT_LEN, 500, _pDrvUSART);
		if (ret == 0) {
			buf[ACTIVATE_ENCRYPTED_ID_TEXT_LEN] = 0;
		} else
			continue;		// 超时重来

		sscanf(buf, "%x %x %x", &EncryptedFLashID[0], &EncryptedFLashID[1], &EncryptedFLashID[2]);

		FLASH_Unlock();											// 把ID密文写入flash
		_writeFlashDevID(EncryptedFLashID);
#ifndef SECURE_DEBUG
		FLASH_ReadOutProtection(ENABLE);
#endif
		FLASH_Lock();

		if (isActivated())
			return;
	}
}

/****************************************************************************
 * 简介   :	比较两个硬件ID是否相同。
 * 参数   :	id1	第一个id
 * 			id2	第二个id
 *
 * 返回值 :	0		相同
 * 			其他	不相同
 * 描述   :
 ****************************************************************************/
int Activatee::_cmpID(const u32 *id1, const u32 *id2)
{
	int i;
	for (i=0; i<3; i++) {
		if (id1[i] != id2[i])
			return 1;
	}
	return 0;
}

/*******************************************************************************
 * 简介   :	从串口等待激活请求的字符串。收到匹配的字符串即返回，否则一直等待。
 * 参数   :	无
 * 返回值 :	无
 * 描述   :
 *******************************************************************************/
void Activatee::_waitActivationRequest(void)
{
	char *activateReq = ACTIVATE_REQUEST;
	uint32_t len = strlen(activateReq);
        char c;
	int i = 0;
	while (1) {
		c = _pDrvUSART->getChar();
		if (c == activateReq[i]) {
			i ++;
			if (i == len) {
				break;
			}
		} else
			i = 0;
	}
}


/* Read Device Universal ID from STM32 */
void Activatee::_readDevID(u32 *PDEVID)
{
    PDEVID[0] = *(vu32*)(0x1ffff7e8);
    PDEVID[1] = *(vu32*)(0x1ffff7ec);
    PDEVID[2] = *(vu32*)(0x1ffff7f0);
}

void Activatee::_readFlashDevID(u32 *PDEVID)
{
    u32 addr = _flashAddr;

    PDEVID[0] = *(vu32*)(addr);
    PDEVID[1] = *(vu32*)(addr + 4);
    PDEVID[2] = *(vu32*)(addr + 8);
}

u32 Activatee::_writeFlashDevID(u32 *PDEVID)
{
    FLASH_Status  status;
    u32 page_address = _flashAddr;
    int i;

    status = FLASH_ErasePage(page_address);
    if (status != FLASH_COMPLETE) {
        return (u32)status;
    }

    /* Program 3 words */
    for (i = 0; i < 3; i++) {
        status = FLASH_ProgramWord(page_address + i*4, PDEVID[i]);
        if (status != FLASH_COMPLETE) {
            return (u32)status;
        }
    }

    return status;
}

