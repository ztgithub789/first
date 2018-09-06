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
 * �ڲ��ĵ�
 *
 *  - ͷָ��(head)��ʾ��ǰ����λ�ã�βָ��(tail)��ʾ��ǰд��λ�á�
 *  - ��ʼ��:
 *      head = tail = 0;
 *      isFull = false;
 *  - ����isFull�жϣ��տ�����ʱ���Ƿ������ж�
 *  - ��д֮ǰҪȷ������־
 *  - ��д���Ҫ��������־
 */
template <typename T, size_t size>
class RingBuffer {

private:
	T _buffer[size];
	int head, tail;					// ͷβ�±�
	OS_EVENT *readSem, *writeSem;	// ����������д������ź���

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
	 * ���   :	��ʼд����
	 * ����   :	��
	 * ����ֵ :	0		��������
	 * 			����	д���ַ
	 * ����   :
	 ****************************************************************************/
//	T* beginWriting(void)
//	{
//		return &_buffer[tail];
//	}

	/****************************************************************************
	 * ���   :	����д����
	 * ����   :	��
	 * ����ֵ :	0		�ɹ�
	 * 			����	ʧ��
	 * ����   :
	 ****************************************************************************/
//	INT8U finishWriting(void)
//	{
//		tail++;
//		if (tail == size)
//			tail = 0;
//
//		// �����ڵȴ�������
//		return OSSemPost(readSem);
//	}

	/****************************************************************************
	 * ���   :	����������д���ݣ�����������������
	 * ����   :
	 * ����ֵ :	0		�ɹ�
	 * 			OS_ERR_TIMEOUT	��ʱ
	 * ����   :
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
	 * ���   :	����������д���ݣ������������������
	 * ����   :
	 * ����ֵ :
	 * 			0			�ɹ�
	 * 			����		OSSemAccept�ķ���ֵ
	 * ����   :
	 *******************************************************************************/
	int writeNoBlock(T *content)
	{
		uint16_t ret;
		ret = OSSemAccept(writeSem);
		if (ret > 0) {
			// �ɹ�
			_buffer[tail] = *content;

			tail++;
			if (tail == size)
				tail = 0;

			return OSSemPost(readSem);
		} else {
			// ʧ��
			return -1;
		}
	}

	/****************************************************************************
	 * ���   :	�����壬���������Ϊ�գ�������
	 * ����   :	buf		���ö�ȡ���ݵĻ��������ɵ������ṩ
	 * 			timeout	�����ȴ��ĳ�ʱֵ����λ: ms
	 * ����ֵ :	ERR_NONE		��ȡ�ɹ��� bufΪ��������
	 * 			����			��ȡʧ�ܣ�����OSSemPend()�Ĵ�����
	 * ����   :
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
			// �ɹ�
			*buf =  _buffer[head] ;
			head++;
			if (head == size) head = 0;
			OSSemPost(writeSem);
			return true;
		}
		return false;
	}
	/****************************************************************************
	 * ���   :	��ȡread()������ʱ���õ��ź�����һ�������������������õ���read()�������˳�������
	 * ����   :	��
	 * ����ֵ :	ָ����Ҫ��ȡ���ź�����ָ��
	 * ����   :
	 ****************************************************************************/
	OS_EVENT *getBlockSem(void)
	{
		return readSem;
	}

	/****************************************************************************
	 * ���   :	�������������
	 * ����   :
	 * ����ֵ :
	 * ����   :
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
