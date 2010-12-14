
/*
 * Generic MC68332 board HAL for ERCOS-RT -- mainly initialization code
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

#ifndef _ERCOS_HAL_MC68332_MISC_H_
#define _ERCOS_HAL_MC68332_MISC_H_

extern volatile long mc68332_intr_nesting;

void mc68332_dispatch (void);
void mc68332_uart_tx (char ch);
char mc68332_uart_rx (void);

#endif

