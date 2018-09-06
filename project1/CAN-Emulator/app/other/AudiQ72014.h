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

#ifndef AUDIQ72014_H_
#define AUDIQ72014_H_

#include <Car/Car.h>


class AudiQ7_2014 : public Car {
public:
	AudiQ7_2014();
	~AudiQ7_2014();
	int init(void);
	void _doTask(void *p_arg);

#ifdef TEST
	enum MsgType{
		MSG_STEER_ANGLE,
		MSG_SHIFT_POS,
		MSG_RADAR_VAL,
	};
	CanTxMsg *getMsg(uint8_t type);
	uint32_t *getMsgTimeInterval(uint8_t type);
#endif
	void updateRadar(CanTMsg *pmsg);
private:

	void _updateShiftPos(CanTMsg *pmsg);
	void _updateSteerAngle(CanTMsg *pmsg);
	void _updatePKeyStatus(CanTMsg *pmsg);
	void _updateRadar(CanTMsg *pmsg);
	void _updateRadar2(CanTMsg *pmsg);
	void _updateParkToSideKeyStatus(CanTMsg *pmsg);
	void _updateTurnIndicatorStatus(CanTMsg *pmsg);
	void _updateTireKeyStatus(CanTMsg *pmsg);

};

#endif /* AudiQ7_2014_H_ */
