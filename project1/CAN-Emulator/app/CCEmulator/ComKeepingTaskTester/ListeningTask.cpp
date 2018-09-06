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

#include "CanHub/CanHub.h"
#include "ListeningTask.h"

ListeningTask::ListeningTask(uint8_t prio) : ZTask(prio) {
	// TODO Auto-generated constructor stub
	cnt628 = cnt6af = cnt6fd = 0;
	_func = NULL;
}

void ListeningTask::_doTask(void *p_arg)
{
	CanHub::Slot *pSlot = CanHub::getInstance()->getSlot();
	if (pSlot == 0) {
		zprintf("ListeningTask: get CanHub slot failure.\n");
		return;
	}

	pSlot->addId(0x628);
	pSlot->addId(0x6af);
	pSlot->addId(0x6fd);

	CanTMsg rxMessage;

	while(1) {
		//OSTimeDlyHMSM(0,0,1,0);
		pSlot->read(&rxMessage, 0);
		switch (rxMessage.msg.StdId) {
		case 0x628:
			cnt628++;
			break;

		case 0x6af:
			cnt6af++;
			break;

		case 0x6fd:
			cnt6af++;
			break;
		default:
			break;
		}
		zprintf(".");
		if(_func) {
			_func(_param);
		}
	}
}

void ListeningTask::setMsgCallBack(func_t func,void *param)
{
	_func = func;
	_param = param;
}

