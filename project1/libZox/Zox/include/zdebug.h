/**************** (C) COPYRIGHT 2011 BroadVision.Co.Ltd ************************
 *
 * 作者       : 孙劲飞
 * 版本       : 
 * 创建日期   : 2012-3-31
 * 描述       : 
 *
 *******************************************************************************/


#ifndef ZDEBUG_H_
#define ZDEBUG_H_

#include <libcommon.h>

/*
 * Debug选项
 */
#ifdef ZDEBUG
#include <stdio.h>
#define zprintf(...) printf(__VA_ARGS__)
#else
#define zprintf(...)
#endif

#endif /* ZDEBUG_H_ */
