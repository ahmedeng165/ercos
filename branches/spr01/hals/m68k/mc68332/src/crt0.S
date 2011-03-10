
| Generic MC68332 board HAL for ERCOS-RT -- interrupt handling and asm
| functions
|
| Copyright 2007,2008 (c) by Cesar Rodriguez Ortega
| Copyright 2004-2005 (c) by Oscar Garcia Poblacion and Oscar Rodriguez Polo
| Space Research Group,
| University of Alcala,
|
| For further information, please visit http://srg.aut.uah.es
|
| This software is provided under the terms of the GNU General Public v2
| Licence. A full copy of the GNU GPL is provided in the file COPYING
| found in the development root.


.global ercos_hal_reset_entry
.global mc68332_reset_entry
.global mc68332_lpstop
.global mc68332_trap0
.global mc68332_trap1
.global mc68332_trap2
.global mc68332_trap3
.global mc68332_trap4
.global mc68332_trap5
.global mc68332_getsp
.global mc68332_oc_tpu_read

.macro MC68332_INTR_ENTRY hnd
	| save data and address registers
	movem.l		%d0-%d7, -(%sp)
	movem.l		%a0-%a6, -(%sp)

	| save usp
	move.l		%usp, %a0
	move.l		%a0, -(%sp)

	| argument 2: context pointer
	move.l		%sp, -(%sp)

	| argument 1: copy of the vector offset word, pushed by hardware
	move.w		(%sp,74), %a1
	move.l		%a1, -(%sp)

	| call the high-level interrupt handler
	jsr		\hnd

	| discard the arguments
	adda.l		#0x8, %sp

	| restore usp
	move.l		(%sp)+, %a0
	move.l		%a0, %usp

	| restore address and data registers
	movem.l		(%sp)+, %a6-%a0
	movem.l		(%sp)+, %d7-%d0

	| the non-return point!
	rte
.endm

.macro MC68332_OC_TPU_READ
	| trigger the execution of the OC function for channel 15 by setting
	| the service request bits to 01
	move.w		(0x00fffe18), %d0
	and.w		#0x3fff, %d0
	or.w		#0x4000, %d0
	move.w		%d0, (0x00fffe18)

	| wait for some cpu cycles
	nop
	nop
	nop

	| the value of register TCR1 is left at address 0xec of the TPURAM
	move.w		(0x00ffffec), %d0
.endm

.section ".romvec", #alloc
.align 4
mc68332_vector_table:
	.long end			| (0) Initial stack pointer
	.long mc68332_reset_entry	| (1) Reset entry point
	.long mc68332_intr_entry	| (2) Bus error
	.long mc68332_intr_entry	| (3) Address error
	.long mc68332_intr_entry	| (4) Illegal Instruction
	.long mc68332_intr_entry	| (5) Zero division
	.long mc68332_intr_entry	| (6) CHK, CHK2 instructions
	.long mc68332_intr_entry	| (7) TRAPcc, TRAPV instructions
	.long mc68332_intr_entry	| (8) Privilege violation
	.long mc68332_intr_entry	| (9) Trace
	.long mc68332_intr_entry	| (10) Line 1010 emulator
	.long mc68332_intr_entry	| (11) Line 1111 emulator
	.long mc68332_intr_entry	| (12) Hardware breakpoint
	.long mc68332_intr_entry	| (13) (Reserved, coprocessor protocol violation)
	.long mc68332_intr_entry	| (14) Format error and uninitialized interrupt
	.long mc68332_intr_entry	| (15) Format error and uninitialized interrupt
	.long mc68332_intr_entry	| (16) Reserved
	.long mc68332_intr_entry	| (17) Reserved
	.long mc68332_intr_entry	| (18) Reserved
	.long mc68332_intr_entry	| (19) Reserved
	.long mc68332_intr_entry	| (20) Reserved
	.long mc68332_intr_entry	| (21) Reserved
	.long mc68332_intr_entry	| (22) Reserved
	.long mc68332_intr_entry	| (23) Reserved
	.long mc68332_intr_entry	| (24) Spurious interrupt
	.long mc68332_intr_entry	| (25) Level 1 interrupt autovector
	.long mc68332_intr_entry	| (26) Level 2 interrupt autovector
	.long mc68332_pit_entry		| (27) Level 3 intr. aut. (pit)
	.long mc68332_intr_entry	| (28) Level 4 interrupt autovector
	.long mc68332_intr_entry	| (29) Level 5 interrupt autovector
	.long mc68332_intr_entry	| (30) Level 6 interrupt autovector
	.long mc68332_intr_entry	| (31) Level 7 interrupt autovector
	.long mc68332_trap0_entry	| (32) Trap vector 0: task dispatch
	.long mc68332_trap1_entry	| (33) Trap vector 1: enable intr.
	.long mc68332_trap2_entry	| (34) Trap vector 2: disable intr.
	.long mc68332_trap3_entry	| (35) Trap vector 3: low power mode
	.long mc68332_trap4_entry	| (36) Trap vector 4: uart tx
	.long mc68332_trap5_entry	| (37) Trap vector 5: uart rx
	.long mc68332_intr_entry	| (38) Trap instruction vector 6
	.long mc68332_intr_entry	| (39) Trap instruction vector 7
	.long mc68332_intr_entry	| (40) Trap instruction vector 8
	.long mc68332_intr_entry	| (41) Trap instruction vector 9
	.long mc68332_intr_entry	| (42) Trap instruction vector 10
	.long mc68332_intr_entry	| (43) Trap instruction vector 11
	.long mc68332_intr_entry	| (44) Trap instruction vector 12
	.long mc68332_intr_entry	| (45) Trap instruction vector 13
	.long mc68332_intr_entry	| (46) Trap instruction vector 14
	.long mc68332_intr_entry	| (47) Trap instruction vector 15

	.long mc68332_intr_entry	| (48) (Reserved, coprocessor)
	.long mc68332_intr_entry	| (49) (Reserved, coprocessor)
	.long mc68332_intr_entry	| (50) (Reserved, coprocessor)
	.long mc68332_intr_entry	| (51) (Reserved, coprocessor)
	.long mc68332_intr_entry	| (52) (Reserved, coprocessor)
	.long mc68332_intr_entry	| (53) (Reserved, coprocessor)
	.long mc68332_intr_entry	| (54) (Reserved, coprocessor)
	.long mc68332_intr_entry	| (55) (Reserved, coprocessor)
	.long mc68332_intr_entry	| (56) (Reserved, coprocessor)
	.long mc68332_intr_entry	| (57) (Reserved, coprocessor)
	.long mc68332_intr_entry	| (58) (Reserved, coprocessor)
	.rept 5
	.long mc68332_intr_entry	| (59-63) (Unassigned, reserved)
	.endr
	.rept 192
	.long mc68332_intr_entry	| (64-255) (Unassigned, reserved)
	.endr

.section ".text"
.align 4

| entry point after a cpu reset
ercos_hal_reset_entry:
mc68332_reset_entry:

	| for a start, initialize the stack pointer. ercos_lah_stack_top points
	| to the stack top (see annotations at kernel/main.c)
	lea		ercos_lah_stack_top, %a6
	movea.l		(%a6), %ssp

	| initialize Status Register: no tracing, supervisor state, interrupt
	| priority mask = 7, no extend flag, no negative, no zero, no overflow,
	| no carry
	move.w		#0x2700, %sr

	| FIXME -- clear bss area
	lea		bss_start, %a0
	lea		end, %a1
1:
	clr.l		(%a0)+
	cmp.l		%a0, %a1
	bgt		1b

	| call C code, in misc.c (it should never return!)
	jsr		mc68332_cstart

	| freeze
1:	bra		1b

| default interrupt entry point
mc68332_intr_entry :
	MC68332_INTR_ENTRY mc68332_intr_handler

| entry point for interrupt 27 (timer interrupt)
mc68332_pit_entry :
	| disable interrupts and save d0
	ori.w		#0x0700, %sr
	move.l		%d0, -(%sp)

	| latch the value of TCR1 free-running timer and restore d0
	MC68332_OC_TPU_READ
	move.w		%d0, (mc68332_oc)
	move.l		(%sp)+, %d0

	| decrement timer countdown and enter into the C handler if 0
	subq.l		#1, (mc68332_pit_countdown)
	beq		1f

	| check if another interrupt was triggered in the first instruction of
	| this handler
	tst.l		(mc68332_pit_interrupted)
	bne		1f
	rte


1:
	| enter into the C handler (src/time.c)
	MC68332_INTR_ENTRY mc68332_pit_handler

| trap 0: dispatch a new task
mc68332_trap0_entry :
	MC68332_INTR_ENTRY mc68332_trap_dispatch

| trap 1: enable interrupts
mc68332_trap1_entry :
	MC68332_INTR_ENTRY mc68332_trap_enable

| trap 2: disable interrupts
mc68332_trap2_entry :
	MC68332_INTR_ENTRY mc68332_trap_disable

| trap 3: enter into low power mode
mc68332_trap3_entry :
	MC68332_INTR_ENTRY mc68332_trap_lowpower

| trap 4: send a character to the serial port
mc68332_trap4_entry :
	MC68332_INTR_ENTRY mc68332_trap_tx

| trap 5: receive a character from the serial port
mc68332_trap5_entry :
	MC68332_INTR_ENTRY mc68332_trap_rx

| void mc68332_trap0 (void * current, void * prev);
mc68332_trap0:
	trap		#0
	rts

| void mc68332_trap1 (void);
| mc68332_trap1:
	trap		#1
	rts

| void mc68332_trap2 (void);
| mc68332_trap2:
	trap		#2
	rts

| void mc68332_trap3 (void);
mc68332_trap3:
	trap		#3
	rts

| void mc68332_trap4 (char ch);
mc68332_trap4:
	| FIXME -- Note that we will use d0 to pass the character to the
	| mc68332_trap_tx function.  For the C calling conventions of gcc, this
	| makes no harm, as d0 may not be preserved when the calle function
	| returns, but this does not have to be true for other compilers
	move.l		(%sp,4), %d0
	trap		#4
	rts

| void mc68332_trap5 (char ch);
mc68332_trap5:
	| FIXME -- Note that we will use d0 to return the value, but this
	| hasn't to work with other compilers different from gcc
	trap		#5
	rts

| void mc68332_lpstop (void);
mc68332_lpstop:
	lpstop		#0x2000
	rts

| uint32_t mc68332_getsp (void);
mc68332_getsp:
	move.l		%sp, %d0
	rts

| uint32_t mc68332_oc_tpu_read (void);
mc68332_oc_tpu_read:
	MC68332_OC_TPU_READ
	rts

| vim:ts=8:syn=asm68k:

