/*
 * Low level interrupt handlers for ERCOS-RT
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
#include <x86/handlers.h>
#include <public/ercos-lah.h>
#include <public/config.h>


#define IDLE_TASK	1

extern volatile uint32_t FPU_Last_Task;

extern OS_task_t task_table[CONFIG_ERCOS_MAX_THREADS + 2];

extern volatile long ia32_intr_nesting;

/* Just debug purposes */
static void register_dump(ia32_context_t *ctx){

	DEBUG("Registros:\nedi:0x%x\nesi:0x%x\nebp:0x%x\nesp:0x%x\nebx:0x%x\nedx:0x%x\necx:0x%x\neax:0x%x\n"
		 "vector:0x%x\neip:0x%x\neflags:0x%x",ctx->edi, ctx->esi,ctx->ebp,ctx->esp,ctx->ebx,ctx->edx,ctx->ecx,ctx->eax,
		 ctx->vector, ctx->eip, ctx->eflags);

}

/*	interrupt handlers	*/

/*
 * This is the generic interrupt handler, it means that every single interrupt
 * which don't have an specific handler is going to arrive here. The only handler
 * written in this function is the vector 7 that is related to the FPU.
 */
void ia32_interrupt_handlers(ia32_context_t *ctx) {

	uint32_t dir;

	switch (ctx->vector) {

	case 0:
		DEBUG("Exception received 0 DIVIDE ERROR, error code: 0x%x vector: 0x%x!! \n", ctx->errcode, ctx->vector);
		register_dump(ctx);
		break;

	case 1:
		DEBUG("Exception received 1 DEBUG EXCEPTION, error code: 0x%x vector: 0x%x!! \n", ctx->errcode, ctx->vector);
		register_dump(ctx);
		break;

	case 2:
		DEBUG("Exception received 2 NMI, error code: 0x%x vector: 0x%x!! \n", ctx->errcode, ctx->vector);
		register_dump(ctx);
		break;

	case 3:
		DEBUG("Exception received 3 BREAKPOINT, error code: 0x%x vector: 0x%x!! \n", ctx->errcode, ctx->vector);
		register_dump(ctx);
		break;

	case 4:
		DEBUG("Exception received 4 OVERFLOW, error code: 0x%x vector: 0x%x!! \n", ctx->errcode, ctx->vector);
		register_dump(ctx);
		break;

	case 5:
		DEBUG("Exception received 5 BOUNDS CHECK, error code: 0x%x vector: 0x%x!! \n", ctx->errcode, ctx->vector);
		register_dump(ctx);
		break;

	case 6:
		DEBUG("Exception received 6 INVALID OPCODE, error code: 0x%x vector: 0x%x!! \n", ctx->errcode, ctx->vector);
		register_dump(ctx);
		break;

	case 7:

		asm volatile("clts" : :);
		//TODO limpio el flag TS

		if (is_interrupted()){
			ercos_lah_panic("floating point in a interrupt!!");
		}else if (FPU_Last_Task == IDLE_TASK) {
			FPU_Last_Task = current->tid;
		}else if (current->tid == FPU_Last_Task) {
			//Do nothing further clts and set flag used fpu
		} else {
			asm volatile("fsave %0": "=m" (task_table[FPU_Last_Task].context.fpu_ctx) : );
			if (current->context.has_used_fpu == 1)
				asm volatile("frstor %0" : : "m" (current->context.fpu_ctx));
			FPU_Last_Task = current->tid;
		}

		current->context.has_used_fpu = 1; //se usa la fpu en esta tarea
		break;

	case 8:
		DEBUG("Exception received 8 DOUBLE FAULT, error code: 0x%x vector: 0x%x!! \n", ctx->errcode, ctx->vector);
		register_dump(ctx);
		break;

	case 9:
		DEBUG("Exception received 9 COPROCESSOR SEGMENT OVERRUN, error code: 0x%x vector: 0x%x!! \n", ctx->errcode, ctx->vector);
		register_dump(ctx);
		break;

	case 10:
		DEBUG("Exception received 10 INVALID TSS, error code: 0x%x vector: 0x%x!! \n", ctx->errcode, ctx->vector);
		register_dump(ctx);
		break;

	case 11:
		DEBUG("Exception received 11 SEGMENT NOT PRESENT, error code: 0x%x vector: 0x%x!! \n", ctx->errcode, ctx->vector);
		register_dump(ctx);
		break;

	case 12:
		DEBUG("Exception received 12 STACK EXCEPTION, error code: 0x%x vector: 0x%x!! \n", ctx->errcode, ctx->vector);
		register_dump(ctx);
		break;

	case 13:
		DEBUG("Exception received GENERAL PROTECTION FAULT, error code: 0x%x vector: 0x%x!!eip:0x%x pila:0x%x\n", ctx->errcode, ctx->vector, ctx->eip, ctx->esp);
		register_dump(ctx);
		ercos_lah_panic("Esta interrupción no debería haberse producido!!!");
		break;

	case 14:
		asm volatile("movl %%cr2, %%eax" : "=a"(dir) : );
		DEBUG("Exception received PAGE FAULT, eip: 0x%x vector: 0x%x!! \n dirección: 0x%x", ctx->eip, ctx->vector, dir);
		register_dump(ctx);
		ercos_lah_panic("Esta interrupción no debería haberse producido!!!");
		break;

	case 15:
		DEBUG("Exception received 15, error code: 0x%x vector: 0x%x!! \n", ctx->errcode, ctx->vector);
		register_dump(ctx);
		break;

	case 16:
		DEBUG("Exception received COPROCESSOR ERROR, error code: 0x%x vector: 0x%x!! \n", ctx->errcode, ctx->vector);
		register_dump(ctx);
		break;

	case 17:
		DEBUG("Exception received 17, error code: 0x%x vector: 0x%x!! \n", ctx->errcode, ctx->vector);
		register_dump(ctx);
		break;

	case 18:
		DEBUG("Exception received 18, error code: 0x%x vector: 0x%x!! \n", ctx->errcode, ctx->vector);
		register_dump(ctx);
		break;

	case 19:
		DEBUG("Exception received 19, error code: 0x%x vector: 0x%x!! \n", ctx->errcode, ctx->vector);
		register_dump(ctx);
		break;
	case 39:		//TODO quitar esto!
		DEBUG("Excepción fantasma del QEMU");
		register_dump(ctx);
		break;

	default:
//		DEBUG("Interrupt number: %d \n errorcode: 0x%x \n",ctx->vector,ctx->errcode);
//		register_dump(ctx);
		ercos_lah_intr_handler(ctx->vector);
//		asm volatile("add $0x1c,%esp \niret");
		break;
	}

}

/*
 *  Interrupt handlers asociados al timer y el clock
 */

// periodic timer handler(APIC)
void periodic_timer_isr() {

	ia32_intr_prologue();

//	printf("@T");
	//llamar a la ISR del kernel
	ercos_lah_intr_handler(TIME_PIT_INTR);

	irq_disable(); //TODO

	asm volatile ("	movl	(prev_handler48_stack), %%esp" : :);

	ia32_intr_epilogue();

	asm volatile("iret" : :);
}

// clock one-shot handler(8259)
void oneshot_timer_isr() {

	ia32_intr_prologue();

	PIC_WRITE8(ADDR_PIC1_1, PIC1_OCW1);		//deshabilita irq
//	printf("\n@O-S\n");
	//llamar a la ISR del kernel
	ercos_lah_intr_handler(TIME_CLOCK_INTR);

	irq_disable();	//TODO

	asm volatile ("movl	(prev_handler32_stack), %%esp" : :);

	ia32_intr_epilogue();

	asm volatile("iret" : :);
}
