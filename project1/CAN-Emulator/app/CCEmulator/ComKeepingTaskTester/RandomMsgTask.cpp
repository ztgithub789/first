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
#include "RandomMsgTask.h"

RandomMsgTask::RandomMsgTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub

}

RandomMsgTask::~RandomMsgTask() {
	// TODO Auto-generated destructor stub
}


void RandomMsgTask::_doTask(void *p_arg)
{
	CanTxMsg msg =
	{
			.StdId = 0x123,
			.ExtId = 0,
			.IDE = 0,
			.RTR = 0,
			.DLC = 8,
			.Data = {1,2,3}
	};

	uint32_t i = 0xaaaaaaaa;
	while(1) {
		i =  i + 0x20;
		msg.StdId = i & 0x7ff;

		//		if (msg.StdId == 0x3c3 || msg.StdId == 0x525 || msg.StdId== 0x66a || msg.StdId==0x638 ||  msg.StdId==0x54b)
		//			continue;

		if(msg.StdId == 0x359 ||
				msg.StdId == 0x3c3 ||
				msg.StdId == 0x54b ||

				msg.StdId == 0x470 ||

				msg.StdId == 0x2c1 ||

				msg.StdId == 0x3e1 ||
				msg.StdId == 0x381 ||
				msg.StdId == 0x3b5 ||
				msg.StdId == 0x4b9 ||
				msg.StdId == 0x4bd ||
				msg.StdId == 0x2c3 ||
				msg.StdId == 0x291 ||
				msg.StdId == 0x181 ||
				msg.StdId == 0x391  )
			continue;
		msg.DLC = i % 9;
		for (int j = 0; j < msg.DLC; j++) {
			msg.Data[j] = (i >> ( 8* (j%4))) & 0xff;
		}
		OSTimeDlyHMSM(0,0,0,20);

		gpDrvCAN->transmit(CAN1, &msg);
	}
}
