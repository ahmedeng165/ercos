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

/*eflags*/
#define EFLAGS_INIT				0x00000002			//Initial Value for new tasks
#define EFLAGS_CF_MASK			0x00000001			// Carry Flag
#define EFLAGS_PF_MASK			0x00000004			// Parity Flag
#define EFLAGS_AF_MASK			0x00000010			// Auxiliary Carry Flag
#define EFLAGS_ZF_MASK			0x00000040			// Zero Flag
#define EFLAGS_SF_MASK			0x00000080			// Sign Flag
#define EFLAGS_TF_MASK			0x00000100			// Trap Flag
#define EFLAGS_IF_MASK			0x00000200			// Interrupt Enable Flag
#define EFLAGS_DF_MASK			0x00000400			// Direction Flag
#define EFLAGS_OF_MASK			0x00000800			// Overflow Flag
#define EFLAGS_IOPL_MASK		0x00003000			// I/O Privilege Level
#define EFLAGS_NT_MASK			0x00004000			// Nested Task
#define EFLAGS_RF_MASK			0x00010000			// Resume Flag
#define EFLAGS_VM_MASK			0x00020000			// Virtual-8086 Mode
#define EFLAGS_AC_MASK			0x00040000			// Alignment Check
#define EFLAGS_VIF_MASK			0x00080000			// Virtual Interrupt Flag
#define EFLAGS_VIP_MASK			0x00100000			// Virtual interrupt pending
#define EFLAGS_ID_MASK			0x00200000			// ID Flag
/*gdt*/
//	ACCESS:			P / DPL(2) / S / TYPE(4)
// 	GRANULARITY		G / (D/B) / L / AVL / segment limit(19:16)
/***************************** ACCESS BITS ************************************/
#define GDT_SYS_SEG				0x00
#define GDT_COD_SEG				0x10			//bit S
#define	GDT_DPL_3				0x60
#define GDT_DPL_0				0x00
#define	GDT_SEG_P				0x80			//bit P
#define GDT_SEG_NP				0x00
#define GDT_DATA_WR				0x02			// TYPE data segment r/w
#define GDT_DATA_WR_SD			0x06			// data segment r/w expand down
#define GDT_DATA_R				0x00
#define GDT_DATA_R_SD			0x04
#define GDT_CODE_EXEC			0x08
#define GDT_CODE_EXEC_R			0x0A
#define GDT_CODE_CONFORMING		0x04
#define GDT_CODE_NO_CONFORMING	0x00

/***************************** GRANULARITY BITS *******************************/
#define GDT_GRANULARITY			0x80			// segmentos 4Gb
#define GDT_DB_CODE				0x40			// direcciones de 32bits
#define GDT_DB_STACK			0x40			//
#define GDT_DB_DATA				0x40			// 4Gb
#define GDT_AVL_SYS				0x10

/************************** SYSTEM DESCRIPTOR TYPES ******************************/
#define	GDT_TYPE_LDT			0x02
#define GDT_TYPE_TSK_GATE		0x05
#define GDT_TYPE_TSS			0x09
#define GDT_TYPE_INT_GATE		0x0E
#define	GDT_TYPE_TRAP_GATE		0x0F
#define GDT_TYPE_CALL_GATE		0x0C

/*idt*/
#define IDT_INT_GATE_FLAGS		0x8E
#define IDT_TRAP_GATE_FLAGS		0x8F
#define IDT_TASK_GATE_FLAGS		0x85

#ifndef NULL
#define NULL      (0)
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

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
