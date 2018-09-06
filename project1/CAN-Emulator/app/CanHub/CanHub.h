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


#ifndef CANHUB_H_
#define CANHUB_H_

#include <includes.h>
#include <utils/RingBuffer/RingBuffer.h>

#define CAN_HUB_SLOT_NR		2	// slot����
#define CAN_HUB_BUFFER_SIZE	3	// ÿ��slot�Ļ����С
#define CAN_HUB_FILTER_SIZE	7	// �������п������ɵ�ID��



class CanHub {
public:

	class Slot {
	public:
		/*
		 * ��ʼ״̬Ϊ�κ�ID������ͨ��(-1)
		 */
		class IdFilter {
		public:
			IdFilter();
			uint8_t addId(uint32_t id);
			uint8_t delId(uint32_t id);
			void clear(void);
			bool isIdValid(uint32_t id);

		private:
			static const uint32_t _invalidId = (uint32_t)-1;
			uint32_t _idList[CAN_HUB_FILTER_SIZE];
		};

		uint8_t read(CanTMsg *pmsg, uint32_t timeout = 0);
		uint8_t write(CanTMsg *pmsg, uint32_t timeout = 0);
		uint8_t addId(uint32_t id);
		uint8_t delId(uint32_t id);
		void  clearFilter(void);
		bool isIdValid(uint32_t id);

	private:

		RingBuffer<CanTMsg, CAN_HUB_BUFFER_SIZE> _ringBuffer;	// slot�õĻ�����
		IdFilter _filer;
	};
	static CanHub *getInstance(void);
	virtual ~CanHub();
	Slot *getSlot(void);

private:
	bool _slotAvailBitmap[CAN_HUB_SLOT_NR];		// 	��¼��ǰ���Է����slot
	Slot _slots[CAN_HUB_SLOT_NR];
	CanHub();
	uint8_t init(void);
	static void _doTask(void *p_arg);
};


#endif /* CANHUB_H_ */
