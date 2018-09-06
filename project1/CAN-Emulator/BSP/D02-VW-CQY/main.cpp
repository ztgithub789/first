/****************** (C) COPYRIGHT 2011 BroadVision.Co.Ltd ******************
 *
 * 作者       : 孙劲飞
 * 版本       : V0.1
 * 创建日期   : 2011-11-16
 * 描述       : Zox主函数定义文件
 *
 ****************************************************************************/

#include <Zox.h>

int main(void)
{
	Zox *z = Zox::getInstance();
	return z->run();
}
