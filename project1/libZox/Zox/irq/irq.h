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
#ifndef  BSP_PRESENT
#define  BSP_PRESENT


#include  <stm32f10x_conf.h>
//#include  <app_cfg.h>
#include  <ucos_ii.h>
#include  <irq_vect.h>

typedef void (*CPU_FNCT_VOID)(void);
typedef void (*isr_t)(void*);

// 中断向量
struct IrqVect {
	isr_t   isr;		// 中断处理函数
	void *data;			// 中断处理函数的参数
};


typedef  union {
	CPU_FNCT_VOID   Fnct;
	void           *Ptr;
} APP_INTVECT_ELEM;


extern IrqVect irqVectTbl[irq_SRC_NBR];


#ifdef __cplusplus
extern "C" {
#endif

void irqVectSet (INT32U int_id, isr_t isr, void *data = 0);
void irqInit    (void);
void irqHandler (INT32U  int_id);

#ifdef __cplusplus
}
#endif

#endif                                                          /* End of module include.                               */
