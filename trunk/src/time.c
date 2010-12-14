
/*
    Copyright 2004-2005 (c) by Aitor Viana Sanchez,
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://atc1.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT on ERC-32.
*/

/*
 * File:    $Source$
 *
 * $Id: time.c 1285 2008-06-23 15:36:09Z cesar $
 *
 * $Revision: 1285 $
 * $Date: 2008-06-23 17:36:09 +0200 (lun, 23 jun 2008) $
 * Last CVS commit by $Author: cesar $
 */

#include <public/glue.h>
#include <private/ercos-hal.h>

#include <ercos/kernel/time.h>
#include <ercos/kernel/ktracer.h>
#include <ercos/kernel/sched.h>
#include <ercos/kernel/traps.h>
#include <ercos/kernel/irq.h>
#include <ercos/kernel/irqev.h>
#include <ercos/kernel/list.h>
#include <ercos/kernel/sem.h>
#include <ercos/kernel/task.h>

#include <ercos/syscall.h>

//----------------------------------------------------------------------------
static uint32_t __div64_32(uint64_t *n, uint32_t base);

/** This macro return the firs thread in the time queue */
#define FIRST_TIME_THREAD   GET_ELEMENT(OS_task_t, time_list.first, time_node)

#define GET_TIME_THREAD(node)   GET_ELEMENT(OS_task_t, node, time_node)

/**
 * This variable stores the timer ticks
 */
volatile static OS_Time_t timer_tick_counter;

/** Time queue  */
static OS_List_t time_list;

/** Microseconds per second */
#define USECS_PER_SEC   1000000

/** \todo comentar esta etiqueta    */
#define OVERHEAD_CLICKS (ERCOS_HAL_PIT_PERIOD + 1)

/**
 * This MACRO normalizes the Timeval system structure.
 * FIXME -- the "else if" clause is really needed ?
 */
#define _OS_Timeval_normalize(t) {\
    if ((t)->useconds >= USECS_PER_SEC) { \
     (t)->useconds -= USECS_PER_SEC; \
       (t)->seconds++; \
   } else if ((int)(t)->useconds < 0) { \
       (t)->useconds += USECS_PER_SEC; \
       (t)->seconds--; \
   } \
}

static uint32_t oneshot_programed = 0;

#define ONESHOT_TIMER_NOT_PROGRAMMED()  (oneshot_programed = 0)

#define ONESHOT_TIMER_PROGRAMMED()      (oneshot_programed = 1)

#define IS_ONESHOT_TIMER_NOT_PROGRAMMED()   (0 == oneshot_programed)

#define OS_ProgramClock(_value)  \
do {    \
    ONESHOT_TIMER_PROGRAMMED();  \
    ercos_hal_clock_init(_value);   \
}while(0)


/** This structure stores the system time in seconds and microseconds   */
volatile static OS_Timeval_t OS_Time;

static uint32_t seconds_per_timer_tick = 0;

static uint32_t useconds_per_timer_tick = 0;

//----------------------------------------------------------------------------

/**
 * Init the General Purpose Timer.
 *
 * This routine initialize the General Purpose Timer by registering its
 * interrupt service routin but not activate the timer count.
 */
static void OS_TimerInit(void);

/**
 * Timer Interrupt Service Routine.
 *
 * \param   level:  This parameter is the interrupt level.
 *
 * This routine manage the timer interrupt. This timer is programmed when a task
 * need to sleep during a certain time interval.
 */
static void OS_TimerISR(int level);

/**
 * Init the system real time clock.
 *
 * The routine register a interrupt service routine for the real time clock,
 * configures the clock tick interval and reset the clock tick counter.
 */
static void OS_ClockInit(void);

/**
 * Interrupt service routine for the real time clock.
 *
 * \param   level:  This parameter is the interrupt level.
 */
static void OS_ClockISR(int level);

//----------------------------------------------------------------------------

/*
 * Function:    void OS_TimeInit(void)
 *
 * This routine init the system timming. The routine init the Real Time Clock
 * and the General Purpose Timer.
 */
void OS_TimeInit(void)
{
    /*  Init the Real Time Clock    */
    OS_ClockInit();
    /*  Init the General Purpose Timer  */
    OS_TimerInit();
}


/******************************************************************************
 ******************************** GP TIMER ************************************
 * ***************************************************************************/

/*
 * Function:    static void OS_TimerInit(void)
 *
 * This function initializes the General Purpose Timer. It basically registers
 * a timer interrupt service routine.
 */
static void OS_TimerInit(void)
{
    uint64_t clicks = OVERHEAD_CLICKS;

    /*  Register the General Purpose Timer ISR  */
    OS_RegisterIRQ(TIMER_TRAP, OS_TimerISR);

    /*  10ms real time clock tick configuration   */
    ercos_hal_pit_init ();

    /*  Reset the time queue    */
    OS_ListReset(&time_list);

    /*  Reset the counter   */
    timer_tick_counter = 0;

    /*  Init the system time structure  */
    OS_Time.seconds = OS_Time.useconds = 0;

    useconds_per_timer_tick = __div64_32(&clicks, CLICKS_PER_SEC) * CLICKS_PER_USEC;
    seconds_per_timer_tick = (uint32_t)clicks;
}

/*
 * Function:    static void OS_TimerISR(int level)
 *
 * This function is the timer interrupt routine
 */
static void OS_ClockISR(int _level)
{
    ONESHOT_TIMER_NOT_PROGRAMMED();

    KTRACE (KTRACE_CLOCKEVENT, 0, 0);

    /*  Mark the interrupt clock event    */
    OS_IRQMarkClockEvent();
}

void OS_ClockBottomISR(void)
{
    OS_task_t *task;
    OS_Node_t *pos;
    OS_Time_t current_time;
    OS_Time_t thread_time;
    OS_Time_t next_clock_val;         // the next clock value to be programmed

    if(OS_ListGetNum(&time_list) == 0)
    {

        OS_ListReset(&time_list);
        //DEBUG ("Reset de la lista de tiempos");

        return;
    }

    /*  This is the timeout of the first thread in the list */
    thread_time = FIRST_TIME_THREAD->abs_timeout;

    /*  Obtain the current system time  */
    current_time = OS_TimeGetTicks();

    if(thread_time > current_time)
    {

        next_clock_val = thread_time - current_time;

        if(next_clock_val < (OS_Time_t)OVERHEAD_CLICKS)
        {
            /*
             * If the remainder timeout of the first thread is less than 2^32
             * clock ticks, we program the remainder value and the thread is
             * left in the time queue, waiting for the next clock interrupt
             */
	    /* DEBUG ("Reprogramming clock with %dus; the first thread's abs_timeout (%d) is future to the current time (%d)",
	    	(uint32_t) next_clock_val, (uint32_t) thread_time, (uint32_t) current_time); */
            OS_ProgramClock((uint32_t)next_clock_val);
        }

        return;
    }
    else
    {
        list_for_each(pos, &time_list)
        {
            if((FIRST_TIME_THREAD->abs_timeout) <= (TIME_PENALTY + OS_TimeGetTicks()))
            {
                /*  Outs the blocked task   */
                task = GET_ELEMENT(OS_task_t, OS_ListGetNode(&time_list), time_node);

                /*
                 * If the task is blocked in a semaphore due to a TimedWait
                 * system call, we outs the thread from the semaphore queue
                 */
                if(task->status | TS_SEM_BLOCKED)
                {
                    OS_SemExtracThreadFromList(task->wait_sem, task);
                }

                /*  Inserts the waked task in the ready queue   */
                OS_SchedTaskReady(task->tid);
            }
            else
                break;
        }
    }

    /*  Activate the one shot timer */
    if(OS_ListGetNum(&time_list) != 0)
    {
        OS_Time_t current_time;
        OS_Time_t clock_program_value;

        //	current_time = OS_TimeGetTicks() + TIME_PENALTY;
        current_time = OS_TimeGetTicks();
        clock_program_value = FIRST_TIME_THREAD->abs_timeout - current_time -
            TIME_PENALTY;

        if((clock_program_value < OVERHEAD_CLICKS) &&
           (FIRST_TIME_THREAD->abs_timeout > current_time))
        {
            /*
             * Program the clock in case the value to be programmed was
             * correct
             */
	    /* DEBUG ("Reprogramming clock with %dus due to changes in the sleep "
		"queue; the first thread's abs_timeout (%d) is future to the "
		"current time (%d)", (uint32_t) clock_program_value,
		(uint32_t) FIRST_TIME_THREAD->abs_timeout, (uint32_t) current_time); */
            OS_ProgramClock((uint32_t)clock_program_value);
        }
        /*
         * In case of a thread timeout was expired during the routine
         * execution, the routine marks a timer event
         */
        else if((FIRST_TIME_THREAD->abs_timeout <= current_time))
            OS_IRQMarkClockEvent();
    }
    else
        OS_ListReset(&time_list);
}

void OS_TimeRemoveFromQueue(OS_task_t *_task)
{
    _task->status &= ~(TS_TIME_BLOCKED);

    /*  Remove the thread from the time_queue   */
    OS_ListExtract(&time_list, &(_task->time_node));

}

/*
 * Function:    void OS_DoSleep(uint32_t _ticks)
 *
 * In this sleep system call implementation the current task is been blocked.
 * For this reason we not insert the current task in the ready queue.
 */

int OS_DoSleep(OS_Time_t _ticks)
{
    OS_Node_t *pos;
    int inserted = 0;
    int retval;
    volatile OS_Time_t current_time = 0;
    OS_Time_t clock_program_value = 0;

    if(_ticks < OS_TimeGetTicks() + TIME_PENALTY)
    {
        retval = 0;
    }
    else {

        /*  Init the absolute clock */
        current->abs_timeout = _ticks;

        KTRACE (KTRACE_UNREADY, current->tid, 0xffffffff);

        /*  Change the task status  */
        current->status |= TS_TIME_BLOCKED;

        if(OS_ListGetNum(&time_list) == 0) {
        	OS_ListInsert(&time_list, &(current->time_node));
    	} else {
	        list_for_each(pos, &time_list)
	        {

	            if(GET_TIME_THREAD(pos)->abs_timeout > current->abs_timeout)
                {
	                if(pos->prev != NULL){
	                    OS_ListAdd(&time_list, &(current->time_node), pos->prev);
	                }
	                else {
	                    OS_ListInsert(&time_list, &(current->time_node));
	                }

	                inserted = 1;
	                break;
	            }
	        }

	        if(!inserted) {
	            OS_ListAppend(&time_list, &(current->time_node));
	        }
    	}

	current_time = OS_TimeGetTicks() + TIME_PENALTY;
        clock_program_value = FIRST_TIME_THREAD->abs_timeout - current_time;

        if(clock_program_value < OVERHEAD_CLICKS &&
                (FIRST_TIME_THREAD->abs_timeout > current_time))
        {
            OS_ProgramClock((uint32_t)clock_program_value);
        }
        else if(FIRST_TIME_THREAD->abs_timeout <= current_time)
        {
            OS_IRQMarkClockEvent();
        }

        retval = -1;
    }

    return retval;
}

/******************************************************************************
 ***************************** REAL TIME CLOCK*********************************
 * ***************************************************************************/

/*
 * Function: static void OS_ClockInit(void)
 *
 * This function initializes the system Real Time Clock
 */
static void OS_ClockInit(void)
{
    /*  Register IRQ for the Real Time Clock    */
    OS_RegisterIRQ(CLOCK_TRAP, OS_ClockISR);
}

/*
 * Function:    static uint32_t OS_GetClock(void)
 *
 * Return the timer ticks.
 */
OS_Time_t OS_TimeGetTicks(void)
{
    OS_Time_t counter = 0;
    OS_Time_t the_counter;


    /*  Get the General Purpose Timer Counter   */
    the_counter = ercos_hal_pit_read ();

    counter = (OS_Time_t)(timer_tick_counter + (OS_Time_t)the_counter);

//    if(counter < CPU_TIMER_RESOLUTION)
//        return 0;

    return (counter);
}

void OS_TimeGetTime(OS_Timeval_t *_system_time)
{
    OS_Time_t the_counter;

    /*  Get the General Purpose Timer Counter   */
    the_counter = ercos_hal_pit_read ();

    _system_time->seconds = OS_Time.seconds + (uint32_t)((uint32_t)the_counter / 1000000);
    _system_time->useconds = OS_Time.useconds + ((uint32_t)the_counter % 1000000);

    _OS_Timeval_normalize(_system_time);
}

/*
 * Function: static void OS_ClockISR(int level)
 *
 * This function implements the Real Time Clock service routine
 */
static void OS_TimerISR(int level)
{
    /*  Increment the real time clock counter ticks */
    timer_tick_counter += (OS_Time_t)((OS_Time_t) ERCOS_HAL_PIT_PERIOD + 1);
    OS_Time.seconds += seconds_per_timer_tick;
    OS_Time.useconds += useconds_per_timer_tick;

    _OS_Timeval_normalize(&OS_Time);

    /*
     * If the one shot timer is not programmed means that it is possible that
     * the next thread to be programmed has the abs_timeout greater than the
     * timer overflow value. So we must call the OS_ClockBottomISR routine in
     * order to try a new one shot programation
     */
    if(IS_ONESHOT_TIMER_NOT_PROGRAMMED() &&
       OS_ListGetNum(&time_list) != 0)
    {
        OS_ClockBottomISR();
    }

}

static uint32_t __div64_32(uint64_t *n, uint32_t base)
{
    uint64_t rem = *n;
    uint64_t b = base;
    uint64_t res, d = 1;
    uint32_t high = rem >> 32;

    /* Reduce the thing a bit first */
    res = 0;
    if (high >= base) {
        high /= base;
        res = (uint64_t) high << 32;
        rem -= (uint64_t) (high*base) << 32;
    }
    while ((uint64_t)b > 0 && b < rem) {
        b = b+b;
        d = d+d;
    }
    do {
        if (rem >= b) {
            rem -= b;
            res += d;
        }
        b >>= 1;
        d >>= 1;
    } while (d);
    *n = res;
    return rem;
}

