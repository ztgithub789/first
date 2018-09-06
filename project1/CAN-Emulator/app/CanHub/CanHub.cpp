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


#include <zdebug.h>
#include "CanHub.h"

CanHub::Slot::IdFilter::IdFilter()
{
	clear();
}

/*******************************************************************************
 * 简介   :	把一个ID加入过滤器，允许这个ID的帧通过
 * 参数   :
 * 返回值 :	0 		成功
 * 			其他	失败
 * 描述   :
 *******************************************************************************/
uint8_t CanHub::Slot::IdFilter::addId(uint32_t id)
{
	int i;
	for (i = 0; i<CAN_HUB_FILTER_SIZE; i++) {
		if (_idList[i] == _invalidId)
			break;
	}

	if (i < CAN_HUB_FILTER_SIZE ) {
		// 成功找到空位
		_idList[i] = id;
		return 0;
	} else {
		// 已经没有空位
		return (uint8_t)-1;
	}
}

/*******************************************************************************
 * 简介   :	把一个ID从过滤器中删除。这个ID的帧将不再能从过滤器中通过。
 * 参数   :
 * 返回值 :	0		成功
 * 			其他	失败
 * 描述   :
 *******************************************************************************/
uint8_t CanHub::Slot::IdFilter::delId(uint32_t id)
{
	int i;
	for (i = 0; i<CAN_HUB_FILTER_SIZE; i++)
		if (_idList[i] == id) {
			_idList[i] = _invalidId;
			return 0;
		}
	return (uint8_t)-1;
}

void CanHub::Slot::IdFilter::clear(void)
{
	int i;
	for (i = 0; i<CAN_HUB_FILTER_SIZE; i++)
		_idList[i] = (uint32_t)-1;
}

/*******************************************************************************
 * 简介   :	判断一个ID是否是有效ID。有效ID将被允许通过，无效ID不被允许通过
 * 参数   :
 * 返回值 :	true
 * 			false
 * 描述   :
 *******************************************************************************/
bool CanHub::Slot::IdFilter::isIdValid(uint32_t id)
{
	int i;
	for (i = 0; i<CAN_HUB_FILTER_SIZE; i++)
		if (_idList[i] == id)
			return true;
	return false;
}

/*******************************************************************************
 * 简介   :	从slot读取数据。用法和DrvCAN的getData()相同。
 * 参数   :
 * 返回值 :	0		成功
 * 			其他	参考OSSemPend的返回值
 *
 * 描述   :
 *******************************************************************************/
uint8_t CanHub::Slot::read(CanTMsg *pmsg, uint32_t timeout)
{
	return _ringBuffer.read(pmsg, timeout);
}

uint8_t CanHub::Slot::write(CanTMsg *pmsg, uint32_t timeout)
{
	return _ringBuffer.write(pmsg, timeout);
}


/*******************************************************************************
 * 简介   :	为指定slot设定过滤器ID
 * 参数   :
 * 返回值 :	0		成功
 * 			其他	失败
 * 描述   :
 *******************************************************************************/
uint8_t CanHub::Slot::addId(uint32_t id)
{
	return _filer.addId(id);
}

uint8_t CanHub::Slot::delId(uint32_t id)
{
	return 0;
}


void  CanHub::Slot::clearFilter(void)
{
	_filer.clear();
}

bool CanHub::Slot::isIdValid(uint32_t id)
{
	return _filer.isIdValid(id);
}

// CanHub code
CanHub *CanHub::getInstance(void)
{
	static CanHub instance;
	return &instance;
}

CanHub::CanHub() {
	// 所有slot均置为可用
	for (int i = 0; i<CAN_HUB_SLOT_NR; i++)
		_slotAvailBitmap[i] = true;
	init();
}

CanHub::~CanHub() {
	// TODO Auto-generated destructor stub
}

/*******************************************************************************
 * 简介   :	获取一个可用的slot
 * 参数   :
 * 返回值 :	非0		指向可用的slot的指针
 * 			0		无可用的slot号
 * 描述   :
 *******************************************************************************/
CanHub::Slot *CanHub::getSlot(void)
{
	int i;
	for (i = 0; i<CAN_HUB_SLOT_NR; i++) {
		if (_slotAvailBitmap[i]) {
			_slotAvailBitmap[i] = false;
			return &_slots[i];
		}
	}
	return 0;
}

static OS_STK _stk[COMMON_TASK_STK_SIZE];

uint8_t CanHub::init(void)
{
	int8_t err;

	err = OSTaskCreateExt((void (*)(void *)) _doTask,
			(void          * ) this,
			(OS_STK        * )&_stk[COMMON_TASK_STK_SIZE- 1],
			(INT8U           ) CAN_HUB_TASK_PRIO,
			(INT16U          ) CAN_HUB_TASK_PRIO,
			(OS_STK        * )&_stk[0],
			(INT32U          ) COMMON_TASK_STK_SIZE,
			(void          * )0,
			(INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

	if (err != OS_ERR_NONE) {
		//reportErr("create CanHub task.", err);
	}

	return err;
}

void CanHub::_doTask(void *p_arg)
{
	CanHub *_this = (CanHub*)p_arg;
  
	while (1) {
		CanTMsg rxMsg;
		uint8_t ret = gpDrvCAN->getData(&rxMsg, 0);
		if (ret == OS_ERR_NONE) {
			// 过滤并转发
			int i;
			for (i = 0; i < CAN_HUB_SLOT_NR; i++) {
				// 仅对已分配的slot进行操作
				if (_this->_slotAvailBitmap[i] == false) {
					if (_this->_slots[i].isIdValid(rxMsg.msg.StdId))
						_this->_slots[i].write(&rxMsg);
				}
			}
		}
	}
}
