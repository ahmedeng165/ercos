
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

#include <public/config.h> /* DEBUG */
#include <public/ercos-lah.h> /* ercos interface to the hal */
#include <mc68332/intr.h> /* interrupt epilogue */
#include <mc68332/misc.h> /* intr. nesting variable */

volatile long mc68332_intr_nesting;

void mc68332_intr_handler (unsigned long ident, struct m68k_hwcontext * ctx) {
	ident &= 0x0fff;
	ident >>= 2;

	DEBUG ("*** ident=%p ctx->pc=%p ctx->sr=%p sr=%p", ident, ctx->pc, ctx->sr, m68k_get_sr ());

	/* execute the architecture-independent handler */
	if (ident == 5 || ident == 10 || ident == 11 || ident == 3 || ident == 8) {
		TRACE (ident, "d");
		TRACE (ctx->pc, "p");
		TRACE (ctx->sr, "p");
		TRACE (m68k_get_sr (), "p");
	}

	/* call architecture-independent handler */
	m68k_atomic_inc (&mc68332_intr_nesting);
	ercos_lah_intr_handler (ident);
	m68k_atomic_dec (&mc68332_intr_nesting);

	/* perform the interrupt epilogue */
	mc68332_intr_epilogue (ctx);
}

