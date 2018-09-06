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

#ifndef MAGOTANTESTER_H_
#define MAGOTANTESTER_H_

#include <ZTask.h>
#include <Car/CarInfo.h>
#include <Car/Tester.h>

class MagotanTester : public Tester {
public:
	CarInfo *pCar;

	MagotanTester();
	virtual ~MagotanTester();
	uint8_t run(CarInfo *pcar);
	void steerLeft(void);
	void steerRight(void);
	void shiftDown(void);
	void shiftUp(void);
	void increaseRadarVal(int nr, int dv);
	void togglePKey(void);
	void changeTurningLight(bool isleft_rightNOfar,uint8_t value);
	
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

	class RadarSimTask : public ZTask {
	public:
		RadarSimTask(uint8_t prio);
		void _doTask(void *p_arg);
	};
	
	class TurningLightSimTask : public ZTask {
	public:
		TurningLightSimTask(uint8_t prio);
		void _doTask(void *p_arg);
	};
	
	class DoorSimTask : public ZTask {
	public:
		DoorSimTask(uint8_t prio);
		void _doTask(void *p_arg);
	};
	
private:
	ShiftSimTask *pshiftTask;
	SteeringWheelSimTask *psteeringTask;
	RadarSimTask *pradarSimTask;
	TurningLightSimTask *pturningSimTask;
	DoorSimTask *pDoorSimTask;
};

#endif /* MAGOTANTESTER_H_ */
