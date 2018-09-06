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

#include "GetKey.h"
#include <secure/Activator.h>
#include <includes.h>

OS_EVENT *KEYBOX;
char keyValue;

GetKey::GetKey(uint8_t prio) :  ZTask(prio) {
}



void GetKey::_doTask(void *p_arg)
{

    Activator *gpActivator = new Activator(gpDrvUSART);
    gpActivator->witerForStr=OSMboxCreate((void *)0);   //创建消息信箱
    gpActivator->GetStrprio = 15;
    gpActivator->gpGetStr = new GetStr(gpActivator->GetStrprio);
    gpActivator->gpGetStr->start(gpActivator->witerForStr);		//启动一个获取字符串任务
    gpActivator->gpGetStr->SetLen(gpActivator->buf, ID_STR_LEN, gpActivator->_pDrvUSART); 
    OSTaskSuspend(gpActivator->GetStrprio);			 //挂起获取字符串任务
     //gpActivator->activate();
     
     while(1)
     {
       gpActivator->activate();
       OSTimeDlyHMSM(0,0,1,0);
     }
/*   
      int a=0, b=0, c=0, d=0, e=0;
      KEYBOX=OSMboxCreate(0);
        //OSTimeDlyHMSM(0,0,0,50);
      while(1)
      {
          while((a = (getchar()&0xff)) != 0xaa )
          {
            printf("a = %x\n", a);
          }
           b = getchar();
           printf("b = %x\n", b);
           getchar();
           getchar();

              switch(b)
              {
               
                  case 0x29: keyValue = 'z'; break;
                  case 0x1f: keyValue = 'x'; break;
                  case 0x2a: keyValue = 'c'; break;
                  
                  case 0x27: keyValue = 'w'; break;
                  case 0x28: keyValue = 's'; break;
                  
                  case 0x26: keyValue = 'a'; break;
                  case 0x21: keyValue = 'd'; break;
                  
                  case 0x18: keyValue = 'y'; break;
                  case 0x31: keyValue = 'u'; break;
                  case 0x2e: keyValue = 'i'; break;
                  case 0x19: keyValue = 'o'; break;
                  
                  case 0x0e: keyValue = 'p'; break;
                  
                  case 0x07: keyValue = '1'; break;
                  case 0x08: keyValue = '2'; break;
                  case 0x09: keyValue = '3'; break;
                  case 0x0b: keyValue = '4'; break;
                  case 0x0c: keyValue = '5'; break;
                  case 0x0d: keyValue = '6'; break;
                  case 0x0f: keyValue = '7'; break;
                  case 0x10: keyValue = '8'; break;
                  case 0x11: keyValue = '9'; break;
                  case 0x14: keyValue = '0'; break;
                  case 0x15: keyValue = '-'; break;
                  case 0x13: keyValue = '='; break;  
                   
                  
                  case 0x1A: keyValue = 'Q'; break;
                  case 0x2F: keyValue = 'W'; break;
                  case 0x12: keyValue = 'E'; break;
                  case 0x1B: keyValue = 'R'; break;
                  case 0x30: keyValue = 'T'; break;
                  case 0x1D: keyValue = 'Y'; break;
                  case 0x20: keyValue = 'U'; break;
                  case 0x24: keyValue = 'I'; break;
                  case 0x22: keyValue = 'O'; break;                  
                  
                  case 0x2c: keyValue = ' '; break;
                  
              default : keyValue = 0; break;
                
              }
              //printf("get keyValue = %c OK!\n",keyValue);
              OSMboxPost(KEYBOX,&keyValue);
              OSTimeDlyHMSM(0,0,0,50);
        
     }
 */       
}

int GetKey::getKeyValue(char *c)
{
  if(OSMboxAccept(KEYBOX)>0)
  {
      *c = keyValue;
      //printf("get c = %c OK!\n",keyValue);
      return 0;
  }else{
      return -1; 
  }
}
/*
KEYPADV2键盘码表：
键名    按下     松开
F1    : AA 01    AA 01 
F2    : AA 02    AA 02 
F3    : AA 03    AA 03 
F4    : AA 04    AA 04 
F5    : AA 05    AA 05  
F6    : AA 06    AA 06 

1     : AA 07    AA 07 
2     : AA 08    AA 08 
3     : AA 09    AA 09 
4     : AA 0B    AA 0B 
5     : AA 0C    AA 0C  
6     : AA 0D    AA 0D 
7     : AA 0F    AA 0F  
8     : AA 10    AA 10  
9     : AA 11    AA 11 
0     : AA 14    AA 14 

CLEAR : AA 15    AA 15 
DOT   : AA 13    AA 13 
ADD   : AA 1A    AA 1A 
MUL   : AA 2F    AA 2F 
JOSTLE: AA 12    AA 12 
SUB   : AA 1B    AA 1B  
DIV   : AA 30    AA 30 
INSERT: AA 1D    AA 1D  
EQU   : AA 20    AA 20 
PAGEUP: AA 24    AA 24 
DELET : AA 22    AA 22 
ENTER : AA 2D    AA 2D 
PAGEDO: AA 25    AA 25 
MODIFY: AA 23    AA 23 
PROGRA: AA 1C    AA 1C 
AUTO  : AA 2C    AA 2C 
MANAUL: AA 1E    AA 1E 
PROG  : AA 0E    AA 0E  

UP    : AA 27    AA 27 
SHIFT : AA 1F    AA 1F 
LEFT  : AA 29    AA 29 
DOWN  : AA 28    AA 28 
RIGHT : AA 2A    AA 2A 


HELP  : AA 2B    AA 2B 

RUN   : AA 26    AA 26 
STOP  : AA 21    AA 21 
BACK  : AA 18    AA 18 
FAST_B: AA 31    AA 31 
FAST_F: AA 2E    AA 2E 
FORWAR: AA 19    AA 19 
*/
