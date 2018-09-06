/*
 *  ListBuffer.h
 *
 *  Created on: 2018��9��1��
 *  Author: huang_lm
 *  ���: ������洢buffer
 *        �������жϴ�������ʹ��(�����ź���)
 */



#ifndef LISTBUFFER_H_
#define LISTBUFFER_H_

#include <ucos_ii.h>
#include <Zox.h>

/*  
 *  �жϴ������е���write()����д����
 *  ���߳��е���read()ʱ������ٽ���
 */
template <typename T>

class ListBuffer {

private:
	typedef struct list_node {
		T data;
		struct list_node  *next;
	}List_Node,*pList_Node;
	//T _buffer[size];
	//int head, tail;					// ͷβ�±�
	//OS_EVENT *readSem, *writeSem;	// ����������д������ź���
public:

	pList_Node head;   //ͷ�ڵ�
	ListBuffer()
	{
		head = new List_Node;

		memset(&head->data, 0, sizeof(T));
		head->next = NULL;
	}

	~ListBuffer() {}

	/****************************************************************************
	 * ���   :	��������д���ݣ�β�巨
	 * ����   :
	 * ����ֵ :	0		�ɹ�
	 * 			
	 * ����   :
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
	 * ���   :	�����壬���������Ϊ�գ��򷵻�-1
	 * ����   :	buf		���ö�ȡ���ݵĻ��������ɵ������ṩ
	 * 			
	 * ����ֵ :	true	��ȡ�ɹ��� bufΪ�������ݣ������ýڵ��������ɾ��
	 * 			false	��ȡʧ�ܣ�����Ϊ��
	 * ����   :
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
		delete head->next;  //�ͷŽڵ��ڴ�ռ�
		head->next = pnode;

		OS_EXIT_CRITICAL();	
		return true;
	}

	/****************************************************************************
	 * ���   :	�������������
	 * ����   :
	 * ����ֵ :
	 * ����   :
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
