
/*
 * Ercos interface to the hal
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

#ifndef _PUBLIC_ERCOS_LAH_H_
#define _PUBLIC_ERCOS_LAH_H_

#include <private/ercos-hal.h>

#include <public/glue.h> /* DEBUG & TRACE */
#include <public/posix.h>

#include <ercos/kernel/main.h>
#include <ercos/kernel/sched.h>
#include <ercos/kernel/traps.h>
#include <ercos/kernel/irqev.h>
#include <ercos/kernel/panic.h>
#include <ercos/kernel/task.h>

/* pointer to the kernel stack base */
extern const void * ercos_lah_stack_base;

/* pointer to the kernel stack top */
extern const void * ercos_lah_stack_top;

/* pointer to the schedule_active variable */
extern volatile uint32_t * ercos_lah_schedule_active;

/* returns a pointer to the hardware context structure in the tcb of the
 * current task */
static inline ercos_hal_hwcontext_t * ercos_lah_current (void) {
	return &current->context;
}

/* returns a pointer to the hardware context structure in the tcb of the
 * previous current task */
static inline ercos_hal_hwcontext_t * ercos_lah_prev_current (void) {
	return &old_current->context;
}

/* ercos entry point */
static inline void ercos_lah_main (void) {
	OS_Start ();
}

/* interrupt entry point */
static inline void ercos_lah_intr_handler (int vector) {
	OS_TrapEntry (vector);
}

/* kernel panic */
static inline void ercos_lah_panic (const char * str) {
	OS_KernelPanic (str);
}

/* interrupt epilogue */
static inline void ercos_lah_intr_epilogue (void) {

	/* interrupts are enabled; insert the current task into the the ready
	 * queue */
	OS_SchedCurrentReady ();

	/* flush pending events */
	OS_IRQHandleEvent ();

	/* call the scheduler */
	OS_Schedule ();
}

#endif

