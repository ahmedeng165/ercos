
/*
 * Generic MC68332 board HAL for ERCOS-RT -- timing primitives
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

#include <public/config.h> /* for DEBUG */
#include <public/posix.h> /* for printf */
#include <ercos-m68k.h>
#include <public/ercos-lah.h> /* calls to ercos */
#include <mc68332/sim.h>
#include <mc68332/time.h>
#include <mc68332/intr.h>
#include <mc68332/crt0.h>

#include <mc68332/oc.h> /* DEBUG */

/* programable interrupt timer irq level */
#define _TIME_PIT_IRQL 0x0300

/* timer status */
#define CLOCK_OFF		0x0
#define CLOCK_WITHIN		0x1
#define CLOCK_EXACT		0x2
#define CLOCK_BEYOND		0x4
#define CLOCK_ON		(CLOCK_BEYOND | CLOCK_EXACT | CLOCK_WITHIN)

/* ** global variables ** */
volatile int mc68332_pit_interrupted = 0;
volatile uint32_t mc68332_pit_countdown = 0;
/* ** end of global variables ** */

/* ** private variables */
static uint32_t _mc68332_pit_reminder = 0;
static int _mc68332_pit_status = 0;
/* ** end of private variables */

#if 0
void test_tpu_oc_hack (void) {
	/* FIXME -- test the TPU hack */
	volatile int i;
	uint32_t a, a1, b, c, d, e, e1, e2, f, g, h, j, k, l, m, n;
	volatile uint32_t v;
#define WAIT(ms) for (i = 0; i < (int) (280.5 * ms); i++)
#define WAIT2(ms) for (i = 0; i < (int) (289 * ms); i++)

#if 0
	mc68332_oc_init ();
	mc68332_oc_latch ();
	a = mc68332_oc_read ();
	b = mc68332_oc_read ();
	c = mc68332_oc_read ();
	WAIT (2);
	d = mc68332_oc_read ();
	WAIT (2);
	e = mc68332_oc_read ();
	WAIT (4);
	f = mc68332_oc_read ();
	WAIT (5);
	g = mc68332_oc_read ();

	mc68332_oc_latch ();
	h = mc68332_oc_read ();
	WAIT (1);
	j = mc68332_oc_read ();
	WAIT (3);
	k = mc68332_oc_read ();

	TRACE (a, "d"); /* 0 */
	TRACE (b, "d"); /* 0 */
	TRACE (c, "d"); /* 0 */
	TRACE (d, "d"); /* 2 */
	TRACE (e, "d"); /* 4 */
	TRACE (f, "d"); /* 8 */
	TRACE (g, "d"); /* 13 */

	TRACE (h, "d"); /* 0 */
	TRACE (j, "d"); /* 1 */
	TRACE (k, "d"); /* 4 */
#endif

	//printf ("a");
	a = mc68332_pit_read ();
	a1 = mc68332_pit_read ();
	WAIT2 (2);
	b = mc68332_pit_read ();
	WAIT2 (2); /* 2 */
	c = mc68332_pit_read ();
	WAIT2 (4); /* 4 */
	d = mc68332_pit_read ();
	WAIT2 (8); /* 8 */
	e = mc68332_pit_read ();
	WAIT2 (1); /* 16 */
	e1 = mc68332_pit_read ();
	WAIT2 (1); /* 17 */
	e2 = mc68332_pit_read ();
	WAIT2 (6); /* 18 */
	f = mc68332_pit_read ();
	WAIT2 (8); /* 24 */
	g = mc68332_pit_read ();
	WAIT2 (8); /* 32 */
	h = mc68332_pit_read ();
	WAIT2 (8);
	j = mc68332_pit_read ();
	WAIT2 (8);
	k = mc68332_pit_read ();
	WAIT2 (8);
	l = mc68332_pit_read ();
	WAIT2 (8);
	m = mc68332_pit_read ();
	WAIT2 (8);
	n = mc68332_pit_read ();

	TRACE (a, "d"); /* 0 */
	TRACE (a1, "d"); /* 0 */
	TRACE (a1 - a, "d"); /* 0 */
	TRACE (b, "d"); /* 0 */
	TRACE (c, "d"); /* 0 */
	TRACE (d, "d"); /* 0 */
	TRACE (e, "d"); /* 4 */
	TRACE (e1, "d"); /* 4 */
	TRACE (e2, "d"); /* 4 */
	TRACE (f, "d"); /* 4 */
	TRACE (g, "d"); /* 4 */
	TRACE (h, "d"); /* 4 */
	TRACE (j, "d"); /* 4 */
	TRACE (k, "d"); /* 4 */
	TRACE (l, "d"); /* 4 */
	TRACE (m, "d"); /* 4 */
	TRACE (n, "d"); /* 4 */

	n = mc68332_pit_read ();
	for (i = 0; i < 2000; i++) {
		v = mc68332_pit_read ();
		v = mc68332_pit_read ();
		v = mc68332_pit_read ();
		v = mc68332_pit_read ();
		v = mc68332_pit_read ();
	}
	TRACE (n, "d");
	TRACE (v, "d");
	TRACE ((v - n) / 10000, "d");
}
#endif

void mc68332_pit_init (void) {
	uint16_t val;

	DEBUG ("Initializing pit on irq %d; vector %#x; %dus period (mod. %#x)",
			_TIME_PIT_IRQL >> 8, TIME_PIT_INTR, TIME_PIT_PERIOD,
			TIME_PIT_MODULUS);

	mc68332_pit_countdown = TIME_PIT_RELOAD;
	_mc68332_pit_status = CLOCK_OFF;

	val = _TIME_PIT_IRQL | (TIME_PIT_INTR & 0xff);
	MC68332_SIM_WRITE16 (MC68332_SIM_PICR, val);

	/* Periodic Interrupt Timer Register:
	 * For a 32.768kHz EXTAL freq. (no prescaler): Modulus / 8192 = period
	 * Some interesting values:
	 * 0x0029 -> 5004.8828125 us
	 * 0x0040 -> 7812.5 us
	 * 0x0047 -> 8666.9921875 us
	 * 0x0052 -> 10009.765625 us
	 * 0x0055 -> 10375.9765625 us
	 * 0x0080 -> 15625 us
	 */

	/* less than 7.5ms is not a good idea... */
	ASSERT (TIME_PIT_PERIOD >= 7500);
	MC68332_SIM_WRITE16 (MC68332_SIM_PITR, TIME_PIT_MODULUS);

	/* initialize the tpu hack for fine timing */
	mc68332_oc_init ();
	mc68332_oc_latch ();
}

uint32_t mc68332_pit_read (void) {
	uint32_t cd, oc;

	/* disable irqs in user mode */
	mc68332_trap2 ();

	oc = mc68332_oc_read ();
	cd = mc68332_pit_countdown;
	if (_mc68332_pit_status & CLOCK_WITHIN) cd += _mc68332_pit_reminder;

	/* enable irqs in user mode */
	mc68332_trap1 ();

	/* DEBUG ("Returning %d", (TIME_PIT_RELOAD - cd) * TIME_PIT_PERIOD); */
	return (TIME_PIT_RELOAD - cd) * TIME_PIT_PERIOD + oc;
}

void mc68332_clock_init (uint32_t us) {
	register uint32_t cd;

	/* DEBUG ("(us=%d); cndw=%d; remdr=%d; status=%d;", us,
			mc68332_pit_countdown, _mc68332_pit_reminder,
			_mc68332_pit_status); */

	/* calculate the countdown required to wait for the requested time and
	 * add 1 to it; this will reduce the number of cases when the ercos
	 * clock handler reprograms the clock timer to sleep for a bit more
	 * time, due to the fact that the task was awaken before requested */
	cd = us / TIME_PIT_PERIOD;
	cd += 1;
	/* DEBUG ("us=%d cd=%d", us, cd); */

	/* disable interrupts */
	mc68332_trap2 ();

	/* if timer was already programmed we have to reset it: if it was
	 * on WITHIN mode, we just add the reminder to the countdown and now
	 * the timer is reset. If it was in BEYOND or EXACT mode, nothing has
	 * to be done! */
	if (_mc68332_pit_status == CLOCK_WITHIN) {
		mc68332_pit_countdown += _mc68332_pit_reminder;
	}

	/* the clock countdown will end before the next timer interrupt */
	if (cd < mc68332_pit_countdown) {
		_mc68332_pit_reminder = mc68332_pit_countdown - cd;
		mc68332_pit_countdown = cd;
		_mc68332_pit_status = CLOCK_WITHIN;
		/* DEBUG ("status=WITHIN, cndw=%d, rmdr=%d",
				mc68332_pit_countdown, _mc68332_pit_reminder); */

	/* the clock countdown will end after the next timer interrupt */
	} else if (cd > mc68332_pit_countdown) {
		_mc68332_pit_reminder = cd - mc68332_pit_countdown;
		_mc68332_pit_status = CLOCK_BEYOND;
		/* DEBUG ("status=BEYOND, cndw=%d, rmdr=%d",
				mc68332_pit_countdown, _mc68332_pit_reminder); */

	/* the clock countdown and the timer countdown lies in the same time
	 * mark :) */
	} else {
		/* DEBUG ("status=EXACT, cndw=%d", mc68332_pit_countdown); */
		_mc68332_pit_status = CLOCK_EXACT;
	}

	/* enable interrupts */
	mc68332_trap1 ();
}

/* This function is called from mc68332_pit_entry (in crt0.S) with interrupt
 * disabled (intr. mask 7).  We don't need to increment intr. nesting variable
 * (src/intr.c) because ercos clock and pit handlers don't make use of
 * ercos_hal_intr_ongoing primitive! But note that this may change in the
 * future! */
void mc68332_pit_handler (unsigned long ident, struct m68k_hwcontext * ctx) {

	ASSERT (mc68332_pit_countdown == 0);
	/* DEBUG ("ctx->sr=%p sr=%p cndw=%d rmdr=%d stat=%d", ctx->sr,
			m68k_get_sr (), mc68332_pit_countdown,
			_mc68332_pit_reminder, _mc68332_pit_status); */
#ifndef CONFIG_NDEBUG
	printf ("@");
#endif

	/* the clock has ended its countdown, so we simulate a clock interrupt */
	if (_mc68332_pit_status & CLOCK_WITHIN) {
		/* DEBUG ("** Simulating clock interrupt"); */
		mc68332_pit_countdown = _mc68332_pit_reminder;
		_mc68332_pit_status = CLOCK_OFF;

		/* call clock architecture-independent handler */
		m68k_intr_level (_TIME_PIT_IRQL);
		ercos_lah_intr_handler (ERCOS_HAL_CLOCK_INTR);

	/* the pit has ended its countdown and the clock has to be reprogrammed
	 */
	} else if (_mc68332_pit_status & CLOCK_BEYOND) {

		/* DEBUG ("** Pit interrupt"); */
		mc68332_pit_countdown = _mc68332_pit_reminder;
		_mc68332_pit_status = CLOCK_WITHIN;
		_mc68332_pit_reminder = TIME_PIT_RELOAD - _mc68332_pit_reminder;

		/* call pit architecture-independent handler */
		m68k_intr_level (_TIME_PIT_IRQL);
		ercos_lah_intr_handler (ERCOS_HAL_PIT_INTR);

	/* both pit and clock have ended their countdowns and both must issue
	 * an interrupt */
	} else if (_mc68332_pit_status & CLOCK_EXACT) {
		mc68332_pit_countdown = TIME_PIT_RELOAD;
		_mc68332_pit_status = CLOCK_OFF;

		/* call both pit and clock architecture-independent handlers */
		m68k_intr_level (_TIME_PIT_IRQL);
		ercos_lah_intr_handler (ERCOS_HAL_CLOCK_INTR);
		ercos_lah_intr_handler (ERCOS_HAL_PIT_INTR);

	/* the pit has ended its countdown and need to be reloaded */
	} else {
		mc68332_pit_countdown = TIME_PIT_RELOAD;

		/* call pit architecture-independent handler */
		m68k_intr_level (_TIME_PIT_IRQL);
		ercos_lah_intr_handler (ERCOS_HAL_PIT_INTR);
	}

	/* perform the interrupt epilogue */
	mc68332_intr_epilogue (ctx);
}

