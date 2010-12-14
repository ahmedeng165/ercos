
/*
 * Motorola cpu 68K helper code for the ERCOS HAL
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

#include <public/config.h> /* data types, DEBUG & ASSERT */
#include <public/posix.h> /* for printf */
#include <ercos-m68k.h>

void m68k_hwcontext_init (struct m68k_hwcontext * ctx, void * stackbase,
		uint32_t stacksize, uint32_t level, void * entry) {

	/* DEBUG ("Initializing hardware context: entry at 0x%p, stack at "
		"0x%p, 0x%p bytes", entry - (void *) task_table, stackbase, stacksize); */
	ASSERT (level <= 7);

	/* now simulate a trap from which we want to return */

	ctx->usp = (uint32_t) stackbase + stacksize;

	ctx->a6 = ctx->a5 = 0;
	ctx->a4 = ctx->a3 = 0;
	ctx->a2 = ctx->a1 = 0;
	ctx->a0 = 0;

	ctx->d7 = ctx->d6 = 0;
	ctx->d5 = ctx->d4 = 0;
	ctx->d3 = ctx->d2 = 0;
	ctx->d1 = ctx->d0 = 0;

	ctx->sr = 0x0000 | level << 8;
	ctx->pc = (uint32_t) entry;
}

