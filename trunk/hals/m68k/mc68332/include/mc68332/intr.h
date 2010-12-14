
/*
 * Generic MC68332 board HAL for ERCOS-RT -- interrupt management primitives
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

#ifndef _ERCOS_HAL_MC68332_INTR_H_
#define _ERCOS_HAL_MC68332_INTR_H_

#include <public/config.h> /* types & DEBUG */
#include <ercos-m68k.h> /* hwcontext struct. & other primitives */
#include <public/ercos-lah.h> /* ercos interface to the hal */
#include <mc68332/time.h> /* pit interrupted variable */
#include <mc68332/trap.h> /* trap dispatch primitive */

static inline void mc68332_intr_epilogue (struct m68k_hwcontext * ctx) {
	/* disable interrupts */
	m68k_intr_disable ();

	/* are we going to dispatch a task or the context of another
	 * interrupt? In the latter case, we just return, stating that the
	 * timer interrupt handler may have been interrupted */
	if ((ctx->sr & 0x0700) != 0x0000) {
		if ((ctx->sr & 0x0700) == 0x0300) mc68332_pit_interrupted = 1;
		/* DEBUG ("Another interrupt was interrupted. Au revoire!\n"); */
		return;
	}
	else {
		/* DEBUG ("This is the first level interrupt"); */
	}

	/* At this point, we will return to an interrupted task. Was it
	 * performing a system call? In that case we cannot flush
	 * events and we will return, clearing the timer_interrupted
	 * flag */
	if (*ercos_lah_schedule_active != 0) {
		mc68332_pit_interrupted = 0;
		/* DEBUG ("Scheduler is active: a system call is "
		   "going on. Au revoire!\n"); */
		return;
	}
	else {
		/* DEBUG ("There is not a system call going on"); */
	}

	/* Ok, we have to flush pending events, with interrupts enabled
	*/
	/* DEBUG ("Flushing pending events (with interrupts enabled)!"); */
	*ercos_lah_schedule_active = 1;
	m68k_intr_enable ();
	ercos_lah_intr_epilogue ();

	/* ercos_lah_intr_epilogue returns with interrupts disabled, so we
	 * don't need to disable it prior to update pit_interrupted */
	mc68332_pit_interrupted = 0;

	/* perform a context switch, in case that the current task was updated
	 */
	mc68332_trap_dispatch (0, ctx);
}

#endif

