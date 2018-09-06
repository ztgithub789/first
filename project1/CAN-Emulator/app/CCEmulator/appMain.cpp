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
#include "Car/AudiA1_2012/AudiA1_2012Tester.h"
#include "Car/AudiA6_2011b/AudiA6_2011bTester.h"
#include "Car/AudiA6_2012/AudiA6_2012Tester.h"
#include "Car/AudiQ3_2012_imported/AudiQ3_2012_importedTester.h"  
#include "Car/AudiQ3_2013_domestic/AudiQ3_2013_domesticTester.h"
#include "Car/AudiA4/AudiA4Tester.h"
#include "Car/AudiQ7_2012/AudiQ7_2012Tester.h"
#include "Car/AudiQ7_2014/AudiQ7_2014Tester.h"
#include "Car/AudiA3_2014/AudiA3_2014Tester.h"
#include "Car/AudiA8/AudiA8Tester.h"
#include "Car/BenZ/BenZTester.h"
#include "Car/BenZ50_500/BenZ50_500Tester.h"
#include "Car/Touareg/TouaregTester.h"
#include "Car/Magotan/MagotanTester.h"
#include "Car/Golf7/Golf7Tester.h"
#include "Car/BMW/BMWTester.h"
#include "Car/ToyotaVellfire/ToyotaVellfireTester.h"
#include "Car/HondaCity_2014/HondaCity_2014Tester.h"
#include "Car/LandRover/LandRoverTester.h"
#include "Car/volvo/volvoTester.h"
#include "Car/volvo_V40/volvo_V40Tester.h"
#include "Car/Cadillac_XT5/Cadillac_XT5Tester.h"
#include "Car/LandRover_Jaguar/LandRover_JaguarTester.h"
#include "Car/NissanX_Trail_2015/NissanX_TrailTester.h"
#include "Car/LEXUS/LEXUSTester.h"
#include "Car/AudiQ7_2016/AudiQ7_2016Tester.h"
#include "Car/PEUGEOT/PEUGEOTTester.h"
#include "Car/LR_B_Sport/LR_B_SportTester.h"
#include "Car/Ben50_OBD/BenZ50_OBDTester.h"
#include "Car/Porsche/PorscheTester.h"
#include "Car/Landrover_DB/Landrover_DBTester.h"
#include "Car/Landrover_DH/Landrover_DHTester.h"
#include "Car/RangeRover/RangeRoverTester.h"
#include "Car/BMW3_Series/BMW3_SeriesTester.h"
#include "Car/Ford/FordTester.h"
#include "Car/HondaVezel/HondaVezelTester.h"
#include "Car/LandCruiser/LandCruiserTester.h"
#include "Car/SUZUK/SUZUKTester.h"
#include "Car/vwTuang/vwTuangTester.h"

//#define   TEST_TURN_SIG
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
		"\n\n\t1:AudiA6 A7 A8 2012.\n"
		"\t2:AudiA6 2011.\n"
		"\t3:AudiA4.\n"
		"\t4:AudiQ7.\n"
		"\t5:AudiQ3_imported.\n"
		"\t6:AudiA1.\n"
		"\t7:Volk Wagon.\n"
		"\t8:AudiQ3_domestic.\n"
		"\t9:Touareg\n"
		"\ta:BenZ\n"
		"\tb:AudiQ3_2013_domestic\n"
		"\tc:AudiA3_2014\n"
        "\td:AudiQ7_2014\n"
        "\te:Golf7\n"
        "\tf:BMW\n"
        "\tg:BenZ50\n"
        "\th:BenZ50_500\n\n";

#define LED(n,status)   gpDrvLED##n->##status()
#define ROW(n,status)   gpDrvROW##n->##status()

static Tester *pTester = NULL;
static CarInfo *gpCar = NULL; 
static OS_TMR *_pTmr;
static OS_TMR *_pTmr0;
static OS_TMR *_pTmr1;
static char PressNum = 0;
static bool isTwoSecond = false;

static void handleTmrTask(void *ptmr,void *callback_arg)
{
	gpDrvTJA1041->enterPownMode();
	OSTimeDlyHMSM(0,0,0,50);
	gpDrvTJA1041->enterNormalMode();
    
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

//static void _msgGREENHeadler(void *param)
//{
//	bool status = gpDrvLEDGreen->getValue();
//	if( 0 == status ){
//		gpDrvLEDGreen->on();
//	}
//	else{
//		gpDrvLEDGreen->off();
//	}
//}

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
    
	switch(_CarNum){
        case 1:
            bdrate = 500;
            bdrate2 = 500;
		    pTester = new AudiA1_2012Tester();
            break;
	    case 2:
            bdrate = 100;
            bdrate2 = 100;
		    pTester = new AudiA6_2011bTester();
		    break;
	    case 3:
		    bdrate = 500;
            bdrate2 = 500;
		    pTester = new AudiA6_2012Tester();
		    break;
	    case 4 :
		    bdrate = 500;
            bdrate2 = 500;
		    pTester = new AudiQ3_2012_importedTester();
		    break;
	    case 5:
		    bdrate = 500;
            bdrate2 = 500;
		    pTester = new AudiQ3_2013_domesticTester(500);
		    break;
	    case 6:
    		bdrate = 100;
            bdrate2 = 100;
    		pTester = new AudiQ3_2013_domesticTester(100);
    		break;        
	    case 7:
    		bdrate = 100;
            bdrate2 = 100;
    		pTester = new AudiA4Tester();
    		break;
	    case 8:
    		bdrate = 100;
            bdrate2 = 100;
    		pTester = new AudiQ7_2012Tester();
    		break;
        case 9:
    		bdrate = 500;
            bdrate2 = 500;
    		pTester = new AudiQ7_2014Tester();
    		break;        
	    case 10:
    		bdrate = 500;
            bdrate2 = 500;
    		pTester = new AudiA3_2014Tester();
    		break; 
        case 11:
            bdrate = 500;
            bdrate2 = 500;
    		pTester = new BenZ50_500Tester();
            break;
        case 12:
            bdrate = 500;
            bdrate2 = 500;
            pTester = new TouaregTester();
            break;
	case 13:
            bdrate = 125;
            bdrate2 = 125;
            pTester = new Volvo_V40Tester();
	     break;
        case 14:
            bdrate = 100;
            bdrate2 = 100;
            pTester = new MagotanTester();
            break;
        case 15:
            bdrate = 500;
            bdrate2 = 500;
            pTester = new Golf7Tester();
            break;
        case 16:
            bdrate = 500;
            bdrate2 = 500;
            pTester = new BMWTester();
            break;
        case 17:
            bdrate = 500;
            bdrate2 = 500;
		    pTester = new ToyotaVellfireTester();
            break;
        case 18:
            bdrate = 125;
            bdrate2 = 125;
		    pTester = new HondaCity_2014Tester();
            break;
        case 19:
            bdrate = 125;
            bdrate2 = 125;
            pTester = new LandRoverTester();
            break;
        case 21:
            bdrate = 125;
            bdrate2 = 125;
            pTester = new VolvoTester();
            break;
	case 22:			
            bdrate  = 500;
            bdrate2 = 500;
            pTester = new NissanX_TrailTester();
            break;
	 case 23:
            bdrate  = 500;
            bdrate2 = 500;
        	pTester = new AudiQ7_2016Tester();
            break;
	 case 24:
            bdrate  = 125;
            bdrate2 = 125;
        	pTester = new PEUGEOTTester();
		break;
	case 25:
		bdrate = 125;
            bdrate2 = 125;
    		pTester = new BenZTester();
    		break;
	case 26:
		bdrate = 500;
            bdrate2 = 500;
    		pTester = new Cadillac_XT5Tester();
    		break;
	case 27:
		bdrate = 125;
            bdrate2 = 125;
    		pTester = new LandRover_JaguarTester();
    		break;
	case 28:
		bdrate = 500;
            bdrate2 = 500;
    		pTester = new LEXUSTester();
    		break;
	case 29:
            bdrate = 125;
            bdrate2 = 125;
    		pTester = new LR_B_SportTester();
		break;
	//case 31:
		//bdrate = 500;
            //bdrate2 = 500;
		//pTester = new BenZ50_OBDTester();
    		//break;
	case 31:
		bdrate = 500;
             bdrate2 = 500;
		pTester = new PorscheTester();
  		break;
	case 32:
            bdrate = 125;
            bdrate2 = 125;
            pTester = new Landrover_DBTester();
		break;
	case 33:
            bdrate = 125;
            bdrate2 = 125;
            pTester = new Landrover_DHTester();
		break;
	case 34:
            bdrate = 500;
            bdrate2 = 500;
            pTester = new RangeRoverTester();
		break;
	case 35:
            bdrate = 500;
            bdrate2 = 500;
            pTester = new BMW3_SeriesTester();
		break;
	case 36:
			bdrate = 500;
			bdrate2 = 500;
			pTester = new FordTester();
			break;
	case 37:
		bdrate = 500;
		bdrate2 = 500;
		pTester = new HondaVezelTester();
		break;
	case 38:
		bdrate = 250;
		bdrate2 = 250;
		pTester = new LandCruiserTester();
		break;
	case 39:
		bdrate = 500;
		bdrate2 = 500;
		pTester = new SUZUKTester();
		break;
	case 41:
		bdrate = 500;
		bdrate2 = 500;
		pTester = new vwTuangTester();
		break;
	default:
		bdrate = 125;
        bdrate2 = 125;
		pTester = new BenZTester();
        break;
	} 
    
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
    static GetKey getkey(GRTKEY_TASK_PRIO);
    getkey.start(0); 
    
	gpDrvCAN->setOnMsgSendHandler((DrvCAN::Send_T)_msgREDHeadler,NULL);
	OSTmrStart(_pTmr,&err_Tmr);

//默认车型

#if defined(AudiA12012)
            bdrate = 500;
            bdrate2 = 500;
		    pTester = new AudiA1_2012Tester();
#elif defined(AudiA62011)	
            bdrate = 100;
            bdrate2 = 100;
		    pTester = new AudiA6_2011bTester();
#elif defined(AudiA62012)	
		    bdrate = 500;
            bdrate2 = 500;
		    pTester = new AudiA6_2012Tester();
#elif defined(AudiQ32012)	
		    bdrate = 500;
            bdrate2 = 500;
		    pTester = new AudiQ3_2012_importedTester();
#elif defined(AudiQ32013_500)	
		    bdrate = 500;
            bdrate2 = 500;
		    pTester = new AudiQ3_2013_domesticTester(500);
#elif defined(AudiQ32013_100)	
    		bdrate = 100;
            bdrate2 = 100;
    		pTester = new AudiQ3_2013_domesticTester(100);
#elif defined(Audi_A4)	
		bdrate = 100;
        bdrate2 = 100;
		pTester = new AudiA4Tester();
#elif defined(AudiQ72012)	
		bdrate = 100;
        bdrate2 = 100;
		pTester = new AudiQ7_2012Tester();
#elif defined(AudiQ72014)	
		bdrate = 500;
        bdrate2 = 500;
		pTester = new AudiQ7_2014Tester();
#elif defined(AudiA3_2014)	
		bdrate = 500;
        bdrate2 = 500;
		pTester = new AudiA3_2014Tester();
#elif defined(BenZ50)	
        bdrate = 500;
        bdrate2 = 500;
		pTester = new BenZ50_500Tester();
#elif defined(touareg)	
        bdrate = 500;
        bdrate2 = 500;
        pTester = new TouaregTester();
#elif defined(Volvo_V40)	
        bdrate = 125;
        bdrate2 = 125;
        pTester = new Volvo_V40Tester();
#elif defined(magotan)	
        bdrate = 100;
        bdrate2 = 100;
        pTester = new MagotanTester();
#elif defined(golf7)	
        bdrate = 500;
        bdrate2 = 500;
        pTester = new Golf7Tester();
#elif defined(bmw)	
        bdrate = 500;
        bdrate2 = 500;
        pTester = new BMWTester();
#elif defined(Toyota)
        bdrate = 500;
        bdrate2 = 500;
     pTester = new ToyotaVellfireTester();
#elif defined(HondaCity)
        bdrate = 125;
        bdrate2 = 125;
     pTester = new HondaCity_2014Tester();
#elif defined(LandRover)
        bdrate = 125;
        bdrate2 = 125;
        pTester = new LandRoverTester();
#elif defined(Volvo)
        bdrate = 125;
        bdrate2 = 125;
        pTester = new VolvoTester();
#elif defined(Nissan)		
        bdrate  = 500;
        bdrate2 = 500;
        pTester = new NissanX_TrailTester();
#elif defined(AudiQ72016)	
        bdrate  = 500;
        bdrate2 = 500;
        pTester = new AudiQ7_2016Tester();
#elif defined(peugeot)
        bdrate  = 125;
        bdrate2 = 125;
        pTester = new PEUGEOTTester();
#elif defined(BenZ45)
	bdrate = 125;
         bdrate2 = 125;
		pTester = new BenZTester();
#elif defined(CadillacXT5)
	bdrate = 500;
         bdrate2 = 500;
		pTester = new Cadillac_XT5Tester();
#elif defined(Jaguar)
	bdrate = 125;
         bdrate2 = 125;
		pTester = new LandRover_JaguarTester();
#elif defined(lexus)
	bdrate = 500;
         bdrate2 = 500;
		pTester = new LEXUSTester();
#elif defined(LR_SB)
	bdrate = 125;
         bdrate2 = 125;
		pTester = new LR_B_SportTester();
#elif defined(Porschejie)
	bdrate = 500;
     bdrate2 = 500;
	pTester = new PorscheTester();
#elif defined(LR_DB)
	bdrate = 125;
     bdrate2 = 125;
     pTester = new Landrover_DBTester();
#elif defined(LR_DH)
	bdrate = 125;
     bdrate2 = 125;
     pTester = new Landrover_DHTester();
#elif defined(RangeRover)
	bdrate = 500;
     bdrate2 = 500;
     pTester = new RangeRoverTester();
#elif defined(BMW3)
	bdrate = 500;
     bdrate2 = 500;
     pTester = new BMW3_SeriesTester();
#elif defined(FORD)
	bdrate = 500;
	bdrate2 = 500;
	pTester = new FordTester();
#elif defined(HondaVezel)
	bdrate = 500;
	bdrate2 = 500;
	pTester = new HondaVezelTester();
#elif defined(LANDCRUISER)
	bdrate = 250;
	bdrate = 250;
	pTester = new LandCruiserTester();
#elif defined(suzuk)
	bdrate = 500;
	bdrate = 500;
	pTester = new SUZUKTester();
#elif defined(vw_Tuang)
	bdrate = 500;
	bdrate = 500;
	pTester = new vwTuangTester();
#else
#endif
 
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
          
	pTester->pAutoControl = new Tester::AutoControlTask(AUTOCONTROL_TASK_PRIO);
	//pTester->pAutoControl->start(gpCar); 
      
    SetAllCarLedStatus(false);
	
#if defined(CarNum1)
	showCarLED(3);
#elif defined(CarNum2)	
	showCarLED(4);
#elif defined(CarNum3)	
	showCarLED(5);
#elif defined(CarNum4)	
	showCarLED(6);
#elif defined(CarNum5)	
	showCarLED(7);
#elif defined(CarNum6)	
	showCarLED(8);
#elif defined(CarNum7)	
	showCarLED(9);
#elif defined(CarNum8)	
	showCarLED(10);
#elif defined(CarNum9)	
	showCarLED(11);
#elif defined(CarNum10)	
    SetAllCarLedStatus(true);
#elif defined(CarNum11)	
	showCarLED(3);
	CarNum= 11;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum12)	
	showCarLED(3);
	CarNum= 12;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum13)	
	showCarLED(3);
	CarNum= 13;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum14)	
	showCarLED(3);
	CarNum= 14;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum15)	
	showCarLED(3);
	CarNum= 15;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum16)	
	showCarLED(3);
	CarNum= 16;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum17)	
	showCarLED(3);
	CarNum= 17;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum18)	
	showCarLED(3);
	CarNum= 18;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum19)	
	showCarLED(3);
	CarNum= 19;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum21)	
	showCarLED(4);
	CarNum= 21;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum22)	
	showCarLED(4);
	CarNum= 22;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum23)	
	showCarLED(4);
	CarNum= 23;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum24)	
	showCarLED(4);
	CarNum= 24;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum25)	
	showCarLED(4);
	CarNum= 25;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum26)	
	showCarLED(4);
	CarNum= 26;
	OSTmrStart(_pTmr1,&err_Tmr1);	
#elif defined(CarNum27)	
	showCarLED(4);
	CarNum= 27;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum28)	
	showCarLED(4);
	CarNum= 28;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum29)
	showCarLED(4);
	CarNum= 29;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum31)
	showCarLED(5);
	CarNum= 31;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum32)
	showCarLED(5);
	CarNum= 32;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum33)
	showCarLED(5);
	CarNum= 33;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum34)
	showCarLED(5);
	CarNum= 34;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum35)
	showCarLED(5);
	CarNum= 35;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum36)
	showCarLED(5);
	CarNum= 36;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum37)
	showCarLED(5);
	CarNum= 37;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum38)
	showCarLED(5);
	CarNum= 38;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum39)
	showCarLED(5);
	CarNum= 39;
	OSTmrStart(_pTmr1,&err_Tmr1);
#elif defined(CarNum41)
	showCarLED(6);
	CarNum= 41;
	OSTmrStart(_pTmr1,&err_Tmr1);
#else
#endif
	
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
                    //pTester->shiftUp();
                    gpCar->shiftPos = CarInfo::SHIFT_POS_P;
                    break;
                case 12://R档
                    //pTester->shiftDown();
                    gpCar->shiftPos = CarInfo::SHIFT_POS_R;
                    break;
                case 13://P键
                    //pTester->togglePKey();
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
			//gpCar->shiftPos = CarInfo::SHIFT_POS_N;
                    break;
                case 15://NAVI phone_answer Remote_Trunk_Key
                    gpCar->TireKey = 2;
                    gpCar->isRTKPressed = true;
                    break;
                case 16://voice Front_Door_Trunk_Key
                    gpCar->TireKey = 3;
                    
                    gpCar->isFDTKPressed = true;
                    break;
                case 17://scroll_up turn_left Page_up
                    gpCar->TireKey = 5;

                    pTester->steerLeft();
//                    gpCar->isTrunLeft = true;	
//                    gpCar->isTrunRight= false;
//                    gpCar->isMiddle= false;

                    gpCar->PageTrunKey = 5;                    
                    break;
                case 18://scroll_down turn_right Page_down
                    gpCar->TireKey = 6;
                    
                    gpCar->PageTrunKey = 6;
//                    gpCar->isTrunLeft = false;    
//                    gpCar->isTrunRight= true;
//                    gpCar->isMiddle= false;

                    pTester->steerRight();
                    break;
                case 19://scroll_ok turn_middle
                    gpCar->TireKey = 7;
                    
//                    gpCar->isTrunLeft = false;	
//                    gpCar->isTrunRight= false;
//                    gpCar->isMiddle= true;
                    break;
                case 20://Turn_signal left(all) left(Audi/BenZ/BWM/Tourage) hangup(BenZNewC) page_left(Golf7/AudiA3) MovePrevious(volvo)                   
                    gpCar->lightPos &= 0xFC;
                    gpCar->lightPos |= 1;
					gpCar->lightCtl = 1;
                    gpCar->TireKey = 8;
                    break;
                case 21://Turn_signal right(all) right(Audi/BenZ/BWM/Tourage) voice_off(BenZNewC) page_right(Golf7/AudiA3) MoveNext(volvo)                 
                    //gpCar->lightPos &= 0xFC;
                    //gpCar->lightPos |= 2; 
                    gpCar->TireKey = 9;
                    
                    gpCar->lightPos &= 0xFC;
                    gpCar->lightPos |= 2;

                    gpCar->lightCtl = 2;
                    break;
                case 22://Menu(Audi/BWM/Tourage) HOME(BenZNewC) hangup(BenZ) answer_phone(Golf7) speed+
                    //gpCar->TireKey = 4;
                    
                    gpCar->speedCtl = 1;
		     // gpCar->lightPos &= 0xFC;
                    //gpCar->lightPos |= 2; 
                    //gpCar->lightCtl = 0;
                    gpCar->TireKey = 4;
                    
                    gpCar->lightPos &= 0xFC;
                    gpCar->lightPos |= 0;
					
					
                    break;
                case 23://signal_flash
                    gpCar->lightPos &= 0xFC;
                    gpCar->lightPos |= 3;
		    gpCar->speedCtl = 0;
		    gpCar->lightCtl = 2;
		    
                    break;
                case 24://静音 back_key
                    gpCar->TireKey = 12;
                    gpCar->speedCtl = 2;
                    gpCar->lightCtl = 3;
                    gpCar->MMIKey = 10;
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

//#ifdef TEST_TURN_SIG
//static void _handleReversig(CarInfo *gpCar)
//{
//	static bool reverseValueStored = false;
//	bool reverseValueCurrent = gpDrvReverseSig->getValue();
//
//	if(reverseValueCurrent != reverseValueStored) {
//		reverseValueStored = reverseValueCurrent;
//		if( reverseValueCurrent) {
//			gpCar->shiftPos = Car::SHIFT_POS_R;
//		} else {
//			gpCar->shiftPos = Car::SHIFT_POS_D;
//		}
//	}
//}
//
//static void _handlePKeyPressedsig(CarInfo *gpCar)
//{
//	static bool PKeyValueStored = false;
//	bool PKeyValueCurrent = gpDrvPKeyPessedSig->getValue();
//
//	if(PKeyValueCurrent != PKeyValueStored) {
//		PKeyValueStored = PKeyValueCurrent;
//		if( PKeyValueCurrent) {
//			gpCar->isPKeyPressed= false;
//		  } else {
//			gpCar->isPKeyPressed= true;
//		}
//	}
//}

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

#if 1
    while(1) { //无操作的主任务  

         OSTimeDlyHMSM(0,0,1,0);
	   
    }
 #endif
 
	return 0;
}
