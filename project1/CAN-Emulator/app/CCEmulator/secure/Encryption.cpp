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

#include <string.h>
#include <stm32f10x.h>
#include "Encryption.h"

/****************************************************************************
 * ���   :	��key��Ϊ��Կ����ptext�������Ĵ����ctext����
 * ����   :	ptext	���ĵ�ַ�����ĳ���Ϊ12�ֽڡ�
 * 			key		��Կ�����ȱ����ڶ���12�ֽڡ�
 * 			ctext	���ĵ�ַ���ɵ������ṩ�����뱣֤���㹻���12�ֽ����ݡ�
 * ����ֵ :	0		�ɹ�
 * 			-1		ʧ��
 * ����   :	�ú���Ŀǰ����ΪӲ��ID����ʹ�á�ptext��ctext�ĳ��Ⱦ�Ϊ12�ֽڡ�
 ****************************************************************************/
int Encryption::encrypt(u32 *ptext, char *key, u32 *ctext)
{
	char _key[ACTIVATE_ID_LEN];
	int i;
	int keyLen = strlen(key);

	if (keyLen > ACTIVATE_ID_LEN)
		return -1;

	// �ӳ�key��12�ֽ�
	for (i = 0; i < ACTIVATE_ID_LEN; i++) {
		int j = i % keyLen;
		_key[i] = key[j];
	}

	for (i = 0; i < 3; i++)
		ctext[i] =	ptext[i] ^ ((u32 *)_key)[i];

	return 0;
}

/****************************************************************************
 * ���   :	��key��Ϊ��Կ����ctext�������Ĵ����ptext����
 * ����   :	ctext	���Ĵ�ŵ�ַ�����ĳ���Ϊ12�ֽڡ�
 * 			key		��Կ�����ȱ����ڶ���12�ֽڡ�
 * 			ptext	���ĵ�ַ���ɵ������ṩ�����뱣֤�ܴ��12�ֽ����ݡ�
 * ����ֵ :	0		�ɹ�
 * 			����	ʧ��
 * ����   :	�ú���Ŀǰ����ΪӲ��ID����ʹ�á�ptext��ctext�ĳ��Ⱦ�Ϊ12�ֽڡ�
 ****************************************************************************/
int Encryption::decrypt(u32 *ctext, char *key, u32 *ptext)
{
	char _key[ACTIVATE_ID_LEN];
	int i;
	int keyLen = strlen(key);

	if (keyLen > ACTIVATE_ID_LEN)
		return -1;

	// �ӳ�key��12�ֽ�
	for (i = 0; i < ACTIVATE_ID_LEN; i++) {
		int j = i % keyLen;
		_key[i] = key[j];
	}

	for (i = 0; i < 3; i++)
		ptext[i] =	ctext[i] ^ ((u32 *)_key)[i];

	return 0;
}
