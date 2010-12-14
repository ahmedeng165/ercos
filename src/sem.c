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
 * $Id: sem.c 1285 2008-06-23 15:36:09Z cesar $
 *
 * $Revision: 1285 $
 * $Date: 2008-06-23 17:36:09 +0200 (lun, 23 jun 2008) $
 * Last CVS commit by $Author: cesar $
 */

#include <public/glue.h>

#include <ercos/kernel/sem.h>
#include <ercos/kernel/types.h>
#include <ercos/kernel/list.h>
#include <ercos/kernel/task.h>
#include <ercos/kernel/ktracer.h>

#include <ercos/syscall.h>

//------------------------------------------------------------------------------

static uint8_t sem_id = 0;

/** This is the maximum number of binary semaphores used in the system  */
OS_Sem_t semaphores[CONFIG_ERCOS_MAX_BINSEM];

#define GET_NEXT_SEM_ID(__sem_id)                    \
do {                                                 \
    __sem_id = sem_id;                               \
    sem_id++;                                        \
    ASSERT (sem_id <= CONFIG_ERCOS_MAX_BINSEM);             \
} while(0);

#define SEM(_id_sem)    (&(semaphores[_id_sem]))

//------------------------------------------------------------------------------

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  OS_SemInit
 *  Description:  This function initializes a binary semaphore. The parameters
 *  taken from the function is the pointer to the binary semaphore structure and
 *  the initial value. The value only can be 0 (catched) or 1 (free).
 * =====================================================================================
 */
int OS_SemInit(OS_SemID_t *_sem, uint32_t _value)
{
    OS_SemID_t id_sem;

    GET_NEXT_SEM_ID(id_sem);
    //DEBUG ("(*, _value=%d); id_sem=%d", _value, id_sem);

    SEM(id_sem)->sem_id = id_sem;

    if(_value <= 0)
        SEM(id_sem)->sem_value = 0;    /*  Binary semaphore    */
    else
        SEM(id_sem)->sem_value = 1;    /*  Counter semaphore = 1 */

    SEM(id_sem)->tid_in = 0;

    OS_ListReset(&(SEM(id_sem)->list));

    *_sem = id_sem;

    return 0;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  OS_SemDestroy
 *  Description:  Destroys a binary semaphore and resets the associated list.
 * =====================================================================================
 */
int OS_SemDestroy(OS_SemID_t *_sem)
{
    OS_SemID_t id_sem = *_sem;

    //DEBUG ("(*_sem=%d)", * _sem);
    OS_ListReset(&(SEM(id_sem)->list));

    return 0;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  OS_SemPost
 *  Description:  This functions release the semaphore and wakes up the task
 *  being blocked on it. A task can be also be waiting for a time event, so this
 *  function vefiries whether the relased task is also waiting for a time event
 *  or not. In case it is, the task is also removed from the time queue.
 * =====================================================================================
 */
int OS_SemPost(OS_SemID_t _sem)
{
    OS_task_t *task;
    OS_Node_t *n;
    OS_SemID_t id_sem = _sem;

    //DEBUG ("(_sem=%d); num=%d", _sem, OS_ListGetNum(&(SEM(id_sem)->list)));
    KTRACE (KTRACE_UNLOCK, current->tid, id_sem);

    if(OS_ListGetNum(&(SEM(id_sem)->list)) != 0)
    {
        /*  Outs the task from the semaphore queue  */
        n = OS_ListGetNode(&(SEM(id_sem)->list));
        task = GET_ELEMENT(OS_task_t, n, resource_node);

	/*  Trace the event */
        KTRACE (KTRACE_LOCK, task->tid, id_sem);

        /*  Store what is the blocked thread in the semaphore structure */
        SEM(id_sem)->tid_in = task->tid;

        /*
         * Verifies if the thread is blocked in the time queue
         * due to a TimedWait system call
         */
        if(task->status | TS_TIME_BLOCKED) {
            OS_TimeRemoveFromQueue(task);
        }

        /*  Reset the wait_sem value    */
        task->wait_sem = 0;

        /*  Add the blocked task to the ready task queue    */
        OS_SchedTaskReady(task->tid);
    }
    else {
        /*  Increment the semaphore counter */
	//DEBUG ("sem. %d now is 1", id_sem);
        SEM(id_sem)->sem_value = 1;
        OS_ListReset(&(SEM(id_sem)->list));
    }

    return 0;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  OS_SemWait
 *  Description:  This function tries to catch a binary semaphore. The calling
 *  task may get blocked in the semaphore queue whether the semaphore is already
 *  taken from another task. If the semaphore was not taken, it is signaled as
 *  taken and the calling task continues the execution.
 * =====================================================================================
 */
int OS_SemWait(OS_SemID_t _sem)
{
    OS_SemID_t id_sem = _sem;

 //  DEBUG ("(_sem=%d); sem_value=%d", _sem, SEM(id_sem)->sem_value);
    if(SEM(id_sem)->sem_value == 0) {

//	DEBUG ("task %d gets blocked on sem. %d", current->tid, id_sem);
	KTRACE (KTRACE_UNREADY, current->tid, id_sem);
        current->status |= TS_SEM_BLOCKED;   /*  Block the caller thread */

        /*
         * The sem_wait field stores the semaphore identifier for which the
         * thread is waitting.
         */
        current->wait_sem = _sem;

        OS_ListAppend(&(SEM(id_sem)->list), &(current->resource_node));

        return -1;
    }
    else {

//	DEBUG ("task %d catched sem. %d", current->tid, id_sem);
        KTRACE (KTRACE_LOCK, current->tid, id_sem);

        /*  Lock the semaphore  */
        SEM(id_sem)->sem_value = 0;

        /*  Store what is the blocked thread in the semaphore structure */
        SEM(id_sem)->tid_in = current->tid;

        /*  The thread must be added to the ready queue */
        return 0;
    }

}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  OS_SemTryWait
 *  Description:  This function tries to catch a binary semaphore. If the
 *  semaphore was not taken before, the calling task catchs it and continue the
 *  execution. If the semaphore is already catched, the function returns an
 *  error but the calling task is not blocked at all.
 * =====================================================================================
 */
int OS_SemTryWait(OS_SemID_t _sem)
{
    int retval = 0;
    OS_SemID_t id_sem = _sem;

    //DEBUG ("(_sem=%d); sem_value=%d", _sem, SEM(id_sem)->sem_value);
    /*  Semaphore catched  */
    if (SEM(id_sem)->sem_value == 1) {

	//DEBUG ("task %d catched sem. %d", current->tid, id_sem);
        KTRACE (KTRACE_LOCK, current->tid, id_sem);

        SEM(id_sem)->sem_value = 0;
        SEM(id_sem)->tid_in = current->tid;
    }
    /*  Semaphore not catched   */
    else
        retval = -1;

    return retval;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  OS_SemIsCatched
 *  Description:  This function returns whether the semaphore is catched or not.
 * =====================================================================================
 */
int OS_SemIsCatched(OS_SemID_t _sem)
{
    OS_SemID_t id_sem = _sem;

    return (SEM(id_sem)->tid_in == current->tid);
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  OS_SemExtractThreadFromList
 *  Description:  This function extracts the given task from the given semaphore
 *  list. This is done because a task may be waiting for both a time event and a
 *  semaphore to be released. If the time event is raised, the task must be
 *  taken out from the semaphore list.
 * =====================================================================================
 */
void OS_SemExtracThreadFromList(OS_SemID_t _sem, OS_task_t *task)
{
    OS_SemID_t id_sem = _sem;

    /*  Delete the semaphore blocked status */
    task->status &= ~(TS_SEM_BLOCKED);

    /*  Extract the thread from the semaphore queue */
    OS_ListExtract(&(SEM(id_sem)->list), &(task->resource_node));

    /*  Reset the wait_sem value    */
    task->wait_sem = 0;
}

