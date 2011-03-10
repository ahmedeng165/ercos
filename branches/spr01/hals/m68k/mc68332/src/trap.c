
/*
 * Generic MC68332 board HAL for ERCOS-RT -- trap handlers
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

#include <ercos-m68k.h> /* public interface */
#include <public/ercos-lah.h> /* public interface */
#include <mc68332/trap.h> /* this file interface */
#include <mc68332/sim.h> /* SIM macros */
#include <mc68332/crt0.h> /* assembler primitives */

#include <public/config.h> /* DEBUG */
#include <public/posix.h> /* printf */

/* ** internal variables ** */
static unsigned _mc68332_sup_ipl_cache = 0;
static unsigned _mc68332_usr_ipl_cache = 0;
/* ** end of internal variables ** */

/* The mc68332_trap_dispatch function stores the (deliberately) interrupted
 * task in a context structure and dispatch the context of another task.
 * The pointers to the context of the interrupted task and the dispatched (new)
 * task are passed to the function through d0 and d1 registers */
void mc68332_trap_dispatch (uint32_t ident, struct m68k_hwcontext * ctx) {
	register struct m68k_hwcontext * curr;
	register struct m68k_hwcontext * prev;

	/* retrieve the pointers to the hardware context structures */
	curr = ercos_lah_current ();
	prev = ercos_lah_prev_current ();

	/* nothing to do if both task are the same */
	if (curr == prev) return;

	/* store the context of the (deliberately) interrupted task in prev */
	*prev = *ctx;

	/* dispatch the task pointed by curr */
	*ctx = *curr;
}

void mc68332_trap_enable (uint32_t ident, struct m68k_hwcontext * ctx) {

	/* if irqs are already enabled, nothing to do */
	if ((ctx->sr & 0x0700) != 0x0700) {
		/* DEBUG ("Interrupts already enabled. Nothing to do");
		 */
		return;
	}

	ctx->sr &= 0xf8ff;
	if (ctx->sr & 0x2000) {
		ctx->sr |= _mc68332_sup_ipl_cache & 0x0700;
	} else {
		ctx->sr |= _mc68332_usr_ipl_cache & 0x0700;
	}
	/* DEBUG ("Restoring sr to 0x%p", ctx->sr); */
}

void mc68332_trap_disable (uint32_t ident, struct m68k_hwcontext * ctx) {

	/* if irqs are already disabled, nothing to do */
	if ((ctx->sr & 0x0700) == 0x0700) {
		/* DEBUG ("Interrupts already disabled. Nothing to do");
		 */
		return;
	}

	/* in the opposite case, we store the current value of sr in
	 * _mc68332_{usr|sup}_ipl_cache and switch to interrupt level 7 on the
	 * next return of interrupt */
	if (ctx->sr & 0x2000) {
		_mc68332_sup_ipl_cache = ctx->sr & 0x0700;
		ctx->sr |= 0x0700;
		/* DEBUG ("Restoring sr to 0x%p, supervisor ipl cache is "
			"now 0x%p", ctx->sr, _mc68332_sup_ipl_cache); */
	} else {
		_mc68332_usr_ipl_cache = ctx->sr & 0x0700;
		ctx->sr |= 0x0700;
		/* DEBUG ("Restoring sr to 0x%p, user ipl cache is now 0x%p",
			ctx->sr, _mc68332_usr_ipl_cache); */
	}
}

void mc68332_trap_lowpower (uint32_t ident, struct m68k_hwcontext * ctx) {
	/*
	 * Interesting manual sections:
	 * 4.3.4 Low-Power Operation
	 * 5.8.2.1 Low-Power Stop
	 * 6.2.1.1 Low-Power Stop Operation from QSM
	 * 8.7 Low-Power Stop Operation
	 */
	// mc68332_lpstop ();

	/* FIXME -- !!! delete this code from here! */
	MC68332_SIM_WRITE8 (MC68332_SIM_SWSR, 0x55);
	MC68332_SIM_WRITE8 (MC68332_SIM_SWSR, 0xaa);
}

void mc68332_trap_tx (uint32_t ident, struct m68k_hwcontext * ctx) {
	uint16_t scsr;
	uint8_t ch = ctx->d0;

	/* wait for TDRE bit to be set */
	do {
		scsr = MC68332_QSM_READ16 (MC68332_QSM_SCSR);
	} while (! (scsr & 0x0100));

	/* now clear TDRE bit */
	MC68332_QSM_WRITE16 (MC68332_QSM_SCSR, scsr & ~0x0100);

	/* write data */
	MC68332_QSM_WRITE16 (MC68332_QSM_SCDR, ch);
}

void mc68332_trap_rx (uint32_t ident, struct m68k_hwcontext * ctx) {
	uint16_t scsr;

	/* wait for data: RDFR bit to be set */
	do {
		scsr = MC68332_QSM_READ16 (MC68332_QSM_SCSR);
	} while (! (scsr & 0x0040));

	/* read data */
	ctx->d0 = MC68332_QSM_READ16 (MC68332_QSM_SCDR) & 0xff;

	/* clear the RDFR bit */
	MC68332_QSM_WRITE16 (MC68332_QSM_SCSR, scsr & ~0x0040);

	if (ctx->d0 == 2) asm ("bra mc68332_reset_entry");
}

