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


#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#include <map>
#include <utils/timer/Timer.h>

#define DEBUG

/*
 * 索引的概念: 状态机的一列代表一个状态。各个状态必须分布在状态机矩阵的各个列上，不能重复。
 * 索引即为列号。
 */
class State {
public:
	enum {
		NO_TIMEOUT = -1,
	};

	State(const char *name = 0)
	{
		_timeout = NO_TIMEOUT;
#ifdef DEBUG
		_name = name;
#endif
	}
	virtual ~State() {}
	virtual void onEnter(void) {}
	virtual void onExit(void) {}

	/****************************************************************************
	 * 简介   : 设置和获取状态的索引
	 * 参数   :
	 * 返回值 :
	 * 描述   :	处于效率的考虑，在状态中保存了该状态在状态机中的索引。
	 ****************************************************************************/
	int getIndex(void) { return _index; }
	void setIndex(int i) { _index = i; }

	/****************************************************************************
	 * 简介   : 设置超时值
	 * 参数   :
	 * 返回值 :
	 * 描述   :	为了便于处理超时事件，超时值保存在状态类里
	 ****************************************************************************/
	int getTimeout(void) { return _timeout; }
	void setTimeout(int t) { _timeout = t; }

#ifdef DEBUG
	/****************************************************************************
	 * 简介   : 设置和获取状态机的名字。用于调试。
	 * 参数   : 如其字面含义
	 * 返回值 : 如其字面含义
	 * 描述   :
	 ****************************************************************************/
	void setName(const char *name) { _name = name; }
	const char *getName(void)      { return _name;}
#endif

private:

	int _index;
	int _timeout;
#ifdef DEBUG
	const char *_name;		// 状态机名字
#endif
};

/*
 * 状态机类
 * stateNr: 状态的个数
 * eventNr:	事件的个数。内部增加了一个超时事件，因此内部的事件总数为evntNr + 1。
 */
template <size_t stateNr, size_t eventNr>
class StateMachine {
public:
	enum Error{
		E_OK				= 0,
		E_NO_INITIAL_STATE	= -1,
		E_EVENT_IGNORED		= -2,
		E_INVALID_STATE		= -3,
		E_INVALID_EVENT		= -4,
	};

	StateMachine(const char *name = 0)
	{
		memset(index, 0, sizeof(index));
		memset(matrix, 0, sizeof(matrix));
		curState  = 0;
		initState = 0;
		eventTimeout = eventNr;
		timer.setMode(Timer::SINGLE_SHOT);
		timer.setTimerCB(&timercb, this);
#ifdef DEBUG
		_name = name;
#endif
	}

	virtual ~StateMachine() {}
	/****************************************************************************
	 * 简介   :	设置状态机的初始状态
	 * 参数   :	s					初始状态
	 * 返回值 :	E_OK				成功
	 * 			E_INVALID_STATE		初始状态无效
	 * 描述   :
	 ****************************************************************************/
	int setInitialState(State *s)
	{
		/*
		 * 检查参数有效性
		 */
		if (getStateIndex(s) == E_INVALID_STATE)
			return E_INVALID_STATE;

		initState = s;
		curState  = s;
		return E_OK;
	}

	/****************************************************************************
	 * 简介   : 为状态机添加转移关系
	 * 参数   :	from	起始状态
	 *  		e		事件
	 *          to		接收e事件后将转移至的下一个状态
	 * 返回值 : E_OK			添加成功
	 * 			E_INVALID_STATE	状态无效
	 * 			E_INVALID_EVENT	事件无效
	 * 描述   :
	 ****************************************************************************/
	int addTransition(State *from, int e , State *to)
	{
		/*
		 * 先检查from, e, to是否有效
		 */
		int ret = getStateIndex(to);
		if (ret == E_INVALID_STATE) {
			return ret;
		}

		ret = getStateIndex(from);
		if (ret == E_INVALID_STATE) {
			return ret;
		}

		if (e >= eventNr)
			return E_INVALID_EVENT;

		int colum = ret;
		matrix[e][colum] = to;
		return E_OK;
	}

	/****************************************************************************
	 * 简介   :	设置状态的超时转移
	 * 参数   :	from	起始状态
	 *  		timeout	超时值，单位:ms
	 *          to		超时后将转移至的下一个状态
	 * 返回值 : E_OK			添加成功
	 * 			E_INVALID_EVENT	事件无效
	 * 描述   :
	 ****************************************************************************/
	int addTimeout(State *from, int timeout , State *to)
	{
		/*
		 * 先检查from, to是否有效
		 */
		int ret = getStateIndex(to);
		if (ret == E_INVALID_STATE) {
			return ret;
		}

		ret = getStateIndex(from);
		if (ret == E_INVALID_STATE) {
			return ret;
		}

		from->setTimeout(timeout);

		int colum = ret;
		matrix[eventTimeout][colum] = to;
		return E_OK;

	}

	/****************************************************************************
	 * 简介   :	运行状态机
	 * 参数   :	无
	 * 返回值 :	OK					状态机正常退出
	 * 			NO_INITIAL_STATE	无初始状态
	 * 描述   :
	 ****************************************************************************/
	int run(void)
	{
		if (curState == 0)
			return E_NO_INITIAL_STATE;
		curState->onEnter();

		int t;
		if ((t = curState->getTimeout()) != State::NO_TIMEOUT) {
			timer.setTimeout(t);
			timer.on();
		}

		return E_OK;
	}

	/****************************************************************************
	 * 简介   :	重启状态机。重启后状态机不会自动运行。再执行run()来运行重启后的状
	 * 			态机
	 * 参数   :	无
	 * 返回值 :	无
	 * 描述   :
	 ****************************************************************************/
	void reset(void)
	{
		curState = initState;
	}

	/****************************************************************************
	 * 简介   :	向状态机发送事件通知
	 * 参数   :	e	发送的事件
	 * 返回值 :	E_OK				事件被处理
	 * 			E_EVENT_IGNORED		事件被忽视
	 * 			E_INVALID_EVENT		无效事件
	 * 描述   :
	 ****************************************************************************/
	int postEvent(int e)
	{
		State *next;
		if (e > eventNr)
			return E_INVALID_EVENT;

		next = matrix[e][curState->getIndex()];
		if (next == 0) {
			return E_EVENT_IGNORED;
		}

		curState->onExit();
		curState = next;
#ifdef DEBUG
		printf("State machine: %s -> %s\n", _name, next->getName());
#endif
		next -> onEnter();

		int t;
		if ((t = next->getTimeout()) != State::NO_TIMEOUT) {
			timer.setTimeout(t);
			timer.on();
		}

		return E_OK;
	}

private:
	State *index[stateNr];						// 状态索引数组，初始化为全0，用于获取状态机对应的列号。
	State *matrix[eventNr + 1][stateNr];		// 状态转移矩阵，初始化为全0。多增加的一行是为了对应超时事件。
	int   eventTimeout;							// 超时事件的编号(为矩阵最后一行的行号)。
	State *curState;		// 当前状态
	State *initState;		// 初始状态
	Timer timer;
#ifdef DEBUG
	const char *_name;
#endif
	/****************************************************************************
	 * 简介   : 获取或分配状态的索引(列号)。该状态被安排在该列。
	 * 参数   :	s	感兴趣的状态的指针
	 * 返回值 :	E_INVALID_STATE	失败
	 * 			其他			索引号
	 * 描述   : 如果s已经被分配了索引，则返回该索引。
	 * 			如果s还未被分配索引，则分配一个索引，再返回被分配的索引。
	 * 			如果已经无法分配索引，则返回E_INVALID_STATE。
	 ****************************************************************************/
	int getStateIndex(State *s)
	{
		int i;

		/*
		 * 先检查索引是否已被分配
		 */
		for (i = 0; i < stateNr; i++) {
			if (index[i] == s) {
				return i;		// 返回已分配的索引
			}
		}

		/*
		 * 到了此处，表明s的索引尚未被分配
		 */
		for (i = 0; i < stateNr; i++) {
			if (index[i] == 0) {
				index[i] = s;		// 分配i给s
				s->setIndex(i);		// 把索引保存到状态中
				return i;
			}
		}

		return E_INVALID_STATE;
	}

	/****************************************************************************
	 * 简介   :	定时器回调函数
	 * 参数   :	data	本对象的this指针
	 * 返回值 :	无
	 * 描述   :
	 ****************************************************************************/
	static void timercb (void *data)
	{
#ifdef DEBU
		printf("Timer triggered\n");
#endif
		StateMachine *s = (StateMachine *)data;
		s->postEvent(s->eventTimeout);
	}
};
#endif /* STATEMACHINE_H_ */
