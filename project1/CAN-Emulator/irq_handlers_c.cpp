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

//#include <stdio.h>
#include "irq_handlers.h"
#include <zdebug.h>
void HardFault_Handler_c(unsigned int * hardfault_args)
{
#if 1 //def ZDEBUG
	unsigned int stacked_r0;
	unsigned int stacked_r1;
	unsigned int stacked_r2;
	unsigned int stacked_r3;
	unsigned int stacked_r12;
	unsigned int stacked_lr;
	unsigned int stacked_pc;
	unsigned int stacked_psr;

	stacked_r0 = ((unsigned long) hardfault_args[0]);
	stacked_r1 = ((unsigned long) hardfault_args[1]);
	stacked_r2 = ((unsigned long) hardfault_args[2]);
	stacked_r3 = ((unsigned long) hardfault_args[3]);

	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr = ((unsigned long) hardfault_args[5]);
	stacked_pc = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);
	printf ("[Hard fault handler]\n");
	printf ("R0 = %x\n", stacked_r0);
	printf ("R1 = %x\n", stacked_r1);
	printf ("R2 = %x\n", stacked_r2);
	printf ("R3 = %x\n", stacked_r3);
	printf ("R12 = %x\n", stacked_r12);
	printf ("LR = %x\n", stacked_lr);
	printf ("PC = %x\n", stacked_pc);
	printf ("PSR = %x\n", stacked_psr);
	printf ("BFAR = %x\n", (*((volatile unsigned long *)(0xE000ED38))));
	printf ("CFSR = %x\n", (*((volatile unsigned long *)(0xE000ED28))));
	printf ("HFSR = %x\n", (*((volatile unsigned long *)(0xE000ED2C))));
	printf ("DFSR = %x\n", (*((volatile unsigned long *)(0xE000ED30))));
	printf ("AFSR = %x\n", (*((volatile unsigned long *)(0xE000ED3C))));
#endif

	__set_FAULTMASK(1);
	NVIC_SystemReset();
	while(1) {}
}
