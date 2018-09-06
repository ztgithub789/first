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
//#include <DrvRelay.h>
#include <DrvM3366.h>
#include <DrvHiResClock.h>
#include <GPTimer.h>
#include <DrvDipSwitch.h>
#include <DrvExti.h>
//#include <DrvLED.h>
#include <DrvCAN.h>
#include <DrvTJA1041.h>
#include <DrvI2C.h>
#include <../DrvTW8835/DrvTW8835.h>
#include <DrvDS241.h>
#include <DrvGPIO.h>
#include <DrvWatchDog.h>

#define REAR_CAMERA_YIN	1
#define FRONT_CAMERA_YIN	0


extern DrvUSART		*gpDrvUSART;

extern DrvCH7025	*gpDrvCH7025;
extern DrvGPIO		*gpDrvRelay;

extern DrvHiResClock	*gpDrvHiResClock;
extern DrvDipSwitch		*gpDrvDipSwitch;

extern DrvGPIO		*gpDrvLEDGreen, *gpDrvLEDRed;
extern DrvCAN	*gpDrvCAN;
extern DrvTJA1041	*gpDrvTJA1041;
extern DrvTJA1041	*gpDrvTJA1041_2;

extern DrvI2C		*gpDrvI2C;
extern DrvTW8835	*gpDrvTW8835;

extern DrvGPIO	*gpDrvACC_CTRL;
extern DrvGPIO	*gpDrvREVERSE_CTRL;
extern DrvGPIO	*gpDrvCAM_PWR_CTRL;
extern DrvGPIO *gpDrvH_LAMP_ACC;

extern DrvGPIO *gpDrvReverseSigDetector;
extern DrvGPIO *gpDrvRecoderExistsDec;
#endif /* BOARD_H_ */
