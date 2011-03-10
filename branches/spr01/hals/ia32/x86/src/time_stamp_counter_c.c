/*
 *
 *                  'T i m e - S t a m p   C o u n t e r'
 *
 *                                   C
 *
 *
 * File 'time_stamp_counter_c.c'
 *
 * Code taken from 'linux/arch/i386/kernel/time.c' (Kernel 2.4.18)
 * Copyright (C) 1991, 1992, 1995  Linus Torvalds
 *
 * ----------------------------------------------------------------------
 *---------------------------------------------------------------------------*/

#include <ercos/lib/stdio.h>
#include <asm.h>
#include <public/config.h>

#define CLOCK_TICK_RATE 1193180
#define HZ 100
#define LATCH  ((CLOCK_TICK_RATE + HZ/2) / HZ)

#define SPEED_SIGMA 200000000ULL
unsigned long long cpu_hz = 0; /* CPU cycles/sec.
								* Detected as we calibrate the TSC */

static unsigned long last_tsc_low;


void calibrate_tsc(void);

#define CALIBRATE_LATCH (5 * LATCH)
#define CALIBRATE_TIME  (5 * 1000020/HZ)

/*
 * calibrate_tsc
 *
 * Give value to 'cpu_hz'.
 * Too much 64-bit arithmetic here to do this cleanly in C, and for
 * accuracy's sake we want to keep the overhead on the CTC speaker (channel 2)
 * output busy loop as low as possible. We avoid reading the CTC registers
 * directly because of the awkward 8-bit access mechanism of the 82C54
 * device.
 */

void calibrate_tsc(void){

	uint8_t aux;
#ifndef CONFIG_NDEBUG
  DEBUG ("Calibrating TSC...\n ");
#endif
  /* Set the Gate high, disable speaker */
re_do_calibration:

  inportb(0x61,aux);
  outportb(0x61, (aux & ~0x02) | 0x01);

  /*
   * Now let's take care of CTC channel 2
   *
   * Set the Gate high, program CTC channel 2 for mode 0,
   * (interrupt on terminal count mode), binary count,
   * load 5 * LATCH count, (LSB and MSB) to begin countdown.
   */
  outportb(0x43, 0xb0);                       /* binary, mode 0, LSB/MSB, Ch 2 */
  outportb(0x42, CALIBRATE_LATCH & 0xff);     /* LSB of count */
  outportb(0x42, CALIBRATE_LATCH >> 8);       /* MSB of count */


  {
    unsigned long startlow, starthigh;
    unsigned long endlow, endhigh;
    unsigned long count;
    unsigned long eax=0, edx=1000000;
    unsigned long cpu_hz_long;

    rdtsc(startlow,starthigh);
    count = 0;
    do {
      count++;
      inportb(0x61,aux);
    } while ((aux & 0x20) == 0);
    rdtsc(endlow,endhigh);
    last_tsc_low = endlow;

    /* Error: ECTCNEVERSET */
    if (count <= 1) {
#ifndef CONFIG_NDEBUG
      DEBUG ("Error: COUNT NEVER SET\n");
#endif
      goto bad_ctc;
    }

    /* 64-bit subtract - gcc just messes up with long longs */
    asm volatile("subl %2,%0\n\t"
	    "sbbl %3,%1"
	    :"=a" (endlow), "=d" (endhigh)
	    :"g" (startlow), "g" (starthigh),
	    "0" (endlow), "1" (endhigh));

    /* Error: ECPUTOOFAST */
    if (endhigh) {
#ifndef CONFIG_NDEBUG
      DEBUG ("Error: CPU TOO FAST\n");
#endif
      goto bad_ctc;
    }

    /* Error: ECPUTOOSLOW */
    if (endlow <= CALIBRATE_TIME) {
#ifndef CONFIG_NDEBUG
      DEBUG ("Error: CPU TOO SLOW\n");
#endif
      goto bad_ctc;
    }

    asm volatile("divl %2"
	    :"=a" (endlow), "=d" (endhigh)
	    :"r" (endlow), "0" (0), "1" (CALIBRATE_TIME));

    // 'endlow' now is 2^32 * (1 / (TSC clocks per usec))

    /* report CPU clock rate in Hz.
     * The formula is (10^6 * 2^32) / (2^32 * 1 / (clocks/us)) =
     * clock/second. Our precision is about 100 ppm.
     */
    asm volatile("divl %2"
	    :"=a" (cpu_hz_long), "=d" (edx)
	    :"r" (endlow),
	    "0" (eax), "1" (edx));
#ifndef CONFIG_NDEBUG
    DEBUG("Detected %u Hz processor.\n", cpu_hz_long);
#endif
    cpu_hz = (unsigned long long)cpu_hz_long;
    if(cpu_hz > ((uint64_t)CONFIG_CPU_SPEED + (uint64_t)SPEED_SIGMA - 1)){
    	DEBUG("a recalibrar que sale algo raro: %u(unsigned)",cpu_hz);
    	goto re_do_calibration;		//TODO dangerous, goto puesto por mi
	}

    return;
  }

  /*
   * The CTC wasn't reliable: we got a hit on the very first read,
   * or the CPU was so fast/slow that the quotient wouldn't fit in
   * 32 bits..
   */
bad_ctc:
  cpu_hz = 0;

#ifndef CONFIG_NDEBUG
  DEBUG("Processor frequency not detected...\n");
#endif
}
