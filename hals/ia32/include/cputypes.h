/*
 Copyright 2008 (c) by Alberto Carrasco Gallardo,
 University of Alcala,
 Space Research Group.

 For further information, please visit http://srg.aut.uah.es

 This software is provided under the terms of the GNU General Public v2
 Licence. A full copy of the GNU GPL is provided in the file COPYING
 found in the development root of ERCOS-RT.
 */

#ifndef IA32_CPUTYPES_H_
#define IA32_CPUTYPES_H_

#include <public/glue.h>


/******************************************************************************
 ******************		SYSTEM STRUCTURES		    ***********
 ******************************************************************************/
typedef struct ia32_context { //Context for processor exceptions

	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;

	uint32_t vector;
	// this 4 pushed by hardware(errcode sometimes) on stack for interrupts
	uint32_t errcode;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;

}__attribute__ ((packed)) ia32_context_t;

//typedef ia32_context_t interrupt_context_t;

typedef struct ia32_fpu_context {

	uint16_t CtrlW, _CtrlW_h;
	uint16_t StatusW, _StatusW_h;
	uint16_t TagW, _TagW_h;
	uint32_t fpu_Data_Pointer;
	uint32_t fpu_Instruction_Pointer;
	uint32_t fpu_Last_Instr_OPcode;
	uint32_t Idunno;
	uint8_t ST0[10], ST1[10], ST2[10], ST3[10], ST4[10], ST5[10], ST6[10],
			ST7[10];

}__attribute__ ((packed)) ia32_fpu_context_t;

typedef ia32_fpu_context_t fpu_context_t;

typedef struct ia32_ext_tss { //!Extended TSS

	uint16_t next_tss, _nt_high;
	uint32_t esp0;
	uint16_t ss0, _ss0h;
	uint32_t esp1;
	uint16_t ss1, _ss1h;
	uint32_t esp2;
	uint16_t ss2, _ss2h;
	uint32_t cr3, eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	uint16_t es, _esh;
	uint16_t cs, _csh;
	uint16_t ss, _ssh;
	uint16_t ds, _dsh;
	uint16_t fs, _fsh;
	uint16_t gs, _gsh;
	uint16_t ldt, _ldth;
	uint16_t trace, iomap;
	uint32_t index;
	uint8_t has_used_fpu;
	ia32_fpu_context_t fpu_ctx;

}__attribute__ ((packed)) ia32_ext_tss_t;

typedef struct ia32_tss { //just for IRQ's... that cannot use fpu at all

	uint16_t next_tss, _nt_high;
	uint32_t esp0;
	uint16_t ss0, _ss0h;
	uint32_t esp1;
	uint16_t ss1, _ss1h;
	uint32_t esp2;
	uint16_t ss2, _ss2h;
	uint32_t cr3, eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	uint16_t es, _esh;
	uint16_t cs, _csh;
	uint16_t ss, _ssh;
	uint16_t ds, _dsh;
	uint16_t fs, _fsh;
	uint16_t gs, _gsh;
	uint16_t ldt, _ldth;
	uint16_t trace, iomap;
	uint32_t index;

}__attribute__ ((packed)) ia32_tss_t;

//typedef ia32_tss_t irq_context_t;

typedef struct ia32_tss_descriptor {

	uint16_t limit_low;
	uint16_t base_low;
	uint16_t flags; //see fig 6-3 Intel system programming guide
	uint16_t base_high;

}__attribute__ ((packed)) ia32_tss_desc_t;

typedef struct ia32_task_register {

	uint16_t limit, base_lo;
	uint16_t base_hi, sel;

}__attribute__ ((packed)) ia32_task_reg_t;

typedef struct ia32_generic_descriptor {

	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t atrib_A;
	uint8_t atrib_B; //4 msb atrib  & 4 lsb limit_high
	uint8_t base_high;

}__attribute__ ((packed)) ia32_descriptor_t;

//static ia32_task_reg_t Task_reg;

#endif /*IA32_CPUTYPES_H_*/
