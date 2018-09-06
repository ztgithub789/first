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
 * 简介   :	用key作为密钥加密ptext，把密文存放于ctext处。
 * 参数   :	ptext	明文地址。明文长度为12字节。
 * 			key		密钥。长度必须在短于12字节。
 * 			ctext	密文地址，由调用者提供，必须保证能足够存放12字节内容。
 * 返回值 :	0		成功
 * 			-1		失败
 * 描述   :	该函数目前仅作为硬件ID加密使用。ptext，ctext的长度均为12字节。
 ****************************************************************************/
int Encryption::encrypt(u32 *ptext, char *key, u32 *ctext)
{
	char _key[ACTIVATE_ID_LEN];
	int i;
	int keyLen = strlen(key);

	if (keyLen > ACTIVATE_ID_LEN)
		return -1;

	// 延长key至12字节
	for (i = 0; i < ACTIVATE_ID_LEN; i++) {
		int j = i % keyLen;
		_key[i] = key[j];
	}

	for (i = 0; i < 3; i++)
		ctext[i] =	ptext[i] ^ ((u32 *)_key)[i];

	return 0;
}

/****************************************************************************
 * 简介   :	用key作为密钥解密ctext，把明文存放于ptext处。
 * 参数   :	ctext	密文存放地址，密文长度为12字节。
 * 			key		密钥。长度必须在短于12字节。
 * 			ptext	明文地址。由调用者提供，必须保证能存放12字节内容。
 * 返回值 :	0		成功
 * 			其他	失败
 * 描述   :	该函数目前仅作为硬件ID解密使用。ptext，ctext的长度均为12字节。
 ****************************************************************************/
int Encryption::decrypt(u32 *ctext, char *key, u32 *ptext)
{
	char _key[ACTIVATE_ID_LEN];
	int i;
	int keyLen = strlen(key);

	if (keyLen > ACTIVATE_ID_LEN)
		return -1;

	// 延长key至12字节
	for (i = 0; i < ACTIVATE_ID_LEN; i++) {
		int j = i % keyLen;
		_key[i] = key[j];
	}

	for (i = 0; i < 3; i++)
		ptext[i] =	ctext[i] ^ ((u32 *)_key)[i];

	return 0;
}
