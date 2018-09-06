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

#ifndef TESTER_H_
#define TESTER_H_

#include <ZTask.h>
#include <stdint.h>
#include <Car/CarInfo.h>

class Tester {
public:
	Tester();
	virtual ~Tester();
	virtual uint8_t run(CarInfo *pcar)=0;
	virtual void shiftDown(void);
	virtual	void shiftUp(void);
	virtual	void steerLeft(void);
	virtual	void steerRight(void);
	virtual	void togglePKey(void);
	virtual	void increaseRadarVal(int nr, int dv);
	class AutoControlTask : public ZTask {
	public:
		AutoControlTask(uint8_t prio);
		void _doTask(void *p_arg);
	};
	AutoControlTask *pAutoControl;
};

#endif /* TESTER_H_ */
