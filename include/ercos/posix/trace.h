
/*
    Copyright 2008 (c) by Cesar Rodriguez Ortega,
    Copyright 2004-2005 (c) by Aitor Viana Sanchez,
    Space Research Group,
    University of Alcala.

    For further information, please visit http://srg.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT project.
*/

/*
 * POSIX tracing primitives
 */

#ifndef _POSIX_TRACE_H_
#define _POSIX_TRACE_H_

#include <public/glue.h>
#include <ercos/posix/time.h>
#include <ercos/posix/pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/* special event ids */
#define POSIX_TRACE_START			0
#define POSIX_TRACE_STOP			1
#define POSIX_TRACE_OVERFLOW			2
#define POSIX_TRACE_RESUME			3

/** The per-packet payload size */
#define TRACE_PAYLOAD_SIZE			18

/* trace levels */
#define POSIX_TRACE_LEVEL_UNDEFINED		0
#define POSIX_TRACE_LEVEL_KERNEL		1

/* macros to get and set the event level and event id */
#define _M					0x0000ffff
#define _S					16
#define POSIX_TRACE_LEVEL_GET(eventid)		((eventid) >> _S)
#define POSIX_TRACE_LEVEL_SET(eventid,lev)	eventid |= lev << _S
#define POSIX_TRACE_EVENTID_GET(eventid)	((eventid) & _M)
#define POSIX_TRACE_EVENTID_SET(eventid,id)	eventid |= (id) & _M
#define POSIX_TRACE_EVENT(level,id)		(((level) << _S) | ((id) & _M))

typedef uint32_t trace_id_t;
typedef uint32_t trace_event_id_t;
typedef uint16_t trace_event_level_t;
typedef void * trace_attr_t;

/** POSIX trace packet structure (36 bytes) */
struct posix_trace_event_info {
	uint8_t posix_data[TRACE_PAYLOAD_SIZE];
	uint8_t posix_fragment_id;
	uint8_t posix_len;
	trace_event_id_t posix_event_id;
	uint32_t posix_thread_id;
#if 0
	/** Id of the event generator process */
	uint8_t posix_pid;

	/** Address at which trace point was invoked */
	void * posix_prog_address;
#endif
	struct timespec posix_timestamp;

} __attribute__ ((packed));

int posix_trace_eventid_open(const char *name, trace_event_id_t *event);
int posix_trace_event(trace_event_id_t event_id, const void *data_ptr,
		size_t data_len);
int posix_trace_create (unsigned pid, trace_attr_t attr, trace_id_t *trace_id);
int posix_trace_eventid_open_with_level (const char *name,
		trace_event_id_t *event, trace_event_level_t level);

#ifdef __cplusplus
}
#endif

#endif

