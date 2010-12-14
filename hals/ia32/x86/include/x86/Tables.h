/*
 * i386 system tables functions declarations and macros for ERCOS-RT
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
#ifndef TABLES_H_
#define TABLES_H_

#include <public/glue.h>


//! Number of architecture defined interrupts: 256
#define IA32_INTERRUPT_NR		256
//! Number of architecture GDT entries
#define	IA32_GDT_ENTRY_NR		8192
//! Number of LDT usefull entries
#define IA32_LDT_ENTRY_NR		8192
//! Kernel Code segment descriptor number
#define IA32_CODE_SEG			0x08
//! Kernel Data segment descriptor number
#define IA32_DATA_SEG			0x10

//Special pointer which includes the limit: The max bytes taken up by the GDT-1
typedef struct gdt_ptr {

	uint16_t limit;
	uint32_t base;

} __attribute__ ((packed)) ia32_gdt_ptr;

/* Setup a descriptor in the Global Descriptor Table */
void gdt_set_entry(int num, unsigned long base, unsigned long limit,
		unsigned char access, unsigned char gran);

void gdt_install();

uint32_t gdt_get_free_entry();


/**********************************   IDT     *********************************/

// Defines an IDT entry (64bits)

typedef struct idt_entry {

	uint16_t base_lo;
	uint16_t sel; // Our kernel segment goes here!
	uint8_t always0; // This will ALWAYS be set to 0!
	uint8_t flags;
	uint16_t base_hi;

} __attribute__ ((packed)) ia32_idt_entry;

typedef struct idt_ptr {

	uint16_t limit; //16 bits limit
	uint32_t base; //32 bits base address

} __attribute__ ((packed)) ia32_idt_ptr;

//This function is used to set an entry in the IDT.
//void idt_set_entry(unsigned char num, unsigned long base, unsigned short sel,
//		unsigned char flags);

/* Installs the IDT */
void idt_install();

#endif /* TABLES_H_ */
