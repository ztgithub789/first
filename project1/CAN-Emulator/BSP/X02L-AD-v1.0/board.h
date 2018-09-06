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



#ifndef BOARD_H_
#define BOARD_H_

#include <DrvUSART.h>
#include <DrvCH7025.h>
#include <DrvRelay.h>
#include <DrvM3366.h>
#include <DrvHiResClock.h>
#include <GPTimer.h>
#include <DrvDipSwitch.h>
#include <DrvExti.h>
#include <DrvLED.h>
#include <DrvCAN.h>
#include <DrvTJA1041.h>
#include <DrvI2C.h>
#include <../DrvTW8835/DrvTW8835.h>
#include <../DrvTW8835/DrvTW8835GPIO.h>
#include <DrvGPIO.h>

#define REAR_CAMERA_YIN	0
#define FRONT_CAMERA_YIN	1

extern DrvUSART		*gpDrvUSART;
extern DrvUSART		*gpDrvUSART2;
extern DrvUSART		*gpDrvUSART3;
extern DrvHiResClock	*gpDrvHiResClock;
extern DrvGPTimer 	*gpDrvTimer;

extern DrvDipSwitch		*gpDrvDipSwitch;
extern DrvExti		*gpDrvRemoteExti1;
extern DrvExti		*gpDrvRemoteExti2;
extern DrvLED		*gpDrvLEDGreen, *gpDrvLEDRed;
extern DrvCAN	*gpDrvCAN;
extern DrvTJA1041	*gpDrvTJA1041;
extern DrvI2C		*gpDrvI2C;
extern DrvTW8835	*gpDrvTW8835;

extern DrvExti		*gpDrvExti_VSYNC;
extern DrvExti		*gpDrvExti_DE;
extern DrvGPIO		*gpDrvBackLightDataPin;
extern DrvGPIO 	*gpDrvReverseSigDetector;
extern DrvGPIO		*gpDrvIRDataPin;

extern DrvTW8835GPIO	*gpDrvCamPwrOnPin;
extern DrvTW8835GPIO	*gpDrvAV1PwrOnPin;
extern DrvTW8835GPIO	*gpDrvAV2PwrOnPin;
extern DrvTW8835GPIO 	*gpDrvAVSwitch1Pin;
extern DrvTW8835GPIO	*gpDrvAVSwitch2Pin;
extern DrvTW8835GPIO	*gpDrvAudioSelPin;
extern DrvTW8835GPIO	*gpDrvNavAsdSelPin;
extern DrvTW8835GPIO	*gpDrvLvdsSelPin;
extern DrvTW8835GPIO	*gpDrvDsEnPin;
extern DrvTW8835GPIO	*gpDrvNavPwrOnSelector;

class ReverseSigDetector {
public:
	~ReverseSigDetector(void);
	static ReverseSigDetector *getInstance(void);
	bool getValue(void);
};

#endif /* BOARD_H_ */
