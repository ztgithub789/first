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


#ifndef MISCCOMMANDS_H_
#define MISCCOMMANDS_H_

#include <stdio.h>
#include <list>
#include <Zox.h>
#include <DrvCAN.h>
#include <utils/optparse/optparse.h>
#include <utils/Shell/Shell.h>

/**
 * 切换到实时数据显示模式
 */
class dataCommand: public Command {
public:
	static dataCommand *getInstance(void)
	{
		static dataCommand instance;
		return &instance;
	}
	enum {
		MODE_DATA = 0,
		MODE_ID,
	};
	void ShowUsage(void);
	const char *GetName(void) { return "data"; }
	const char *GetHelpStr(void) { return "Print received data in realtime"; }
	void PrintRxMsg(CanTMsg *pMsg);
	int Execute(const int argc, const char *argv[]);
	static void  CANReadingTask (void *p_arg);
	static void  CAN2ReadingTask (void *p_arg);
protected:
	dataCommand();

private:
	DrvCAN		*pdrvCAN;
//	DrvUSART	*pdrvUSART;
//	DrvSysTick	*pdrvSysTick;
	static void dataModeReceiveCB(CanRxMsg *RxMsg, void *);
	static void IDModeReceiveCB(CanRxMsg *RxMsg, void *);
	int receiveCnt1, receiveCnt2;
	void doDataMode(INT8U mode = MODE_DATA);
	void doIDMode(void);
	void showFilters(void);
	list<int> includeList, excludeList;
	typedef list<int>::iterator Itor_T;
	bool filterPassed(int id);
	INT8U _mode;
};

class testCommand2 : public Command {
public:
	const char *GetName(void) { return "testCommand2"; }
	const char *GetHelpStr(void) { return "One-line help text of command2"; }

	int Execute(const int argc, const char *argv[])
	{
		printf("running %s\n", GetName());
		return 0;
	}
};

#endif /* MISCCOMMANDS_H_ */
