
/*
 * Generic MC68332 board HAL for ERCOS-RT -- assembler functions
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

#ifndef _ERCOS_HAL_MC68332_CRT0_H_
#define _ERCOS_HAL_MC68332_CRT0_H_

#include <public/config.h>

void ercos_hal_reset_entry (void);
void mc68332_reset_entry (void);
void mc68332_trap0 (void);
static inline void mc68332_trap1 (void) {
	asm ("trap #1");
}

static inline void mc68332_trap2 (void) {
	asm ("trap #2");
}

void mc68332_trap3 (void);
void mc68332_trap4 (char c);
char mc68332_trap5 (void);
void mc68332_lpstop (void);

uint32_t mc68332_getsp (void);
uint16_t mc68332_oc_tpu_read (void);

#endif

