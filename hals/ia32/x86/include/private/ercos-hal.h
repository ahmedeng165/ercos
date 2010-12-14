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

#include <x86/crt0.h>
#include <x86/misc.h>		/* context init */
#include <x86/serial.h>		/* serial tx & rx */
#include <x86/time.h>
#include <x86/console/text_console.h>

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


typedef ia32_ext_tss_t ercos_hal_hwcontext_t;

void ercos_hal_reset_entry(void); /* in src/crt0.S */

static inline void ercos_hal_hwcontext_init(ercos_hal_hwcontext_t * ctx,
		void * stackbase, uint32_t stacksize, uint32_t level, void * entry) {

	ia32_context_init(ctx, stackbase, stacksize, level, entry);

}

static inline void ercos_hal_dispatch(void) {

	ia32_dispatch();
}

static inline void ercos_hal_power_down(void) {

	ia32_power_down();
}

static inline void ercos_hal_pit_init(void) {

	timer_config();
}

static inline void ercos_hal_clock_init(uint32_t us) {

	clock_init(us);
}

static inline uint32_t ercos_hal_pit_read(void) {

	return ia32_get_timer();
}

static inline void ercos_hal_intr_disable(void) {

	irq_disable();
}

static inline void ercos_hal_intr_enable(void) {

	irq_enable();
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

	return (bool_t) is_interrupted();
}

/* #if 0 */
static inline void ercos_hal_uart_tx(uint8_t c) {

	serial_tx(c);
//	text_console_putchar(c);
}

static inline uint8_t ercos_hal_uart_rx(void) {

	return serial_rx();
}
/* #endif */

#endif

