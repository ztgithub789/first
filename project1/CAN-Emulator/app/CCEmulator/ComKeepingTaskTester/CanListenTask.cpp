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

#include "CanListenTask.h"
#include <utils/Time/Time.h>
#include <board.h>

ComKeepingTaskListeningTask::ComKeepingTaskListeningTask(uint8_t prio) : ZTask(prio){
	// TODO Auto-generated constructor stub

}

extern CanTxMsg gLastSentMsg;

void ComKeepingTaskListeningTask::_doTask(void *p_arg)
{
	CanTMsg rxMsg;
	uint8_t ret;
	while(1) {

		ret = gpDrvCAN->getData(&rxMsg, 0);
#ifdef ZDEBUG
		if (ret == OS_ERR_NONE) {
			zprintf("Received: %08d   %x: ", Time::getTimems(), rxMsg.msg.StdId);
			for (int i  = 0; i<rxMsg.msg.DLC; i++) {
				zprintf("%02x ", rxMsg.msg.Data[i]);
			}

			zprintf("\n");
		}
#endif
	}
}
