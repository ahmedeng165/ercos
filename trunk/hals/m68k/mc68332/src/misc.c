
/*
 * Generic MC68332 board HAL for ERCOS-RT -- mainly initialization code
 *
 * Copyright 2008 (c) by Cesar Rodriguez Ortega,
 * Space Research Group,
 * University of Alcala.
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 * This software is provided under the terms of the GNU General Public v2
 * Licence. A full copy of the GNU GPL is provided in the file COPYING
 * found in the development root of ERCOS-RT project.
 */

#include <public/config.h> /* basic types */
#include <public/posix.h> /* printf */
#include <public/ercos-lah.h> /* current task and previous current task; lah_main */

#include <mc68332/sim.h>

static void mc68332_uart_init (void) {
	uint16_t val;

	/* _mdump_init (); */

	/* Nominal baud rate for 16.78Mhz
	 * FIXME -- there is something wrong in this table
	 *  1200 ( 1199.7) -> 0x0165
	 *  2400 ( 2405.0) -> 0x00da
	 *  4800 ( 4810.0) -> 0x006d
	 *  9600 ( 9532.5) -> 0x0037
	 * 19200 (19418.1) -> 0x0016
	 * 38400 (37449.1) -> 0x000e
	 * 76800 (74898.3) -> 0x0007
	 */
	/* baud rate: 19200 baud */
	MC68332_QSM_WRITE16 (MC68332_QSM_SCCR0, 0x001b);

	/* 1 start bit, 1 stop bit, 8 data bits, parity disabled, transmiter
	 * enabled, all irqs disabled, receiver enabled */
	MC68332_QSM_WRITE16 (MC68332_QSM_SCCR1, 0x000c);

	/* set TDRE flag */
	val = MC68332_QSM_READ16 (MC68332_QSM_SCSR);
	MC68332_QSM_WRITE16 (MC68332_QSM_SCSR, val | 0x0100);

	DEBUG ("Serial port initialized: 19200 baud, 8n1, interrupt "
			"enabled");
}

void mc68332_uart_tx (char ch) {
	mc68332_trap4 (ch);
}

char mc68332_uart_rx (void) {
	return mc68332_trap5 ();
}


#include <mc68332/time.h>

void mc68332_cstart (void) {

	/* initialize the serial port */
	mc68332_uart_init ();

//TODO	mc68332_app_init ();

	/* call arch-independent initialization stuff */
	ercos_lah_main ();
}

void mc68332_dispatch (void) {

	/* if the current task is the same as the previous task, then the
	 * scheduler picked up the same task to be dispatched: we will return y
	 * aqui paz y despues gloria */
	if (ercos_lah_prev_current () == ercos_lah_current ()) return;

	/* Ok, we have to dispatch the task pointed by current, storing the
	 * hardware state in the prev tcb. We invoke mc68332_trap_dispatch via
	 * the trap 0 (see crt0.S) and get this task blocked in the following
	 * call. Dispatching this task will resume the execution just after the
	 * next statement */
	mc68332_trap0 ();

	/* At this point, this task is now ready again: we will return as if
	 * nothing had happened */
	return;
}

