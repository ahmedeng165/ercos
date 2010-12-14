/*
    Copyright 2004-2005 (c) by Aitor Viana Sanchez,
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://atc1.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT.
*/

/*
 * File:    $Source$
 *
 * $Id: time.c 1281 2008-06-17 18:24:57Z cesar $
 *
 * $Revision: 1281 $
 * $Date: 2008-06-17 20:24:57 +0200 (mar, 17 jun 2008) $
 * Last CVS commit by $Author: cesar $
 */

#include <public/glue.h>
#include <ercos/posix/time.h>
#include <ercos/syscall.h>
#include <ercos/lib/stdio.h> /* for ASSERT */

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

/*
 * Function:    int nanosleep(const struct timespec *req, struct timespec *rem)
 */
int nanosleep(const struct timespec *req, struct timespec *rem)
{
    uint16_t ret_val = 0;
    OS_Time_t clicks;

    _POSIX_timespec_to_ticks(req, clicks);

    SYS_Sleep(clicks);

    return ret_val;
}

int clock_gettime(clockid_t clk_id, struct timespec *res)
{
    OS_Timeval_t time;

    ASSERT (clk_id == CLOCK_REALTIME);
    SYS_TimeGetTime(&time);
    res->tv_nsec = time.useconds * 1000;
    res->tv_sec = time.seconds;
    return 0;
}

int clock_nanosleep (clockid_t clk_id, int flags, const struct timespec *req,
		struct timespec *rem) {

	OS_Time_t us;

	/* only CLOCK_REALTIME clock is supported */
	if (clk_id != CLOCK_REALTIME) return -1;

	/* translate the requested time to ticks (us) */
	_POSIX_timespec_to_ticks (req, us);

	/* absolute sleep ? */
	if (flags & TIMER_ABSTIME) {
		SYS_AbsSleep (us);
		return 0;
	}

	/* relative sleep */
	SYS_Sleep (us);
	return 0;
}

