
/*
 * M68K cpu helper stuff to implement a HAL for ERCOS -- component interface
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

#ifndef _PUBLIC_ERCOS_M68K_H_
#define _PUBLIC_ERCOS_M68K_H_

#include <public/glue.h>
#include <public/config.h> /* data types */

struct m68k_hwcontext {
	uint32_t usp;

	uint32_t a0;
	uint32_t a1;
	uint32_t a2;
	uint32_t a3;
	uint32_t a4;
	uint32_t a5;
	uint32_t a6;

	uint32_t d0;
	uint32_t d1;
	uint32_t d2;
	uint32_t d3;
	uint32_t d4;
	uint32_t d5;
	uint32_t d6;
	uint32_t d7;

	uint16_t sr;
	uint32_t pc;
};

typedef struct m68k_hwcontext m68k_hwcontext_t;

void m68k_hwcontext_init (m68k_hwcontext_t * ctx,
		void * stackbase, uint32_t stacksize, uint32_t level,
		void * entry);

uint16_t m68k_get_sr (void);
void m68k_atomic_inc (volatile long *i);
void m68k_atomic_dec (volatile long *i);
void m68k_intr_disable (void);
void m68k_intr_enable (void);
void m68k_intr_level (unsigned irl);

#endif

