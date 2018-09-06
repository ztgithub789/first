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
#include "AudiQ7_2016Tester.h"
#include "AudiQ7_2016_common.h"

using namespace AudiQ7_2016;

AudiQ7_2016Tester::AudiQ7_2016Tester() {
	// TODO Auto-generated constructor stub
	pCar = NULL;
}

AudiQ7_2016Tester::~AudiQ7_2016Tester() {
	// TODO Auto-generated destructor stub
	delete pshiftsimTask;
	pshiftsimTask = NULL;

	delete psteeringTask;
	psteeringTask = NULL;

	delete pKeyTask;
	pKeyTask = NULL;

	delete pradarSimTask;
	pradarSimTask = NULL;

	delete pturnSimTask;
	pturnSimTask = NULL;


 	//delete ptirekeySimTask;
	//ptirekeySimTask = NULL;
	

}

void AudiQ7_2016Tester::shiftDown()
{
	if (pCar->shiftPos < CarInfo::SHIFT_POS_MAX - 1)
		pCar->shiftPos ++;
}

void AudiQ7_2016Tester::shiftUp()
{
	if (pCar->shiftPos > 0)
		pCar->shiftPos --;
}

void AudiQ7_2016Tester::steerLeft()
{
	if (pCar->steerAngle > -MAX_ABS_STEER_ANGLE)
		pCar->steerAngle -= 0x5;
}

void AudiQ7_2016Tester::steerRight()
{
	if (pCar->steerAngle < MAX_ABS_STEER_ANGLE)
		pCar->steerAngle += 0x5;
}

void AudiQ7_2016Tester::togglePKey()
{
	if (pCar->isPKeyPressed)
		pCar->isPKeyPressed = false;
	else
		pCar->isPKeyPressed = true;
}

/*******************************************************************************
 * 简介   :	增加指定雷达探头的数值，如果超过了最大值，自动折回到最小值
 * 参数   :	nr		探头编号。前面从左至右为0, 1, 2, 3，后面从左至右为4, 5, 6, 7
 * 			dv		增量
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void AudiQ7_2016Tester::increaseRadarVal(int nr, int dv)
{
	uint8_t maxRadarVal[CarInfo::RADAR_NR] = {0x5a, 0x74, 0x74, 0x5a, 0x3c, 0xa0, 0xa0, 0x3c};

	pCar->radarVal[nr] = pCar->radarVal[nr] + dv;
	// 保证在循环周期内有一次0xff
	if (pCar->radarVal[nr] >  maxRadarVal[nr])
		pCar->radarVal[nr] = 0xff;
}

uint8_t AudiQ7_2016Tester::run(CarInfo *pcar)
{
	pCar = pcar;
	pCar->maxAbsSteerAngle = MAX_ABS_STEER_ANGLE;

	pshiftsimTask = new ShiftSimTask(SHIFT_SIM_TASK_PRIO);
	pshiftsimTask->start(pCar);

	psteeringTask = new SteeringWheelSimTask(STEERING_WHEEL_SIM_TASK_PRIO);
	psteeringTask->start(pCar);

	pKeyTask = new PKeySimTask(P_KEY_SIM_TASK_PRIO);
	pKeyTask->start(pCar);

	pradarSimTask = new RadarSimTask(RADAR_SIM_TASK_PRIO);
	pradarSimTask->start(pCar);


	pturnSimTask = new TurnSimTask( TURN_SIM_TASK_PRIO);
	pturnSimTask->start(pCar);
        
    ptirekeySimTask = new TireKeySimTask(TIREKEYT_SIM_TASK_PRIO); //TIREKEY_SIM_TASK_PRIO
	ptirekeySimTask->start(pCar); 
	
	return 0;
}

AudiQ7_2016Tester::ShiftSimTask::ShiftSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}

void AudiQ7_2016Tester::ShiftSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x144,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x80, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00} //00 80 00 fe 80 00 48 00
	};
	//0x80默认为P档

	CarInfo *pCar = (CarInfo*)p_arg;
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);

		uint8_t shiftPos = pCar->shiftPos;

		switch(shiftPos) {
		case CarInfo::SHIFT_POS_P:
			msg.Data[1] = 0x80;
			break;
		case CarInfo::SHIFT_POS_R:
			msg.Data[1] = 0x70;
			break;
		case CarInfo::SHIFT_POS_N:
			msg.Data[1] = 0x60;
			break;
		case CarInfo::SHIFT_POS_D:
			msg.Data[1] = 0x50;
			break;
		case CarInfo::SHIFT_POS_S:
			msg.Data[1] = 0xc0;
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



//#if 0
AudiQ7_2016Tester::TireKeySimTask::TireKeySimTask(uint8_t prio) : ZTask(prio) {

}
void AudiQ7_2016Tester::TireKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x5bf,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 4,
	  .Data = {0x00, 0x00, 0x00, 0x0}
	};
    
	CarInfo *pCar = (CarInfo*)p_arg;
    static uint8_t times = 0, status = 0, oldkey = 0, newkey = 0;   
    
	while(1) {
        OSTimeDlyHMSM(0,0,0,100);

        newkey = pCar->TireKey;

        if(newkey != 0)
        {
            if((newkey != 5)&&(newkey != 6)&&(newkey != 11)&&(newkey != 12))
            {
                if(newkey != oldkey)
                {
                    oldkey = newkey;
                    times = 0;
                    status = 0x01;
                }else{
                    times++;
                    if(times >= 1)
                    {
                        status++;
                        if(status >=4)
                            status = 4;
                    }else{
                        status = 1;
                    }
                }
            }
        }else{
            times = 0;
            oldkey = 0;
            status = 0;
        }          

        switch(newkey)
        {
            case 0://没有按任何键
                msg.Data[0]=0;
                msg.Data[1]=0;
                msg.Data[2]=0;
                msg.Data[3]=0;
                break;
            case 1://*/MODE
                msg.Data[0]=0x21;
                msg.Data[1]=0;
                msg.Data[2]=status;
                msg.Data[3]=0; 
		   printf("***\n");
                break;
            case 2://导航
                msg.Data[3]= 0;
                msg.Data[1]=0;
                msg.Data[2]=status;
                msg.Data[0]=0x1b;
		  printf("NAVI\n");
                break;
            case 3://语音
                msg.Data[1]=0;
                msg.Data[0]=0x19;
                msg.Data[2]=status;
                msg.Data[3]=0; 
		  printf("voice\n");
                break; 				
  		case 8://左
                msg.Data[1]=0;
                msg.Data[0]=0x03;
                msg.Data[2]=status;
                msg.Data[3]=0;   
		   printf("left\n");
                break;
		case 9://右
                msg.Data[0]=0x02;
                msg.Data[1]=0;
                msg.Data[2]=status;
                msg.Data[3]=0; 	
		   printf("right\n");
		  break;
		case 5://上
                msg.Data[0]=0x06;
                msg.Data[1]=0;
                msg.Data[2]=0x01;
                msg.Data[3]=0;  
		   printf("up\n");
		   break;
		case 6://下
                msg.Data[0]=0x06;
                msg.Data[1]=0;
                msg.Data[2]=0x0f;
                msg.Data[3]=0; 
		   printf("down\n");
		   break;
		case 7://ok
                msg.Data[0]=0x07;
                msg.Data[1]=0;
                msg.Data[2]=status;
                msg.Data[3]=0;   
		  printf("ok\n");
		  break;
		case 12://no_voice
                msg.Data[0]=0x20;
                msg.Data[1]=0;
                msg.Data[2]=status;
                msg.Data[3]=0;
		  printf("no voice\n");
		  break;
		case 10://voice_up
                msg.Data[0]=0x12;
                msg.Data[1]=0;
                msg.Data[2]=0x01;
                msg.Data[3]=0; 
		   printf("voice up\n");
		   break;
		case 11://voice_down
                msg.Data[0]=0x12;
                msg.Data[1]=0;
                msg.Data[2]=0x0f;
                msg.Data[3]=0; 
                printf("voice down\n");
		  break;
				
		case 4://view
                msg.Data[0]=0x23;
                msg.Data[1]=0;
                msg.Data[2]=status;
                msg.Data[3]=0; 
		   printf("view\n");
		  break;
		//case 14://return
               // msg.Data[0]=0x08;
               // msg.Data[1]=0;
               // msg.Data[2]=status<<3;
               // msg.Data[3]=0; 



#if 0				
		case 15://phone_on
                msg.Data[0]=0x1c;
                msg.Data[1]=0;
                msg.Data[2]=status<<3;
                msg.Data[3]=0;
		case 16://上一曲
                msg.Data[0]=0x16;
                msg.Data[1]=0;
                msg.Data[2]=status<<3;
                msg.Data[3]=0;
		case 17://下一曲
                msg.Data[0]=0x15;
                msg.Data[1]=0;
                msg.Data[2]=status<<3;
                msg.Data[3]=0;
#endif
				                      
            default ://
                msg.Data[0]=0;
                msg.Data[1]=0;
                msg.Data[2]=0;
                msg.Data[3]=0;
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
//#endif


//转角
AudiQ7_2016Tester::SteeringWheelSimTask::SteeringWheelSimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

void AudiQ7_2016Tester::SteeringWheelSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x086,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x0, 0x00, 0x00, 0xaa, 0x00, 0x00} //00 00 00 00 11 60 00 9f
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
		int32_t angle = pCar->steerAngle;

		//int32_t value = (int32_t)(((msg.Data[3]&0x3f)<<8)|(msg.Data[2]));  msg.Data[3]&0x3f)>0x15
		#if 0
		if (angle<=0) {
			//angle = (value - 0x2000) * MAX_ABS_STEER_ANGLE / MAX_ABS_CAN_STEER_ANGLE;
			angle = angle * MAX_ABS_CAN_STEER_ANGLE/MAX_ABS_STEER_ANGLE + 0x0000;

		} else {
			//angle = -value * MAX_ABS_STEER_ANGLE / MAX_ABS_CAN_STEER_ANGLE;
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE/MAX_ABS_STEER_ANGLE + 0x0020;

		}
		#else
		if (angle <= 0) {
			angle = -angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + 0x4000;
		} else {
			angle = angle * MAX_ABS_CAN_STEER_ANGLE / MAX_ABS_STEER_ANGLE + 0x6000;
		}
		#endif

		msg.Data[3] = (angle & 0xff00) >> 8;
		msg.Data[2] = (angle & 0x00ff) >> 0;

		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}



AudiQ7_2016Tester::PKeySimTask::PKeySimTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

/*******************************************************************************
 * 简介   :	处理P键。注意平时没有P键帧，只有当按了P键后才会有P键帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void AudiQ7_2016Tester::PKeySimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = 0,
			.ExtId = P_KEY_STATUS_MSG_ID,
			.IDE = CAN_ID_EXT,
			.RTR = 0,
			.DLC = 4,
			.Data = {0x22, 0x95, 0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	while(1) {
		OSTimeDlyHMSM(0,0,0,50);

		static bool oldv = false;
		bool newv = pCar->isPKeyPressed;
		if (oldv != newv){
			if (newv) {
				msg.Data[2] = 0x11;
                msg.Data[3] = 0x01;
			} else {
				msg.Data[2] = 0x0;
                msg.Data[3] = 0x0;
			}
			gpDrvCAN->transmit(CAN1, &msg);
			oldv = newv;
		}
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}

AudiQ7_2016Tester::RadarSimTask::RadarSimTask(uint8_t prio) : ZTask(prio) {

}

/*******************************************************************************
 * 简介   :	模拟雷达帧
 * 参数   :
 * 返回值 :
 * 描述   :
 *******************************************************************************/
void AudiQ7_2016Tester::RadarSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = 0,
			.ExtId = RADAR_MSG_ID,
			.IDE = CAN_ID_EXT,
			.RTR = 0,
			.DLC = 8,
			.Data = {0x42, 0x93, 0x00, 0x00, 0x00, 0x00,0x00, 0x00}
	};

	CarInfo *pCar = (CarInfo*)p_arg;

	static bool dir = true;

	while(1) {
		OSTimeDlyHMSM(0,0,0,50);

		if(dir==true ){	
			// 前帧
			msg.Data[1] = 0x92;
			msg.Data[2] = pCar->radarVal[CarInfo::RADAR_LFS];
			msg.Data[3] = pCar->radarVal[CarInfo::RADAR_LFM];
			msg.Data[4] = pCar->radarVal[CarInfo::RADAR_RFM];
			msg.Data[5] = pCar->radarVal[CarInfo::RADAR_RFS];

		} else {		
			msg.Data[1] = 0x93;
			msg.Data[2] = pCar->radarVal[CarInfo::RADAR_LRS];
			msg.Data[3] = pCar->radarVal[CarInfo::RADAR_LRM];
			msg.Data[4] = pCar->radarVal[CarInfo::RADAR_RRM];
			msg.Data[5] = pCar->radarVal[CarInfo::RADAR_RRS];
			pCar->isRadaBack = false;
		}
		dir = !dir;

		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			/* Release any owned resources; */ 
			/* De-allocate any dynamic memory; */
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}



//转向灯
AudiQ7_2016Tester::TurnSimTask::TurnSimTask(uint8_t prio) : ZTask(prio) {

}
void AudiQ7_2016Tester::TurnSimTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
	  .StdId = 0x366,
	  .ExtId = 0,
	  .IDE = 0,
	  .RTR = 0,
	  .DLC = 8,
	  .Data = {0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0xf0}
	};
    
	CarInfo *pCar = (CarInfo*)p_arg;
    
	while(1) {
		OSTimeDlyHMSM(0,0,0,100);
        
		switch(pCar->lightPos){
            case 0://none
                msg.Data[2] = 0;
                msg.Data[3] = 0;
                break;
            case 1://left
                msg.Data[2] = 0x80;
                msg.Data[3] = 0x0a;
		   //printf("turn left\n");
                break;
            case 2://right
                msg.Data[3] = 0x15;
		   //printf("turn right\n");
                break;
             case 3://right
                msg.Data[2] = 0x10;
                msg.Data[3] = 0x1e;
		   //printf("turn flash\n");
                break;
            default:
                msg.Data[3] = 0;
                break;
        }
        
		gpDrvCAN->transmit(CAN1, &msg);
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}


AudiQ7_2016Tester::CarTimeTask::CarTimeTask(uint8_t prio):ZTask(prio){
// TODO Auto-generated constructor stub
}

void AudiQ7_2016Tester::CarTimeTask::_doTask(void * p_arg){
    CanTxMsg msg =
        {
          .StdId = 0x6b6,
          .ExtId = 0,
          .IDE = 0,
          .RTR = 0,
          .DLC = 6,
          .Data = {0x11, 0x10, 0x09, 0x0e, 0x22, 0x00}//0x11, 0x10, 0x09, 0x0e, 0x22, 0x00//2016年01月09日14时34分00秒
        };

    uint8_t year = 0;
    uint8_t mon = 1;
    uint8_t day = 1;
    uint8_t hour = 0;
    uint8_t min = 0;
    uint8_t sec = 0;
    uint8_t count = 0;
    
    while(1){
        OSTimeDlyHMSM(0,0,0,200);

        count++;
        if(5==count){
            count = 0;
            sec += 0x01;
            if(sec>0x3b){
                sec = 0;
                min += 0x01;
                if(min>0x3b){
                    min =0;
                    hour += 0x01;
                    if(hour>0x17){
                        hour = 0;
                        day += 0x01;
                        if(day>0x1e){
                            day = 0x01;
                            mon += 0x10;
                            if(mon>0xc){
                                mon = 0x01;
                                year += 0x01;
                                msg.Data[1] = year;
                            }else{
                                msg.Data[0] = mon;
                            }
                        }else{
                            msg.Data[2] = day;
                        }
                    }else{
                        msg.Data[3] = hour;
                    }
                }else{
                    msg.Data[4] = min;
                }
            }else{
                msg.Data[5] = sec;
            }
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



