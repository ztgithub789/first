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

#ifndef COMKEEPINGTASK_H_
#define COMKEEPINGTASK_H_
#include <ZTask.h>
#include <CanHub/CanHub.h>

class ComKeepingTask : public ZTask {
public:
	enum {
		STATUS_F1,
		STATUS_00
	};

	ComKeepingTask(uint8_t prio);
	void _doTask(void *p_arg);
	CanHub::Slot *_pSlot;
	void sendData(uint32_t id, uint8_t DLC, const char *pData);
	void dumpMsg(CanTMsg *pMsg);

	void sendStartSeq(void);
	void handle497(CanTMsg *pMsg);
	void handle607(CanTMsg *pMsg);
	void sendWakeupSeq(void);
	void handle67f(void);
	void handle6ed(void);
	void handlePKey(CanTMsg *pMsg);
	bool isPKeyPressed;
//	static void tmrCallback(void *ptmr, void *parg);
//	bool canResetBeHandled;
//	OS_TMR *_607HandlerTmr;
	uint8_t status497;
};

#endif /* COMKEEPINGTASK_H_ */
