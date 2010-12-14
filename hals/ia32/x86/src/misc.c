/*
 * i386 miscellaneous functions for ERCOS-RT
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

#include <public/glue.h> /* basic types */
#include <public/posix.h> /* printf */
#include <public/ercos-lah.h> /* current task and previous current task; lah_main */
#include <x86/misc.h>
#include <x86/Tables.h>
#include <x86/console/text_console.h>
#include <Multiboot.h>

/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

#define CPUID(eax, res)	\
		asm volatile("cpuid" : "=d" (res) : "a" (eax));

extern volatile long ia32_intr_nesting;

/*
 * This function is the first one executed after init asm code, and is in charge of initilize all the
 * system hardware and call the main in the kernel.
 */
void ia32_cstart(unsigned long proc_info, unsigned long magic, unsigned long addr) {

	multiboot_info_t *mbi;
	uint32_t aux;

	CPUID(1,aux);

	// let's start with C
//	text_console_basic_create ();
	serial_init();
	gdt_install();
	idt_install();

	init_paging();

	if(CHECK_FLAG(aux,9)){
		DEBUG("hay apic");
	}
	else{
		DEBUG("NO hay apic");
		ercos_lah_panic("Sin apic no puede funcionar el timer periodico... "
				"so we'd better not continue");
	}

	enable_APIC();
	pic_init();

#ifndef CONFIG_NDEBUG
		DEBUG("info del procesador pasada en EDX al inicio es: 0x%x", proc_info);
		mbi = (multiboot_info_t *) addr;

		DEBUG("comprobaciones Multiboot...\n");

		if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
			DEBUG("el numero magico no es correcto!!, vale: 0x%x\n",magic);
		else
			DEBUG("el numero magico es correcto!!, vale: 0x%x\n", magic);

		DEBUG("flags  y dirección de la cabecera multiboot: 0x%x  y 0x%x", mbi->flags, addr);

		/* Are mem_* valid? */
		if (CHECK_FLAG (mbi->flags, 0))
		DEBUG ("mem_lower = %uKB, mem_upper = %uKB\n",
		(unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);
		else
			DEBUG("Bit 0 de flags no esta activo...\n");

		/* Is the section header table of ELF valid? */
		if (CHECK_FLAG (mbi->flags, 5)) {

			elf_section_header_table_t *elf_sec = &(mbi->u.elf_sec);

			DEBUG ("elf_sec: num = %u, size = 0x%x,"
					" addr = 0x%x, shndx = 0x%x\n",
					(unsigned) elf_sec->num, (unsigned) elf_sec->size,
					(unsigned) elf_sec->addr, (unsigned) elf_sec->shndx);
		} else
			DEBUG("Bit 5 de flags no activo...\n");

		/* Are mmap_* valid? */
		if (CHECK_FLAG (mbi->flags, 6)) {

			memory_map_t *mmap;

			DEBUG ("mmap_addr = 0x%x, mmap_length = 0x%x\n",
					(unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
			for (mmap = (memory_map_t *) mbi->mmap_addr; (unsigned long) mmap
					< mbi->mmap_addr + mbi->mmap_length; mmap
					= (memory_map_t *) ((unsigned long) mmap + mmap->size
							+ sizeof(mmap->size)))
				DEBUG (" size = 0x%x, base_addr = 0x%x%x,"
						" length = 0x%x%x, type = 0x%x\n",
						(unsigned) mmap->size,
						(unsigned) mmap->base_addr_high,
						(unsigned) mmap->base_addr_low,
						(unsigned) mmap->length_high,
						(unsigned) mmap->length_low,
						(unsigned) mmap->type);
		} else
			DEBUG("Bit 6 de flags no activo...\n "
					"Fin de las comprobaciones Multiboot\n");

		DEBUG("Fin de las comprobaciones Multiboot\n");
	#endif

		DEBUG("Entrando al codigo del kernel...\n");

	//llamar al kernel

	ercos_lah_main();

}

/*
 * This function is the task's dispatcher
 */
void ia32_dispatch(void) {

	uint16_t sel[3];

	if (current == old_current)
		return;

	sel[2] = 0;
	sel[2] =  (uint16_t)  (current->context.index * 8);

	// salto a la TSS de current (produce un Task Switch)
	asm volatile("ljmp	*%[selec]" : : [selec] "m" (*sel));
}

/*
 * This function initialize a context for a task
 * parameters:
 * 	-ctx: task hw context
 *  -stackbase: base address of the stack area for the task
 *  -stacksize: size of the stack area
 *  -level: no function associated
 *  -entry: entry point for the task's code
 */
void ia32_context_init(ia32_ext_tss_t * ctx, void * stackbase,
		uint32_t stacksize, uint32_t level, void * entry) {

	memset((void *) ctx, 0, (sizeof(ia32_ext_tss_t)));

//	DEBUG("iniciando contexto hardware: \nDirección de la TSS: 0x%x\nBase de la pila: 0x%x\nTamaño de la pila: 0x%x\nPunto de entrada: 0x%x", (uint32_t *)ctx,stackbase, stacksize, entry);

	ctx->index = gdt_get_free_entry();

	//incluir la entrada en la gdt de la TSS (contexto)	límite= size -1
	gdt_set_entry(ctx->index, (unsigned long) ctx,
			(sizeof(ia32_ext_tss_t) - 1), (GDT_DPL_0| GDT_SEG_P | GDT_TYPE_TSS
					| GDT_SYS_SEG), 0x00);

	ctx->esp = (uint32_t) stackbase + stacksize -1;
	ctx->eip = (uint32_t) entry;
	ctx->eflags = (uint32_t)0x0202; // IF

	ctx->cs = (uint16_t) IA32_CODE_SEG;
	ctx->ds = (uint16_t) IA32_DATA_SEG;
	ctx->ss = (uint16_t) IA32_DATA_SEG;
	ctx->fs = (uint16_t) IA32_DATA_SEG;
	ctx->gs = (uint16_t) IA32_DATA_SEG;
	ctx->es = (uint16_t) IA32_DATA_SEG;

	asm volatile("movl %%cr3, %0" : "=a" (ctx->cr3) : );

}
/*
 * \todo This function is just for debug purpose and does have to be erased later
 */
static void dump_mem(uint32_t base, uint16_t limit) {

	uint8_t mem;
	uint16_t i;

	DEBUG("dump de TSS: base: 0x%x, tamaño: 0x%x", base, limit);

	for (i = 0; i < limit; i++) {
		mem = readb((void *) (base + i));
		DEBUG("[%d]0x%x",i, mem);
	}
}
/*
 * this function is the idle task code as long as the tracer is not invoked
 */
void ia32_power_down(void) {

	asm volatile ("1: \n "
				"hlt \n"
				"jmp 1b");
}

/*
 * This function returns if there is an interrupt going on or not
 */
uint32_t is_interrupted() {

	return ia32_intr_nesting;
}

/*****************************************************************************
 INTERRUPT RELATED CALLS
 *****************************************************************************/

/*
 * This routine masks a given interrupt
 */
void CPU_MaskInterrupt(uint32_t vector) {

	uint32_t apic_mask;
	uint8_t mask;

	if (vector == TIME_PIT_INTR) { // Lapic
		apic_mask = apic_read(APIC_LVTT);
		if (!(apic_mask & LVT_MASKED)) {
			apic_mask |= LVT_MASKED;
			apic_write(APIC_LVTT, apic_mask);
		}
	} else { //TODO Hacer para los dos 8259
		vector = vector - TIME_CLOCK_INTR;
		if (vector > 7 || vector < 0) {

			DEBUG("El vector %d no es correcto (>7).\n", vector);
			return;
		}else{
			PIC_READ8(ADDR_PIC1_1, mask);
			mask |= (1 << vector);
			PIC_WRITE8(ADDR_PIC1_1,mask);
		}
	}
}

/*
 * This routine masks all interrupts (only the NMI will remain un-masked)
 */
void CPU_MaskAllInterrupts() {

	uint8_t mask;
	uint32_t apic_mask;

	//8259
	mask = 0xFF;
	PIC_WRITE8(ADDR_PIC1_1,mask);
	//apic
	apic_mask = apic_read(APIC_LVTT);
	if (!(apic_mask & LVT_MASKED)) {
		apic_mask |= LVT_MASKED;
		apic_write(APIC_LVTT, apic_mask);
	}
}

/*
 * This routine returns TRUE in case the given vector is a valid
 * interrupt vector, and FALSE in any other case.
 */
int CPU_IsInterrupt(uint32_t vec) {

	if (vec >= 32 && vec < 40)
		return 1;
	if(vec == TIME_PIT_INTR)
		return 1;

	return 0;
}

/*
 * This routine shall clear the given interrupt.
 */
void CPU_ClearInterrupt(uint32_t vector) {

	if (vector == TIME_PIT_INTR) {
		apic_write(APIC_EOI,APIC_EOI_ACK);
		return;
	}
	//! EOI al pic para habilitar interrupciones
	if ((vector == TIME_CLOCK_INTR) ){//|| (vector == 0x24)){
		PIC_WRITE8(ADDR_PIC1_0, PIC_EOI);
		return;
// TODO		(A write to the EOI register must not be included in the handler
//			routine for an NMI, SMI, INIT, ExtINT, or SIPI.) SPG cap 9.8.1
	//	apic_write(APIC_EOI,APIC_EOI_ACK);
	}

	apic_write(APIC_EOI,APIC_EOI_ACK);	//clear IPI due to force interrupt

	DEBUG("Vector %d: no se conoce controlador.\n", vector);

}

/*
 * This routine enables the given interrupt
 */
void CPU_EnableInterrupt(uint32_t vector) {

	uint8_t mask = 0;
	uint32_t apic_mask;


	if (vector == TIME_PIT_INTR) { // apic
		apic_mask = apic_read(APIC_LVTT);
		if ((apic_mask & LVT_MASKED) != 0) {
			apic_mask &= TIMER_MASK;
			apic_write(APIC_LVTT, apic_mask);
			DEBUG("el LVTT vale: 0x%x",apic_mask);
		}
	} else {
		if(vector == TIME_CLOCK_INTR){
			//TODO init_timer habilita y la isr deshabilita la irq(ver handlers.c y time.c)
			return;
		}
		else{	//TODO solo para el pic principal!
			vector = vector - TIME_CLOCK_INTR;
			PIC_READ8(ADDR_PIC1_1, mask);
			mask &= ~(1 << vector);
			PIC_WRITE8(ADDR_PIC1_1,mask);
		}

//		if ((vector < 0) || (vector >= 7)) {
//
//			DEBUG("Vector incorrecto del PIC(%d), no existe interrupcion a habilitar\n", vector);
//
//			return;
//		}

	}
}
