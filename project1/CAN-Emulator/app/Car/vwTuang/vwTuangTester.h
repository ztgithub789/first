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

#ifndef VWTUANG_TESTER_H_
#define VWTUANG_TESTER_H_

#include <ZTask.h>
#include <Car/CarInfo.h>
#include <Car/Tester.h>

class vwTuangTester : public Tester {
public:
	CarInfo *pCar;

	vwTuangTester();
	~vwTuangTester();
	uint8_t run(CarInfo *pcar);

	void shiftDown(void);
	void shiftUp(void);
	void steerLeft(void);
	void steerRight(void);
	void togglePKey(void);
	void increaseRadarVal(int nr, int dv);

	class ShiftSimTask : public ZTask {
	public:
		ShiftSimTask(uint8_t prio);
		void _doTask(void *p_arg);
	};

	class SteeringWheelSimTask : public ZTask {
	public:
		SteeringWheelSimTask(uint8_t prio);
		void _doTask(void *p_arg);
	};

	class PKeySimTask : public ZTask {
	public:
		PKeySimTask(uint8_t prio);
		void _doTask(void *p_arg);
	};

	class RadarSimTask : public ZTask {
	public:
		RadarSimTask(uint8_t prio);
		void _doTask(void *p_arg);
	};

	class TurnSimTask : public ZTask {
	public:
		TurnSimTask(uint8_t prio);
		void _doTask(void *p_arg);
	};

	class TireKeySimTask : public ZTask {
	public:
		TireKeySimTask(uint8_t prio);
		void testShowVersion(void);
		void _doTask(void *p_arg); //TireKey
	};

	class MMIKeyTask : public ZTask {
	public:
		MMIKeyTask(uint8_t prio);
		void _doTask(void *p_arg); //MMIKey
	};

	class BackLightTask : public ZTask {
	public:
		BackLightTask(uint8_t prio);
		void _doTask(void *p_arg);
	private:
		void _sendMsg1(void);
		void _sendMsg2(void);
	};

	class PageTrunKeyTask : public ZTask {
	public:
		PageTrunKeyTask(uint8_t prio);
		void _doTask(void *p_arg);
	};
	
private:
	ShiftSimTask         	*pshiftTask;
	SteeringWheelSimTask 	*psteeringTask;
	PKeySimTask          	*pKeyTask;
	TurnSimTask          	*pturnSimTask;
	TireKeySimTask       	*ptirekeySimTask;
	RadarSimTask         	*pradarSimTask;
	MMIKeyTask           	*pmmiKeyTask;
	BackLightTask			*pBackLightTask;
	PageTrunKeyTask 		*pPageTrunKeyTask;
};
#endif /* AUDIA62012TESTER_H_ */
