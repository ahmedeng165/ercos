/*
 * i386 system tables functions for ERCOS-RT
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

#include <public/glue.h>
#include <public/posix.h> /* printf */
#include <x86/Tables.h>
#include <x86/x86.h>
#include <x86/serial.h>
#include <arch.h>
#include <x86/handlers.h>
#include <ercos/lib/string.h>

#define HANDLER(vector)	vec0 + ((vec1 - vec0)*vector)
#define IRQ_STACK_POINTER(vector) extern uint32_t irq##vector##_stack
#define STUB_IRQ(vector) extern void stub_handler##vector()

/*extern handler for exception 0 from crt0.S*/
extern void vec0(void);
extern void vec1(void);
/* extern pointer to IDT table from crt0.S */
extern ia32_idt_entry ia32_idt_table[IA32_INTERRUPT_NR];
static ia32_idt_ptr ip;

/* extern stubs for IRQ handlers (same numbers than in crt0.S)*/
STUB_IRQ(32);
STUB_IRQ(48);

/* memory reserved for irq stacks */
extern char hal_irq_stack[HAL_IRQ_STACK_TOTAL_SIZE];

/* extern irqs stack parameters from crt0.S*/
IRQ_STACK_POINTER(32);
IRQ_STACK_POINTER(48);

/* extern FPU var. Stores the id of the last task that used FPU.
 * it does Have to be initialized to IDLE(1)*/
extern volatile uint32_t FPU_Last_Task;

static ia32_descriptor_t gdt[IA32_GDT_ENTRY_NR];
static ia32_gdt_ptr gp;

/*kernel TSS*/
static ia32_ext_tss_t k_TSS;
static ia32_task_reg_t k_TSS_selector;

/*******	Declaracion de funciones	******/

/* \todo This function is just for debug purpose, and HAVE to be ERASED later*/
static void dump_gdt(uint32_t base, uint16_t limit) {

	uint8_t mem;
	uint16_t i;

	printf("dump de GDT entry recently created:\n");
	for (i = 0; i < limit; i++) {
		mem = readb((void *) (base + i));
		printf("[Byte %d]0x%x\n",(i%8), mem);
	}
	return;
}

/*
 * This function load the GDT register with a GDT pointer
 */
static inline void gdt_load() {

	asm volatile("lgdt %0 \n"
			"ljmp $8, $1f \n" //set eip
			"1:  \n" : :"m" (gp));

}

/*
 * This function write the fields of a GDT entry with the info passed as parameters
 * parameters:
 * 	-num: is the index inside the table
 *  -base: base address of the segment
 *  -limit: limit for the segment
 *  -access: access bits
 *  -gran: granularity bits
 *
 *  SEE the format of this sort of descriptor going through Intel documentation
 */
void gdt_set_entry(int num, unsigned long base, unsigned long limit,
		unsigned char access, unsigned char gran) {

	/* Setup the descriptor base address */
	gdt[num].base_low = (uint16_t)(base & 0xFFFF);
	gdt[num].base_middle = (uint8_t)((base >> 16) & 0xFF);
	gdt[num].base_high = (uint8_t)((base >> 24) & 0xFF);

	/* Setup the descriptor limits */
	gdt[num].limit_low = (uint16_t)(limit & 0xFFFF);
	gdt[num].atrib_B = (uint8_t)((limit >> 16) & 0x0F);

	/* Finally, set up the granularity and access flags */
	gdt[num].atrib_B |= (gran & 0xF0);
	gdt[num].atrib_A = access;

#ifndef CONFIG_NDEBUG
//	printf("direccion del contexto: 0x%x\n",  base);
//	dump_gdt((gp.base+(num*8)), 8);
#endif
	return;
}
/*
 * This function calculate the next free index into the GDT
 */
uint32_t gdt_get_free_entry() {

	uint32_t i;

	for (i = 3; i < IA32_GDT_ENTRY_NR; i++) {

		if ((gdt[i].atrib_A & 0x80) == 0)
			break;

	}

	return i;
}
/*
 * This function is called from main after power up. This function writes the NULL(mandatory)
 * and code and data segments descriptors used by all task. Also is written the kernel
 * TSS into the Table. This table is updated every time a task is created, writing the task
 * context(TSS) in the next free entry of the table. At the end of the function the GDT and
 * the Task Register are loaded.
 */
void gdt_install() {
	/* Setup the GDT pointer and limit */

	gp.limit = (uint16_t)(8 * IA32_GDT_ENTRY_NR) - 1;
	gp.base = (uint32_t) &gdt[0];

	/* 			NULL descriptor */
	gdt_set_entry(0, 0, 0, 0, 0);

	/*			Code segment	*/
	gdt_set_entry(1, 0, 0xFFFFFFFF, (0x10 | GDT_DPL_0 | GDT_SEG_P
			| GDT_CODE_EXEC_R | GDT_CODE_NO_CONFORMING), (GDT_GRANULARITY| GDT_DB_CODE | GDT_AVL_SYS));

	/*			Data segment	*/
	gdt_set_entry(2, 0, 0xFFFFFFFF, (0x10 | GDT_DPL_0 | GDT_SEG_P
			| GDT_DATA_WR), (GDT_GRANULARITY| GDT_DB_DATA | GDT_AVL_SYS));

	gdt_set_entry(3, (unsigned long) &k_TSS, (sizeof(ia32_ext_tss_t) - 1),
			(GDT_DPL_0| GDT_SEG_P | GDT_TYPE_TSS | GDT_SYS_SEG), 0x00);

	/* Flush out the old GDT and install the new changes! */
	gdt_load();

	k_TSS.index = 3;
	k_TSS_selector.sel = 3 * 8;
	ltr(k_TSS_selector.sel);


}

/**********************************   IDT     *********************************/
/*
 * This function write the fields of a idt entry with the info passed as parameters
 * parameters:
 * 	-num: is the index inside the table
 *  -base: base address of the idt table
 *  -sel: selector for the code segment
 *  -flags: access bits and type of interrupt bits
 */
static void idt_set_entry(unsigned char num, unsigned long base, unsigned short sel,
		unsigned char flags) {

	ia32_idt_table[num].base_lo = (uint16_t)(base & 0xFFFF);
	ia32_idt_table[num].always0 = 0x00;
	ia32_idt_table[num].flags = flags;
	ia32_idt_table[num].sel = sel;
	ia32_idt_table[num].base_hi = (uint16_t)((base >> 16) & 0xFFFF);

	return;
}

/*
 * This function load the idt register with a idt pointer
 */
static inline void idt_load() {

	asm volatile("lidt %[idtr] \n" : :[idtr]"m" (ip));
}

/*
 * This function is called from main after power up, the interrupt vectors table is
 * all zeros since it is created with asm directive .fill 256,8,0 in crt0.S
 */
void idt_install() {

	uint32_t i;

	ip.base=(uint32_t)&ia32_idt_table[0];
	ip.limit=(uint16_t)(8*IA32_INTERRUPT_NR) -1;


	FPU_Last_Task = 1;

	irq48_stack = (uint32_t) ((char *) &hal_irq_stack[0] + CONFIG_ERCOS_IRQ_STACK_SIZE
			- 1);
	irq32_stack = (uint32_t) ((char *) &hal_irq_stack[0] + (CONFIG_ERCOS_IRQ_STACK_SIZE*2)
			- 1);

	for(i=0; i<IA32_INTERRUPT_NR; i++){
		idt_set_entry(i, (unsigned long) ((void *) HANDLER(i)), IA32_CODE_SEG,
			IDT_INT_GATE_FLAGS);
	}

	idt_set_entry(TIME_PIT_INTR, (unsigned long) ((void *) stub_handler48), IA32_CODE_SEG,
				IDT_INT_GATE_FLAGS);

	idt_set_entry(TIME_CLOCK_INTR, (unsigned long) ((void *) stub_handler32), IA32_CODE_SEG,
				IDT_INT_GATE_FLAGS);

	idt_load();

	return;
}
