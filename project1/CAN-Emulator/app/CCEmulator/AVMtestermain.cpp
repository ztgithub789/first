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
#include <stdio.h>
#include <utils/Shell/Shell.h>
#include <Zox.h>
#include "includes.h"
#include <board.h>
#include "common.h"
#include <CanHub/CanHub.h>
#include "RandomMsgTask.h"
#include "secure/GetKey.h"
#include "ComKeepingTaskTester/ListeningTask.h"
#include "ComKeepingTaskTester/StimTask.h"
#include "ComKeepingTaskTester/ComKeepingTaskTester.h"
//#include "RVC-D02/include/sysconfig.h"
/*------------------------ Car header file ------------------------*/
#include "Car/CarInfo.h"
#include "Car/Tester.h"

#include "Car/vwTuang/vwTuangTester.h"
#include "Car/vwTuang/VWTuAng_common.h"

const uint16_t STK_SIZE = 512;
static OS_STK _stk[STK_SIZE*2];

static const char *date = "2018-04-02";
const static char *version = "2.3.6";
const static char *banner =
		"\n********************************************************\n"
		"RoadPassion Electronics Co., Ltd. Guangzhou, China.\n"
		"Model: RVC-D02AD Tester.\n"
		"Version: %s\n"
		"********************************************************\n\n";
const static char *cartype = 
		"\n\n\t1:VWTuang TuGuan.\n";

#define LED(n,status)   gpDrvLED##n->##status()
#define ROW(n,status)   gpDrvROW##n->##status()

static Tester *pTester = NULL;
static CarInfo *gpCar = NULL; 
static OS_TMR *_pTmr;
static OS_TMR *_pTmr0;
static OS_TMR *_pTmr1;
static char PressNum = 0;
static bool isTwoSecond = false;

/********test ID*********/
using namespace vwTuang;
/************************/
static void handleTmrTask(void *ptmr,void *callback_arg)
{
	gpDrvTJA1041->enterPownMode();
	OSTimeDlyHMSM(0,0,0,50);
	gpDrvTJA1041->enterNormalMode();

	gpDrvTJA1041_2->enterPownMode();
	OSTimeDlyHMSM(0,0,0,50);
	gpDrvTJA1041_2->enterNormalMode();
    
	bool status = gpDrvLEDRed->getValue();
	if( 0 == status ){
		gpDrvLEDRed->on();
		gpDrvLEDGreen->off();
	} else {
		gpDrvLEDRed->off();
		gpDrvLEDGreen->on();
	}
}

static void _msgREDHeadler(void *param)
{
	INT8U err_Tmr;
	OSTmrStop(_pTmr,OS_TMR_OPT_NONE,(void*)0,&err_Tmr);
    
	bool status = gpDrvLEDRed->getValue();
	if( 0 == status ){
		gpDrvLEDRed->on();
	}
	else{
		gpDrvLEDRed->off();
	}
	OSTmrStart(_pTmr,&err_Tmr);
}

void SetCar(char _CarNum)
{
    int bdrate = 0;
    int bdrate2 = 0;   

    if(pTester->pAutoControl != NULL){
        delete pTester->pAutoControl;
        pTester->pAutoControl = NULL;
    }    
    if(gpCar != NULL){
        delete gpCar;
        gpCar = NULL;
    }
    if(pTester != NULL){
        delete pTester;
        pTester = NULL;
    }

	bdrate = 500;
	bdrate2 = 500;
	pTester = new vwTuangTester();

	if (gpDrvCAN->setBaudrate(CAN1, bdrate) != DrvCAN::E_OK) {
		zprintf("bdrate set failure.\n");
	}else{
        zprintf("bdrate:%d ok.\n",bdrate);
    }
#ifdef USE_BVLOADER
	if (gpDrvCAN->setBaudrate(CAN2, bdrate2) != DrvCAN::E_OK) {
		zprintf("bdrate2 set failure.\n");
	}else{
        zprintf("bdrate2:%d ok.\n",bdrate2);
    }
#endif
 
	gpCar = new CarInfo(); 
	pTester->run(gpCar);
}

static void _SetCarHandler(void *ptmr, void *parg){
    INT8U err_Tmr;
    OSTmrStop(_pTmr,OS_TMR_OPT_NONE,(void *)0,&err_Tmr);
    
    PressNum = 0;
    isTwoSecond = true;
    
    OSTmrStart(_pTmr,&err_Tmr);
}

static void _CarNumLEDHandler(void *ptmr, void *parg){
    INT8U err_Tmr;
    OSTmrStop(_pTmr,OS_TMR_OPT_NONE,(void *)0,&err_Tmr);
    
    char n = (*(char *)parg)%10+2;
    static char status = 1;
        
    if(0==status){
        status = 1;
        switch(n){
            case 3:
                LED(3,off);
                break;
            case 4:
                LED(4,off);
                break;
            case 5:
                LED(5,off);
                break;
            case 6:
                LED(6,off);
                break;
            case 7:
                LED(7,off);
                break;
            case 8:
                LED(8,off);
                break;
            case 9:
                LED(9,off);
                break;
            case 10:
                LED(10,off);
                break;
            case 11:
                LED(11,off);
                break;
            default:
                break;      
        }
    }else if(1==status){
        status = 0;
        switch(n){
            case 3:
                LED(3,on);
                break;
            case 4:
                LED(4,on);
                break;
            case 5:
                LED(5,on);
                break;
            case 6:
                LED(6,on);
                break;
            case 7:
                LED(7,on);
                break;
            case 8:
                LED(8,on);
                break;
            case 9:
                LED(9,on);
                break;
            case 10:
                LED(10,on);
                break;
            case 11:
                LED(11,on);
                break;
            default:
                break;                
        }
    }
    
    OSTmrStart(_pTmr,&err_Tmr);
}

void SetAllCarLedStatus(bool status)
{
    if(status)
    {
        LED(3,on);
        LED(4,on);
        LED(5,on);
        LED(6,on);
        LED(7,on);
        LED(8,on);
        LED(9,on);
        LED(10,on);

        LED(11,on);
    }else{
        LED(3,off);
        LED(4,off);
        LED(5,off);
        LED(6,off);
        LED(7,off);
        LED(8,off);
        LED(9,off);
        LED(10,off);

        LED(11,off);    
    }
}

bool showCarLED(unsigned char _CarNum)
{
     SetAllCarLedStatus(false);
     switch(_CarNum){
       case 3:
           LED(3,on);
           break;
       case 4:
           LED(4,on);
           break;
       case 5:
           LED(5,on);
           break;
       case 6:
           LED(6,on);
           break;
       case 7:
           LED(7,on);
           break;
       case 8:
           LED(8,on);
           break;
       case 9:
           LED(9,on);
           break;
       case 10:
           LED(10,on);
           break;
       case 11:
           LED(11,on);
           break;       
       default:
           SetAllCarLedStatus(true);
           break;
     }
     return true;
}

void writeROW(unsigned char num)
{          
    switch(num){
        case 0:
            ROW(1,off);
            ROW(2,on);
            ROW(3,on);
            ROW(4,on);
            ROW(5,on);
            break;
        case 1:
            ROW(1,on);
            ROW(2,off);
            ROW(3,on);
            ROW(4,on);
            ROW(5,on);      
            break;
        case 2:
            ROW(1,on);
            ROW(2,on);
            ROW(3,off);
            ROW(4,on);
            ROW(5,on);      
            break;
        case 3:
            ROW(1,on);
            ROW(2,on);
            ROW(3,on);
            ROW(4,off);
            ROW(5,on);      
            break;
        case 4:
            ROW(1,on);
            ROW(2,on);
            ROW(3,on);
            ROW(4,on);
            ROW(5,off);      
            break;      
        default:
            break;
    }
}

unsigned char readCOL()
{  
    if(gpDrvCOL1->getValue()){
        OSTimeDlyHMSM(0,0,0,120);
        if(gpDrvCOL1->getValue()){
            return 1;
        }else{
            return -1;
        }
    }else if(gpDrvCOL2->getValue()){
            OSTimeDlyHMSM(0,0,0,120);
            if(gpDrvCOL2->getValue()){
                return 2;
            }else{
                return -1;
            }
    }else if(gpDrvCOL3->getValue()){
            OSTimeDlyHMSM(0,0,0,120);
            if(gpDrvCOL3->getValue()){
                return 3;
            }else{
                return -1;
            }
    }else if(gpDrvCOL4->getValue()){
            OSTimeDlyHMSM(0,0,0,120);
            if(gpDrvCOL4->getValue()){
                return 4;
            }else{
                return -1;
            }
    }else if(gpDrvCOL5->getValue()){
            OSTimeDlyHMSM(0,0,0,120);
            if(gpDrvCOL5->getValue()){
                return 5;
            }else{
                return -1;
            }
    }else{
        return -1; 
    }
}

int readBoardKey(char *_keycode)
{   
    char i, col, j;
    
    for(i=0,j=0; i<5; i++)
    {
       writeROW(i);
       if((col=readCOL()) != -1){
         *_keycode = col + j;
         return 0;
       }else{
         j +=5; 
       }
    }  
    *_keycode = 0;
    return -1;
}

void _CreateCar(void *prg)
{
    int bdrate = 0;
    int bdrate2 = 0;
    OSTimeDlyHMSM(0,0,0,500);
    static char KeyCode = 0;
    static char CarNum = 0;
    bool auto_control = false;
    gpDrvTJA1041_2->enterNormalMode();
    
	zprintf(banner, version, date);
//红绿灯循环闪烁定时器    
	INT8U err_Tmr;
	_pTmr = OSTmrCreate(50,						
            			50,
            			OS_TMR_OPT_PERIODIC,
            			(OS_TMR_CALLBACK)handleTmrTask, 
            			(void*)0,
            			(INT8U*)"47",
            			&err_Tmr);
	if (err_Tmr != OS_ERR_NONE){
        zprintf("OSTmrCreate function failed to create Tmr\n");
        zprintf("err_Tmr=%d\n",err_Tmr);
	}
//延时2s等待车型选择定时器    
    INT8U err_Tmr0;
    _pTmr0 = OSTmrCreate(300,
                         0,
                         OS_TMR_OPT_ONE_SHOT,
                         (OS_TMR_CALLBACK)_SetCarHandler,
                         (void *)&CarNum,
                         (INT8U *)"45",
                         &err_Tmr0);
   if (err_Tmr0 != OS_ERR_NONE){
        zprintf("OSTmrCreate function failed to create Tmr0\n");
        zprintf("err_Tmr0=%d\n",err_Tmr0);
	}
//车型LED指示灯闪烁定时器
    INT8U err_Tmr1;
    _pTmr1 = OSTmrCreate(50,
                         60,
                         OS_TMR_OPT_PERIODIC,
                         (OS_TMR_CALLBACK)_CarNumLEDHandler,
                         (void*)&CarNum,
                         (INT8U *)"46",
                         &err_Tmr1);
    if (err_Tmr1 != OS_ERR_NONE){
        zprintf("OSTmrCreate function failed to create Tmr1\n");
        zprintf("err_Tmr1=%d\n",err_Tmr1);
	}
//处理产品激活任务    
   // static GetKey getkey(GRTKEY_TASK_PRIO);
  //  getkey.start(0); 
    
	gpDrvCAN->setOnMsgSendHandler((DrvCAN::Send_T)_msgREDHeadler,NULL);
	OSTmrStart(_pTmr,&err_Tmr);

//默认车型

	bdrate = 500;
	bdrate = 500;
	pTester = new vwTuangTester();
#if 0
	if (gpDrvCAN->setBaudrate(CAN1, bdrate) != DrvCAN::E_OK) {
		zprintf("bdrate set failure.\n");
	}else{
		zprintf("bdrate:%d ok.\n",bdrate);
	}

	if (gpDrvCAN->setBaudrate(CAN2, bdrate2) != DrvCAN::E_OK) {
		zprintf("bdrate2 set failure.\n");
	}else{
		zprintf("bdrate2:%d ok.\n",bdrate2);
	}
#endif   
	gpCar = new CarInfo(); 
	pTester->run(gpCar);				
          
	pTester->pAutoControl = new Tester::AutoControlTask(AUTOCONTROL_TASK_PRIO);
	pTester->pAutoControl->start(gpCar); 
      
    SetAllCarLedStatus(false);

	showCarLED(6);
	CarNum= 41;
	OSTmrStart(_pTmr1,&err_Tmr1);

	while(1) { 		              
        if(readBoardKey(&KeyCode)==0){
            switch(KeyCode) {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:  
                case 9:
                case 10: 
                /* Car to choose */
                    if(KeyCode != 10){
                        if(0==PressNum){
                            //printf("first=%d\n",KeyCode);
                            OSTmrStop(_pTmr1,OS_TMR_OPT_NONE,(void *)0,&err_Tmr1);
                            PressNum += 1;
                            showCarLED(KeyCode+2);
                            CarNum = 0;
                            CarNum = 10*KeyCode;                           
                            OSTmrStart(_pTmr0,&err_Tmr0);
                            continue;
                        }else if(1==PressNum){
                            //printf("second=%d\n",KeyCode);
                            OSTmrStop(_pTmr0,OS_TMR_OPT_NONE,(void *)0,&err_Tmr0);
                            PressNum = 0;
                            CarNum += KeyCode;
                            OSTmrStart(_pTmr1,&err_Tmr1);
                            SetCar(CarNum);
                            zprintf("car:%d ok.\n",CarNum);
                        }
                    }else{
                        OSTmrStop(_pTmr1,OS_TMR_OPT_NONE,(void *)0,&err_Tmr1);                        OSTmrStop(_pTmr0,OS_TMR_OPT_NONE,(void *)0,&err_Tmr0);

                        CarNum = 0;
                        CarNum = KeyCode;
                        showCarLED(CarNum+2);
                        SetCar(CarNum);
                        zprintf("car:%d ok.\n",CarNum);
                    }                    
                    break;
                case 11://P档
                    gpCar->shiftPos = CarInfo::SHIFT_POS_P;
                    break;
                case 12://R档
                    gpCar->shiftPos = CarInfo::SHIFT_POS_R;
                    break;
                case 13://P键
                    if(gpCar->isPKeyPressed) {
                	    gpCar->isPKeyPressed= false;
                    } else {
                	    gpCar->isPKeyPressed= true;
                    }
					gpCar->TireKey = 11;
                    break;
                case 14://* return
                    gpCar->TireKey = 1;
					if(gpCar->isACCOn) {
                 	   gpCar->isACCOn= false;
                    } else {
                 	   gpCar->isACCOn= true;
                    }
                    break;
                case 15://NAVI phone_answer Remote_Trunk_Key
                    gpCar->TireKey = 2;
                    break;
                case 16://voice Front_Door_Trunk_Key
                    gpCar->TireKey = 3; 
                    break;
                case 17://scroll_up turn_left Page_up
                    gpCar->TireKey = 5;
                    pTester->steerLeft();                  
                    break;
                case 18://scroll_down turn_right Page_down
                    gpCar->TireKey = 6;                    
                    pTester->steerRight();
                    break;
                case 19://scroll_ok turn_middle
                    gpCar->TireKey = 7;
                    break;
                case 20://Turn_signal left(all) left(Audi/BenZ/BWM/Tourage) hangup(BenZNewC) page_left(Golf7/AudiA3) MovePrevious(volvo)                   
                    gpCar->lightPos &= 0xF8;
                    gpCar->lightPos |= 1;
                    gpCar->TireKey = 8;
                    break;
                case 21://Turn_signal right(all) right(Audi/BenZ/BWM/Tourage) voice_off(BenZNewC) page_right(Golf7/AudiA3) MoveNext(volvo)                 
                    gpCar->TireKey = 9;     
                    gpCar->lightPos &= 0xF8;
                    gpCar->lightPos |= 2;
                    break;
                case 22://Menu(Audi/BWM/Tourage) HOME(BenZNewC) hangup(BenZ) answer_phone(Golf7) speed+
                    gpCar->TireKey = 4;
                    gpCar->lightPos &= 0xF8;
                    break;
                case 23://signal_flash
                    gpCar->lightPos &= 0xF8;
                    gpCar->lightPos |= 4;
                    gpCar->TireKey = 10;
                    break;
                case 24://静音 back_key
                    gpCar->TireKey = 12;
                    break;                
                case 25://自动   
                    if(auto_control==false) {
                        auto_control = true;
                        pTester->pAutoControl = new Tester::AutoControlTask(AUTOCONTROL_TASK_PRIO);
                        pTester->pAutoControl->start(gpCar);
                    } else {
                        delete pTester->pAutoControl;
                        pTester->pAutoControl = NULL;
                        auto_control = false;
                    }
                    break;
                default:			
                    break;
            }
        }else{
            if(isTwoSecond){                
                SetCar(CarNum/10);
                zprintf("car:%d ok.\n",CarNum/10);
                isTwoSecond = false;
            }
            
            gpCar->TireKey = 0; 
            gpCar->MMIKey = 0;
            gpCar->PageTrunKey = 0;
            gpCar->isRTKPressed = false;
            gpCar->isFDTKPressed = false; 
        }
             
		OSTimeDlyHMSM(0,0,0,25);
	}
  
/*  CLEAR  ALL  */
//DELETE_ALL:
	if((auto_control == true)&&(pTester->pAutoControl != NULL)) {
		delete pTester->pAutoControl;
		pTester->pAutoControl = NULL;
	}
    if(gpCar != NULL){
    	delete gpCar;
    	gpCar = NULL;
    }
    if(pTester != NULL){
    	delete pTester;
    	pTester = NULL;	
    }


}

int  appMain(void *p_arg)
{

	(void)p_arg;

	uint8_t err = OSTaskCreateExt((void (*)(void *))_CreateCar,
			(void          * )0,
			(OS_STK        * )&_stk[STK_SIZE*2- 1],
			(INT8U           )19,
			(INT16U          )19,
			(OS_STK        * )&_stk[0],
			(INT32U          )STK_SIZE*2,
			(void          * )0,
			(INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	if (err != OS_ERR_NONE) {
		zprintf("ZTask: create task failure. Err: %d, pir = %d\n", err, 19);
	}  

	SetAllCarLedStatus(false);
	CanTMsg msg;

	int bdrate = 500;
    int bdrate2 = 500;

	const static uint32_t idList[] = {
		SHIFT_POS_MSG_ID,
		STEERING_WHEER_ANGLE_MSG_ID,
		RADAR_MSG_ID,
		TURNIDICATOR_STATUS_MSD_ID,
		P_KEY_STATUS_MSG_ID,
		STEERING_WHEEL_SIDE_KEY_MSG_ID,
		BV_STATUS_MSG_ID,

		TESTER_MSG_ID1,
		TESTER_MSG_ID2,
		TESTER_MSG_ID3,
		0x0,
	};
	gpDrvCAN->addFilters(idList);

#if 1//def USE_BVLOADER	
	if (gpDrvCAN->setBaudrate(CAN1, bdrate) != DrvCAN::E_OK) {
		zprintf("bdrate1 set failure.\n");
	}else{
        zprintf("bdrate1:%d ok.\n",bdrate);
    }

	if (gpDrvCAN->setBaudrate(CAN2, bdrate2) != DrvCAN::E_OK) {
		zprintf("bdrate22 set failure.\n");
	}else{
        zprintf("bdrate22:%d ok.\n",bdrate2);
    }
#endif

#if 1
    while(1) 
    {   
		uint8_t err = gpDrvCAN->getData(&msg,0);
		if (err == OS_ERR_NONE) 
		{
			if(msg.CANx == CAN2){
				if(msg.msg.IDE == CAN_ID_STD) 
				{
					switch(msg.msg.StdId) 
					{
						case SHIFT_POS_MSG_ID:
						case STEERING_WHEER_ANGLE_MSG_ID:
						case TURNIDICATOR_STATUS_MSD_ID:
						case P_KEY_STATUS_MSG_ID:
						case STEERING_WHEEL_SIDE_KEY_MSG_ID:
							gpDrvLED8->getValue() ? gpDrvLED8->off() : gpDrvLED8->on();
							break;
						default:
							break;
					}
				}else{
					switch(msg.msg.ExtId) 
					{
						case RADAR_MSG_ID:
						case BV_STATUS_MSG_ID:
							gpDrvLED8->getValue() ? gpDrvLED8->off() : gpDrvLED8->on();
							break;
						default:
							break;
					}
				}
			}else{
				// CAN1
				if(msg.msg.IDE == CAN_ID_STD) 
				{
					switch(msg.msg.StdId) 
					{
						case TESTER_MSG_ID1:
						case TESTER_MSG_ID2:
							gpDrvLED9->getValue() ? gpDrvLED9->off() : gpDrvLED9->on();
							break;
						default:
							break;
					}
				}else{
					switch(msg.msg.ExtId) 
					{
						case TESTER_MSG_ID3:
							gpDrvLED9->getValue() ? gpDrvLED9->off() : gpDrvLED9->on();
							break;
						default:
							break;
					}
				}
			}
		}
         OSTimeDlyHMSM(0,0,0,20);
    }
 #endif
 
	return 0;
}
