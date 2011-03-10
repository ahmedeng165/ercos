/*
 * i386 miscellaneous functions declarations for ERCOS-RT
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
#ifndef MISC_H_
#define MISC_H_


#include <arch.h>
#include <x86/Tables.h>
#include <x86/paging.h>
#include <x86/crt0.h>

#define VBase 	0xB000
#define PIC_WRITE8		outportb
#define PIC_READ8		inportb

void ia32_cstart(unsigned long proc_info,unsigned long magic, unsigned long addr);
void ia32_dispatch(void);
void ia32_context_init(ia32_ext_tss_t * ctx, void * stackbase,
							uint32_t stacksize, uint32_t level, void * entry);
uint32_t is_interrupted();
void ia32_power_down();
void CPU_MaskInterrupt(uint32_t vector);
void CPU_MaskAllInterrupts();
int CPU_IsInterrupt(uint32_t vec);
void CPU_ClearInterrupt(uint32_t vector);
void CPU_EnableInterrupt(uint32_t vector);


#endif /* MISC_H_ */
