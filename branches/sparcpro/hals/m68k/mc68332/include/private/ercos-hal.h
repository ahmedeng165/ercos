
/*
 * Generic MC68332 board HAL for ERCOS-RT -- component interface
 *
 * Copyright 2008 (c) by Cesar Rodriguez Ortega,
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

#include <public/config.h> /* public defines */

#include <ercos-m68k.h> 	/* m68k related functionality */
#include <mc68332/time.h> /* timing related macros */
#include <mc68332/crt0.h> /* trap calling functions */
#include <mc68332/misc.h> /* miscellaneous code and intr. nesting variable */

/* number of us between two consecutive (simulated) timer interrupts (32 bit value) */
#define ERCOS_HAL_PIT_PERIOD (TIME_PIT_PERIOD * TIME_PIT_RELOAD)

/* minimum number of us that the clock timer can measure (32 bit value) */
#define ERCOS_HAL_CLOCK_RESOL (TIME_PIT_PERIOD)

/* interrupt number for the timer */
#define ERCOS_HAL_PIT_INTR (TIME_PIT_INTR)

/* fictitious interrupt number for the clock */
#define ERCOS_HAL_CLOCK_INTR (ERCOS_HAL_MC68332_CLOCK_INTR)

/* size of the interrupt table */
#define ERCOS_HAL_MAX_INTR 128

typedef m68k_hwcontext_t ercos_hal_hwcontext_t;

void ercos_hal_reset_entry (void); /* in src/crt0.S */

static inline void ercos_hal_hwcontext_init (ercos_hal_hwcontext_t * ctx,
		void * stackbase, uint32_t stacksize, uint32_t level,
		void * entry) {

	return m68k_hwcontext_init (ctx, stackbase, stacksize, level,
			entry);
}

static inline void ercos_hal_dispatch (void) {
	mc68332_dispatch ();
}

static inline void ercos_hal_power_down (void) {
	/*
	 * Interesting manual sections:
	 * 4.3.4 Low-Power Operation
	 * 5.8.2.1 Low-Power Stop
	 * 6.2.1.1 Low-Power Stop Operation from QSM
	 * 8.7 Low-Power Stop Operation
	 */
	mc68332_trap3 ();
}

static inline void ercos_hal_pit_init (void) {
	mc68332_pit_init ();
}

static inline void ercos_hal_clock_init (uint32_t us) {
	mc68332_clock_init (us);
}

static inline uint32_t ercos_hal_pit_read (void) {
	return mc68332_pit_read ();
}

static inline void ercos_hal_intr_disable (void) {
	mc68332_trap2 ();
}

static inline void ercos_hal_intr_enable (void) {
	mc68332_trap1 ();
}

static inline void ercos_hal_intr_mask (uint32_t nr) {
	/* nothing can be done here */
}

static inline void ercos_hal_intr_umask (uint32_t nr) {
	/* nothing can be done here */
}

static inline void ercos_hal_intr_clear (uint32_t nr) {
	/* nothing can be done here */
}

static inline bool_t ercos_hal_intr_ongoing (void) {
	return (bool_t) mc68332_intr_nesting;
}

/* #if 0 */
static inline void ercos_hal_uart_tx (uint8_t c) {
	mc68332_uart_tx (c);
}

static inline uint8_t ercos_hal_uart_rx (void) {
	return mc68332_uart_rx ();
}
/* #endif */

#endif

