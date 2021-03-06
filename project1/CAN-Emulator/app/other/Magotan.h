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

#ifndef MAGOTAN_H_
#define MAGOTAN_H_

#include <Car/Car.h>
#include <CanHub/CanHub.h>

class Magotan : public Car {
public:
	Magotan(bool needScreenControllerTask);
	~Magotan();
	int init(void);
	void _doTask(void *p_arg);
	
private:
	void _updateSteerAngle(CanTMsg *pmsg);
	void _updateLightStatus(CanTMsg *pmsg);
	void _updateACCStatus(CanTMsg *pmsg);
	void _updateShiftPos(CanTMsg *pmsg);
	void _updateRadar(CanTMsg *pmsg);
//	static void _taskTmr(void *ptmr,void *callback_arg);
	CanHub::Slot *_pSlot;
	bool _needScreenControllerTask;
};

#endif /* MAGOTAN_H_ */
