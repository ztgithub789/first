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
#include <DrvRelay.h>
#include <DrvHiResClock.h>
#include <GPTimer.h>
#include <DrvDipSwitch.h>
#include <DrvLED.h>
#include <DrvCAN.h>
#include <DrvTJA1041.h>
#include <DrvGPIO.h>
#include <DrvWatchDog.h>

extern DrvUSART		*gpDrvUSART;
extern DrvGPIO		*gpDrvLEDGreen, *gpDrvLEDRed;
extern DrvCAN	    *gpDrvCAN;
extern DrvTJA1041	*gpDrvTJA1041;
extern DrvTJA1041	*gpDrvTJA1041_2;
extern DrvGPIO		*gpDrvReverseSig;
extern DrvGPIO		*gpDrvTurnLeftSig;
extern DrvGPIO		*gpDrvTurnRightSig;
extern DrvGPIO		*gpDrvPKeyPessedSig;

extern DrvGPIO *gpDrvLED3;
extern DrvGPIO *gpDrvLED4;
extern DrvGPIO *gpDrvLED5;
extern DrvGPIO *gpDrvLED6;
extern DrvGPIO *gpDrvLED7;
extern DrvGPIO *gpDrvLED8;
extern DrvGPIO *gpDrvLED9;
extern DrvGPIO *gpDrvLED10;
extern DrvGPIO *gpDrvLED11;

extern DrvGPIO *gpDrvCOL1;
extern DrvGPIO *gpDrvCOL2;
extern DrvGPIO *gpDrvCOL3;
extern DrvGPIO *gpDrvCOL4;
extern DrvGPIO *gpDrvCOL5;

extern DrvGPIO *gpDrvROW1;
extern DrvGPIO *gpDrvROW2;
extern DrvGPIO *gpDrvROW3;
extern DrvGPIO *gpDrvROW4;
extern DrvGPIO *gpDrvROW5;


#endif /* BOARD_H_ */
