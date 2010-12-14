
/*
 * Generic MC68332 board HAL for ERCOS-RT -- fine timing primitives
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


#ifndef _ERCOS_HAL_MC68332_OC_H_
#define _ERCOS_HAL_MC68332_OC_H_

#include <mc68332/crt0.h>

void mc68332_oc_init (void);
void mc68332_oc_latch (void);
uint32_t mc68332_oc_read (void);

#endif

