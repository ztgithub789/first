/****************** (C) COPYRIGHT 2011 BroadVision.Co.Ltd ******************
 *
 * 作者       : 孙劲飞
 * 版本       : V0.1
 * 创建日期   : 2011-11-16
 * 描述       :	Zox类头文件
 *
 ****************************************************************************/

#ifndef ZOX_H_
#define ZOX_H_

#include <list>
#include <GPTimer.h>
#include <typedef.h>

//
// 以下为Zox配置选项
//

#define MS_2_TICK(ms) (ms / (1000/OS_TMR_CFG_TICKS_PER_SEC))

class Zox {
public:
	//Zox();
	virtual ~Zox();
	int init(void);
	int run();
	const char *version;
	static void registerDriver(Driver *);
private:
	static list<Driver*>	driverList;
	static void taskInit (void *p_arg);
	void _NVIC_init(void);
};

#endif /* ZOX_H_ */
