/**************** (C) COPYRIGHT 2011 BroadVision.Co.Ltd ************************
 *
 * ����       : �ﾢ��
 * �汾       : 
 * ��������   : 2012-3-31
 * ����       : 
 *
 *******************************************************************************/


#ifndef ZDEBUG_H_
#define ZDEBUG_H_

#include <libcommon.h>

/*
 * Debugѡ��
 */
#ifdef ZDEBUG
#include <stdio.h>
#define zprintf(...) printf(__VA_ARGS__)
#else
#define zprintf(...)
#endif

#endif /* ZDEBUG_H_ */
