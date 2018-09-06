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
 * �����ĸ���: ״̬����һ�д���һ��״̬������״̬����ֲ���״̬������ĸ������ϣ������ظ���
 * ������Ϊ�кš�
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
	 * ���   : ���úͻ�ȡ״̬������
	 * ����   :
	 * ����ֵ :
	 * ����   :	����Ч�ʵĿ��ǣ���״̬�б����˸�״̬��״̬���е�������
	 ****************************************************************************/
	int getIndex(void) { return _index; }
	void setIndex(int i) { _index = i; }

	/****************************************************************************
	 * ���   : ���ó�ʱֵ
	 * ����   :
	 * ����ֵ :
	 * ����   :	Ϊ�˱��ڴ���ʱ�¼�����ʱֵ������״̬����
	 ****************************************************************************/
	int getTimeout(void) { return _timeout; }
	void setTimeout(int t) { _timeout = t; }

#ifdef DEBUG
	/****************************************************************************
	 * ���   : ���úͻ�ȡ״̬�������֡����ڵ��ԡ�
	 * ����   : �������溬��
	 * ����ֵ : �������溬��
	 * ����   :
	 ****************************************************************************/
	void setName(const char *name) { _name = name; }
	const char *getName(void)      { return _name;}
#endif

private:

	int _index;
	int _timeout;
#ifdef DEBUG
	const char *_name;		// ״̬������
#endif
};

/*
 * ״̬����
 * stateNr: ״̬�ĸ���
 * eventNr:	�¼��ĸ������ڲ�������һ����ʱ�¼�������ڲ����¼�����ΪevntNr + 1��
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
	 * ���   :	����״̬���ĳ�ʼ״̬
	 * ����   :	s					��ʼ״̬
	 * ����ֵ :	E_OK				�ɹ�
	 * 			E_INVALID_STATE		��ʼ״̬��Ч
	 * ����   :
	 ****************************************************************************/
	int setInitialState(State *s)
	{
		/*
		 * ��������Ч��
		 */
		if (getStateIndex(s) == E_INVALID_STATE)
			return E_INVALID_STATE;

		initState = s;
		curState  = s;
		return E_OK;
	}

	/****************************************************************************
	 * ���   : Ϊ״̬�����ת�ƹ�ϵ
	 * ����   :	from	��ʼ״̬
	 *  		e		�¼�
	 *          to		����e�¼���ת��������һ��״̬
	 * ����ֵ : E_OK			��ӳɹ�
	 * 			E_INVALID_STATE	״̬��Ч
	 * 			E_INVALID_EVENT	�¼���Ч
	 * ����   :
	 ****************************************************************************/
	int addTransition(State *from, int e , State *to)
	{
		/*
		 * �ȼ��from, e, to�Ƿ���Ч
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
	 * ���   :	����״̬�ĳ�ʱת��
	 * ����   :	from	��ʼ״̬
	 *  		timeout	��ʱֵ����λ:ms
	 *          to		��ʱ��ת��������һ��״̬
	 * ����ֵ : E_OK			��ӳɹ�
	 * 			E_INVALID_EVENT	�¼���Ч
	 * ����   :
	 ****************************************************************************/
	int addTimeout(State *from, int timeout , State *to)
	{
		/*
		 * �ȼ��from, to�Ƿ���Ч
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
	 * ���   :	����״̬��
	 * ����   :	��
	 * ����ֵ :	OK					״̬�������˳�
	 * 			NO_INITIAL_STATE	�޳�ʼ״̬
	 * ����   :
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
	 * ���   :	����״̬����������״̬�������Զ����С���ִ��run()�������������״
	 * 			̬��
	 * ����   :	��
	 * ����ֵ :	��
	 * ����   :
	 ****************************************************************************/
	void reset(void)
	{
		curState = initState;
	}

	/****************************************************************************
	 * ���   :	��״̬�������¼�֪ͨ
	 * ����   :	e	���͵��¼�
	 * ����ֵ :	E_OK				�¼�������
	 * 			E_EVENT_IGNORED		�¼�������
	 * 			E_INVALID_EVENT		��Ч�¼�
	 * ����   :
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
	State *index[stateNr];						// ״̬�������飬��ʼ��Ϊȫ0�����ڻ�ȡ״̬����Ӧ���кš�
	State *matrix[eventNr + 1][stateNr];		// ״̬ת�ƾ��󣬳�ʼ��Ϊȫ0�������ӵ�һ����Ϊ�˶�Ӧ��ʱ�¼���
	int   eventTimeout;							// ��ʱ�¼��ı��(Ϊ�������һ�е��к�)��
	State *curState;		// ��ǰ״̬
	State *initState;		// ��ʼ״̬
	Timer timer;
#ifdef DEBUG
	const char *_name;
#endif
	/****************************************************************************
	 * ���   : ��ȡ�����״̬������(�к�)����״̬�������ڸ��С�
	 * ����   :	s	����Ȥ��״̬��ָ��
	 * ����ֵ :	E_INVALID_STATE	ʧ��
	 * 			����			������
	 * ����   : ���s�Ѿ����������������򷵻ظ�������
	 * 			���s��δ�����������������һ���������ٷ��ر������������
	 * 			����Ѿ��޷������������򷵻�E_INVALID_STATE��
	 ****************************************************************************/
	int getStateIndex(State *s)
	{
		int i;

		/*
		 * �ȼ�������Ƿ��ѱ�����
		 */
		for (i = 0; i < stateNr; i++) {
			if (index[i] == s) {
				return i;		// �����ѷ��������
			}
		}

		/*
		 * ���˴˴�������s��������δ������
		 */
		for (i = 0; i < stateNr; i++) {
			if (index[i] == 0) {
				index[i] = s;		// ����i��s
				s->setIndex(i);		// ���������浽״̬��
				return i;
			}
		}

		return E_INVALID_STATE;
	}

	/****************************************************************************
	 * ���   :	��ʱ���ص�����
	 * ����   :	data	�������thisָ��
	 * ����ֵ :	��
	 * ����   :
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
