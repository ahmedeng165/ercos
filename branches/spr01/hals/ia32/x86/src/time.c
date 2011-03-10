/*
 * i386 time routines(pic, 8254, Local Apic) for ERCOS-RT
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

#include <x86/time.h>
#include <x86/misc.h>
#include <public/ercos-lah.h>
#include <public/config.h>


unsigned long long apic_timer_hz;

/*
 *--------------------------*
 *-- calibrate_apic_timer --*
 *--------------------------*
 *
 * Uses the PIT counter 2 (speaker).
 *
 * Code taken from linux sources
 */

#define LATCH  ((RATE + FREQ/2) / FREQ)
#define CALIBRATE_LATCH (5 * LATCH)
#define CALIBRATE_TIME  (5 * 1000020/FREQ)

static void calibrate_apic(void) {

	uint8_t aux;

	calibrate_tsc();

	if (cpu_hz == 0)
		// TSC not calibrated yet
		goto bad_calibration;

	/* Set the Gate high, disable speaker */
	inportb(0x61,aux);
	outportb(0x61, (aux & ~0x02) | 0x01);

	/*
	 * Now let's take care of PIT counter 2
	 *
	 * Set the Gate high, program PIT counter 2 for mode 0,
	 * (interrupt on terminal count mode), binary count,
	 * load 5 * LATCH count, (LSB and MSB) to begin countdown.
	 */
	outportb(COUNT_CONTROL_REG, 0xb0);
	/* binary, mode 0, LSB/MSB, Ch 2 */
	outportb(COUNT_2_ADDR, CALIBRATE_LATCH & 0xff);
	/* LSB of count */
	outportb(COUNT_2_ADDR, CALIBRATE_LATCH >> 8);
	/* MSB of count */

	{
		unsigned long startlow, starthigh;
		unsigned long endlow, endhigh;
		unsigned long count;
		uint32_t t1 = 0xFFFFFFFF;
		uint32_t t2;

		/*
		 * Start the counter for calibration.
		 * We assume that calibrate_tsc was already called.
		 */

		rdtsc(startlow, starthigh);
		apic_write(APIC_TMICT, t1);
		count = 0;
		do {
			count++;
			inportb(0x61, aux);
		} while ((aux & 0x20) == 0);
		t2 = apic_read(APIC_TMCCT);
		rdtsc(endlow, endhigh);

		/* Error: PIT NEVER SET */
		if (count <= 1) {
			DEBUG ("Error: PIT NEVER SET\n");
			goto bad_calibration;
		}

		/* 64-bit subtract - gcc just messes up with long longs */
		asm volatile("subl %2,%0\n\t"
				"sbbl %3,%1"
				:"=a" (endlow), "=d" (endhigh)
				:"g" (startlow), "g" (starthigh),
				"0" (endlow), "1" (endhigh));

		/* Error: CPU TOO FAST */
		if (endhigh) {
			DEBUG ("Error: APIC TOO FAST\n");
			goto bad_calibration;
		}

		/* Error: CPU TOO SLOW */
		if (endlow <= CALIBRATE_TIME) {
			DEBUG ("Error: APIC TOO SLOW\n");
			goto bad_calibration;
		}

		apic_timer_hz = ((t1 - t2) * cpu_hz) / endlow;

		DEBUG("Detected %u Hz APIC timer clock.\n",
				(unsigned long) apic_timer_hz);
		DEBUG("Max programmable time: %dsecs\n", (0xFFFFFFFF/apic_timer_hz));
		return;
	}

	/*
	 * The PIT wasn't reliable: we got a hit on the very first read,
	 * or the CPU was so fast/slow that the quotient wouldn't fit in
	 * 32 bits or the TSC hasn't been calibrated yet.
	 */
	bad_calibration: apic_timer_hz = 0;
	DEBUG("TSC's bad calibration\n");
}

/*
 * \todo this function is just for debug purpose and does have to be erased later*/
#ifndef CONFIG_NDEBUG
static void comprueba_APIC(void) {

	uint32_t aux;
	bool_t test_OK;

	test_OK=TRUE;

	aux = apic_read(APIC_LVT0);
	if (aux != (DELIVERY_MODE_ExtINT)){
		DEBUG("El registro LVT0 del apic vale 0x%x y tendría que valer 0x%x", aux, ( DELIVERY_MODE_ExtINT));
		test_OK=FALSE;
	}
	aux = apic_read(APIC_LVT1);
	if (aux != ( DELIVERY_MODE_NMI)){
		DEBUG("El registro LVT1 del apic vale 0x%x y tendría que valer 0x%x", aux, (DELIVERY_MODE_NMI));
		test_OK=FALSE;
	}
	aux = apic_read(APIC_LVTERR);
	if (aux != LVT_MASKED){
		DEBUG("El registro LVTERR del apic vale 0x%x y tendría que valer 0x%x", aux, LVT_MASKED);
		test_OK=FALSE;
	}
	aux = apic_read(APIC_LVTT);
	if (aux != (PERIODIC_MODE | TRIGGER_LEVEL | TIME_PIT_INTR)){
		DEBUG("El registro LVTT del apic vale 0x%x y tendría que valer 0x%x", aux, (LVT_MASKED | PERIODIC_MODE | TIME_PIT_INTR));
		test_OK=FALSE;
	}
	aux = apic_read(APIC_LVTPC);
	if (aux != LVT_MASKED){
		DEBUG("El registro LVTEPC del apic vale 0x%x y tendría que valer 0x%x", aux, LVT_MASKED);
		test_OK=FALSE;
	}
	aux = apic_read(APIC_TDCR);
	if (aux != APIC_TDR_DIV){ //Divide por 1.
		DEBUG("El registro TDCR del apic vale 0x%x y tendría que valer 0x%x", aux, APIC_TDR_DIV);
		test_OK=FALSE;
	}
	if(test_OK!=FALSE)
		DEBUG("APIC CORRECTO!\n");
}
#endif

/*
 * This function configures the PIT count
 */

void timer_config(void) {

	uint32_t cuenta;
	uint64_t tmp;

	apic_write(APIC_LVTT, (PERIODIC_MODE | TRIGGER_LEVEL | TIME_PIT_INTR));

	tmp = (uint64_t)((TIME_PIT_PERIOD * APIC_CLOCK_FREQ)/1000000);
	cuenta = (uint32_t) tmp;

	DEBUG("cuenta escrita en el apic: %d",cuenta);

	apic_write(APIC_TMICT, cuenta);
	apic_write(APIC_SPIV,0x000001FF);

#ifndef CONFIG_NDEBUG
	comprueba_APIC();
#endif
}

/*
 * This function program a count in the one shot timer
 */
void clock_init(uint32_t _ticks) {

	uint16_t count;
	uint64_t aux;

	irq_disable(); //the count have to be writen without interrupts

	TIME_WRITE8(COUNT_CONTROL_REG, COUNT_MODE);

	aux = (uint64_t)((_ticks * RATE)/1000000);
	count=(aux & 0xFFFF);

//	DEBUG("cuenta programada es %d, solicitado %d",count,_ticks);

	TIME_WRITE8(COUNT_0_ADDR,(count & 0xFF));
	TIME_WRITE8(COUNT_0_ADDR,((count >> 8) & 0xFF));

	PIC_WRITE8(ADDR_PIC1_1, 0xFE);	//habilita la interrupci�n

	irq_enable();
}

/*
 * This function get the timer counter value in micro seconds
 */
uint32_t ia32_get_timer(void) {

	uint64_t auxvar;
	uint32_t cnt;

	irq_disable();

	cnt=apic_read(APIC_TMCCT);

	irq_enable();

	auxvar = ((uint64_t)cnt*1000000)/APIC_CLOCK_FREQ;

	return (TIME_PIT_PERIOD - (uint32_t)auxvar); // in usecs
}

/*
 * This function initializes the programmable interrupt controller 8259
 */
void pic_init(void) {

	//! See anotations on time.h
	PIC_WRITE8(ADDR_PIC1_0, PIC1_ICW1);

	PIC_WRITE8(ADDR_PIC1_1, PIC1_ICW2);

	PIC_WRITE8(ADDR_PIC1_1, PIC1_ICW3);

	PIC_WRITE8(ADDR_PIC1_1, PIC1_ICW4);

	PIC_WRITE8(ADDR_PIC2_0, PIC2_ICW1);

	PIC_WRITE8(ADDR_PIC2_1, PIC2_ICW2);

	PIC_WRITE8(ADDR_PIC2_1, PIC2_ICW3);

	PIC_WRITE8(ADDR_PIC2_1, PIC2_ICW4);

	//! mask all interrupts
	PIC_WRITE8(ADDR_PIC1_1, PIC1_OCW1);

	PIC_WRITE8(ADDR_PIC2_1, PIC2_OCW1);

	DEBUG("PIC habilitado(?)");

}

/*
 * This function configures the Advanced programmable interrupt controller.
 */
void enable_APIC(void) {

	uint32_t eax, edx, lvr, isr;
	int i, j;

	rdmsr(APIC_MSR_BASE,eax,edx);
	DEBUG("número en eax=0x%x",(eax & 0xfff));
	if (eax & MSR_APIC_ENABLE) {
#ifndef CONFIG_NDEBUG
		uint32_t aux = 0;
		aux = (edx & 0x0000000f);
		aux <<= 20;
		eax >>= 12;
		aux |= (eax & 0x000fffff);
		DEBUG("APIC ya estaba habilitado... direcci�n: %x\n", (aux << 12));
#endif

	} else {

		DEBUG("No estaba habilitando el Apic, so we're going to enable it...\n");

		eax |= MSR_APIC_ENABLE;
		wrmsr(APIC_MSR_BASE,eax,edx);
	}

	lvr = apic_read(APIC_LVR);
	DEBUG("versión del apic: 0x%x, max num de ints: %d", GET_APIC_VERSION(lvr), GET_APIC_MAXLVT(lvr));
	DEBUG("ID del apic: 0x%x",GET_APIC_ID(apic_read(APIC_ID)));

	/* Pound the ESR really hard over the head with a big hammer - mbligh */
	apic_write(APIC_ESR,0);
	apic_write(APIC_ESR,0);
	apic_write(APIC_ESR,0);
	apic_write(APIC_ESR,0);

	/*
	 * Intel recommends to set DFR, LDR and TPR before enabling
	 * an APIC.  See e.g. "AP-388 82489DX User's Manual" (Intel
	 * document number 292116).  So here it goes...
	 */
	init_apic_ldr();

	/*
	 * Set Task Priority to not accept nothing
	 */
//	apic_write(APIC_TASKPRI, 0xFF);

	/*
	 * After a crash, we no longer service the interrupts and a pending
	 * interrupt from previous kernel might still have ISR bit set.
	 *
	 * Most probably by now CPU has serviced that pending interrupt and
	 * it might not have done the ack_APIC_irq() because it thought,
	 * interrupt came from i8259 as ExtInt. LAPIC did not get EOI so it
	 * does not clear the ISR bit and cpu thinks it has already serivced
	 * the interrupt. Hence a vector might get locked. It was noticed
	 * for timer irq (vector 0x31). Issue an extra EOI to clear ISR.
	 */
	for (i = APIC_ISR_NR - 1; i >= 0; i--) {
		isr = apic_read(APIC_ISR + i*0x10);
		for (j = 31; j >= 0; j--) {
			if (isr & (1<<j))
				apic_write(APIC_EOI, APIC_EOI_ACK);
		}
	}

	apic_write(APIC_LVT0, (DELIVERY_MODE_ExtINT));
	apic_write(APIC_LVT1, (DELIVERY_MODE_NMI));
	apic_write(APIC_LVTT, LVT_MASKED);

	/* Due to the Pentium erratum 3AP. */
	apic_write(APIC_ESR, 0);

	apic_write(APIC_LVTERR, LVT_MASKED);
	apic_write(APIC_ESR, 0);

	if(GET_APIC_MAXLVT(lvr)>=4)
		apic_write(APIC_LVTPC, LVT_MASKED);

	if(GET_APIC_MAXLVT(lvr)>=5)
		apic_write(APIC_LVTTM, LVT_MASKED);

	apic_write(APIC_TASKPRI, 0x00);

	apic_write(APIC_TDCR, APIC_TDR_DIV); //TODO Divide por X. (ver time.h)

	/* APIC ID = 1 force interrupt related */
	apic_write(APIC_ICR2,0x01000000);

	calibrate_apic();
}
