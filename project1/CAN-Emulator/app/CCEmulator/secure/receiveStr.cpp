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
 * ���   :	��ָ����ʱ�䷶Χ�ڣ��Ӵ��ڽ���һ��ָ�����ȵ��ַ�����
 * 			ָ�����ȵ��ַ������ճɹ������߳�ʱ���ú������ء�
 * 			�ú���ԭ�������Ӵ��ڻ�ȡ���ַ����������ַ���������Ӱ���'\0'���ڵ��κ��ַ���
 * ����   :	buf		�����ַ����Ļ����������ȱ��벻С��len
 * 			len		�ַ����ĳ���
 * 			timeout	��ʱֵ����λ��ms
 * 			pDrvUSART	��ʹ�õĴ�������
 * ����ֵ :	ָ�����ȵ��ַ����Ѿ����յ�	0
 * 			��ʱ						1
 * ����   :
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
		if(ret == 0)// �ɹ������ַ�
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
        OSMboxPost(Str_Box,_buf); //�����䷢һ����Ϣ
        i = 0;
        //getchar();
        _pDrvUSART->clearUnreadChar();
        OSTaskSuspend(OS_PRIO_SELF);//���������Լ�
        getchar();
    }
  }
}