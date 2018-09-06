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

#include <utils/Time/Time.h>
#include "ComKeepingTask.h"

ComKeepingTask::ComKeepingTask(uint8_t prio) : ZTask(prio){
	// TODO Auto-generated constructor stub
		isPKeyPressed = false;
		status497 = STATUS_00;
}

void ComKeepingTask::sendData(uint32_t id, uint8_t DLC, const char *pData)
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

void ComKeepingTask::dumpMsg(CanTMsg *pMsg)
{
	zprintf("%x ", pMsg->msg.StdId);
	for (int i = 0; i<pMsg->msg.DLC; i++) {
		zprintf("%02x ", pMsg->msg.Data[i]);
	}
	zprintf("\n");
}

void ComKeepingTask::sendWakeupSeq(void)
{
    //	2 -> 12154888:             6fd  01 01 04 00 04 00 00 00  --  .  .  .  .  .  .  .  .
	//	2 -> 12154888:             628  02 c2 03 01 0b 00 05 00  --  .  .  .  .  .  .  .  .
	//	2 -> 12154896:             6af  00 00 00 00 00           --  .  .  .  .  .
		OSTimeDlyHMSM(0,0,0,5);
		const char d1[] = {0x01, 0x01, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00};
		sendData(0x6fd, 8, d1);

		OSTimeDlyHMSM(0,0,0,5);
		const char d2[] = {0x02, 0xc2, 0x03, 0x01, 0x0b, 0x00, 0x05, 0x00};
		sendData(0x628, 8, d2);

		OSTimeDlyHMSM(0,0,0,5);
		const char d3[] = {0x00, 0x00, 0x00, 0x00};
		sendData(0x6af, 4, d3);
}

void ComKeepingTask::sendStartSeq(void)
{
	//	{{{
	    //	2 -> 18660104:    d1   628  80 25 42 c1 03 01 0b 00  --  .  %  B  .  .  .  .  . d1
		//	2 -> 18660154:    d2   628  c0 05 00 78 07 c3 e0 00  --  .  .  .  x  .  .  .  . d2
		//	2 -> 18660204:    d3   628  c1 00 00 00 0a 10 00 01  --  .  .  .  .  .  .  .  . d3
		//	2 -> 18660254:    d4   628  c2 03 00 00 00 00 96 01  --  .  .  .  .  .  .  .  . d4
		//	2 -> 18660284:    d5   6fd  08 01 01 00 04 00 00 00  --  .  .  .  .  .  .  .  . d5
		//	2 -> 18660304:    d6   628  c3 01 08 32 01 32 01 32  --  .  .  .  2  .  2  .  2 d6
		//	2 -> 18660354:    d7   628  c4 01 f0 00 00 00 08 01  --  .  .  .  .  .  .  .  . d7
		//	2 -> 18660354:    d8   628  42 cf 00                 --  B  .  .                d8
		//	2 -> 18660354:    d9   628  42 d6 08                 --  B  .  . (有时没有)     d9
		//	}}}

		const static char d1[] = {0x80, 0x25, 0x42, 0xc1, 0x03, 0x01, 0x0b, 0x00};
		const static char d2[] = {0xc0, 0x05, 0x00, 0x78, 0x07, 0xc3, 0xe0, 0x00};
		const static char d3[] = {0xc1, 0x00, 0x00, 0x00, 0x0a, 0x10, 0x00, 0x01};
		const static char d4[] = {0xc2, 0x03, 0x00, 0x00, 0x00, 0x00, 0x96, 0x01};
		const static char d5[] = {0x08, 0x01, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00};
		const static char d6[] = {0xc3, 0x01, 0x08, 0x32, 0x01, 0x32, 0x01, 0x32};
		const static char d7[] = {0xc4, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x08, 0x01};
		const static char d8[] = {0x42, 0xcf, 0x00};
//		const static char d9[] = {0x42, 0xd6, 0x08};
		// send starting sequence
		OSTimeDlyHMSM(0,0,0,50);
		sendData(0x628, 8, d1);

		OSTimeDlyHMSM(0,0,0,50);
		sendData(0x628, 8, d2);

		OSTimeDlyHMSM(0,0,0,50);
		sendData(0x628, 8, d3);

		OSTimeDlyHMSM(0,0,0,50);
		sendData(0x628, 8, d4);

		OSTimeDlyHMSM(0,0,0,30);
		sendData(0x6fd, 8, d5);

		OSTimeDlyHMSM(0,0,0,20);
		sendData(0x628, 8, d6);

		OSTimeDlyHMSM(0,0,0,5);
		sendData(0x628, 8, d7);

		OSTimeDlyHMSM(0,0,0,5);
		sendData(0x628, 3, d8);

//		OSTimeDlyHMSM(0,0,0,5);
//		sendData(0x628, 3, d9);

}

void ComKeepingTask::handle497(CanTMsg *pMsg)
{
	static uint8_t statusOld = STATUS_00;

	if ((pMsg->msg.Data[6] & 0x02) != 0x0) {
		status497 = STATUS_F1;
	} else
		status497 = STATUS_00;

	if (statusOld != status497 && status497 == STATUS_F1) {
		OSTimeDlyHMSM(0,0,0,5);
		const static char d[] = {0x42, 0xda, 0xf1, 0x01, 0x01, 0x01};
		sendData(0x628, 6, d);
	}

	statusOld = status497;
}




void ComKeepingTask::handle607(CanTMsg *pMsg)
{
	static uint32_t lastTime = 0;

//	if (firstByteHiOld != firstByteHiNew) {
//		if (firstByteHiNew == 0x10) {
//			sendStartSeq();
//		}
//	}
//
//	firstByteHiOld = firstByteHiNew;
	// 处理 607 12 c2，但10s之内只处理一次
	if (pMsg->msg.Data[0] == 0x12 &&
			pMsg->msg.Data[1] == 0xc2) {
		uint32_t now = Time::getTimems();
		if (now - lastTime > 10 * 1000) {
			sendWakeupSeq();
			OSTimeDlyHMSM(0,0,0,20);
			sendStartSeq();
			lastTime = Time::getTimems();
		}
	}

	if ((pMsg->msg.Data[0] & 0xf0) == 0x20) {
		//  response
		OSTimeDlyHMSM(0,0,0,5);
		uint8_t DLC = pMsg->msg.DLC;
		char d628[8];
		for (int i = 0; i < DLC; i++) {
			d628[i] = pMsg->msg.Data[i];
		}

		d628[0] = d628[0] & 0x0f | 0x40;
		sendData(0x628, DLC, d628);
		zprintf("607 handled\n");
	}
}

void ComKeepingTask::handle67f(void)
{
//	 - 如果处于唤醒且开的状态，收到67f帧，延时10ms，发送两帧：
//	{{{
//	00023447   628: xx....
//	00023457   6af: 00 00 32 32 32
//	}}}
//
//	其中628帧按以下规律变化：
//	{{{
//	628: 32 c2 03 01 0b 00 05 00
//	628: 32 c3 78 07 c3 e0 00 00
//	628: 32 c4 0a
//	628: 32 cd 10
//	628: 32 ce 00 01 03 00 00 00
//	628: 32 cf 00
//	628: 32 d0 96
//	628: 32 d1 01 01
//	628: 32 d6 08
//	628: 32 d7 32 01
//	628: 32 d8 32 01
//	628: 32 d9 32 01
//	628: 32 da f0 00 00 00
	static int i = 0;

	const static char d6af_00[] = {0x00, 0x00, 0x32, 0x32, 0x32};
	const static char d6af_f1[] = {0xf1, 0x00, 0x32, 0x32, 0x32};
	const static char d628[][8] = {
			{0x32, 0xc2, 0x03, 0x01, 0x0b, 0x00, 0x05, 0x00},	// 0
			{0x32, 0xc3, 0x78, 0x07, 0xc3, 0xe0, 0x00, 0x00},
			{0x32, 0xc4, 0x0a},
			{0x32, 0xcd, 0x10},
			{0x32, 0xce, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00},
			{0x32, 0xcf, 0x00},
			{0x32, 0xd0, 0x96},
			{0x32, 0xd1, 0x01, 0x01},
			{0x32, 0xd6, 0x08},
			{0x32, 0xd7, 0x32, 0x01},
			{0x32, 0xd8, 0x32, 0x01},
			{0x32, 0xd9, 0x32, 0x01},	// 11
	};

	uint8_t d628_DLC[] = {
			8,// 0
			8,
			3,
			3,
			8,
			3,
			3,
			4,
			3,
			4,
			4,
			4,	// 11
	};

	const static char d628_12_P_false[] = {0x32, 0xda, 0xf0, 0x00, 0x00, 0x00};
	const static char d628_12_P_true[] = {0x32, 0xda, 0x11, 0x01, 0x01, 0x01};
	if (i != 12)
		sendData(0x628, d628_DLC[i], d628[i]);
	else {
		if (isPKeyPressed)
			sendData(0x628, 6, d628_12_P_true);
		else
			sendData(0x628, 6, d628_12_P_false);
	}

	if (status497 == STATUS_F1)
		sendData(0x6af, 5, d6af_f1);
	else
		sendData(0x6af, 5, d6af_00);

	i ++;
	if (i == 13)
		i = 0;
}

void ComKeepingTask::handle6ed(void)
{
//	6fd: 08 01 01 00 04 00 00 00
	const static char d[] = {0x08, 0x01, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00};
	sendData(0x6fd, 8, d);
}

void ComKeepingTask::handlePKey(CanTMsg *pMsg)
{
	if ((pMsg->msg.Data[2] & 0x01) == 0) {
		isPKeyPressed = false;
		zprintf("Pkey to false\n");
	} else {
		isPKeyPressed = true;
		zprintf("Pkey to true\n");
	}


//	const static char d[] = {0x42, 0xda, 0xf1, 0x01, 0x01, 0x01};
//	sendData(0x628, 6, d);
//
//	OSTimeDlyHMSM(0,0,0,50);

	static char d1[] = {0x42, 0xd6, 0x38};
	if (isPKeyPressed) {
		d1[2] = 0x38;
		sendData(0x628, 3, d1);
	} else {
		d1[2] = 0x08;
		sendData(0x628, 3, d1);
	}

}

//void ComKeepingTask::tmrCallback(void *ptmr, void *parg)
//{
//	ComKeepingTask *_this = (ComKeepingTask*)ptmr;
//	_this->canResetBeHandled = true;
//	printf("timer ddd\n");
//	uint8_t err;
//	OSTmrStop(_this->_607HandlerTmr, OS_TMR_OPT_NONE, 0, &err);
//	OSTmrStart(_this->_607HandlerTmr, &err);
//	if (err != OS_ERR_NONE)
//		printf("Timer start failure.\n");
//}

void ComKeepingTask::_doTask(void *p_arg)
{
	_pSlot = CanHub::getInstance()->getSlot();
	uint8_t ret;
	//canResetBeHandled = true;

	if (_pSlot == 0) {
		zprintf("AudiA6_2012 getSlot failure.\n");
		return ;
	}

	ret =  _pSlot->addId(0x607);
	ret |= _pSlot->addId(0x67f);
	ret |= _pSlot->addId(0x6ed);
	ret |= _pSlot->addId(0x497);
	ret |= _pSlot->addId(0x603);

	if (ret != 0) {
		zprintf("AudiA6_2012 ID failure.\n");
		return ;
	}

//	uint8_t err;
//	_607HandlerTmr = OSTmrCreate( 10 ,
//			0,
//			OS_TMR_OPT_ONE_SHOT,
//			tmrCallback,
//			(void *)this, 0, &err);
//	if (err != OS_ERR_NONE) {
//		printf("Timer create failure.\n");
//	}
//	OSTmrStart(_607HandlerTmr, &err);
//	if (err != OS_ERR_NONE)
//		printf("Timer start failure.\n");
//	printf("Timer started\n");

	// 发送唤醒序列
	sendWakeupSeq();
	OSTimeDlyHMSM(0,0,0,20);
	sendStartSeq();

	while(1) {
		CanTMsg msg;

		while(1) {
			uint8_t err = _pSlot->read(&msg);
			if (err == OS_ERR_NONE) {
				switch(msg.msg.StdId) {
				case 0x497:
					handle497(&msg);
					break;
				case 0x607:
					// 发送开始序列
					handle607(&msg);
					break;
				case 0x67f:
					handle67f();
				//	printf(".");
					break;
				case 0x6ed:
					handle6ed();
					//printf("*");
					break;
				case 0x603:
					handlePKey(&msg);
					//printf("P\n");
					break;
				default:
					break;
				}
			}
		}
	}
}
