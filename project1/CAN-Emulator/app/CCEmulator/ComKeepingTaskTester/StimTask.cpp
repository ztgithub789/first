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

#include "StimTask.h"
#include "includes.h"
StimTask::StimTask(uint8_t prio) :  ZTask(prio) {
}

static void send(const uint8_t *pData,uint8_t data_len,uint16_t CanId)
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

	for (int i = 0; i<data_len; i++)
		msg.Data[i] = pData[i];

	msg.StdId = CanId;
	gpDrvCAN->transmit(CAN1, &msg);
}

void StimTask::_doTask(void *p_arg)
{
	static int i = 0;
	while(1) {
		i++;
		OSTimeDlyHMSM(0,0,0,100);

		// 周期100ms
		{
			static const uint8_t data[]= {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
			send(data, 8, 0x497);
		}

		// 周期200ms
		if ( i % 2 == 0) {
			static const uint8_t data[]= {0x08, 0x01, 0x09, 0x00, 0x00, 0x00, 0x00};
			send(data, 7, 0x6ed);
		}

//		// 周期20s
//		if (i%200 == 0) {
//			static const uint8_t data[]= {0x12, 0xc2};
//			send(data, 2, 0x607);
//		}
	}
}
