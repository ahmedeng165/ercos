/*
 * i386 uart functions declarations and macros for ERCOS-RT
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
#ifndef SERIAL_H_
#define SERIAL_H_

#include <public/config.h>
#include <arch.h>

/********************	SERIAL PORT REGISTERS	*******************************/
//!		Base address
#define SERIAL_COM1_BASE_ADDR		base	//0x3F8
//!		Transmitter Holding Buffer(Write, DLAB=0)
#define SERIAL_THR_REG				(SERIAL_COM1_BASE_ADDR+0)
//!		Receiver Buffer	(Read, DLAB=0)
#define	SERIAL_RBR_REG				(SERIAL_COM1_BASE_ADDR+0)
//!		Divisor Latch Low Byte(Read/Write, DLAB=1)
#define SERIAL_DLL_REG				(SERIAL_COM1_BASE_ADDR+0)
//!		Interrupt Enable Register(Read/Write, DLAB=0)
#define SERIAL_IER_REG				(SERIAL_COM1_BASE_ADDR+1)
//!		Divisor Latch High Byte(Read/Write, DLAB=1)
#define SERIAL_DLM_REG				(SERIAL_COM1_BASE_ADDR+1)
//!		Interrupt Identification Register(Read)
#define SERIAL_IIR_REG				(SERIAL_COM1_BASE_ADDR+2)
//!		FIFO Control Register(Write)
#define SERIAL_FCR_REG				(SERIAL_COM1_BASE_ADDR+2)
//!		Line Control Register(Read/Write)
#define SERIAL_LCR_REG				(SERIAL_COM1_BASE_ADDR+3)
//!		Modem Control Register(Read/Write)
#define SERIAL_MCR_REG				(SERIAL_COM1_BASE_ADDR+4)
//!		Line Status Register(Read)
#define	SERIAL_LSR_REG				(SERIAL_COM1_BASE_ADDR+5)
//!		Modem Status Register(Read)
#define	SERIAL_MSR_REG				(SERIAL_COM1_BASE_ADDR+6)
//!		Scratch Register(Read/Write)			//useless
#define SERIAL_SR_REG				(SERIAL_COM1_BASE_ADDR+7)
/***************************	SERIAL PORT CONFIG	***************************/
/* Line Control Register (LCR) bits */
#define LCR_STB		0x04
#define LCR_PEN		0x08
#define LCR_EPS		0x10
#define LCR_STICKP		0x20
#define LCR_BREAK		0x40
#define LCR_DLAB		0x80

/* Line Status Register (LSR) bits */
#define LSR_DR			0x01
#define LSR_OE			0x02
#define LSR_PE			0x04
#define LSR_FE			0x08
#define LSR_BI			0x10
#define LSR_THRE		0x20
#define LSR_TEMT		0x40

/* Modem Control Register (MCR) bits */
#define MCR_DTR		0x01
#define MCR_RTS		0x02
#define MCR_OUT1		0x04
#define MCR_OUT2		0x08
#define MCR_LOOP		0x10

/* Modem Status Register (MSR) bits */
#define MSR_DCTS		0x01
#define MSR_DDSR		0x02
#define MSR_TERI		0x04
#define MSR_DDCD		0x08
#define MSR_CTS		0x10
#define MSR_DSR		0x20
#define MSR_RI			0x40
#define MSR_DCD		0x80

/* Interrupt Identification Register (IIR) bits */
#define IIR_PENDING	0x01
#define IIR_PRIM		0x06
#define IIR_SEC		0x04
#define IIR_THIRD		0x02
#define IIR_FORTH		0x00
#define IIR_INTR(iir)	((iir) & 0x06)

#define IIR_DDCD		0x08
#define IIR_CTS		0x10
#define IIR_DSR		0x20
#define IIR_RI		0x40
#define IIR_DCD		0x80

/* Interrupt Enable Register (IER) bits */
#define IER_IER0		0x01
#define IER_IER1		0x02
#define IER_IER2		0x04
#define IER_IER3		0x08


#define SERIAL_DLM		0x00
#define SERIAL_DLL		(CONFIG_UART_CLK/(16*CONFIG_BAUDIOS))
#define SERIAL_CONFIG_IER			0x00 	//(IER_IER1 | IER_IER0 | IER_IER2)
#define SERIAL_CONFIG_FCR			0x07	// 1byte, clear fifos, enable fifo
#define SERIAL_CONFIG_FCR_NO_FIFO	0x00
#define SERIAL_CONFIG_LCR_1			0x80	// no parity, 1 stop, 8bits, DLAB=1
#define SERIAL_CONFIG_LCR_0			0x00	// DLAB=0
#define SERIAL_CONFIG_MCR			0x00	//RTS=DTR=OUT1=OUT2=0	(BEFORE)//0x0B	// RTS, DTR, auxoutput2
#define SERIAL_PORT_IRQ		0x04

#define SERIAL_WRITE8 		outportb

#define SERIAL_READ8 		inportb

void serial_init(void);
void serial_tx(char val);
unsigned char serial_rx(void);
//void uart_handler(void);
void serial_close(void);

#endif /* SERIAL_H_ */
