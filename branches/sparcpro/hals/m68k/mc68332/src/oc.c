
/*
 * Generic MC68332 board HAL for ERCOS-RT -- fine timing primitives
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

/* Stuff in this file encodes initialization and support for fine timing
 * primitives in this hal.  We use 68332 TPU channel 15 to implement a
 * microsecond-accuracy free-running timer which we can be read at any time.
 *
 * Three primitives allow code in src/time.c (main user of this code) to read
 * the timer in order to return a fine-grained time mark each time the hal is
 * requested to return the current time.  The idea is as follows:  once
 * initialized via mc68332_oc_init, a 16-bit free timer starts to run with an
 * update period of 250ns.  This gives an overflow period of about 16ms.  The
 * mc68332_oc_latch allows the user to fix a time mark and mc68332_oc_read
 * returns the number of microseconds from the last time mark.
 *
 * To be able to have a correct notion of time, the mc68332_oc_latch gets
 * called at the Periodic Interrupt Timer interrupt, each 10ms.  So
 * mc68332_oc_read always returns the number of microseconds from the last PIT
 * interrupt (see src/crt0.S for more details).
 *
 * This implementation makes use of OC (Output Compare) TPU micro-codded
 * function.  Note than mc68332_oc_tpu_read function lives in crt0.S.  For more
 * information see document reference Freescale TPUPN12/D, available in the web
 */

#include <public/config.h> /* data types & DEBUG */
#include <public/posix.h> /* for printf */
#include <mc68332/sim.h>

volatile uint16_t mc68332_oc;

void mc68332_oc_init (void) {
	uint16_t val;

	/* At 16Mhz, set the prescaler to 4 cycles (250ns TCR1 increment
	 * period); arbitrate number is 15 */
	MC68332_TPU_WRITE16 (MC68332_TPU_TPUMCR, 0x004f);

	/* set channel 15 to function 0xe (Output Compare) in CFSR0 */
	val = MC68332_TPU_READ16 (MC68332_TPU_CFSR0);
	val &= 0x0fff;
	val |= 0xe000;
	MC68332_TPU_WRITE16 (MC68332_TPU_CFSR0, val);

	/* CHANNEL_CONTROL OC's function parameter */
	/* 0x0083: TBS=0100, PAC=000, PSC=11: capture TCR1, match TCR1; do not
	 * change pin state on match; do not force any state */
	MC68332_TPURAM_WRITE16 (15 * 16, 0x0083);

	/* OFFSET OC's function parameter: 0 cycles, not used */
	MC68332_TPURAM_WRITE16 (15 * 16 + 2, 0x0000);

	/* REF_ADDR1 OC's function parameter: points to TCR1 output, not used
	 */
	MC68332_TPURAM_WRITE16 (15 * 16 + 4, 0x00ec);

	/* host sequence bits are 11, so an immediate interrupt will be
	 * generated (and ignored) */
	val = MC68332_TPU_READ16 (MC68332_TPU_HSQR0);
	val |= 0xc000;
	MC68332_TPU_WRITE16 (MC68332_TPU_HSQR0, val);

	/* disable interrupt for channel 15 */
	val = MC68332_TPU_READ16 (MC68332_TPU_CIER);
	val &= 0x7fff;
	MC68332_TPU_WRITE16 (MC68332_TPU_CIER, val);

	/* disable the service request bits */
	val = MC68332_TPU_READ16 (MC68332_TPU_HSRR0);
	val &= 0x3fff;
	MC68332_TPU_WRITE16 (MC68332_TPU_HSRR0, val);

	/* set channel 15 to the maximum priority */
	val = MC68332_TPU_READ16 (MC68332_TPU_CPR0);
	val |= 0xc000;
	MC68332_TPU_WRITE16 (MC68332_TPU_CPR0, val);
}

void mc68332_oc_latch (void) {
	mc68332_oc = mc68332_oc_tpu_read ();
}

uint16_t dummy_mc68332_oc_tpu_read (void) {
	volatile int i;
	uint16_t req;
#if 0
	uint16_t prio;
#endif

	/* FIXME -- this function should be executed with interrupts disabled,
	 * or at least in mutual exclusion with any other code using TPURAM
	 * 0xec memory cell
	 */

	/* Trigger the function execution, as follows:
	 * 1. Set the channel priority to the minimum, so it will be disabled
	 *    in the TPU.  NOTE that this step can be skip if we can assure
	 *    that this function will be called always with the OC function
	 *    disabled
	 * 2. Set service request bits to 01, selecting host-initiated pulse
	 *    mode service.
	 * 3. Trigger function execution by enabling channel at the maximum
	 *    priority
	 * The function _mc68332_oc_init already set all parameters to the OC
	 * function
	 */

#if 0
	/* 1. set channel priority to <disabled> priority */
	prio = MC68332_TPU_READ16 (MC68332_TPU_CPR0);
	prio &= 0x3fff;
	MC68332_TPU_WRITE16 (MC68332_TPU_CPR0, prio);
#endif

	/* 2. set service request bits to 01 */
	req = MC68332_TPU_READ16 (MC68332_TPU_HSRR0);
	req &= 0x3fff;
	req |= 0x4000;
	MC68332_TPU_WRITE16 (MC68332_TPU_HSRR0, req);

#if 0
	/* 3. the non-return point: set channel 15 to high priority */
	prio |= 0xc000;
	MC68332_TPU_WRITE16 (MC68332_TPU_CPR0, prio);
#endif

#if 0
	i = 0;
	while ((MC68332_TPU_READ16 (MC68332_TPU_HSRR0) & 0xc000) != 0) i++;
	TRACE (i, "p");
#endif
	i = 0;
	i++;

	/* read the value of TCR1 from the TPURAM, at offset 0xec */
	return MC68332_TPURAM_READ16 (0xec);
}

uint32_t mc68332_oc_read (void) {
	uint32_t curr;

	/* get the current value of TCR1 */
	curr = mc68332_oc_tpu_read ();

	/* modulo subtract the current value of TCR1 to the last read by
	 * _mc68332_oc_latch */
	if (curr <= mc68332_oc) curr += 0x10000;

	/* FIXME -- return the number of us */
	return  (curr - mc68332_oc) / 4;
}

