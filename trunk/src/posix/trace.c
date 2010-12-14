/*
    Copyright 2008 (c) by Cesar Rodriguez Ortega,
    Copyright 2005-2006 (c) by Aitor Viana Sanchez,
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://srg.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT.
*/

#include <public/glue.h>

#include <ercos/posix/trace.h>
#include <ercos/posix/intr.h>

#include <ercos/kernel/tracer.h>

#include <ercos/lib/crc32.h>
#include <ercos/lib/stdio.h>
#include <ercos/lib/string.h>

#include <public/config.h>
#include <ercos/syscall.h>

/** This array stores the last used event identifier for each trace level */
static uint8_t _trace_ids[CONFIG_ERCOS_TRACER_NLEVELS];

/* forward headers */
static void _posix_trace_save_event (struct posix_trace_event_info * ev);
static void _posix_trace_event_fill_common (struct posix_trace_event_info * ev,
		trace_event_id_t id);
static void _posix_trace_event_fill (struct posix_trace_event_info * ev,
		const void * buff, unsigned size, uint8_t frag);
/* end of forward headers */

int posix_trace_create (unsigned pid, trace_attr_t attr, trace_id_t *trace_id)
{
	/* we can only trace the current process -> minimal kernel */
	if(pid != 0)
	return -1;

	/* we have only one buffer stream so... */
	*trace_id = 1;

	return 0;
}

int posix_trace_eventid_open_with_level (const char *name,
		trace_event_id_t *event, trace_event_level_t level) {

	/* NOTE -- the name field is simply ignored, which is strictly not
	 * POSIX compliant */

	ASSERT (level < CONFIG_ERCOS_TRACER_NLEVELS);

	/* highest 16 bits will store the level and the lowest 16 bits the
	 * event identifier */
	*event = 0;
	POSIX_TRACE_LEVEL_SET (*event, level);

	/* in mutual exclusion: read the next trace id and increment the last
	 * used */
	intr_disable ();
	POSIX_TRACE_EVENTID_SET (*event, _trace_ids[level]);
	_trace_ids[level]++;
	intr_enable ();

	return 0;
}

int posix_trace_eventid_open (const char *name, trace_event_id_t *event) {
	return posix_trace_eventid_open_with_level (name, event,
			POSIX_TRACE_LEVEL_UNDEFINED);
}

int posix_trace_event (trace_event_id_t id, const void * buff, size_t size) {
	struct posix_trace_event_info ev;
	unsigned level, len, frag;

	/* get the event level */
	level = POSIX_TRACE_LEVEL_GET (id);
	ASSERT (level < CONFIG_ERCOS_TRACER_NLEVELS);
	ASSERT (size <= (TRACE_PAYLOAD_SIZE * 255));

	/* if the trace data fits into one event, the fragment id is 0xff; in
	 * the opposite case, it is 1 */
	frag = size <= TRACE_PAYLOAD_SIZE ? 0xff : 1;

	/* fill common fields for all fragments, even it there is only one
	 * fragment */
	_posix_trace_event_fill_common (&ev, id);

	/* if the trace data is too longer to fit into one trace event, it will
	 * be fragmented; all the fragments preserve the same event id and show
	 * an incremental fragment id; the last has fragment id equal to 0xff
	 * if data was not fragmented (ie, fits into one event) and equal to
	 * zero if there was any previous fragment */
	do {
		/* calculate the buffer size to store in this trace packet (event) */
		len = (size >= TRACE_PAYLOAD_SIZE) ? TRACE_PAYLOAD_SIZE : size;

		/* fill the event structure */
		_posix_trace_event_fill (&ev, buff, len, frag);

		/* save the event */
		_posix_trace_save_event (&ev);

		/* update pointers */
		buff += len;
		size -= len;

		/* increment the fragment id if it is not the last fragment */
		frag = size > TRACE_PAYLOAD_SIZE ? frag + 1 : 0;

	} while (size);

	return 0;
}

static void _posix_trace_save_event (struct posix_trace_event_info * ev) {
	uint32_t crc;

	/* calculate the CRC */
	crc = crc32 (ev, sizeof (struct posix_trace_event_info));

	/* store the header information in the kernel trace stream buffer */
#ifndef CONFIG_ERCOS_DROP_APP_TRACE
	ASSERT (POSIX_TRACE_LEVEL_GET (ev->posix_event_id) !=
			POSIX_TRACE_LEVEL_KERNEL);
	SYS_TracerSaveEvent (ev, crc);
#endif
}

static void _posix_trace_event_fill_common (struct posix_trace_event_info * ev,
		trace_event_id_t id) {
	/* fill timestamp */
	clock_gettime (CLOCK_REALTIME, &ev->posix_timestamp);

	/* other fields */
#if 0
	ev->posix_pid = 0;
	ev->posix_prog_address = 0x0;
#endif
	ev->posix_thread_id = SYS_GetCurrentTid ();
	ev->posix_event_id = id;
}

static void _posix_trace_event_fill (struct posix_trace_event_info * ev,
		const void * buff, unsigned size, uint8_t frag) {

	/* fill the event fields */
	memcpy (&ev->posix_data, buff, size);
	ev->posix_len = size;
	ev->posix_fragment_id = frag;
}

