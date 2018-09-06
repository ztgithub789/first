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


#include "Tester.h"

Tester::Tester() {
	// TODO Auto-generated constructor stub
}

Tester::~Tester() {
	// TODO Auto-generated destructor stub
}

Tester::AutoControlTask::AutoControlTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
}

void Tester::AutoControlTask::_doTask(void *p_arg)
{
	uint16_t time = 0;	
    bool auto_left = true;
    bool radar_refresh = true;

	CarInfo *pCar = (CarInfo*)p_arg;
    
	pCar->steerAngle=-pCar->maxAbsSteerAngle;
	uint16_t delay =(uint16_t) 1000 * 5 / pCar->maxAbsSteerAngle;
	uint8_t RADAR_FF=60;
    
	while(1) {
		OSTimeDlyHMSM(0,0,0,delay*2); //delay
		time++;
		/* shift */
		if( pCar->shiftPos ==CarInfo::SHIFT_POS_R ){
//          if(  pCar->isPKeyPressed== true ){
//          if( !radar_refresh ) {
		    /* steering angle */
			if(pCar->steerAngle==pCar->maxAbsSteerAngle) {
				auto_left = true;
			}else if(pCar->steerAngle==-pCar->maxAbsSteerAngle) {
				auto_left = false;
			}            
			if(auto_left ==true) {
				pCar->steerAngle -= 0x5;
			} else {
				pCar->steerAngle += 0x5;
			}
            
			RADAR_FF++;
			if(RADAR_FF <60) {
				goto SHIFT;
			}
			for(uint8_t nr=1;nr<CarInfo::RADAR_NR;nr+=4) {
				pCar->radarVal[nr] = pCar->radarVal[nr] + 1;
				pCar->radarVal[nr+1] = pCar->radarVal[nr+1] + 1;		
				if(pCar->radarVal[nr] >=0xa0) {
					pCar->radarVal[nr]=0xff; 
				}
				if(pCar->radarVal[nr+1] >=0xa0) {
					pCar->radarVal[nr+1]=0xff;	
				}
			}
			if(pCar->radarVal[6] ==0xff) {
				RADAR_FF=0;
			}
SHIFT:               
			if(time*5/pCar->maxAbsSteerAngle ==2) {
				pCar->shiftPos = CarInfo::SHIFT_POS_D;
//              pCar->isPKeyPressed = false;
				radar_refresh = true;
				time=0;
			}
		} else {
			if( time*5/pCar->maxAbsSteerAngle ==1) {
				for(uint8_t nr=0;nr<4;nr+=3) {
					pCar->radarVal[nr] = pCar->radarVal[nr]+5;
					pCar->radarVal[nr+4] = pCar->radarVal[nr+4]+5;
					if(pCar->radarVal[nr] >=0x60) pCar->radarVal[nr]=0xff;
					if(pCar->radarVal[nr+4] >=0x60) pCar->radarVal[nr+4]=0xff;
				}		
				pCar->shiftPos = CarInfo::SHIFT_POS_R;
//              pCar->isPKeyPressed = true;
				radar_refresh = false;
				time=0;
			}
		}
        
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}
