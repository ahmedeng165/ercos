
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

#ifndef _POSIX_TIME_H_
#define _POSIX_TIME_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <public/glue.h>
#include <ercos/kernel/time.h>

/* for clock_nanosleep */
#define TIMER_ABSTIME		0x00000001

/* number of clock ticks */
typedef OS_Time_t clock_t;

struct timespec {
	time_t tv_sec;
	int32_t tv_nsec;
};

typedef enum {
	CLOCK_REALTIME = 0,
} clockid_t;

int nanosleep(const struct timespec *req, struct timespec *rem);
int clock_nanosleep (clockid_t clk_id, int flags, const struct timespec *req,
		struct timespec *rem);
int clock_gettime(clockid_t clk_id, struct timespec *res);

/**
 * \brief Converts a timespec structure into clock ticks.
 *
 * \param _timespec  The original timespec structure.
 * \param _ticks     The variable that will store the clock ticks value.
 */
#define _POSIX_timespec_to_ticks(_timespec,_ticks)                      \
    do {                                                                \
       _ticks = (uint64_t) (_timespec)->tv_sec * CLICKS_PER_SEC;        \
       if ((_timespec)->tv_nsec > 1000) {                               \
           _ticks += (_timespec)->tv_nsec / 1000ul;                     \
       }                                                                \
       /* DEBUG ("ts->sec=%d ts->nsec=%d --> <%p %p> (%d)",             \
        (_timespec)->tv_sec,                                            \
        (_timespec)->tv_nsec, (long) (_ticks >> 32),                    \
        (long) (_ticks & 0xffffffff), (long) (_ticks & 0xffffffff)); */ \
    } while (0)


#ifdef __cplusplus
}
#endif

#endif

