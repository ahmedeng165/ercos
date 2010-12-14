
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

/* NOTES --
 * At the present moment, the kernel interrupts itself (interrupt handlers) and
 * generate trace events.  This has to be taken into consideration for the
 * analyzer process design and implementation */

#include <public/glue.h>
#include <private/ercos-hal.h>

#include <ercos/kernel/irq.h>
#include <ercos/kernel/tracer.h>

#include <ercos/posix/trace.h>

#include <ercos/lib/string.h>
#include <ercos/lib/crc32.h>
#include <ercos/lib/stdio.h> /* printf */

#include <public/config.h>

/** The stream buffer where the events are logged */
struct _OS_TracerBuffer {

	/** The stream buffer */
	struct OS_TracerEvent buff[CONFIG_ERCOS_TRACER_BUFF_SIZE];

	/** The read cursor */
	volatile unsigned int rcur;

	/** The write cursor */
	volatile unsigned int wcur;

	/** The next sequence number for the reader thread */
	volatile uint16_t rseq;

	/** The next sequence number for the writer thread */
	volatile uint16_t wseq;
};

/** The stream buffer */
static struct _OS_TracerBuffer _tb;

/* return true if b is grater than a */
static int _OS_TracerGrater (uint16_t a, uint16_t b) {
	ASSERT (sizeof (_tb.buff[0].seq = sizeof (uint16_t)));
	ASSERT (sizeof (_tb.rseq) == sizeof (uint16_t));

	if (a < 0x8000) return a < b && b < (a + 0x8000);
	return a < b || b < (uint16_t) (a + 0x8000);
}

static void _OS_TracerSendEvent (const struct OS_TracerEvent * oev) {

#ifndef CONFIG_ERCOS_DROP_TRACE
	const char * ptr;
	int i;

	/* get a (const char *) cast to the event memory area */
	ptr = (const char *) oev;

	/* FIXME -- write the event to the serial line */
	for (i = 0; i < sizeof (struct OS_TracerEvent); i++) {
		ercos_hal_uart_tx (ptr[i]);
	}
#else
	//printf ("<%x %x %x %p> 678901234567890\n", oev->seq, oev->ev.posix_event_id, oev->ev.posix_fragment_id, oev->crc);
	//printf ("<%x %x %x> 2345678901234567890\n", oev->seq, oev->ev.posix_event_id, oev->ev.posix_fragment_id);
	//printf ("#");
	printf ("<########################################>");
#endif
}

void _OS_TracerReadAndSend (void) {
	struct OS_TracerEvent tmp;

	/* test if the next slot has a valid packet */
	if (! _OS_TracerGrater (_tb.rseq, _tb.buff[_tb.rcur].seq)) {
		//DEBUG ("power down because _tb.buff[rcur].seq=%p is not grater than _tb.rseq=%p", _tb.buff[_tb.rcur].seq, _tb.rseq);
		ercos_hal_power_down ();
		return;
	}

	/* copy the next slot to a temporal variable */
	memcpy (&tmp, _tb.buff + _tb.rcur, sizeof (struct OS_TracerEvent));

	/* if the last sequence number and this seq. nr aren't in order, we
	 * cannot send this packet */
	if (! _OS_TracerGrater (_tb.rseq, tmp.seq)) {
		//DEBUG ("power down because tmp.rseq=%p is not grater than _tb.seq=%p", tmp.seq, _tb.rseq);
		ercos_hal_power_down ();
		return;
	}

	/* send the event */
	//DEBUG ("p %x seq %x", _tb.rcur, tmp.seq);
	_OS_TracerSendEvent (&tmp);

	/* update pointers */
	_tb.rseq = tmp.seq;
	_tb.rcur++;
	if (_tb.rcur >= CONFIG_ERCOS_TRACER_BUFF_SIZE) _tb.rcur = 0;
}

void OS_TracerInit(void) {

	/* store in the first slot an event with a seq. nr. equal to 0xffff, to
	 * prevent the reader thread to send it */
	_tb.buff[0].seq = 0xffff;

	/* initialize write cursor */
	_tb.rcur = 0;
	_tb.wcur = 0;

	/* initialize the sequence numbers */
	_tb.rseq = 0xffff;
	_tb.wseq = 0;
}

void OS_TracerSaveEvent(const struct posix_trace_event_info * ev, uint32_t crc) {
	unsigned int i;

	/* get a sequence number for the event, increment the sequence number
	 * and allocate a slot in the buffer, atomically */
	OS_DisableIRQ ();
	i = _tb.wcur;
	_tb.wcur++;
	if (_tb.wcur == CONFIG_ERCOS_TRACER_BUFF_SIZE) _tb.wcur = 0;
	_tb.buff[i].seq = _tb.wseq;
	_tb.wseq++;
	OS_EnableIRQ ();

	/* copy the event to the last slot of the circular buffer and update
	 * the crc */
	memcpy (&_tb.buff[i].ev, ev, sizeof (struct posix_trace_event_info));
	_tb.buff[i].crc = crc32_append (crc, &_tb.buff[i].seq, sizeof (uint16_t));
}

void OS_TracerReaderThread (void) {
	/* repeat forever the "retrieve and send" process */
	while (1) _OS_TracerReadAndSend ();
}

