/* TODO clean this file
 *
 * i386 time macros and function declarations for ERCOS-RT
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

#ifndef TIME_H_
#define TIME_H_

#include <public/glue.h>
#include <public/config.h>
#include <arch.h>		//TODO ver si se puede sacar esto de aqui

#define TIME_WRITE8		outportb
#define TIME_WRITE16	outportw
#define TIME_READ16		inportw
#define TIME_READ8		inportb

#define TIME_PIT_INTR 	48
#define TIME_CLOCK_INTR	32

//! periodo del apic
#define APIC_MAX_USECS		0xFFFFFFFF / ((APIC_CLOCK_FREQ/1000000)+1)
#define APIC_CLOCK_FREQ		apic_timer_hz //TODO cambiar??
#define TIME_PIT_PERIOD		CONFIG_ERCOS_CLICKS_PER_TICK

#define TIME_CLOCK_FREQ 	APIC_CLOCK_FREQ

#define FREQ				20	//cada 50ms un tick  (not used, see above define)

/***************************  8254  *********************************/
//!	1.193180 MHz
#define RATE				1193180
//!	prescaler del contador 0
#define COUNT_2				(RATE/FREQ)+1	//not used
//! control word
#define COUNT_MODE		(CW_COUNTER0 | CW_RW_LEAST_FIRST | CW_MODE_0 | CW_BINARY)

//! 50 ms (1/20 seg)
#define CALIBRATION_COUNT	RATE/20

#define COUNT_0_ADDR		0x40
#define COUNT_1_ADDR		0x41
#define COUNT_2_ADDR		0x42

#define CW_COUNTER0			0x00
#define CW_COUNTER1			0x40
#define CW_COUNTER2			0x80
#define CW_READ_BACK		0xC0

#define CW_RW_COUNTER_LATCH	0x00
#define CW_RW_LEAST_ONLY	0x10
#define CW_RW_MOST_ONLY		0x20
#define CW_RW_LEAST_FIRST	0x30

#define CW_MODE_0			0x00
#define CW_MODE_1			0x02
#define	CW_MODE_2			0x04
#define CW_MODE_3			0x06
#define CW_MODE_4			0x08
#define CW_MODE_5			0x0A

#define CW_BINARY			0x00
#define CW_BCD				0x01
//! cont 0, rt counter
#define COUNT_0_MODE		0x30
#define COUNT_2_READ		0x80
#define COUNT_0_READ		0x00
#define COUNT_CONTROL_REG	0x43

/***************************	IRQ Numbers		*******************************/
//!  PIC 1
#define TIMER_IRQ       	0x00

//!  PIC 2
#define	CMOSTIMER_IRQ		0x00

/******************************		PIC		***********************************/
/*	_____|___|__Interrupt addr_|_______|_trigger_|________|_1 PIC___|_�ICW4__**/
/*	ICW1:| 0 |	A7	A6	 A5	   |	1  | LTIM    |	 ADI  |	 SNGL	|   IC4  **/
#define PIC1_ICW1		0x11 	//activo por flanco, un pic, espero ICW4(pongo a 1 el ultimo porque cesar lo tiene... aunque la documentación....)
#define PIC2_ICW1		0x11	// *****  SURE?????

/*		 |______Interrupt vector____|_________|   **/
/*	ICW2:| T7	T6	 T5	  T4	T3  | X  X  X |   **/
#define PIC1_ICW2		TIME_CLOCK_INTR
#define PIC2_ICW2		TIME_CLOCK_INTR + 8

#define PIC1_ICW3		0x04	// pic slave conected to pin 2
#define PIC2_ICW3		0x02	// pic secundario conectado a linea 2 de IR

#define PIC1_ICW4		0x01	//8086 mode
#define PIC2_ICW4		0x01
/*	OCW1:	interrupt mask==> 1=masked interrupt	0=enabled interrupt	*/
#define PIC1_OCW1		0xFF	// masked all lines
#define PIC2_OCW1		0xFF	// masked all lines except line 0


#define PIC1_OCW3_IR	0x02	// read IR register on next RD pulse
#define PIC1_OCW3_IS	0x03	// read IS register on next RD pulse
#define ADDR_PIC1_0     0x20
#define ADDR_PIC1_1     0x21
#define ADDR_PIC2_0     0xA0
#define ADDR_PIC2_1     0xA1
#define PIC_EOI         0x20	//PIC end of interrupt
#define ADDR_CMOS_RTCLK_0	0x70
#define ADDR_CMOS_RTCLK_1	0x71

// ------------------------ APIC ----------------------------
#define 	IS_THERE_APIC	0x00000200
//! APIC Base MSR Address
#define 		APIC_MSR_BASE	0x1B
//! APIC MSR Enable/Disable flag (bit 11)
#define		MSR_APIC_ENABLE		0x00000800
//! APIC Base Address
#define         APIC_BASE 		0xFEE00000
//! ID Register
#define         APIC_ID         0x20
#define                 APIC_ID_MASK            (0x0F<<24)
#define                 GET_APIC_ID(x)          (((x)>>24)&0x0F)
//! Version Register
#define         APIC_LVR        0x30
#define                 APIC_LVR_MASK           0xFF00FF
#define                 GET_APIC_VERSION(x)     ((x)&0xFF)
#define                 GET_APIC_MAXLVT(x)      (((x)>>16)&0xFF)
#define                 APIC_INTEGRATED(x)      ((x)&0xF0)

//! Task Priority Register
#define         APIC_TASKPRI    0x80
//! Arbitration Priority Register
#define         APIC_ARBPRI     0x90
//! Processor Priority Register
#define         APIC_PROCPRI    0xA0
//! End of Interrupt Register
#define         APIC_EOI        0xB0
#define                 APIC_EOI_ACK            0x0  /* Write this to the EOI */
//! Reserved Register
#define         APIC_RRR        0xC0
//! Logical Destination Register
#define         APIC_LDR        0xD0
#define 			APIC_LDR_MASK		(0xFF<<24)
#define				GET_APIC_LOGICAL_ID(x)	(((x)>>24)&0xFF)
#define				SET_APIC_LOGICAL_ID(x)	(((x)<<24))
#define				APIC_ALL_CPUS		0xFF
//! Destination Format Register		//inicialmente son 1s
#define         APIC_DFR        0xE0
#define				APIC_DFR_VALUE	0xFFFFFFFF
//! Spurious Interrupt Vector Register
#define         APIC_SPIV       0xF0
//! In Service Register(bits 0:31)
#define         APIC_ISR        0x100
#define         APIC_ISR_NR     0x8     /* Number of 32 bit ISR registers. */
//! In Service Register(bits 32:63)
#define         	APIC_ISR2        0x110
//! In Service Register(bits 64:95)
#define         	APIC_ISR3        0x120
//! In Service Register(bits 96:127)
#define         	APIC_ISR4        0x130
//! In Service Register(bits 128:159)
#define         	APIC_ISR5        0x140
//! In Service Register(bits 160:191)
#define         	APIC_ISR6        0x150
//! In Service Register(bits 192:223)
#define         	APIC_ISR7        0x160
//! In Service Register(bits 224:255)
#define         	APIC_ISR8        0x170
//! Trigger Mode Register(bits 0:31)
#define         APIC_TMR        0x180
//! Trigger Mode Register(bits 32:63)
#define         	APIC_TMR2        0x190
//! Trigger Mode Register(bits 64:95)
#define         	APIC_TMR3        0x1A0
//! Trigger Mode Register(bits 96:127)
#define         	APIC_TMR4        0x1B0
//! Trigger Mode Register(bits 128:159)
#define         	APIC_TMR5        0x1C0
//! Trigger Mode Register(bits 160:191)
#define         	APIC_TMR6        0x1D0
//! Trigger Mode Register(bits 192:223)
#define         	APIC_TMR7        0x1E0
//! Trigger Mode Register(bits 224:255)
#define         	APIC_TMR8        0x1F0
//! Interrupt Request Register (bits 0:31)
#define         APIC_IRR        0x200
//! Interrupt Request Register (bits 32:63)
#define         	APIC_IRR2        0x210
//! Interrupt Request Register (bits 64:95)
#define         	APIC_IRR3        0x220
//! Interrupt Request Register (bits 96:127)
#define         	APIC_IRR4        0x230
//! Interrupt Request Register (bits 128:159)
#define         	APIC_IRR5        0x240
//! Interrupt Request Register (bits 160:191)
#define         	APIC_IRR6        0x250
//! Interrupt Request Register (bits 192:223)
#define         	APIC_IRR7        0x260
//! Interrupt Request Register (bits 224:255)
#define         	APIC_IRR8        0x270
//! Error Status Register
#define         APIC_ESR        0x280
//! Corrected machine check error count
#define			APIC_LVTCMCI	0x2F0
//! Interrupt Command Register (bits 0:31)
#define         APIC_ICR        0x300
//!	Interrupt Command Register (bits 32:63)
#define         APIC_ICR2       0x310
//! LVT Timer Register
#define         APIC_LVTT       0x320
//! Timer Register Fields
#define				ONE_SHOT_MODE		0x00000			// bit 17 = 0
#define				PERIODIC_MODE		0x20000			// bit 17 = 1
#define				TIMER_MASK			0xFFFEFFFF			// bit 16 = 1
#define				TIMER_NOT_MASKED	0x00000000			// bit 16 = 0
//! LVT Thermal Monitor Register
#define			APIC_LVTTM		0x330
//! LVT Performance Monitoring Counters Register
#define         APIC_LVTPC      0x340
//! LVT LINT0 Register
#define         APIC_LVT0       0x350
//! LVT LINT1 Register
#define         APIC_LVT1       0x360
//! LVT Error Register
#define         APIC_LVTERR     0x370
//!	Other LVT's Register Fields
#define				LVT_MASKED				0x10000		// bit 16 = 1
#define				LVT_NOT_MASKED			0x00000		// bit 16 = 0
#define				TRIGGER_LEVEL			0x8000		// bit 15 = 1
#define				TRIGGER_EDGE			0x0000		// bit 15 = 0
#define 			POLARITY_ACT_LOW		0x2000		// bit 13 = 1
#define 			POLARITY_ACT_HIGH		0x0000		// bit 13 = 0
#define				DELIVERY_MODE_FIXED		0x000		// bits 10-8 = 000
#define				DELIVERY_MODE_SMI		0x200		// bits 10-8 = 010
#define				DELIVERY_MODE_NMI		0x400		// bits 10-8 = 100
#define				DELIVERY_MODE_ExtINT	0x700		// bits 10-8 = 111
#define				DELIVERY_MODE_INIT		0x500		// bits 10-8 = 101
//! Initial Count Register (for Timer)
#define         APIC_TMICT      0x380
//! Current Count Register (for Timer)
#define         APIC_TMCCT      0x390
//! Divide Configuration Register (for Timer)
#define         APIC_TDCR       0x3E0
#define                 APIC_TDR_DIV_1          0xB
#define                 APIC_TDR_DIV_2          0x0
#define                 APIC_TDR_DIV_4          0x1
#define                 APIC_TDR_DIV_8          0x2
#define                 APIC_TDR_DIV_16         0x3
#define                 APIC_TDR_DIV_32         0x8
#define                 APIC_TDR_DIV_64         0x9
#define                 APIC_TDR_DIV_128        0xA

#ifdef 	CONFIG_APIC_DIVISOR_1
	#define  APIC_TDR_DIV	APIC_TDR_DIV_1
#elif	CONFIG_APIC_DIVISOR_2
	#define  APIC_TDR_DIV  APIC_TDR_DIV_2
#elif	CONFIG_APIC_DIVISOR_4
	#define  APIC_TDR_DIV  APIC_TDR_DIV_4
#elif	CONFIG_APIC_DIVISOR_8
	#define  APIC_TDR_DIV  APIC_TDR_DIV_8
#elif	CONFIG_APIC_DIVISOR_16
	#define  APIC_TDR_DIV  APIC_TDR_DIV_16
#elif	CONFIG_APIC_DIVISOR_32
	#define  APIC_TDR_DIV  APIC_TDR_DIV_32
#elif	CONFIG_APIC_DIVISOR_64
	#define  APIC_TDR_DIV  APIC_TDR_DIV_64
#else
	#define  APIC_TDR_DIV  APIC_TDR_DIV_128
#endif

/*	TODO sacar esto a asm.h
 * APIC Write operations
 */
static inline void apic_write(unsigned int reg, unsigned int v) {
	//this method saves 1 asm instruction
		asm volatile("movl %%eax, (%%edx)":   : "d" (APIC_BASE+reg), "a" (v));

//	*((volatile unsigned int *) (APIC_BASE+reg)) = v;
}
/* TODO sacar esto a asm.h
 * APIC Read operations
 */
static inline uint32_t apic_read(uint32_t reg) {

	return *((volatile unsigned int *) (APIC_BASE+reg));
}
/* TODO sacar esto a time.c
 * taken from linux source code */
static inline void init_apic_ldr(void){

	unsigned long val;

	apic_write(APIC_DFR, APIC_DFR_VALUE);
	val = apic_read(APIC_LDR) & ~APIC_LDR_MASK;
	val |= SET_APIC_LOGICAL_ID(1);
	apic_write(APIC_LDR, val);
}

// TODO sacar esto al time.c???
//! ciclos/segundo de la cpu (ver time_stamp_counter_c.c)
extern unsigned long long cpu_hz;
//! CPU Bus cycles/sec.
extern unsigned long long apic_timer_hz; //Detected as we calibrate the APIC timer

void clock_init(uint32_t _ticks);
void timer_config(void);
uint32_t ia32_get_timer(void);
extern void calibrate_tsc(void);
void enable_APIC(void);
void pic_init(void);

#endif /* TIME_H_ */
