/*
 * cpu.h created for Ercos_cmps project.
 *
 *  Author: Alberto Carrasco Gallardo, 2010 (c)
 *  Space Research Group,
 *  University of Alcala.
 *
 *  For further information, please visit http://srg.aut.uah.es
 *
 *  This software is provided under the terms of the GNU General Public v2
 *  Licence. A full copy of the GNU GPL is provided in the file COPYING
 *  found in the development root of ERCOS-RT project.
 *
 */

#ifndef CPU_H_
#define CPU_H_

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

#endif /* CPU_H_ */
