
/*
    Copyright 2008 (c) by Cesar Rodriguez Ortega,
    Copyright 2004-2005 (c) by Aitor Viana Sanchez,
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://srg.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT.
*/

/** Interface with the tracer implementation in the OS level (trace logging and
 * retrieving primitives).
 * \file include/kernel/tracer.h
 */

#ifndef _KERNEL_TRACER_H_
#define _KERNEL_TRACER_H_

#include <public/glue.h>
#include <ercos/posix/trace.h>

/** The trace buffer stores ERCOS_TRACER_BUFF_SIZE items of this type (42
 * bytes) */
struct OS_TracerEvent {
	struct posix_trace_event_info ev;
	uint32_t crc;
	uint16_t seq;
} __attribute__ ((packed));

/** This function initializes the stream buffer */
void OS_TracerInit(void);

/** This function logs a trace event */
void OS_TracerSaveEvent (const struct posix_trace_event_info * ev,
		uint32_t crc);

/** This function implements the reader thread and shall be executed from the
 * idle thread.  I will not return */
void OS_TracerReaderThread (void);

#endif
