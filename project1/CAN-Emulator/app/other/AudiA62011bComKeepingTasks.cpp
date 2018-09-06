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

#include <includes.h>
#include "AudiA62011b_common.h"
#include "AudiA62011bComKeepingTasks.h"

using namespace AudiA62011_b;
extern OS_EVENT               *App_RearViewCameraMbox;   /* CAN to Uart */

//原厂检测通信
const u8 AudiCheckAn1[]={"10 00 02 50 89"};

const u8 AudiCheckAn2[]={"5A 9B 34 46 30 39 31 30 34 34 31 44 20 20 30 30 33 30 03 0F 91 FE 00 06 46 22 00 00 4A 37 37 32 5F 5F 52 65 61 72 76 69 65 77 20 20 20 20 20 20"};

const u8 AudiCheckAn3[]={"29 00 0A 71 B8 01 03 01 04 01 06 01 08"};

const u8 AudiCheckAn4[]={"5A 91 0E 34 4C 30 39 30 37 34 34 31 42 20 20 20 FF "};

const u8 AudiCheckAn5[]={"54 FF 00"};

const u8 AudiCheckAn6[]={"58 00"};

const u8 AudiCheckAn7[]={"5A 86 0F 35 32 35 50 41 38 2D 4A 39 39 32 31 51 53 20 50 41 38 2D 30 31 39 30 39 2E 30 39 2E 30 39 2D 2D 48 30 33 2D 2D 2D 30 30 34 30 31 30 35 35 FF"};

const u8 AudiCheckAn8[]={"5A 9C 00 00 00 00 00 00 00 00 00 00 33 31 2E 31 35 2E 3C 37"};

void sendData(uint32_t id, uint8_t DLC, const char *pData)
{
	CanTxMsg msg =
	{
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {0xf0, 0x0f, 0x0, 0x02, 0x00, 0x02, 0x08, 0x00}
	};


	msg.StdId	= id;
	msg.DLC		= DLC;
	for (int i = 0; i<DLC; i++) {
		msg.Data[i] = pData[i];
	}
	gpDrvCAN->transmit(CAN1, &msg);

}


static void BSP_CAN_Send_Data(u32 CANID, const char *P)
{
  CanTxMsg TxMessage;
  u32 i;
  zprintf(">");
  TxMessage.StdId = CANID;
  TxMessage.ExtId = 0;
  TxMessage.RTR=CAN_RTR_DATA;
  TxMessage.IDE=CAN_ID_STD;
  TxMessage.DLC=0;
  for(i=0;i<8;i++)
  {
      TxMessage.Data[i] = 0;
  }
  for(i=0;*P!=0x00;)
  {
     if(*P>='0'&&*P<='9')
     {
       TxMessage.Data[i]=(*P-'0')<<4;
            P++;
     }else if(*P>='A'&&*P<='F'){
        TxMessage.Data[i]=(*P-'A'+10)<<4;
             P++;
     }else if(*P>='a'&&*P<='F'){
       TxMessage.Data[i]=(*P-'a'+10)<<4;
            P++;
     }
     if(*P>='0'&&*P<='9')
     {
       TxMessage.Data[i]=TxMessage.Data[i]|(*P-'0');
       TxMessage.DLC++;
       i++;
     }else if(*P>='A'&&*P<='F'){
       TxMessage.Data[i]=TxMessage.Data[i]|(*P-'A'+10);
       TxMessage.DLC++;
       i++;
     }else if(*P>='a'&&*P<='F'){
       TxMessage.Data[i]=TxMessage.Data[i]|(*P-'a'+10);
       TxMessage.DLC++;
       i++;
     }
     P++;
  }
#ifdef   DEBUG
     BSP_CAN_PrintRxMsg((CanRxMsg *)&TxMessage);
#endif
 	gpDrvCAN->transmit(CAN1, &TxMessage);
}

void BSP_CAN_SEND_MY(u32 CANID,u8 *P,u8 sel,u8 *cnt)
{
	u8 i=0;
     u16 timeout;
     u8 k=0;
     u8 DLC=0;
     u8 Sendpage = 0;
     u8 SendDatatemp[100];
     CanTxMsg TxMessage;
     u8 TransmitMailbox;
     u8 *Ptemp;
     zprintf(">");
     //BSP_LED_Toggle(2);

    //CPU_CRITICAL_ENTER();
     TxMessage.StdId = CANID;
     TxMessage.ExtId = 0;
     TxMessage.IDE = 0;
     TxMessage.RTR = 0;
     TxMessage.DLC = 0;
     Ptemp = P;

     for(i=0;i<8;i++)
     {
         TxMessage.Data[i] = 0;
     }
     i=0;
     while(*Ptemp!=0x00)
     {
         if(*Ptemp>='0'&&*Ptemp<='9')
         {
           SendDatatemp[i]=(*Ptemp-'0')<<4;
                Ptemp++;
         }else if(*Ptemp>='A'&&*Ptemp<='F'){
            SendDatatemp[i]=(*Ptemp-'A'+10)<<4;
                Ptemp++;
         }else if(*Ptemp>='a'&&*Ptemp<='F'){
           SendDatatemp[i]=(*Ptemp-'a'+10)<<4;
                Ptemp++;
         }
         if(*Ptemp>='0'&&*Ptemp<='9')
         {
           SendDatatemp[i]=SendDatatemp[i]|(*Ptemp-'0');
           DLC++;
           i++;
         }else if(*Ptemp>='A'&&*Ptemp<='F'){
           SendDatatemp[i]=SendDatatemp[i]|(*Ptemp-'A'+10);
           DLC++;
           i++;
         }else if(*Ptemp>='a'&&*Ptemp<='F'){
           SendDatatemp[i]=SendDatatemp[i]|(*Ptemp-'a'+10);
           DLC++;
           i++;
         }
         Ptemp++;
     }
     if(DLC>5){
        Sendpage += (DLC-5)/7;
        if((DLC-5)%7>0)
        {
           Sendpage += 2;
        }else{
           Sendpage ++;
        }
     }else{
        Sendpage = 1;
     }
     Ptemp = SendDatatemp;

     if(sel==0)
     {
          for(k=0;k<8&&DLC>0;k++)
          {
             TxMessage.Data[k] = *Ptemp;
             TxMessage.DLC++;
             DLC--;
             Ptemp++;
          }
          TxMessage.StdId = CANID;
          #ifdef   DEBUG
                BSP_CAN_PrintRxMsg((CanRxMsg *)&TxMessage);
          #endif
          TransmitMailbox=CAN_Transmit(CAN1, &TxMessage);
          timeout = 0;
          while((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (timeout != 0x1FF))
          {
            timeout++;
          }
          timeout = 0;
          while((CAN_MessagePending(CAN1, CAN_FIFO0) < 1) && (timeout != 0x1FF))
          {
            timeout++;
          }
     }else if(sel==1){
    //     Ptemp = SendDatatemp;
         for(i=0;i<Sendpage;i++)
         {
               if(i==0)
               {
                 if(Sendpage>1)
                 {
                    TxMessage.Data[0] = (0x20+*cnt);
                 }else{
                    TxMessage.Data[0] = (0x10+*cnt);
                 }
                    (*cnt)++;
                    *cnt = *cnt&0x0F;
                    TxMessage.Data[1] = 0x00;
                    TxMessage.Data[2] = DLC;
                    TxMessage.DLC = 3;
                    for(k=0;k<5&&DLC>0;k++)
                    {
                       TxMessage.Data[k+3] = *Ptemp;
                       TxMessage.DLC++;
                       DLC--;
                       Ptemp++;
                    }
                  TxMessage.StdId = CANID;
                    #ifdef   DEBUG
                          BSP_CAN_PrintRxMsg((CanRxMsg *)&TxMessage);
                    #endif
                  TransmitMailbox=CAN_Transmit(CAN1, &TxMessage);
                    timeout = 0;
                    while((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (timeout != 0x1FF))
                    {
                      timeout++;
                    }
                    timeout = 0;
                    while((CAN_MessagePending(CAN1, CAN_FIFO0) < 1) && (timeout != 0x1FF))
                    {
                      timeout++;
                    }
               }else{
                    if(DLC>7){
                        TxMessage.Data[0] = (0x20+*cnt);
                    }else{
                        TxMessage.Data[0] = (0x10+*cnt);
                    }
                    (*cnt)++;
                    *cnt = *cnt&0x0F;
                    TxMessage.DLC = 1;
                    for(k=0;k<7&&DLC>0;k++)
                    {
                       TxMessage.Data[k+1] = *Ptemp;
                       TxMessage.DLC++;
                       DLC--;
                       Ptemp++;
                    }
                  TxMessage.StdId = CANID;
                    #ifdef   DEBUG
                          BSP_CAN_PrintRxMsg((CanRxMsg *)&TxMessage);
                    #endif
                  TransmitMailbox=CAN_Transmit(CAN1, &TxMessage);
                    timeout = 0;
                    while((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (timeout != 0x1FF))
                    {
                      timeout++;
                    }
                    timeout = 0;
                    while((CAN_MessagePending(CAN1, CAN_FIFO0) < 1) && (timeout != 0x1FF))
                    {
                      timeout++;
                    }
               }
         }
     }else if(sel==2){
       if((u8)((*P)&0x0F)!=0x0F)
          TxMessage.Data[0]=0xB0+((*P)&0x0F)+1;
       else
          TxMessage.Data[0]=0xB0;
       TxMessage.DLC = 1;
       TxMessage.StdId = CANID;
       #ifdef   DEBUG
            BSP_CAN_PrintRxMsg((CanRxMsg *)&TxMessage);
       #endif
//       TransmitMailbox=CAN_Transmit(&TxMessage);
//        timeout = 0;
//        while((CAN_TransmitStatus(TransmitMailbox) != CANTXOK) && (timeout != 0x1FF))
//        {
//          timeout++;
//        }
//        timeout = 0;
//        while((CAN_MessagePending(CAN_FIFO0) < 1) && (timeout != 0x1FF))
//        {
//          timeout++;
//        }
            gpDrvCAN->transmit(CAN1, &TxMessage);
     }
//     CPU_CRITICAL_EXIT();
}

///////////////////////////////////////////////////////////////////////////////////
// UARTResolutionTask
///////////////////////////////////////////////////////////////////////////////////

UARTResolutionTask::UARTResolutionTask(uint8_t prio) : ZTask(prio){
}

void UARTResolutionTask::_doTask(void *p_arg)
{
	const static char d[] = {0x00, 0x00, 0x32, 0x32, 0x32};
	while(1) {
		sendData(0x543, 5, d);
		OSTimeDlyHMSM(0,0,0,50);
	}
}

///////////////////////////////////////////////////////////////////////////////////
// CanProtocol1Task
///////////////////////////////////////////////////////////////////////////////////

CanProtocol1Task::CanProtocol1Task(uint8_t prio) : ZTask(prio){
}

void CanProtocol1Task::_doTask(void *p_arg)
{
    u8   err;
    CanRxMsg    *RxMsg;
    CanRxMsg    RxMsgP1;

    u8 Recall = 0;
    u8 parkflag= 0;
    (void)p_arg;
    u8 AudiSysCheckCnt=0;

    // added by sjf
    _pSlot = CanHub::getInstance()->getSlot();
	uint8_t ret;

	if (_pSlot == 0) {
		zprintf("AudiA6_2011 getSlot failure.\n");
		return ;
	}

	ret =  _pSlot->addId(0x6d9);
	ret |= _pSlot->addId(0x545);
	ret |= _pSlot->addId(0x200);
	ret |= _pSlot->addId(0x4fc);

	if (ret != 0) {
		zprintf("AudiA6_2011 ID failure.\n");
		return ;
	}

    // end add

	while (1) {

		//RxMsg = (CanRxMsg *)(OSMboxPend(App_CanProtocol1Mbox, OS_TICKS_PER_SEC / 10, &err));
		CanTMsg msg;
		err = _pSlot->read(&msg, 100);
		RxMsg = &msg.msg;


		RxMsgP1 = *RxMsg;
        if(err == OS_ERR_NONE)
        {
#ifdef DEBUG
            BSP_CAN_PrintRxMsg(&RxMsgP1);
#endif
            if(RxMsgP1.StdId==0x000006d9)
            {
                if(RxMsgP1.Data[0]==0x12&&RxMsgP1.Data[1]==0xC1)
                {
                    Recall = 1;
                    OSTimeDlyHMSM(0, 0, 0, 2);//20
                    BSP_CAN_Send_Data(0x000006dc,"80 19 42 C1 03 00 0B 00");
                    OSTimeDlyHMSM(0, 0, 0, 7);//50
                    BSP_CAN_Send_Data(0x000006dc,"C0 03 00 38 01 FF 00 00");
                    OSTimeDlyHMSM(0, 0, 0, 7);//50
                    BSP_CAN_Send_Data(0x000006dc,"C1 00 00 00 0A 02 96 00");
                    OSTimeDlyHMSM(0, 0, 0, 7);//50
                    BSP_CAN_Send_Data(0x000006dc,"C2 00 00 32 32 32 FF 00");
                    err = OSMboxPost(App_RearViewCameraMbox, (void *)Recall);
#ifdef   DEBUG
              //      printf("Recall==1\n");
#endif
                }else if(RxMsgP1.Data[0]==0x22&&RxMsgP1.Data[1]==0xD2&&RxMsgP1.Data[2]==0x00){
                    Recall = 2;
                      OSTimeDlyHMSM(0, 0, 0, 2);//20
                    BSP_CAN_Send_Data(0x000006dc,"42 D2 00 00 00 00 00 00");
                    OSMboxPost(App_RearViewCameraMbox, (void *)Recall);
#ifdef   DEBUG
             //       printf("Recall==2\n");
#endif
                }else if(RxMsgP1.Data[0]==0x22&&RxMsgP1.Data[1]==0xD2&&RxMsgP1.Data[2]==0x01){
                    Recall = 2;
                    OSTimeDlyHMSM(0, 0, 0, 2);//20;
                    BSP_CAN_Send_Data(0x000006dc,"42 D2 01 00 00 00 00 00");
                    OSMboxPost(App_RearViewCameraMbox, (void *)Recall);
//                    if(PIPONOFFflag==1)
//                    {
//                        PIPONOFFflag = 0;
//                        PIPOFF();
//                    }else{
//                        PIPONOFFflag = 1;
//                        PIPON();
//                    }
#ifdef   DEBUG
               //     printf("Recall==2\n");
#endif
                }else if(RxMsgP1.Data[0]==0x22&&RxMsgP1.Data[1]==0xD7&&RxMsgP1.Data[2]==0x00&&RxMsgP1.Data[3]==0x00){
                //退出倒车
                  Recall = 2;
                      OSTimeDlyHMSM(0, 0, 0, 2);//20
                    BSP_CAN_Send_Data(0x000006dc,"42 D7 00 00 00 00 00 00");
                    //CamPowerCtrl(CAMPOWER_OFF);
                    OSMboxPost(App_RearViewCameraMbox, (void *)Recall);
#ifdef   DEBUG
               //     printf("Recall==2\n");
#endif
                }else if(RxMsgP1.Data[0]==0x22&&RxMsgP1.Data[1]==0xD7&&RxMsgP1.Data[2]==0x01&&RxMsgP1.Data[3]==0x01){
                  //进入倒车
                  Recall = 3;
                      OSTimeDlyHMSM(0, 0, 0, 2);//20
                    BSP_CAN_Send_Data(0x000006dc,"42 D7 01 01 00 00 00 00");
                    //CamPowerCtrl(CAMPOWER_ON);


                    OSMboxPost(App_RearViewCameraMbox, (void *)Recall);
#ifdef   DEBUG
                 //   printf("Recall==3\n");
#endif
                }else if(RxMsgP1.Data[0]==0x22&&RxMsgP1.Data[1]==0xD7&&RxMsgP1.Data[2]==0x00&&RxMsgP1.Data[3]==0x01){
                    Recall = 3;
                      OSTimeDlyHMSM(0, 0, 0, 2);//20
                    BSP_CAN_Send_Data(0x000006dc,"42 D7 00 01 00 00 00 00");
                    OSMboxPost(App_RearViewCameraMbox, (void *)Recall);
#ifdef   DEBUG
                   // printf("Recall==3\n");
#endif
                }else if(RxMsgP1.Data[0]==0x22&&RxMsgP1.Data[1]==0xD7&&RxMsgP1.Data[2]==0x02&&RxMsgP1.Data[3]==0x01){
                    Recall = 3;
                    OSTimeDlyHMSM(0, 0, 0, 2);//20
                    BSP_CAN_Send_Data(0x000006dc,"42 D7 02 01 00 00 00 00");
                    OSMboxPost(App_RearViewCameraMbox, (void *)Recall);
#ifdef   DEBUG
                  //  printf("Recall==3\n");
#endif
                }
            }
            if(RxMsgP1.StdId==0x00000545)
            {
              if((u8)(RxMsgP1.Data[0]&0xF0)==0x10&&parkflag==0){
            	      OSTimeDlyHMSM(0, 0, 0, 1);//0
                      BSP_CAN_Send_Data(0x000006dc,"42 D7 FF 01 00 00 00 00");

                    /*  PIP_Reveral(AVInputBVflag);*/
                /*      AVInputBVflag = 1-AVInputBVflag;
                      InputSel(AVInputBVflag);
                      TW99_Input_sel(1-AVInputBVflag);*/
                      parkflag = 1;
                  }else if((u8)(RxMsgP1.Data[0])==0x00&&parkflag == 1){
                	  OSTimeDlyHMSM(0, 0, 0, 1);//0
                      BSP_CAN_Send_Data(0x000006dc,"42 D7 FF 00 00 00 00 00");
                    /*  PIP_Reveral(AVInputBVflag);*/
                    /*  AVInputBVflag = 1-AVInputBVflag;
                      InputSel(AVInputBVflag);
                      TW99_Input_sel(1-AVInputBVflag);*/
                      parkflag = 0;
                  }
            }
            if(RxMsgP1.StdId==0x00000200)
            {
               if((u8)(RxMsgP1.Data[0])==0x49)
               {
                  BSP_CAN_Send_Data(0x00000249,"00 D0 00 03 FC 04 01");
                  AudiSysCheckCnt = 0;
               }
            }
            if(RxMsgP1.StdId==0x000004FC)
            {
               if((u8)(RxMsgP1.Data[0])==0xA0)
               {
                  BSP_CAN_Send_Data(0x00000300,"A1 0F 8A FF 4A FF");
                  AudiSysCheckCnt = 0;
               }else if((u8)(RxMsgP1.Data[0]&0xF0)==0x10){
                  if((u8)(RxMsgP1.Data[1])==0x00&&(u8)(RxMsgP1.Data[2])==0x02&&(u8)(RxMsgP1.Data[3])==0x10&&(u8)(RxMsgP1.Data[4])==0x89)
                  {
                          BSP_CAN_SEND_MY(0x00000300,&RxMsgP1.Data[0],2,0);
                          OSTimeDlyHMSM(0, 0, 0, 50);
                          BSP_CAN_SEND_MY(0x00000300,(u8 *)AudiCheckAn1,1,&AudiSysCheckCnt);
                  }else if((u8)(RxMsgP1.Data[1])==0x00&&(u8)(RxMsgP1.Data[2])==0x02&&(u8)(RxMsgP1.Data[3])==0x1A&&(u8)(RxMsgP1.Data[4])==0x9B){
                          BSP_CAN_SEND_MY(0x00000300,&RxMsgP1.Data[0],2,0);
                          OSTimeDlyHMSM(0, 0, 0, 50);
                          BSP_CAN_SEND_MY(0x00000300,(u8 *)AudiCheckAn2,1,&AudiSysCheckCnt);
                  }else if((u8)(RxMsgP1.Data[1])==0x00&&(u8)(RxMsgP1.Data[2])==0x04&&(u8)(RxMsgP1.Data[3])==0x31&&(u8)(RxMsgP1.Data[4])==0xB8&&(u8)(RxMsgP1.Data[5])==0x00&&(u8)(RxMsgP1.Data[6])==0x00){
                          BSP_CAN_SEND_MY(0x00000300,&RxMsgP1.Data[0],2,0);
                          OSTimeDlyHMSM(0, 0, 0, 50);
                          BSP_CAN_SEND_MY(0x00000300,(u8 *)AudiCheckAn3,1,&AudiSysCheckCnt);
                  }else if((u8)(RxMsgP1.Data[1])==0x00&&(u8)(RxMsgP1.Data[2])==0x02&&(u8)(RxMsgP1.Data[3])==0x1A&&(u8)(RxMsgP1.Data[4])==0x91){
                          BSP_CAN_SEND_MY(0x00000300,&RxMsgP1.Data[0],2,0);
                          OSTimeDlyHMSM(0, 0, 0, 50);
                          BSP_CAN_SEND_MY(0x00000300,(u8 *)AudiCheckAn4,1,&AudiSysCheckCnt);
                  }else if((u8)(RxMsgP1.Data[1])==0x00&&(u8)(RxMsgP1.Data[2])==0x03&&(u8)(RxMsgP1.Data[3])==0x14&&(u8)(RxMsgP1.Data[4])==0xFF&&(u8)(RxMsgP1.Data[5])==0x00){
                          BSP_CAN_SEND_MY(0x00000300,&RxMsgP1.Data[0],2,0);
                          OSTimeDlyHMSM(0, 0, 0, 50);
                          BSP_CAN_SEND_MY(0x00000300,(u8 *)AudiCheckAn5,1,&AudiSysCheckCnt);
                  }else if((u8)(RxMsgP1.Data[1])==0x00&&(u8)(RxMsgP1.Data[2])==0x04&&(u8)(RxMsgP1.Data[3])==0x18&&(u8)(RxMsgP1.Data[4])==0x02&&(u8)(RxMsgP1.Data[5])==0xFF&&(u8)(RxMsgP1.Data[6])==0x00){
                          BSP_CAN_SEND_MY(0x00000300,&RxMsgP1.Data[0],2,0);
                          OSTimeDlyHMSM(0, 0, 0, 50);
                          BSP_CAN_SEND_MY(0x00000300,(u8 *)AudiCheckAn6,1,&AudiSysCheckCnt);
                  }else if((u8)(RxMsgP1.Data[1])==0x00&&(u8)(RxMsgP1.Data[2])==0x02&&(u8)(RxMsgP1.Data[3])==0x1A&&(u8)(RxMsgP1.Data[4])==0x86){
                          BSP_CAN_SEND_MY(0x00000300,&RxMsgP1.Data[0],2,0);
                          OSTimeDlyHMSM(0, 0, 0, 50);
                          BSP_CAN_SEND_MY(0x00000300,(u8 *)AudiCheckAn7,1,&AudiSysCheckCnt);
                  }else if((u8)(RxMsgP1.Data[1])==0x00&&(u8)(RxMsgP1.Data[2])==0x02&&(u8)(RxMsgP1.Data[3])==0x1A&&(u8)(RxMsgP1.Data[4])==0x9C){
                          BSP_CAN_SEND_MY(0x00000300,&RxMsgP1.Data[0],2,0);
                          OSTimeDlyHMSM(0, 0, 0, 50);
                          BSP_CAN_SEND_MY(0x00000300,(u8 *)AudiCheckAn8,1,&AudiSysCheckCnt);
                  }
               }else if((u8)(RxMsgP1.Data[0])==0xA3){
                       BSP_CAN_Send_Data(0x00000300,"A1 0F 8A FF 4A FF");
               }

            }
         }
        OSTimeDlyHMSM(0, 0, 0, 10);
      }
}

///////////////////////////////////////////////////////////////////////////////////
// RearViewCameraTask
///////////////////////////////////////////////////////////////////////////////////

RearViewCameraTask::RearViewCameraTask(uint8_t prio) : ZTask(prio){
}

void RearViewCameraTask::_doTask(void *p_arg)
{
    uint8_t  err;
    uint32_t Recall;
    uint8_t *msg;
    u16 timecnt = 56;//500
    u32 timeoutcnt = 0;
    (void)p_arg;
    Recall  = 0;
  //BSP_CAN_Filter((u32 *)AudiP1Filter);
//  while(1){
  //  OSTimeDlyHMSM(0, 0, 0, 200);
 // }
retry:
    BSP_CAN_Send_Data(0x000006dc,"02 C2 03 00 0B 00 03 00");
    while(1) {
        msg = (uint8_t *)(OSMboxPend(App_RearViewCameraMbox, OS_TICKS_PER_SEC / 10, &err));
        Recall = (uint32_t)msg;
       // printf("TaskRearViewCameraMbox_err %d\n",err);
        if(err == OS_ERR_NONE)
        {
             timeoutcnt = 0;
           if(Recall==1)
           {
              Recall = 0;
              break;
           }
        }else{
           timeoutcnt++;
           if(timeoutcnt>60)
           {
             timeoutcnt = 0;
              goto retry;
           }
        }
    }
//#define WAIT_FOR_RECALL2
#ifdef WAIT_FOR_RECALL2
   while(1) {
        msg = (CPU_INT08U *)(OSMboxPend(App_RearViewCameraMbox, OS_TICKS_PER_SEC / 10 , &err));
        if(err == OS_NO_ERR)
        {
           Recall = (CPU_INT32U)msg;
           if(Recall==2)
           {
              Recall = 0;
              break;
           }
        }
    }
#endif

    while (1) {
     //   Recall = *(u8 *)(OSMboxPend(App_RearViewCameraMbox, OS_TICKS_PER_SEC / 10, &err));
        BSP_CAN_Send_Data(0x000006dc,"32 D5 32 00 00 00 00 00");
        OSTimeDlyHMSM(0, 0, 0, timecnt);
        BSP_CAN_Send_Data(0x000006dc,"32 D6 32 00 00 00 00 00");
        OSTimeDlyHMSM(0, 0, 0, timecnt);
        BSP_CAN_Send_Data(0x000006dc,"32 C2 03 00 0B 00 03 00");
        OSTimeDlyHMSM(0, 0, 0, timecnt);
        BSP_CAN_Send_Data(0x000006dc,"32 C3 38 01 FF 00 00 00");
        OSTimeDlyHMSM(0, 0, 0, timecnt);
        BSP_CAN_Send_Data(0x000006dc,"32 C4 0A 00 00 00 00 00");
        OSTimeDlyHMSM(0, 0, 0, timecnt);
        BSP_CAN_Send_Data(0x000006dc,"32 CF 00 00 00 00 00 00");
        OSTimeDlyHMSM(0, 0, 0, timecnt);
        BSP_CAN_Send_Data(0x000006dc,"32 D0 96 00 00 00 00 00");
        OSTimeDlyHMSM(0, 0, 0, timecnt);
        BSP_CAN_Send_Data(0x000006dc,"32 D1 00 00 00 00 00 00");
        OSTimeDlyHMSM(0, 0, 0, timecnt);
        BSP_CAN_Send_Data(0x000006dc,"32 D2 00 00 00 00 00 00");
        OSTimeDlyHMSM(0, 0, 0, timecnt);
        BSP_CAN_Send_Data(0x000006dc,"32 D3 02 00 00 00 00 00");
        OSTimeDlyHMSM(0, 0, 0, timecnt);
        BSP_CAN_Send_Data(0x000006dc,"32 D4 32 00 00 00 00 00");
        OSTimeDlyHMSM(0, 0, 0, timecnt);
   //     ParkENflag = 1;
    }
}
