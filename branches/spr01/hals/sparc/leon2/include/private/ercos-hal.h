/*
 * HAL for ERCOS-RT -- component interface
 *
 * Copyright 2009 (c) by Alberto Carrasco Gallardo,
 * Space Research Group,
 * University of Alcala.
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 * This software is provided under the terms of the GNU General Public v2
 * Licence. A full copy of the GNU GPL is provided in the file COPYING
 * found in the development root of ERCOS-RT project.
 */

#ifndef _PUBLIC_ERCOS_HAL_H_
#define _PUBLIC_ERCOS_HAL_H_

#include <public/glue.h> /* data types */

#include <arch.h>
#include <leon2/timer.h>
#include <leon2/serial.h>
#include <leon2/leon2.h>

extern void CPU_PowerDown(void);

/* number of us between two consecutive timer interrupts (16 bit value) */
#define ERCOS_HAL_PIT_PERIOD (TIME_PIT_PERIOD)

/* minimum number of us that the clock timer can measure (32 bit value) */
#define ERCOS_HAL_CLOCK_RESOL ((1000000/TIME_CLOCK_FREQ) + 1)

/* interrupt number for the timer */
#define ERCOS_HAL_PIT_INTR (TIME_PIT_INTR)

/* interrupt number for the clock */
#define ERCOS_HAL_CLOCK_INTR (TIME_CLOCK_INTR)

/* size of the interrupt table */
#define ERCOS_HAL_MAX_INTR 256


typedef CPU_context_t ercos_hal_hwcontext_t;


void ercos_hal_reset_entry(void); /* in src/crt0.S */

static inline void ercos_hal_hwcontext_init(ercos_hal_hwcontext_t * ctx,
		void * stackbase, uint32_t stacksize, uint32_t level, void * entry) {

	CPU_ContextInit(ctx, (uint8_t *)stackbase, stacksize, level, entry);
}

static inline void ercos_hal_dispatch(void) {

	CPU_Switch_Tasks();
}

static inline void ercos_hal_power_down(void) {

	CPU_PowerDown();
}

static inline void ercos_hal_pit_init(void) {

	CPU_TimerInit();
}

static inline void ercos_hal_clock_init(uint32_t us) {

	CPU_ClockInit(us);
}

static inline uint32_t ercos_hal_pit_read(void) {

	volatile uint32_t value;
	CPU_GetTimer(value);
	return value;

}

static inline void ercos_hal_intr_disable(void) {

	_CPU_DisableIRQ();
}

static inline void ercos_hal_intr_enable(void) {

	_CPU_EnableIRQ();
}

static inline void ercos_hal_intr_mask(uint32_t nr) {

	CPU_MaskInterrupt(nr);
}

static inline void ercos_hal_intr_umask(uint32_t nr) {

	CPU_EnableInterrupt(nr);
}

static inline void ercos_hal_intr_clear(uint32_t nr) {

	CPU_ClearInterrupt(nr);
}

static inline bool_t ercos_hal_intr_ongoing(void) {

	return (bool_t) CPU_IS_IN_TRAP_HANDLER();
}

/* #if 0 */
static inline void ercos_hal_uart_tx(uint8_t c) {

	CPU_SerialTx(c);
}

static inline void ercos_hal_uart_dbg_tx(uint8_t c) {

	CPU_SerialTxDebug(c);
}

static inline uint8_t ercos_hal_uart_rx(void) {

	int car;
	CPU_SerialRx((int *)&car);
	return (uint8_t)car;
}
/* #endif */

#endif

