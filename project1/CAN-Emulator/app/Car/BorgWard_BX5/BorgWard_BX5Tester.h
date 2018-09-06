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

#ifndef BORGWARD_BX5TESTER_H
#define BORGWARD_BX5TESTER_H

#include <ZTask.h>
#include <Car/CarInfo.h>
#include <Car/Tester.h>

class BorgWard_BX5Tester : public Tester {
public:
	CarInfo *pCar;
		
	BorgWard_BX5Tester();
	~BorgWard_BX5Tester();
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
        
	class CarTimeTask : public ZTask{
	public:
		CarTimeTask(uint8_t prio);
		void _doTask(void *p_arg);
	};
private:
	ShiftSimTask *pshiftsimTask;
	SteeringWheelSimTask *psteeringTask;
	RadarSimTask *pradarSimTask;
	TurnSimTask *pturnSimTask;
	CarTimeTask *pCarTimeTask;
};
#endif /* AUDIA62012TESTER_H_ */
