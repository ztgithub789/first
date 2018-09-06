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


#ifndef SETBD_H_
#define SETBD_H_
#include <vector>
#include <stdio.h>
#include <Zox.h>
#include <drivers/include/DrvCAN.h>
#include <utils/Shell/Shell.h>

class setbdCommand : public Command {
public:
	setbdCommand();
	const char *GetName(void) { return "setbd"; }
	const char *GetHelpStr(void) { return "Set baud rate"; }
	void ShowUsage(void);
	int Execute(const int argc, const char *argv[]);

private:
	static const int CAN1_MASK = 1;
	static const int CAN2_MASK = 2;

	/**
	 * @brief 自动检测波特率
	 * @param CANx 需要设置的端口。
	 */
	static void AutoProbeBD(CAN_TypeDef *CANx);
	static void AutoProbeBD_CB(CanRxMsg* RxMsg, void*);
	static int dataReceived;
	static DrvCAN *pdrvCAN;
};

#endif /* SETBD_H_ */
