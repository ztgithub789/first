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


#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <ucos_ii.h>
#include <Zox.h>

/*
 * 内部文档
 *
 *  - 头指针(head)表示当前读的位置，尾指针(tail)表示当前写的位置。
 *  - 初始化:
 *      head = tail = 0;
 *      isFull = false;
 *  - 满靠isFull判断，空靠读的时候是否阻塞判断
 *  - 读写之前要确认满标志
 *  - 读写完后要更新满标志
 */
template <typename T, size_t size>
class RingBuffer {

private:
	T _buffer[size];
	int head, tail;					// 头尾下标
	OS_EVENT *readSem, *writeSem;	// 用于阻塞读写任务的信号量

public:

	RingBuffer()
	{
		head = tail = 0;

		readSem = OSSemCreate(0);
		writeSem = OSSemCreate(size);
		if (readSem == 0 || writeSem == 0) {
			//zprintf("Sem creation failure.\n");
			return;
		}
	}

	~RingBuffer() {}

	/****************************************************************************
	 * 简介   :	开始写操作
	 * 参数   :	无
	 * 返回值 :	0		缓冲已满
	 * 			其他	写入地址
	 * 描述   :
	 ****************************************************************************/
//	T* beginWriting(void)
//	{
//		return &_buffer[tail];
//	}

	/****************************************************************************
	 * 简介   :	结束写操作
	 * 参数   :	无
	 * 返回值 :	0		成功
	 * 			其他	失败
	 * 描述   :
	 ****************************************************************************/
//	INT8U finishWriting(void)
//	{
//		tail++;
//		if (tail == size)
//			tail = 0;
//
//		// 唤醒在等待的任务
//		return OSSemPost(readSem);
//	}

	/****************************************************************************
	 * 简介   :	往缓冲区中写数据，如果已满，则会阻塞
	 * 参数   :
	 * 返回值 :	0		成功
	 * 			OS_ERR_TIMEOUT	超时
	 * 描述   :
	 ****************************************************************************/
	int write(T *content, INT32U timeout = 0)
	{
		uint8_t err;

		OSSemPend(writeSem, timeout, &err);
		if (err != OS_ERR_NONE)
			return err;

		_buffer[tail] = *content;

		tail++;
		if (tail == size)
			tail = 0;

		return OSSemPost(readSem);
	}

	/*******************************************************************************
	 * 简介   :	往缓冲区中写数据，如果已满，不会阻塞
	 * 参数   :
	 * 返回值 :
	 * 			0			成功
	 * 			其他		OSSemAccept的返回值
	 * 描述   :
	 *******************************************************************************/
	int writeNoBlock(T *content)
	{
		uint16_t ret;
		ret = OSSemAccept(writeSem);
		if (ret > 0) {
			// 成功
			_buffer[tail] = *content;

			tail++;
			if (tail == size)
				tail = 0;

			return OSSemPost(readSem);
		} else {
			// 失败
			return -1;
		}
	}

	/****************************************************************************
	 * 简介   :	读缓冲，如果缓冲区为空，则阻塞
	 * 参数   :	buf		放置读取内容的缓冲区，由调用者提供
	 * 			timeout	阻塞等待的超时值，单位: ms
	 * 返回值 :	ERR_NONE		读取成功， buf为所读内容
	 * 			其他			读取失败，返回OSSemPend()的错误码
	 * 描述   :
	 ****************************************************************************/
	INT8U read(T* buf, INT32U timeout = 0)
	{
		INT8U	err;

		OSSemPend(readSem, MS_2_TICK(timeout), &err);
		if (err != OS_ERR_NONE)
			return err;

		*buf = _buffer[head];

		head ++;
		if (head == size)
			head = 0;

		OSSemPost(writeSem);

		return OS_ERR_NONE;
	}

	bool readNoBlock(T *buf)
	{
		uint16_t ret;
		ret = OSSemAccept(readSem);
		if (ret > 0) {
			// 成功
			*buf =  _buffer[head] ;
			head++;
			if (head == size) head = 0;
			OSSemPost(writeSem);
			return true;
		}
		return false;
	}
	/****************************************************************************
	 * 简介   :	获取read()在阻塞时所用的信号量。一般用于在其他任务中让调用read()的任务退出阻塞。
	 * 参数   :	无
	 * 返回值 :	指向所要获取的信号量的指针
	 * 描述   :
	 ****************************************************************************/
	OS_EVENT *getBlockSem(void)
	{
		return readSem;
	}

	/****************************************************************************
	 * 简介   :	清除掉所有内容
	 * 参数   :
	 * 返回值 :
	 * 描述   :
	 ****************************************************************************/
	void clear(void)
	{
#if OS_CRITICAL_METHOD == 3u                          /* Allocate storage for CPU status register      */
		OS_CPU_SR  cpu_sr = 0u;
#endif
		INT8U err;

		OS_ENTER_CRITICAL();

		head = tail = 0;

		OSSemSet (readSem, 0, &err);
		OSSemSet (writeSem, size, &err);
		if (err != OS_ERR_NONE) {
			//zprintf("Ring buffer Sem reset failure.\n");
		}
		OS_EXIT_CRITICAL();
	}

};


#endif /* RINGBUFFER_H_ */
