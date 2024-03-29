
/*
    Copyright 2004-2005 (c) by Aitor Viana Sanchez,
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://atc1.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT on ERC-32.
*/

/*!
 * \file include/kernel/time.h
 * \brief Kernel level functions and MACROS related to the timing services.
 */

#ifndef _KERNEL_TIME_H_
#define _KERNEL_TIME_H_

#include <public/glue.h>
#include <private/ercos-hal.h>
#include <ercos/kernel/types.h>
#include <ercos/kernel/task.h>
#include <ercos/config.h>
#include <ercos/kernel/irq.h>

/**
 * \brief OS Timeval structure.
 *
 * Stores the system time in seconds and microseconds.
 */
typedef struct _timeval
{
    /** Number of seconds   */
    uint32_t seconds;

    /** Number of microseconds  */
    uint32_t useconds;
}OS_Timeval_t;

#define CLICKS_PER_SEC      (1000000)
#define CLICKS_PER_MSEC     (CLICKS_PER_SEC / 1000)
#define CLICKS_PER_USEC     (CLICKS_PER_MSEC / 1000)

/**
 * \brief Time penalty inter calls in ticks
 *
 * The time penalty inter calls are about 40us and we must add the start/stop
 * overhead time.
 */
#define TIME_PENALTY        50ULL

/**
 * \brief Checks if the IRQ vector number corresponds to one of the timer IRQs.
 *
 * \param vec  IRQ vector.
 * \return     True if the vector corresponds to one of the timer IRQs.
 */
#define IS_TIMER_INTERRUPT(vec) ((vec == ERCOS_HAL_PIT_INTR) || (vec == ERCOS_HAL_CLOCK_INTR))
/******************************************************************************
 ******************************** GP TIMER ************************************
 * ***************************************************************************/
/**
 * \brief This routine init the system timming.
 */
void OS_TimeInit(void);

/**
 * \brief Halts and unregisters the timer IRQs.
 */
void OS_TimerHalt(void);

/**
 * This function allows to configure the one shot timer.
 *
 * \param _ticks  This parameter is the number of ticks interval.
 * \return        The routine returns 0 value in case of the thread is not
 *                blocked in the time queue. This means that the thread must be
 *                added into the ready queue. In any other case the routine
 *                returns -1 value.
 *
 * The function also stores the task in the time queue.
 *
 * In this sleep system call implementation the current task is been blocked.
 * For this reason we not insert the current task in the ready queue.
 */
int OS_DoSleep(OS_Time_t _ticks);


/******************************************************************************
 ***************************** REAL TIME CLOCK*********************************
 * ***************************************************************************/

/**
 * \brief Get the clock ticks.
 *
 * \return The routine returns a 64 bit integer that contains the system timer
 * ticks
 *
 * This routine retrive the clock ticks.
 */
OS_Time_t OS_TimeGetTicks(void);

void OS_TimeGetTime(OS_Timeval_t *_system_time);

/**
 * \brief Second half of the Clock IRQ service routine.
 */
void OS_ClockBottomISR(void);

/**
 * \brief Removes a given thread from the time queue.
 *
 * \param _task  Reference to the task to remove from the time queue.
 */
void OS_TimeRemoveFromQueue(OS_task_t *_task);

#endif

