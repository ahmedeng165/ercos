
/*
    Copyright 2008 (c) by Cesar Rodriguez Ortega,
    Copyright 2005-2006 (c) by Aitor Viana Sanchez,
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://atc1.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT.
*/

/** Kernel tracing primitives.  Note that trace events generated from code in
 * this file use the POSIX trace facilities, and the inclusion of the file
 * <posix/trace.h> from this file means a break in the abstraction layer
 * organization.
 * \file include/kernel/ktracer.h
 */

#ifndef _KERNEL_KTRACER_H_
#define _KERNEL_KTRACER_H_

#include <public/glue.h> /* DEBUG & ASSERT */
#include <ercos/lib/stdio.h> /* printf */

#include <ercos/posix/trace.h>
#include <ercos/kernel/tracer.h>
#include <ercos/kernel/time.h>
#include <ercos/lib/crc32.h>

/* kiwi trace event identifiers */
#define KTRACE_CREATE		POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 0)
#define KTRACE_EXIT		POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 1)

#define KTRACE_READY		POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 2)
#define KTRACE_UNREADY		POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 3)
#define KTRACE_EXEC		POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 4)

#define KTRACE_LOCK		POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 5)
#define KTRACE_UNLOCK		POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 6)

#define KTRACE_PRIO		POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 7)

/* system call event identifiers */
#define KTRACE_SYS_SLEEP	POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 100)
#define KTRACE_SYS_ABSSLEEP	POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 101)
#define KTRACE_SYS_TASKCREATE	POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 102)
#define KTRACE_SYS_SEMWAIT	POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 103)
#define KTRACE_SYS_SEMTIMEDWAIT	POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 104)
#define KTRACE_SYS_SEMPOST	POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 105)
#define KTRACE_SYS_SEMTRYWAIT	POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 106)
#define KTRACE_SYS_MUTEXINIT	POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 107)
#define KTRACE_SYS_MUTEXLOCK	POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 108)
#define KTRACE_SYS_MUTEXUNLOCK	POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 109)
#define KTRACE_SYS_MUTEXTRYLOCK	POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 110)
#define KTRACE_SYS_THREADEXIT	POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 111)
#define KTRACE_SYS_IRQMARKEVENT	POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 112)

/* other interesting events */
#define KTRACE_HANDLEEVENTS	POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 200)
#define KTRACE_CLOCKEVENT	POSIX_TRACE_EVENT (POSIX_TRACE_LEVEL_KERNEL, 201)

/* FIXME -- check the real overhead of making this function inline */
/* an inline function to fill the event and call the primitive to store the
 * event */
static inline void _ktr (unsigned id, tid_t tid, uint32_t arg1,
		uint32_t arg2) {

	struct posix_trace_event_info ev;
	OS_Timeval_t tm;
	uint32_t crc;

	/* this function relies that an event can store at least 8 bytes */
	ASSERT (TRACE_PAYLOAD_SIZE >= 8);

	/* get current time */
	OS_TimeGetTime (&tm);

	/* fill the event structure fields */
	* (uint32_t *) ev.posix_data = arg1;
	* (uint32_t *) (ev.posix_data + 4) = arg2;
	ev.posix_fragment_id = 0xff;
	ev.posix_len = 8;
	ev.posix_event_id = id;
	ev.posix_thread_id = tid;
#if 0
	ev.posix_prog_address = 0;
	ev.posix_pid = 0;
#endif

	/* fill event timestamp */
	ev.posix_timestamp.tv_nsec = tm.useconds * 1000;
	ev.posix_timestamp.tv_sec = tm.seconds;

	/* the event structure is initialize; calculate the crc */
	crc = crc32 (&ev, sizeof (struct posix_trace_event_info));

	/* now store the event in the trace stream */
	OS_TracerSaveEvent (&ev, crc);
}

#ifdef CONFIG_ERCOS_KERNEL_TRACER
#define KTRACE(id,arg1,arg2)			_ktr (id, 0, arg1, arg2)
#else
#define KTRACE(id,arg1,arg2)
#endif

#endif

