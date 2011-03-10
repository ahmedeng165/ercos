
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

#ifndef _ERCOS_HAL_MC68332_TRAP_H_
#define _ERCOS_HAL_MC68332_TRAP_H_

void mc68332_trap_dispatch (uint32_t ident, struct m68k_hwcontext * ctx);
void mc68332_trap_enable (uint32_t ident, struct m68k_hwcontext * ctx);
void mc68332_trap_disable (uint32_t ident, struct m68k_hwcontext * ctx);
void mc68332_trap_lowpower (uint32_t ident, struct m68k_hwcontext * ctx);
void mc68332_trap_tx (uint32_t ident, struct m68k_hwcontext * ctx);

#endif

