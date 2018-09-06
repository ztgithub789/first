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
#ifndef CAN_H_
#define CAN_H_

#include <vector>
#include <ucos_ii.h>
#include <stm32f10x_can.h>
#include <stm32f10x.h>
#include <Driver.h>
#include <utils/RingBuffer/RingBuffer.h>

class DrvCAN_Receiver {
public:
	virtual ~DrvCAN_Receiver() {};
	virtual int onMsgReceived(CanRxMsg*) = 0;
};

struct ResCAN {
	uint32_t		RCC_APB2Periph_CANTx;
	GPIO_TypeDef	*GPIO_Port_CANTx;
	uint16_t		GPIO_Pin_CANTx;
	uint32_t		RCC_APB2Periph_CANRx;
	GPIO_TypeDef	*GPIO_Port_CANRx;
	uint16_t		GPIO_Pin_CANRx;
	uint32_t 		GPIO_CAN_Remap;
};

typedef struct {
	CAN_TypeDef *CANx;
	CanRxMsg msg;
} CanTMsg;

class DrvCAN : public Driver {
public:
	typedef void  (*Send_T)(CAN_TypeDef *CANx,void *);
	typedef  void (*ReceiveCB_T)(CanTMsg *,void * );
	enum PortNrEnum {
		port1 = 1,
		port2,
		port_both,
	};

	enum RetVal {
		E_OK 				= 0,		// OK
		E_INVALID_PARAM		= -1,		// 参数无效
	};

	typedef struct {
	    u16 BaudRateValue;
	    u8  CAN_SJW;
	    u8  CAN_BS1;
	    u8  CAN_BS2;
	    u16 CAN_Prescaler;
	} BaudRateTypeDef;

	static BaudRateTypeDef  baudRate[];

	class CanMsgRingBuffer {
	public:
		CanMsgRingBuffer(size_t size);
		~CanMsgRingBuffer(void);
		int write(CanTMsg *content, INT32U timeout = 0);
		int writeNoBlock(CanTMsg *content);
		INT8U read(CanTMsg* buf, INT32U timeout = 0);
		OS_EVENT *getBlockSem(void);
		void clear(void);
	private:
		int head, tail;
		OS_EVENT *readSem, *writeSem;
		CanTMsg *_buffer;
		size_t _size;
	};

	DrvCAN(size_t size,struct ResCAN *pResCAN1,struct ResCAN *pResCAN2=NULL);
	virtual ~DrvCAN();
	int init(void);
	void showPortInfo (void);
	void showCountInfo (void);
	int setBaudrate(CAN_TypeDef *CANx, int br, uint8_t mode = CAN_Mode_Normal);
	uint8_t transmit(CAN_TypeDef* CANx, CanTxMsg *TxMessage);
	void addFilters(const uint32_t *PFilterIDList);
	vector<int> getBaudrateList(void);
	static void CAN1_RX0_IRQ_Handler(void*);
	static void CAN2_RX0_IRQ_Handler(void*);
	void RX0_IRQ_Handler(PortNrEnum p);
	INT8U getData(CanTMsg *pmsg, INT32U timeout = 0);
	void enableIRQ(CAN_TypeDef *CANx);
	void disableIRQ(CAN_TypeDef *CANx);
	void resetOverflowCnt(CAN_TypeDef *CANx);
	int getOverflowCnt(CAN_TypeDef *CANx);
	void clearBuffer(void);
	void setOnMsgReceivedHandler(ReceiveCB_T func, void *param = 0);
	void clearOnMsgReceivedHandler(void);
	void setOnMsgSendHandler(Send_T func, void *param = 0);
	CanMsgRingBuffer *getRingBuffer(void);
	void resizeRingBuffer(size_t size);
	void setupReceivePortNum(PortNrEnum num);
private:
	int baudRate2Index(const u16);
	void GPIO_Configuration(struct ResCAN *pResCAN);
	void NVIC_Configuration(void);
	static void _canReverseTask(void *argv);
	void *_msgRcvCallBackFuncParam;
	void *_msgSendCallBackFuncParam;
	Send_T _msgSendCallBackFunc;

	int baudRateNr;									// 有效波特率个数
	void *_data;
	CanMsgRingBuffer	 *_pRingBuffer;			// CAN1, CAN2 的消息都放在同一个环形缓冲区中
	int overflowCnt1;							// 溢出计数
	int overflowCnt2;
	OS_STK _stk[SMALL_TASK_STK_SIZE];
	OS_EVENT *_pCANRxSem;
	ReceiveCB_T _msgRcvCallBackFunc;

	struct ResCAN *_pResCAN1;
	struct ResCAN *_pResCAN2;

	size_t _ringBufferSize;

	PortNrEnum _receiveIrqPortNum;
public:

};
#endif /* CAN_H_ */
