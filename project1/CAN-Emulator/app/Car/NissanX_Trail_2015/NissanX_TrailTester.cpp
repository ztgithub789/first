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
#include "NissanX_TrailTester.h"
#include "NissanX_Trail_common.h"

using namespace NISSANX_TRAIL_NP;

NissanX_TrailTester::NissanX_TrailTester(){
// TODO Auto-generated constructor stub
    pCar = NULL;
}

NissanX_TrailTester::~NissanX_TrailTester(){
    delete pShiftSimTask;
    pShiftSimTask = NULL;

    delete pTireKeySimTask;
    pTireKeySimTask = NULL;
}

uint8_t NissanX_TrailTester::run(CarInfo * pcar){
    pCar = pcar;
    pCar->maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;
    
    pShiftSimTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
    pShiftSimTask->start(pCar);

    pTireKeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO);
    pTireKeySimTask->start(pCar);

    return 0;
}

NissanX_TrailTester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio){
}

void NissanX_TrailTester::ShiftSimTask::_doTask(void * p_arg){
    CanTxMsg msg =
	{
	  .StdId = SHIFT_POS_MSD_ID,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 2,
	  .Data = {0x08, 0x00}
	};

    CarInfo *pCar = (CarInfo *)p_arg;
 
    while(1){
            OSTimeDlyHMSM(0,0,0,100);
    
            uint8_t shiftPos = pCar->shiftPos;
    
            switch(shiftPos) {
            case CarInfo::SHIFT_POS_P:
                msg.Data[0] = 0x08;
                break;
            case CarInfo::SHIFT_POS_R:
                msg.Data[0] = 0x10;
                break;
            case CarInfo::SHIFT_POS_N:
                msg.Data[0] = 0x18;
                break;
            case CarInfo::SHIFT_POS_D:
                msg.Data[0] = 0x20;
                break;
            default:
                break;
            }
    
            gpDrvCAN->transmit(CAN1, &msg);
            
            if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
                /* Release any owned resources; */ 
                /* De-allocate any dynamic memory; */
                OSTaskDel(OS_PRIO_SELF);
                break;
            }
        }
    
}

NissanX_TrailTester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio){
}

void NissanX_TrailTester::TireKeySimTask::_doTask(void * p_arg){
    CanTxMsg msg =
    {
        .StdId = STEERING_BUTTON_MSG_ID,
        .ExtId = 0,
        .IDE = 0,
        .RTR = 0,
        .DLC = 8,
        .Data = {0xf0, 0x0c, 0x0a, 0x01, 0xff, 0xff, 0xff, 0xff}//f0 0c 0a 01 ff ff ff ff
    };

    CarInfo *pCar = (CarInfo *)p_arg;
    static uint8_t pressTimeCnt = 0;
    static uint8_t transmitCnt = 0;
    static uint8_t preKey  = 0;

    while(1){
        OSTimeDlyHMSM(0,0,0,100);

        uint8_t newKey = pCar->TireKey;
        switch(newKey){ 
            case 0:
                break;
            case 1:
                break;
            case 2:
                msg.Data[4] = 0x81;
                preKey = 2;
                pressTimeCnt++;
                break;
//            case 3:
//                break;
            case 4:
                break;
            case 5:
                msg.Data[4] = 0x82;
                preKey = 5;
                pressTimeCnt++;
                break;
            case 6:
                msg.Data[4] = 0x83;
                preKey = 6;
                pressTimeCnt++;
                break;
            case 7:
                msg.Data[4] = 0x85;
                preKey = 7;
                pressTimeCnt++;
                break;
//            case 8:
//                break;
//            case 9:
//                break;
            case 10:
                msg.Data[4] = 0x87;
                preKey = 10;
                pressTimeCnt++;
                break;
            case 11:
                msg.Data[4] = 0x86;
                preKey = 11;
                pressTimeCnt++;
                break;
//            case 12:
//                break;
            default:
                msg.Data[4] = 0xff;
            break;
        }
        
        if(newKey != 0){                       
            transmitCnt++;
            if(transmitCnt > 5){
                msg.Data[4] = 0xff;
                transmitCnt = 0;
            }
            
            gpDrvCAN->transmit(CAN1, &msg);
        } else{
            if(pressTimeCnt == 1){
                gpDrvCAN->transmit(CAN1, &msg);
                
                switch(preKey){
                    case 2:
                        msg.Data[4] = 0x01;
                        break;
                    case 5:
                        msg.Data[4] = 0x02;
                        break;
                    case 6:
                        msg.Data[4] = 0x03;
                        break;
                    case 7:
                        msg.Data[4] = 0x05;
                        break;
                    case 10:
                        msg.Data[4] = 0x07;
                        break;
                    case 11:
                        msg.Data[4] = 0x06;
                        break;
                    default:
                        msg.Data[4] = 0xff;
                        break;
                }
                
                gpDrvCAN->transmit(CAN1, &msg);
                msg.Data[4] = 0xff;
                
                pressTimeCnt = 0;
                transmitCnt = 0;
            } else{
                pressTimeCnt = 0;
                transmitCnt = 0;
                
                msg.Data[4] = 0xff;
                gpDrvCAN->transmit(CAN1, &msg);                
            }                     
        }
        

        if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
        /* Release any owned resources; */ 
        /* De-allocate any dynamic memory; */
            OSTaskDel(OS_PRIO_SELF);
            break;
        }
    }
}
