
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

#include <public/glue.h>
#include <private/ercos-hal.h>

#include <ercos/kernel/task.h>
#include <ercos/kernel/types.h>
#include <ercos/kernel/sem.h>
#include <ercos/kernel/traps.h>
#include <ercos/kernel/tracer.h>
#include <ercos/kernel/irqev.h>
#include <ercos/kernel/ktracer.h>
#include <ercos/kernel/time.h>
#include <ercos/kernel/irq.h>

#include <ercos/syscall.h>

//------------------------------------------------------------------------------

volatile uint32_t schedule_active = 0;

//------------------------------------------------------------------------------

/**
 * The execution of this routine is mandatory for all system calls. It sets
 * a flag in order to avoid the scheduling a allow the system call anidation
 */
static void OS_SyscallEntry(void);

/**
 * The execution of this routine is mandatory for all the systems calls. It is
 * executed at the end of the system call to perform the event handling and
 * the scheduling (if it is necessary)
 */
static void OS_SyscallExit(void);

//------------------------------------------------------------------------------

/** This variable notifies the schedule necessity   */
static char need_reesched = 0;

/**
 * This MACRO sets the need_reesched flag in order to force a scheduling when
 * the system call returns
 */
#define SET_NEEDREESCHED_FLAG()    \
do {    \
    need_reesched = 1;  \
}while(0);

#define SET_NEEDREESCHED()    \
do {    \
    need_reesched = 1;  \
    OS_SchedCurrentReady();  \
}while(0);

/**
 * This MACRO clears the need_reesched flag in order to avoid the scheduling
 * when the system call returns
 */
#define CLEAR_NEEDREESCHED_FLAG()  \
do {    \
    need_reesched = 0;  \
}while(0);

/** This MACRO verifies if a scheduling is needed   */
#define NEED_REESCHED           (need_reesched == 1)

#define SET_SCHEDULE_ACTIVE() \
do {    \
    OS_DisableIRQ();    \
    schedule_active = 1;  \
    OS_EnableIRQ();    \
}while(0);

#define CLEAR_SCHEDULE_ACTIVE() \
do {    \
    schedule_active = 0;  \
}while(0);

//------------------------------------------------------------------------------

static void OS_SyscallEntry(void)
{
    SET_SCHEDULE_ACTIVE();
}

static void OS_SyscallExit(void)
{
    /* Check for a stack overflow */
#ifndef CONFIG_NDEBUG
 //TODO   OS_CheckCurrentStackGuard ();
#endif

    OS_DisableIRQ();

    OS_IRQHandleEvent();

    if(NEED_REESCHED) {
        CLEAR_NEEDREESCHED_FLAG();

        /*
         * Schedule. The Schedule must clear the schedule_active flag
         */
        OS_Schedule();
        ercos_hal_dispatch ();
    }
    else
        CLEAR_SCHEDULE_ACTIVE();    // Clear the schedule_active flag

    OS_EnableIRQ();
}

/******************************************************************************
 * TIME
 *****************************************************************************/

void SYS_Sleep(OS_Time_t _ticks)
{
    OS_Time_t the_ticks;
    int retval;

    /* it's better to get the current time as soon as possible; this call
     * doesn't need to be protected with the OS_SyscallEntry() exclusion */
    the_ticks = (OS_Time_t)OS_TimeGetTicks() + _ticks;

    OS_SyscallEntry();

    KTRACE (KTRACE_SYS_SLEEP, (uint32_t) (the_ticks >> 32), (uint32_t) the_ticks);

    retval = OS_DoSleep(the_ticks);             /*  In time.c   */

    if(0 == retval) {
        SET_NEEDREESCHED();
    }else {
        SET_NEEDREESCHED_FLAG();
    }

    OS_SyscallExit();
}

void SYS_AbsSleep(OS_Time_t _tick)
{
    OS_SyscallEntry();
    int retval;

    KTRACE (KTRACE_SYS_ABSSLEEP, (uint32_t) (_tick >> 32), (uint32_t) _tick);

    retval = OS_DoSleep(_tick);             /*  In time.c   */

    if(0 == retval) {
        SET_NEEDREESCHED();
    }else {
        SET_NEEDREESCHED_FLAG();
    }

    OS_SyscallExit();
}

void SYS_TimeGetTime(OS_Timeval_t *_system_time)
{
    OS_TimeGetTime(_system_time);
}

/******************************************************************************
 * TASK
 *****************************************************************************/

tid_t SYS_TaskCreate(uint32_t *_stackaddr, uint32_t  _stacksize, void *_ip,
		void* _arg, uint8_t _priority)
{
    tid_t tid;

    OS_SyscallEntry();

    KTRACE (KTRACE_SYS_TASKCREATE, (uint32_t) _ip, _priority);

    tid = OS_TaskCreate(_stackaddr, _stacksize, _ip, _arg, _priority);    /*  In sched.c  */

    /*
     * We need to add the current thread to the sched queue in order to
     * reschedule correctly
     */
    SET_NEEDREESCHED();

    OS_SyscallExit();

    return tid;
}

/******************************************************************************
 * PRIORITY
 *****************************************************************************/

void SYS_SetPriority(tid_t _tid, OS_Priority_t _priority)
{
    tid_t tid;

    OS_SyscallEntry();

    tid = OS_GetCurrentTid();

    /*
     * The thread is only inserted in the task queue if it is not blocked in
     * any other queue.
     */
    if(tid != _tid && !OS_TaskIsBlocked(_tid))
    {
        OS_SchedRemoveThread(_tid);
        OS_SetPriority(_tid, _priority);    /*  In sched.c  */
        OS_SchedTaskReady(_tid);
    }
    else
        OS_SetPriority(_tid, _priority);    /*  In sched.c  */

    /*
     * We need to add the current thread to the sched queue in order to
     * reschedule correctly
     */
    SET_NEEDREESCHED();

    OS_SyscallExit();
}

void SYS_SetTmpPriority(tid_t _tid, OS_Priority_t _priority)
{
    tid_t tid;

    OS_SyscallEntry();

    tid = OS_GetCurrentTid();

    /*  If the caller thread changes another thread priority, both the caller
     * (current) thread and the target thread must be inserted in the sched
     * queue.
     * The thread is only inserted in the task queue if it is not blocked in
     * any other queue.
     */
    if(tid != _tid && !OS_TaskIsBlocked(tid)) {
        /*  Remove the thread from prev sched queue */
        OS_SchedRemoveThread(_tid);

        OS_SetTmpPriority(_tid, _priority);    /*  In sched.c  */

        OS_SchedTaskReady(_tid);
    }
    else
        OS_SetTmpPriority(_tid, _priority);    /*  In sched.c  */

    /*
     * We need to add the current thread to the sched queue in order to
     * reschedule correctly
     */
    SET_NEEDREESCHED();

    OS_SyscallExit();
}

OS_Priority_t SYS_GetPriority(tid_t _tid)
{
    int retval;

    OS_SyscallEntry();
    retval = OS_GetPriority(_tid);    /*  In sched.c  */

    SET_NEEDREESCHED();

    OS_SyscallExit();

    return retval;
}

/******************************************************************************
 * SEMAPHORES
 *****************************************************************************/

int SYS_SemWait(OS_SemID_t _sem)
{
    int retval = 0;

    OS_SyscallEntry();

    KTRACE (KTRACE_SYS_SEMWAIT, current->tid, _sem);

    retval = OS_SemWait(_sem);    /*  In sem.c  */

    if(0 == retval) {
        SET_NEEDREESCHED();
    }else {
        SET_NEEDREESCHED_FLAG();
    }

    OS_SyscallExit();

    return 0;
}

int SYS_SemTimedWait(OS_SemID_t _sem, OS_Time_t _ticks)
{
    int retval = 0;
    //DEBUG ("(_sem=%d, _ticks=<%p %p> (%u))", _sem, (unsigned) (_ticks >> 32), (unsigned) _ticks, (unsigned) _ticks);
    OS_Time_t the_ticks = OS_TimeGetTicks() + _ticks;

    OS_SyscallEntry();

    KTRACE (KTRACE_SYS_SEMTIMEDWAIT, _sem, (uint32_t) the_ticks);

    if(0 == OS_DoSleep(the_ticks))
    {
        /*  The task is not blocked */
        SET_NEEDREESCHED();

        /*  The timeout expired */
        retval = -1;
    }else {
        SET_NEEDREESCHED_FLAG();

        if(0 == OS_SemWait(_sem)) {
            /*  The task is blocked in the time queue   */

            SET_NEEDREESCHED();
            OS_TimeRemoveFromQueue(current);
        }
        /*  The task is blocked in the two queues   */
        else {
            /*  The task is blocked in the semaphore and in the time queue;
	     *  syscall exit will switch to another task  */
            OS_SyscallExit();

	    /* at this point, the task is ready again */

            OS_SyscallEntry();
            if(!OS_SemIsCatched(_sem)) {
                retval = -1;
            }

            SET_NEEDREESCHED();
        }
    }

    OS_SyscallExit();

    /*
     * If retval == 0 the semaphore has been catched, else
     * the timeout has been expired
     */
    return retval;
}

int SYS_SemPost(OS_SemID_t _sem)
{
    int retval;

    OS_SyscallEntry();
    KTRACE (KTRACE_SYS_SEMPOST, current->tid, _sem);
    retval = OS_SemPost(_sem);    /*  In sem.c  */
    SET_NEEDREESCHED();
    OS_SyscallExit();

    return retval;
}

int SYS_SemInit(OS_SemID_t *_sem, uint32_t _value)
{
    int retval;

    OS_SyscallEntry();

    retval = OS_SemInit(_sem, _value);    /*  In sem.c  */

    SET_NEEDREESCHED();

    OS_SyscallExit();

    return retval;
}

int SYS_SemTryWait(OS_SemID_t _sem)
{
    int retval;

    OS_SyscallEntry();

    KTRACE (KTRACE_SYS_SEMTRYWAIT, current->tid, _sem);

    retval = OS_SemTryWait(_sem);       /*  in sem.c    */

    SET_NEEDREESCHED();

    OS_SyscallExit();

    return retval;
}

int SYS_SemDestroy(OS_SemID_t *_sem)
{
    int retval;

    OS_SyscallEntry();
    retval = OS_SemDestroy(_sem);       /*  in sem.c    */

    SET_NEEDREESCHED();

    OS_SyscallExit();

    return retval;
}

/******************************************************************************
 * MUTEX
 *****************************************************************************/

int SYS_MutexInit(OS_MutexID_t *_mutex, uint8_t _type)
{
    int retval;

    OS_SyscallEntry();

    KTRACE (KTRACE_SYS_MUTEXINIT, current->tid, _type);

    retval = OS_MutexInit(_mutex, _type);  /*  in mutex.c  */

    SET_NEEDREESCHED();

    OS_SyscallExit();

    return retval;
}

void SYS_MutexSetPrioCeiling(OS_MutexID_t *_mutex, int prioceiling)
{

    OS_SyscallEntry();

    OS_MutexSetPrioCeiling(*_mutex, prioceiling);

    SET_NEEDREESCHED();

    OS_SyscallExit();
}

int SYS_MutexDestroy(OS_MutexID_t *_mutex)
{
    int retval;

    OS_SyscallEntry();
    retval = OS_MutexDestroy(_mutex);

    SET_NEEDREESCHED();

    OS_SyscallExit();

    return retval;
}

int SYS_MutexLock(OS_MutexID_t _mutex)
{
    int retval;

    OS_SyscallEntry();

    KTRACE (KTRACE_SYS_MUTEXLOCK, current->tid, _mutex);

    retval = OS_MutexLock(_mutex);          /*  in mutex.c */

    if(0 == retval) {
        SET_NEEDREESCHED();
    }else {
        SET_NEEDREESCHED_FLAG();
    }

    OS_SyscallExit();

    return 0;
}

int SYS_MutexUnLock(OS_MutexID_t _mutex)
{
    int retval;

    OS_SyscallEntry();

    KTRACE (KTRACE_SYS_MUTEXUNLOCK, current->tid, _mutex);

    retval = OS_MutexUnlock(_mutex);        /*  in mutex.c */

    SET_NEEDREESCHED();

    OS_SyscallExit();

    return retval;
}

int SYS_MutexTryLock(OS_MutexID_t _mutex)
{
    int retval;

    OS_SyscallEntry();

    KTRACE (KTRACE_SYS_MUTEXTRYLOCK, current->tid, _mutex);

    retval = OS_MutexTryLock(_mutex);

    SET_NEEDREESCHED();

    OS_SyscallExit();

    return retval;
}

/******************************************************************************
 * THREADS
 *****************************************************************************/

int SYS_GetCurrentTid(void)
{
    int retval;

    OS_SyscallEntry();

    retval = OS_GetCurrentTid();    /*  in kernel/sched.c  */

    SET_NEEDREESCHED();

    OS_SyscallExit();

    return retval;
}


void SYS_TaskGetStackParameters(uint32_t *_stackaddr, uint32_t *_stacksize)
{

    OS_SyscallEntry();

    OS_TaskGetStackParameters(_stackaddr, _stacksize); /*  in kernel/task.c  */

    SET_NEEDREESCHED();

    OS_SyscallExit();

}

void SYS_ThreadExit(void)
{
    OS_SyscallEntry();

    KTRACE (KTRACE_SYS_THREADEXIT, current->tid, 0);

    OS_ThreadExit();                    /*  in task.c   */

    SET_NEEDREESCHED_FLAG()

    OS_SyscallExit();
}

/******************************************************************************
 * IRQ MANAGEMENT
 *****************************************************************************/

void SYS_Disable (void)
{
    OS_DisableIRQ ();
}

void SYS_Enable (void)
{
    OS_EnableIRQ ();
}

void SYS_IntrLock(uint32_t _vector)
{

    OS_SyscallEntry();

    OS_MaskInterrupt(_vector);

    SET_NEEDREESCHED();

    OS_SyscallExit();

}

void SYS_IntrUnlock(uint32_t _vector)
{

    OS_SyscallEntry();

    ercos_hal_intr_umask (_vector);

    SET_NEEDREESCHED();

    OS_SyscallExit();

}

void SYS_RegisterIRQ(uint32_t _vector, void (*handler)(int level))
{
    OS_RegisterIRQ(_vector, handler);
}

void SYS_RestoreIRQ(uint32_t _vector)
{
    OS_RestoreIRQ(_vector);
}



/******************************************************************************
 * EVENTS
 *****************************************************************************/

void SYS_IRQMarkEvent(OS_SemID_t _sem)
{

    OS_SyscallEntry();

    KTRACE (KTRACE_SYS_IRQMARKEVENT, _sem, 0);

    OS_IRQMarkEvent(_sem);

    CLEAR_NEEDREESCHED_FLAG();

    OS_SyscallExit();

}

/******************************************************************************
 * TRACER
 *****************************************************************************/


void SYS_TracerSaveEvent(const struct posix_trace_event_info * ev, uint32_t crc)
{
#ifdef CONFIG_ERCOS_TRACER
    OS_SyscallEntry();
    OS_TracerSaveEvent (ev, crc);
    SET_NEEDREESCHED();
    OS_SyscallExit();
#endif
}

