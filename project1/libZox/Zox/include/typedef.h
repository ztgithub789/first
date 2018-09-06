/**************** (C) COPYRIGHT 2011 BroadVision.Co.Ltd ************************
 *
 * 作者       : 孙劲飞
 * 版本       : 
 * 创建日期   : 2011-12-5
 * 描述       : 类型定义
 *
 *******************************************************************************/
#ifndef TYPEDEF_H_
#define TYPEDEF_H_

#include <stdint.h>


/*!< STM32F10x Standard Peripheral Library old types (maintained for legacy purpose) */
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  /*!< Read Only */
typedef const int16_t sc16;  /*!< Read Only */
typedef const int8_t sc8;   /*!< Read Only */

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  /*!< Read Only */
typedef const uint16_t uc16;  /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */

#endif /* TYPEDEF_H_ */
