/*
    Copyright 2005-2006 (c) by Aitor Viana Sanchez,
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://atc1.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT.
*/

/*
 * Formatted console output
 */

/*  Standard headers    */
#include <ercos/lib/string.h>
#include <ercos/lib/stdio.h>

/*  Architecture specific headers   */
#include <private/ercos-hal.h>

/*
 * Write a formatted string
 */

int sprintf(char * buffer, const char *fmt,...)
{
	va_list args;

	va_start(args, fmt);
	return vsprintf(buffer,fmt,args);
}

int printf(const char *fmt,...)
{
	char buffer[512];
	int ret, i;
	va_list args;

	va_start(args, fmt);

	/* vsprintf do the real work */
	ret = vsprintf (buffer, fmt, args);
	ASSERT (ret < 512);

	/* write the string to the serial port */
	for (i = 0; i < 512 && buffer[i]; i++) {
		/* prepend '\r' to complete carriage return '\n' */
		if (buffer[i] =='\n') ercos_hal_uart_tx ('\r');
		ercos_hal_uart_tx (buffer[i]);
	}

	return i;
}
