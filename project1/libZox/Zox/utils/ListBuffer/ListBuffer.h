/*
 *  ListBuffer.h
 *
 *  Created on: 2018年9月1日
 *  Author: huang_lm
 *  简介: 单链表存储buffer
 *        适用于中断处理函数中使用(不带信号量)
 */



#ifndef LISTBUFFER_H_
#define LISTBUFFER_H_

#include <ucos_ii.h>
#include <Zox.h>

/*  
 *  中断处理函数中调用write()进行写操作
 *  在线程中调用read()时会进入临界区
 */
template <typename T>

class ListBuffer {

private:
	typedef struct list_node {
		T data;
		struct list_node  *next;
	}List_Node,*pList_Node;
	//T _buffer[size];
	//int head, tail;					// 头尾下标
	//OS_EVENT *readSem, *writeSem;	// 用于阻塞读写任务的信号量
public:

	pList_Node head;   //头节点
	ListBuffer()
	{
		head = new List_Node;

		memset(&head->data, 0, sizeof(T));
		head->next = NULL;
	}

	~ListBuffer() {}

	/****************************************************************************
	 * 简介   :	往链表中写数据，尾插法
	 * 参数   :
	 * 返回值 :	0		成功
	 * 			
	 * 描述   :
	 ****************************************************************************/
	int write(T *content)
	{
		pList_Node newNode;

		#if OS_CRITICAL_METHOD == 3u /* Allocate storage for CPU status register */
		OS_CPU_SR  cpu_sr = 0u;
		#endif
		OS_ENTER_CRITICAL();
		
		newNode = new List_Node;
		pList_Node pnode = head;

		while(pnode->next){
			pnode = pnode->next;
		}

		pnode->next  = newNode;
		newNode ->data = *content;  
		newNode ->next = NULL;

		OS_EXIT_CRITICAL();
		return 0;
	}

	/****************************************************************************
	 * 简介   :	读缓冲，如果缓冲区为空，则返回-1
	 * 参数   :	buf		放置读取内容的缓冲区，由调用者提供
	 * 			
	 * 返回值 :	true	读取成功， buf为所读内容，并将该节点从链表中删除
	 * 			false	读取失败，链表为空
	 * 描述   :
	 ****************************************************************************/
	bool read(T* buf)
	{
		if(head->next == NULL){
			//zprintf("Read err: the list is null!\n");
			return false;
		}

		#if OS_CRITICAL_METHOD == 3u /* Allocate storage for CPU status register */
		OS_CPU_SR  cpu_sr = 0u;
		#endif
		OS_ENTER_CRITICAL();
		
		pList_Node pnode = head->next->next;
		*buf = head->next->data;
		delete head->next;  //释放节点内存空间
		head->next = pnode;

		OS_EXIT_CRITICAL();	
		return true;
	}

	/****************************************************************************
	 * 简介   :	清除掉所有内容
	 * 参数   :
	 * 返回值 :
	 * 描述   :
	 ****************************************************************************/
	bool clear(void)
	{
		if(head->next == NULL){
			printf("Clear err: the list is null!\n");
			return false;
		}
		
		#if OS_CRITICAL_METHOD == 3u /* Allocate storage for CPU status register */
		OS_CPU_SR  cpu_sr = 0u;
		#endif
		OS_ENTER_CRITICAL();

		pList_Node pnode = head->next->next;
		while(head->next){
			delete head->next;
			head->next = pnode;
			if(head->next){
				pnode = head->next->next;
			}
		}
		
		OS_EXIT_CRITICAL();
		return true;
	}

};

#endif /* LISTBUFFER_H_ */
