/****************** (C) COPYRIGHT 2011 BroadVision.Co.Ltd ******************
 *
 * ����       : �ﾢ��
 * �汾       : V0.1
 * ��������   : 2011-11-16
 * ����       :	Zox��ͷ�ļ�
 *
 ****************************************************************************/

#ifndef ZOX_H_
#define ZOX_H_

#include <list>
#include <GPTimer.h>
#include <typedef.h>

//
// ����ΪZox����ѡ��
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
