/*
    Copyright 2004-2005 (c) by Aitor Viana Sanchez, Pablo Parra Espada
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://atc1.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT on LEON3.
*/

/*!
 * \file arch/sparc/LEON3/serial.c
 * \brief Definition of the serial input/output functions
 */
#include <public/glue.h>

#include <leon2/serial.h>
#include <leon2/platform.h>

//------------------------------------------------------------------------------

void CPU_SerialTx (char car)
{
    /*
     * This variable is necessary in order to manipulate the UARTA.status
     * in a local register instead of a global one....pa molar
     */
    volatile uint32_t uart_status;

    /*  Waiting fot the channelis ready */
    while(!((uart_status = REGMAP.uart_1_status) & LEON2_UART_THE));

    REGMAP.uart_1_data = car;

    /*  Wait for the correct character transmision  */
    while(!((uart_status = REGMAP.uart_1_status) & LEON2_UART_THE));
}

void CPU_SerialRx (int *car)
{
    /*
     * This variable is necessary in order to manipulate the ERC32_MEC.uart_status
     * in a local register instead of a global one....pa molar
     */
    volatile uint32_t uart_status;

    /*  Waiting for ready data in the a channel */
    while(!((uart_status = REGMAP.uart_1_status) & LEON2_UART_DR));
    *car = REGMAP.uart_1_data;
}

void CPU_SerialRxDebug (int car)
{
}

