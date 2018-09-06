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

#include "receiveStr.h"
#include <utils/Time/Time.h>

/****************************************************************************
 * 简介   :	在指定的时间范围内，从串口接收一个指定长度的字符串。
 * 			指定长度的字符串接收成功，或者超时，该函数返回。
 * 			该函数原样拷贝从串口获取的字符，不会在字符串后面添加包括'\0'在内的任何字符。
 * 参数   :	buf		保存字符串的缓冲区，长度必须不小于len
 * 			len		字符串的长度
 * 			timeout	超时值。单位：ms
 * 			pDrvUSART	所使用的串口驱动
 * 返回值 :	指定长度的字符串已经接收到	0
 * 			超时						1
 * 描述   :
 ****************************************************************************/
uint8_t	receiveStr(char *buf, uint8_t len, INT32U timeout, DrvUSART *pDrvUSART)
{
	INT32U start = Time::getTimems();
	INT32U end;
	int i = 0;
    int ret=0;

	while (1) {
		char c;
		//ret = pDrvUSART->readCharNonBlock(&c);
        ret = pDrvUSART->getChar();
		if(ret == 0)// 成功接收字符
        {
			buf[i++] = c;
            for(;i<=len;)
            {
              c = pDrvUSART->getChar();
              buf[i++] = c;
              if (i == len) {
                  return 0;
              }
            }
            return -1;
		}else{
          return -1;
        }
	}
}



GetStr::GetStr(uint8_t prio) :  ZTask(prio) {
}


void GetStr::SetLen(char *buf, uint8_t len, DrvUSART *pDrvUSART)
{
  _buf = buf;
  _len=len;
  _pDrvUSART = pDrvUSART;
}

void GetStr::_doTask(void *p_arg)
{
  OS_EVENT *Str_Box = (OS_EVENT*)p_arg;
  char i = 0;
  while(1){
    char c;    
    c=getchar();
    _buf[i++] = c;
    if(i==_len)
    {
        //printf(_buf);
        OSMboxPost(Str_Box,_buf); //向信箱发一则消息
        i = 0;
        //getchar();
        _pDrvUSART->clearUnreadChar();
        OSTaskSuspend(OS_PRIO_SELF);//挂起任务自己
        getchar();
    }
  }
}