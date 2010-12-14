
/*
 * Generic MC68332 board HAL for ERCOS-RT -- time related macros
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

#ifndef _ERCOS_HAL_MC68332_TIME_H_
#define _ERCOS_HAL_MC68332_TIME_H_

#include <public/config.h>

/* see mc68332_pit_init in src/time.c for more details about the next macros */
#define TIME_PIT_MODULUS (CONFIG_ERCOS_CLICKS_PER_TICK * 8192 / 1000000)
#define TIME_PIT_PERIOD (TIME_PIT_MODULUS * 1000000 / 8192)
#define TIME_PIT_RELOAD (0xffffffff / TIME_PIT_PERIOD)
//#define TIME_PIT_RELOAD (50)

#define TIME_PIT_INTR	0x1b
#define ERCOS_HAL_MC68332_CLOCK_INTR  0x2b /* mostly random */

extern volatile int mc68332_pit_interrupted;

void mc68332_pit_init (void);
uint32_t mc68332_pit_read (void);
void mc68332_clock_init (uint32_t us);

#endif

