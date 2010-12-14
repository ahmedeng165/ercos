/*
 * i386 function and constants declarations of C runtime code for ERCOS-RT
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
#ifndef _CRT0_H_
#define _CRT0_H_


//#ifndef ASM

void startup (void);
void page_enable(void);
void pmode_enable(void);
void paging_disable(void);
void ia32_interrupt_entry (void);
void irq_enable(void);
void irq_disable(void);


/*kernel init stack pointer*/
extern	const void * ercos_lah_stack_top;
extern	const void * ercos_lah_stack_base;

/* defined in the linker script */
extern unsigned CODE_START;
extern unsigned CODE_END;
extern unsigned DATA_START;
extern unsigned DATA_END;
extern unsigned bss_start;
extern unsigned end;

//#endif

#endif
