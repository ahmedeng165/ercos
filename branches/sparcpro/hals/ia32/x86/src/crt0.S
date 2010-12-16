/*
	C run time management for ERCOS kernel on Intel ia32 architecture

   	Copyright 2009 (c)	Alberto Carrasco Gallardo
	University of Alcala
	e-mail:	acarrasco@srg.aut.uah.es

	This software is provided under the terms of the GNU General Public v2
	Licence. A full copy of the GNU GPL is provided in the file COPYING
	found in the development root of ERCOS-RT.
*/
#include <x86/x86.h>

/* The magic number for the Multiboot header. */
#define MULTIBOOT_HEADER_MAGIC          0x1BADB002

/* The flags for the Multiboot header. */
#ifdef __ELF__
# define MULTIBOOT_HEADER_FLAGS         0x00000003
#else
# define MULTIBOOT_HEADER_FLAGS         0x00010003
#endif

/* The magic number passed by a Multiboot-compliant boot loader. */
#define MULTIBOOT_BOOTLOADER_MAGIC      0x2BADB002

/* C symbol format. HAVE_ASM_USCORE is defined by configure. */
#ifdef HAVE_ASM_USCORE
# define EXT_C(sym)                     _ ## sym
#else
# define EXT_C(sym)                     sym
#endif

/*
 This macro creates a stub for a given interrupt vector. The parameters are the interrupt
 number and the name of the Interrupt Service Routine. This macro also create variables that
 are used to save pointers to the irq stack and the previous task stack in order to make the
 stack switching before the ISR. The irq stack pointer is initialized in "Tables.c"
 and the previous task stack pointer is saved every time a irq ocurred.
 */
#define HANDLER_STUB(vector, isr)\
		.global	prev_handler##vector##_stack	;\
		.global irq##vector##_stack  ; \
		.global stub_handler##vector ; \
	irq##vector##_stack: .space 4	;\
	prev_handler##vector##_stack:	.space 4 	;\
	stub_handler##vector:		;\
		cli			;\
		movl	%esp, prev_handler##vector##_stack		;\
		movl	(irq##vector##_stack), %esp	;\
		call	isr;

#define NOERR_CODE_INT(vector) \
.align 4	; 	\
vec##vector:		;	\
	pushl   $0 		;		\
	pushl   $vector			;	\
	jmp ia32_interrupt_entry;

#define ERR_CODE_INT(vector) \
.align 4	; 	\
vec##vector:	;		\
	pushl   $vector  ; 	\
	jmp ia32_interrupt_entry ; \
	.word   0x5a5a	;


// Global symbols
.global ercos_hal_reset_entry
.global paging_enable
.global paging_disable
.global ia32_interrupt_entry
.global ia32_idt_table
.global pmode_enable
.global irq_enable
.global irq_disable
.global page_dir
.global page_tables
.global vec0
.global vec1

.global FPU_Last_Task
.global ia32_intr_nesting
/* hal stack for irqs*/
.global hal_irq_stack

/*kernel init stack pointer*/
.extern	ercos_lah_stack_top
.extern ercos_lah_stack_base

/* defined in the linker script */
.extern CODE_START
.extern CODE_END
.extern DATA_START
.extern DATA_END
.extern bss_start
.extern end

.section ".rodata"


.align 8
ia32_idt_table:    /*idt: processor specific exceptions*/
    .fill   256, 8, 0	    /*se queja el compilador con size=8 (en linux no)*/
ia32_idt_end:


.section ".multiboot"
/* Align 32 bits boundary. */
 .align  4
/* Multiboot header. */
multiboot_header:
    /* magic */
 	.long   MULTIBOOT_HEADER_MAGIC
 	/* flags */
 	.long   MULTIBOOT_HEADER_FLAGS
 	/* checksum */
 	.long   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

multiboot_info_ptr:
	.space 4
magic_number:
	.space 4


.text

ercos_hal_reset_entry:
	//finit
	// En el registro edx esta el modelo, familia, etc... del procesador
	// ver cap 8 de system_programing_guide de Intel(R).
	movl	%edx, processor_info
	// No interesan las interrupciones, aunque en teoria estan deshabilitadas...
	cli
	movl 	%ebx, multiboot_info_ptr
	movl	%eax, magic_number

// eflags a cero
    pushl 	$0
    popf

	//load the kernel stack into the stack pointer register
	movl		ercos_lah_stack_top, %esp

// clear bss area
	movl	bss_start, %edi
	movl 	end, %ecx
	subl	%edi, %ecx			// tama�o bss
	xorl	%eax, %eax			// limpiar
	rep 	stosl						// repetir %ecx veces mov eax a posicion de edi

	/* Push the pointer to the Multiboot information structure. */
     pushl   (multiboot_info_ptr)
     /* Push the magic value. */
     pushl   (magic_number)

     pushl 	 (processor_info)
 /* Now enter the C main function... */
     call    EXT_C(ia32_cstart)
	// freeze
1:	jmp		1b


/*
When an interrupt is requested�
	- the CPU finishes executing current instruction.
	- pushes flag register onto stack..
	- disables the INTR input by clearing the IF (interrupt flag) in the
	  flag register.
	- clears the TF (trap flag) in the flag register.
	- pushes the current CS contents onto the stack.
	- pushes the current IP contents onto the stack.
	- does an indirect far jump to the start of the ISR	(HERE).
	- (Interrupt Service Routine)
*/

/******************************************************************************/
NOERR_CODE_INT(0)
NOERR_CODE_INT(1)
NOERR_CODE_INT(2)
NOERR_CODE_INT(3)
NOERR_CODE_INT(4)
NOERR_CODE_INT(5)
NOERR_CODE_INT(6)
NOERR_CODE_INT(7)
/*TODO int 8 error code... en principio lo mete el procesador */
/*	pero en la practica no parece ser así */
NOERR_CODE_INT(8)
NOERR_CODE_INT(9)
ERR_CODE_INT(10)
ERR_CODE_INT(11)
ERR_CODE_INT(12)
ERR_CODE_INT(13)
ERR_CODE_INT(14)
NOERR_CODE_INT(15)
NOERR_CODE_INT(16)
ERR_CODE_INT(17)
NOERR_CODE_INT(18)
NOERR_CODE_INT(19)	//Hasta aqui las excepciones del procesador
NOERR_CODE_INT(20)
NOERR_CODE_INT(21)
NOERR_CODE_INT(22)
NOERR_CODE_INT(23)
NOERR_CODE_INT(24)
NOERR_CODE_INT(25)
NOERR_CODE_INT(26)
NOERR_CODE_INT(27)
NOERR_CODE_INT(28)
NOERR_CODE_INT(29)
NOERR_CODE_INT(30)
NOERR_CODE_INT(31)
NOERR_CODE_INT(32)
NOERR_CODE_INT(33)
NOERR_CODE_INT(34)
NOERR_CODE_INT(35)
NOERR_CODE_INT(36)
NOERR_CODE_INT(37)
NOERR_CODE_INT(38)
NOERR_CODE_INT(39)
NOERR_CODE_INT(40)
NOERR_CODE_INT(41)
NOERR_CODE_INT(42)
NOERR_CODE_INT(43)
NOERR_CODE_INT(44)
NOERR_CODE_INT(45)
NOERR_CODE_INT(46)
NOERR_CODE_INT(47)
NOERR_CODE_INT(48)
NOERR_CODE_INT(49)
NOERR_CODE_INT(50)
NOERR_CODE_INT(51)
NOERR_CODE_INT(52)
NOERR_CODE_INT(53)
NOERR_CODE_INT(54)
NOERR_CODE_INT(55)
NOERR_CODE_INT(56)
NOERR_CODE_INT(57)
NOERR_CODE_INT(58)
NOERR_CODE_INT(59)
NOERR_CODE_INT(60)
NOERR_CODE_INT(61)
NOERR_CODE_INT(62)
NOERR_CODE_INT(63)
NOERR_CODE_INT(64)
NOERR_CODE_INT(65)
NOERR_CODE_INT(66)
NOERR_CODE_INT(67)
NOERR_CODE_INT(68)
NOERR_CODE_INT(69)
NOERR_CODE_INT(70)
NOERR_CODE_INT(71)
NOERR_CODE_INT(72)
NOERR_CODE_INT(73)
NOERR_CODE_INT(74)
NOERR_CODE_INT(75)
NOERR_CODE_INT(76)
NOERR_CODE_INT(77)
NOERR_CODE_INT(78)
NOERR_CODE_INT(79)
NOERR_CODE_INT(80)
NOERR_CODE_INT(81)
NOERR_CODE_INT(82)
NOERR_CODE_INT(83)
NOERR_CODE_INT(84)
NOERR_CODE_INT(85)
NOERR_CODE_INT(86)
NOERR_CODE_INT(87)
NOERR_CODE_INT(88)
NOERR_CODE_INT(89)
NOERR_CODE_INT(90)
NOERR_CODE_INT(91)
NOERR_CODE_INT(92)
NOERR_CODE_INT(93)
NOERR_CODE_INT(94)
NOERR_CODE_INT(95)
NOERR_CODE_INT(96)
NOERR_CODE_INT(97)
NOERR_CODE_INT(98)
NOERR_CODE_INT(99)
NOERR_CODE_INT(100)
NOERR_CODE_INT(101)
NOERR_CODE_INT(102)
NOERR_CODE_INT(103)
NOERR_CODE_INT(104)
NOERR_CODE_INT(105)
NOERR_CODE_INT(106)
NOERR_CODE_INT(107)
NOERR_CODE_INT(108)
NOERR_CODE_INT(109)
NOERR_CODE_INT(110)
NOERR_CODE_INT(111)
NOERR_CODE_INT(112)
NOERR_CODE_INT(113)
NOERR_CODE_INT(114)
NOERR_CODE_INT(115)
NOERR_CODE_INT(116)
NOERR_CODE_INT(117)
NOERR_CODE_INT(118)
NOERR_CODE_INT(119)
NOERR_CODE_INT(120)
NOERR_CODE_INT(121)
NOERR_CODE_INT(122)
NOERR_CODE_INT(123)
NOERR_CODE_INT(124)
NOERR_CODE_INT(125)
NOERR_CODE_INT(126)
NOERR_CODE_INT(127)
NOERR_CODE_INT(128)
NOERR_CODE_INT(129)
NOERR_CODE_INT(130)
NOERR_CODE_INT(131)
NOERR_CODE_INT(132)
NOERR_CODE_INT(133)
NOERR_CODE_INT(134)
NOERR_CODE_INT(135)
NOERR_CODE_INT(136)
NOERR_CODE_INT(137)
NOERR_CODE_INT(138)
NOERR_CODE_INT(139)
NOERR_CODE_INT(140)
NOERR_CODE_INT(141)
NOERR_CODE_INT(142)
NOERR_CODE_INT(143)
NOERR_CODE_INT(144)
NOERR_CODE_INT(145)
NOERR_CODE_INT(146)
NOERR_CODE_INT(147)
NOERR_CODE_INT(148)
NOERR_CODE_INT(149)
NOERR_CODE_INT(150)
NOERR_CODE_INT(151)
NOERR_CODE_INT(152)
NOERR_CODE_INT(153)
NOERR_CODE_INT(154)
NOERR_CODE_INT(155)
NOERR_CODE_INT(156)
NOERR_CODE_INT(157)
NOERR_CODE_INT(158)
NOERR_CODE_INT(159)
NOERR_CODE_INT(160)
NOERR_CODE_INT(161)
NOERR_CODE_INT(162)
NOERR_CODE_INT(163)
NOERR_CODE_INT(164)
NOERR_CODE_INT(165)
NOERR_CODE_INT(166)
NOERR_CODE_INT(167)
NOERR_CODE_INT(168)
NOERR_CODE_INT(169)
NOERR_CODE_INT(170)
NOERR_CODE_INT(171)
NOERR_CODE_INT(172)
NOERR_CODE_INT(173)
NOERR_CODE_INT(174)
NOERR_CODE_INT(175)
NOERR_CODE_INT(176)
NOERR_CODE_INT(177)
NOERR_CODE_INT(178)
NOERR_CODE_INT(179)
NOERR_CODE_INT(180)
NOERR_CODE_INT(181)
NOERR_CODE_INT(182)
NOERR_CODE_INT(183)
NOERR_CODE_INT(184)
NOERR_CODE_INT(185)
NOERR_CODE_INT(186)
NOERR_CODE_INT(187)
NOERR_CODE_INT(188)
NOERR_CODE_INT(189)
NOERR_CODE_INT(190)
NOERR_CODE_INT(191)
NOERR_CODE_INT(192)
NOERR_CODE_INT(193)
NOERR_CODE_INT(194)
NOERR_CODE_INT(195)
NOERR_CODE_INT(196)
NOERR_CODE_INT(197)
NOERR_CODE_INT(198)
NOERR_CODE_INT(199)
NOERR_CODE_INT(200)
NOERR_CODE_INT(201)
NOERR_CODE_INT(202)
NOERR_CODE_INT(203)
NOERR_CODE_INT(204)
NOERR_CODE_INT(205)
NOERR_CODE_INT(206)
NOERR_CODE_INT(207)
NOERR_CODE_INT(208)
NOERR_CODE_INT(209)
NOERR_CODE_INT(210)
NOERR_CODE_INT(211)
NOERR_CODE_INT(212)
NOERR_CODE_INT(213)
NOERR_CODE_INT(214)
NOERR_CODE_INT(215)
NOERR_CODE_INT(216)
NOERR_CODE_INT(217)
NOERR_CODE_INT(218)
NOERR_CODE_INT(219)
NOERR_CODE_INT(220)
NOERR_CODE_INT(221)
NOERR_CODE_INT(222)
NOERR_CODE_INT(223)
NOERR_CODE_INT(224)
NOERR_CODE_INT(225)
NOERR_CODE_INT(226)
NOERR_CODE_INT(227)
NOERR_CODE_INT(228)
NOERR_CODE_INT(229)
NOERR_CODE_INT(230)
NOERR_CODE_INT(231)
NOERR_CODE_INT(232)
NOERR_CODE_INT(233)
NOERR_CODE_INT(234)
NOERR_CODE_INT(235)
NOERR_CODE_INT(236)
NOERR_CODE_INT(237)
NOERR_CODE_INT(238)
NOERR_CODE_INT(239)
NOERR_CODE_INT(240)
NOERR_CODE_INT(241)
NOERR_CODE_INT(242)
NOERR_CODE_INT(243)
NOERR_CODE_INT(244)
NOERR_CODE_INT(245)
NOERR_CODE_INT(246)
NOERR_CODE_INT(247)
NOERR_CODE_INT(248)
NOERR_CODE_INT(249)
NOERR_CODE_INT(250)
NOERR_CODE_INT(251)
NOERR_CODE_INT(252)
NOERR_CODE_INT(253)
NOERR_CODE_INT(254)
NOERR_CODE_INT(255)
/*HASTA AQUI*/


HANDLER_STUB(32, oneshot_timer_isr)
HANDLER_STUB(48, periodic_timer_isr)

 /*
  * En la pila tenemos...
  *  0(%esp) vector
  *  4(%esp) error code
  *  8(%esp) eip
  * 12(%esp) cs
  * 16(%esp) eflags
  */

ia32_interrupt_entry:
    // save general purpose registers(long size)
	pusha

	mov	$0x10, %ax
	mov %ax, %ds		//data segment = 0x10
	mov	%ax, %es

// argumento: 	puntero al contexto ia32_context_t
	pushl %esp
	cld
	call ia32_interrupt_handlers		// high level handler

	// descartar los argumentos aqui...
	add	$4, %esp			//descarto argumentos

	popa		//restore general purpose registers

	add	$8, %esp	//discard num vector y errcode

	iret

/****************************************************************************
******			protected mode enabling									*****
*****************************************************************************/
pmode_enable:
	// Se activa el bit de menor peso de cr0 ( PE = Protection Enable )
   	// con esto se entra en el MODO PROTEGIDO
    	movl 	%cr0, %eax
    	orl 	$0x00000001, %eax
    	movl 	%eax, %cr0
	ret

/******************************************************************************
***** 	paging enabling and disabling	***************************************
*******************************************************************************/
paging_enable:
	movl	%cr0, %eax
// flip the paging bit on in CR0,
	orl		$0x80000000, %eax
	movl 	%eax, %cr0
// and far jump to flush the CPU queues.
	ljmp 	$0x08, $enter_paging
enter_paging:
	ret

paging_disable:
	movl 	%cr0, %eax
	andl	$0x7FFFFFFF, %eax
	movl	%eax, %cr0
	ret
/******************************************************************************
 ***		IRQ's disabling and enabling 	***********************************
 ******************************************************************************/
irq_disable:
	cli
//	movl	$0xFEE00080, %edx
//	movl	$0xFF, %eax
//	movl	%eax, (%edx)
	ret

irq_enable:
//	movl	$0xFEE00080, %edx
//	movl	$0x00, %eax
//	movl	%eax, (%edx)
	sti
	ret


.section ".bss"
.align 4096
page_dir:
	.space 	1024*4
.align 4096
page_tables:
	.space	4096*(NUM_TABLES)
processor_info:	.space 4
FPU_Last_Task: .space 4
ia32_intr_nesting: .space 4
hal_irq_stack:
	.space	HAL_IRQ_STACK_TOTAL_SIZE

