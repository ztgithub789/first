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


#include "../Tester.h"

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

	CarInfo *pCar = (CarInfo*)p_arg;
    
	pCar->steerAngle=-pCar->maxAbsSteerAngle;
	uint16_t delay =(uint16_t) 1000 * 5 / pCar->maxAbsSteerAngle;
    
	while(1) {
		OSTimeDlyHMSM(0,0,0,delay*2); //delay
		time++;

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
            
		if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) { 
			OSTaskDel(OS_PRIO_SELF);
			break;
		}
	}
}
