/*
 * i386 uart functions for ERCOS-RT
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
#include <x86/serial.h>
#include <public/posix.h> /* printf */

//base address of COM1
unsigned short int base;

/*
 * This function configures the uart.
 */
void serial_init(void) {
	uint8_t lcr;

	base = 0x3f8;//*(unsigned short int *) 0x400;

	SERIAL_WRITE8(SERIAL_LCR_REG, (LCR_PEN | LCR_EPS | 3));
	SERIAL_WRITE8(SERIAL_MCR_REG, SERIAL_CONFIG_MCR);
	//config modem

	SERIAL_READ8(SERIAL_LCR_REG,lcr);
	lcr |= SERIAL_CONFIG_LCR_1;
	SERIAL_WRITE8(SERIAL_LCR_REG, lcr);
	SERIAL_WRITE8(SERIAL_DLL_REG, SERIAL_DLL);
	//baud rate
	SERIAL_WRITE8(SERIAL_DLM_REG, SERIAL_DLM);
	//cargo valor
	lcr &= ~SERIAL_CONFIG_LCR_1;
	SERIAL_WRITE8(SERIAL_LCR_REG, lcr);

	//No fifos?
	SERIAL_WRITE8(SERIAL_FCR_REG, SERIAL_CONFIG_FCR_NO_FIFO);
	//habilitar interrupcion?
	SERIAL_WRITE8(SERIAL_IER_REG, SERIAL_CONFIG_IER);

	SERIAL_READ8(SERIAL_RBR_REG,lcr);
	//limpiar receptor
	SERIAL_READ8(SERIAL_LSR_REG, lcr);
	//limpiar estado de la linea
	SERIAL_READ8(SERIAL_MSR_REG, lcr);
	//limpiar estado del modem
	DEBUG("uart @!!\n");
}

/*
 * This function sends a character to the uart
 */
void serial_tx(char valor) {

	register unsigned char lsr;

	do {
		SERIAL_READ8(SERIAL_LSR_REG, lsr);
	} while (!(lsr & LSR_THRE));

	SERIAL_WRITE8(SERIAL_THR_REG, valor);

	return;
}
/*
 * This function receives a character from the uart
 */
unsigned char serial_rx(void) {

	register unsigned char lsr = 0, valor = 0;

	do {
		SERIAL_READ8(SERIAL_LSR_REG, lsr);
	} while (!(lsr & LSR_DR));

	SERIAL_READ8(SERIAL_RBR_REG, valor);
	return valor;
}

/*
 * \todo This function close the serial port (USELESS)
 */
void serial_close(void) {

	//! inhibir interrupcion
	SERIAL_WRITE8(SERIAL_IER_REG, 0);
	//! deshabilitar FIFO
	SERIAL_WRITE8(SERIAL_FCR_REG, 0);
	//! desconectar UART
	SERIAL_WRITE8(SERIAL_MCR_REG, 0);
}

