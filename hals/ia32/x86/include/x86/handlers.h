/*
 * i386 handlers functions declarations for ERCOS-RT
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

#ifndef HANDLERS_H_
#define HANDLERS_H_

#include <public/glue.h>

#include <cputypes.h>
#include <x86/misc.h>
#include <asm.h>
#include <public/posix.h> /* printf */
#include <public/ercos-lah.h>

extern void irq_disable(void);
extern void irq_enable(void);

extern volatile long ia32_intr_nesting;

void ia32_interrupt_handlers(ia32_context_t *ctx);
void handler_irq48(void);
void handler_irq32(void);
//void register_dump(ia32_context_t *ctx);

static inline void ia32_intr_epilogue() {
	/* interrupts should be already disabled by the processor */

	--ia32_intr_nesting;

	/* are we going to dispatch a task or the context of another
	 * interrupt? In the latter case, we just return, stating that the
	 * timer interrupt handler may have been interrupted */
	if (is_interrupted()) {
		DEBUG ("Another interrupt was interrupted. Au revoire!\n");
		return;
	} else {
		//DEBUG ("This is the first level interrupt");
	}

	/* At this point, we will return to an interrupted task. Was it
	 * performing a system call? In that case we cannot flush
	 * events and we will return, clearing the timer_interrupted
	 * flag */
	if (*ercos_lah_schedule_active != 0) {

			/*DEBUG ("Scheduler is active: a system call is "
			   "going on. Au revoire!\n");*/
		return;
	} else {
		// DEBUG ("There is not a system call going on");
	}

	/* Ok, we have to flush pending events, with interrupts enabled
	 */
	/* DEBUG ("Flushing pending events (with interrupts enabled)!"); */
	*ercos_lah_schedule_active = 1;

	irq_enable();

	ercos_lah_intr_epilogue();

	/* perform a context switch, in case that the current task was updated
	 */
	ia32_dispatch();
}

static inline void ia32_intr_prologue() {

	++ia32_intr_nesting;

	//Stack switch??????????????
}

#endif /* HANDLERS_H_ */
