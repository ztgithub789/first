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

#include <Zox.h>
#include "Encryption.h"
#include <board.h>
#include "key.h"

#include "Activator.h"

static const char *ACTIVATE_REQUEST	= "ActiReq\r";

#define ACTIVATE_RESULT_SUCCESS	"success\r\n"
#define ACTIVATE_RESULT_FAILURE	"failure\r\n"



Activator::Activator(DrvUSART *pDrvUSART) {
	_pDrvUSART = pDrvUSART;
}

Activator::~Activator() {
}

/****************************************************************************
 * 简介   :	激活
 * 参数   :	无
 * 返回值 :	成功	0
 * 			失败	非0
 * 描述   :
 ****************************************************************************/
int Activator::activate(void)
{
	//int i;
	//int ret;
    INT8U err;
	/*
	 * TODO: 按目前USART驱动和__read()的实现，此处如果直接调用getchar()，
	 * 可能会读到之前缓存在驱动中的内容(USART_char)。这里直接调用驱动的API将缓存清掉，
	 * 然后再调用getchar()。
	 *
	 * 需要更一致的方法处理这个问题。
	 */
	_pDrvUSART->clearUnreadChar();		// 清除掉缓存， 防止getchar()读到陈旧的内容

    
	//ret = receiveStr(buf, ID_STR_LEN, 500, _pDrvUSART);
    
    //gpGetStr->SetLen(buf, ID_STR_LEN, _pDrvUSART); 
    OSTaskResume (GetStrprio);    //获取字符串任务继续
    _pDrvUSART->puts(ACTIVATE_REQUEST);
    OSMboxPend(witerForStr,500,&err); //等待消息邮箱的消息
    //OSTaskSuspend(GetStrprio);
    
	if (err == OS_ERR_NONE) {
		// 成功接收，后边补'\0\'
		buf[ID_STR_LEN] = 0;
        //printf(buf);
	} else {
		return -1;
	}

	u32 DevID[3];
	u32 EncryptedDevID[3];

	sscanf(buf, "%x %x %x", &DevID[0], &DevID[1], &DevID[2]);
	Encryption::encrypt(DevID, ACTIVATE_KEY, EncryptedDevID);									// 加密ID

	sprintf(buf, "%08x %08x %08x\r", EncryptedDevID[0], EncryptedDevID[1], EncryptedDevID[2]);		// 发送密文ID
	_pDrvUSART->puts(buf);

    return 0;
}
