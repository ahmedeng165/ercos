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
 * $Id: mutex.c 1285 2008-06-23 15:36:09Z cesar $
 *
 * $Revision: 1285 $
 * $Date: 2008-06-23 17:36:09 +0200 (lun, 23 jun 2008) $
 * Last CVS commit by $Author: cesar $
 */

#include <public/glue.h>

#include <ercos/kernel/mutex.h>
#include <ercos/kernel/sched.h>
#include <ercos/kernel/types.h>
#include <ercos/kernel/list.h>
#include <ercos/kernel/task.h>
#include <ercos/kernel/ktracer.h>

//------------------------------------------------------------------------------

#define MUTEX_FREE  255

static uint8_t mutex_id = 0;

static OS_Mutex_t mutexes[CONFIG_ERCOS_MAX_RESEM];

#define GET_NEXT_MUTEX_ID(__mutex_id)                    \
do {                                                     \
   __mutex_id = mutex_id + CONFIG_ERCOS_MAX_BINSEM;             \
   mutex_id++;                                           \
   ASSERT (mutex_id <= CONFIG_ERCOS_MAX_RESEM);                 \
} while(0);


#define MUTEX(__mutex_id)   (&(mutexes[(__mutex_id)]))

#define GET_MUTEX_ID(__mutex_id)    ((__mutex_id) - CONFIG_ERCOS_MAX_BINSEM)

//------------------------------------------------------------------------------


/**
 * \todo Documentar esta funciÃ³n
 */
static void OS_MutexLockInherit(OS_MutexID_t _mutex);

/**
 * \todo Documentar esta funciÃ³n
 */
static void OS_MutexPrioCeiling(OS_MutexID_t _mutex, OS_task_t *_task);

//------------------------------------------------------------------------------

int OS_MutexInit(OS_MutexID_t *_mutex, int _type)
{
    OS_MutexID_t id_mutex;
    register unsigned index;

    /*  Get a new mutex identifier */
    GET_NEXT_MUTEX_ID(id_mutex);
    index = GET_MUTEX_ID (id_mutex);

    /*  Init the mutex counter and some other mutex fields */
    MUTEX(index)->counter = 1;
    MUTEX(index)->in_thread = 0;
    MUTEX(index)->prio = SCHED_MIN_PRIORITY;
    MUTEX(index)->next = NULL;
    MUTEX(index)->mutex_id = id_mutex;

    if (_type > MUTEX_PRIO_TYPES || _type < 0)
        MUTEX(index)->type = MUTEX_PRIO_NONE;
    else
        MUTEX(index)->type = _type;

    /*  Init the mutex queue */
    OS_ListReset(&(MUTEX(index)->list));

    /* return the mutex to the user */
    *_mutex = id_mutex;
    DEBUG ("(*_mutex=%d, _type=%d)", id_mutex, _type);

    return 0;
}

int OS_MutexSetPrioCeiling(OS_MutexID_t _mutex, int _prioceiling)
{
    OS_MutexID_t id_mutex = GET_MUTEX_ID(_mutex);
    int retval = 0;


    if(MUTEX(id_mutex)->type == MUTEX_PRIO_CEILING)
    {
        MUTEX(id_mutex)->prio = _prioceiling;

        retval = 0;
    }
    else {
        retval = -1;
    }

    return retval;
}

int OS_MutexDestroy(OS_MutexID_t *_mutex)
{
    OS_MutexID_t id_mutex = *_mutex;

    id_mutex = GET_MUTEX_ID(id_mutex);

    OS_ListReset(&(MUTEX(id_mutex)->list));

    return 0;
}

int OS_MutexLock(OS_MutexID_t _mutex)
{
    OS_MutexID_t id_mutex = GET_MUTEX_ID(_mutex);
    int retval = 0;

    //DEBUG ("(_mutex=%d)", _mutex);

    if(MUTEX(id_mutex)->counter == 0) {

        KTRACE (KTRACE_UNREADY, current->tid, _mutex);

        /*  Block the current task  */
        current->status = TS_SEM_BLOCKED;

        switch(MUTEX(id_mutex)->type) {

            /*  inherit policy  */
            case MUTEX_PRIO_INHERIT:
                /*  The mutex queue is sorted by priority   */
                OS_ListInsert(&(MUTEX(id_mutex)->list), &(current->resource_node));
                OS_MutexLockInherit(_mutex);
                break;
            case MUTEX_PRIO_CEILING:
                /*  The thread is blocked in the sema  */
                OS_ListInsert(&(MUTEX(id_mutex)->list), &(current->resource_node));
                break;
            /*  No policy   */
            case MUTEX_PRIO_NONE:
                /*  The mutex queue is sorted by priority   */
                OS_ListAppend(&(MUTEX(id_mutex)->list), &(current->resource_node));
                break;
        }

        retval = -1;
    }
    else {

        /** Trace the event in case of need */
        KTRACE (KTRACE_LOCK, current->tid, _mutex);

        /*  Lock the semaphore  */
        MUTEX(id_mutex)->counter = 0;

        /*  Stablish the next thread that has the mutex */
        MUTEX(id_mutex)->in_thread = current->tid;

        MUTEX(id_mutex)->next = current->pmutex;
        current->pmutex = MUTEX(id_mutex);

        if(MUTEX_PRIO_CEILING == MUTEX(id_mutex)->type)
        {
            OS_MutexPrioCeiling(_mutex, current);
        }
        else if(MUTEX_PRIO_INHERIT == MUTEX(id_mutex)->type)
        {
            MUTEX(id_mutex)->prio = current->priority;
        }

        /*
         * Because the current thread that tries to catch the semaphore is not
         * been blocked, we need to add it to the sched queue in order to
         * reschedule
         */
        retval = 0;
    }

    return retval;
}

int OS_MutexUnlock(OS_MutexID_t _mutex)
{
    OS_task_t *task;
    OS_MutexID_t id_mutex = GET_MUTEX_ID(_mutex);

    //DEBUG ("(_mutex=%d)", _mutex);

    current->pmutex = MUTEX(id_mutex)->next;
    MUTEX(id_mutex)->next = NULL;

    KTRACE (KTRACE_UNLOCK, current->tid, _mutex);

    switch(MUTEX(id_mutex)->type)
    {
        case MUTEX_PRIO_INHERIT:
            if(MUTEX(id_mutex)->in_thread) {

                /*
                 * Restore the thread base prioriry. It is not necessary to call
                 * SYS_RestorePriority because when we exit from unlock function
                 * a reschedule is performed.
                 */
                if(current->pmutex != NULL)
                    OS_SetTmpPriority(current->tid, current->pmutex->prio);
                else
                    OS_RestorePriority(current->tid);

            }
            break;

        case MUTEX_PRIO_CEILING:
        {
            OS_Mutex_t *pnext = current->pmutex;
            int priority = current->base_priority;

            if(pnext)
            {
                while(pnext)
                {
                    priority = ((pnext->prio < priority)? pnext->prio : priority);

                    pnext = pnext->next;
                }

                if(priority < current->base_priority)
                    OS_SetTmpPriority(current->tid, priority);
            }
            else
                OS_RestorePriority(current->tid);

            break;
        }
        case MUTEX_PRIO_NONE:
            break;
	default :
	     ASSERT (0);
    }

    /*  There is some thread blocked in the queue   */
    if(OS_ListGetNum(&(MUTEX(id_mutex)->list)) != 0)
    {

        /*  Outs the task from the mutex queue  */
        task = GET_ELEMENT(OS_task_t, OS_ListGetNode(&(MUTEX(id_mutex)->list)), resource_node);

        /*  Stablish the next thread that has the mutex */
        MUTEX(id_mutex)->in_thread = task->tid;

        KTRACE (KTRACE_LOCK, task->tid, _mutex);

        MUTEX(id_mutex)->next = task->pmutex;
        task->pmutex = MUTEX(id_mutex);

        if(MUTEX(id_mutex)->type == MUTEX_PRIO_CEILING)
            OS_MutexPrioCeiling(_mutex, task);

        /*  Insert the task into the sched queue    */
        OS_SchedTaskReady(task->tid);

    }
    else {
        MUTEX(id_mutex)->in_thread = 0;
        MUTEX(id_mutex)->counter = 1;
        OS_ListReset(&MUTEX(id_mutex)->list);
    }

    return 0;
}

int OS_MutexTryLock(OS_MutexID_t _mutex)
{
    int retval = 0;
    OS_MutexID_t id_mutex = GET_MUTEX_ID(_mutex);

    if (MUTEX(id_mutex)->counter == 1) {

        KTRACE (KTRACE_LOCK, current->tid, _mutex);

        MUTEX(id_mutex)->counter = 0;

        /*  update the mutex pointers state   */
        MUTEX(id_mutex)->next = current->pmutex;
        current->pmutex = MUTEX(id_mutex);
    }
    else
        retval = -1;

    return retval;
}

static void OS_MutexLockInherit(OS_MutexID_t _mutex)
{
    tid_t prio, current_prio;
    OS_MutexID_t id_mutex = GET_MUTEX_ID(_mutex);

    //DEBUG ("(_mutex=%d)", _mutex);

    prio = OS_GetPriority(MUTEX(id_mutex)->in_thread);
    current_prio = current->priority;

    /*
     * In case of the current thread priority is greater than
     * the priority of the in thread, we increase temporaly the in
     * thread priority to the current priority .
     * The SYS_SetPriority() is calling when the thread is READY
     * so it must be removed from his sched queue and added to the
     * new queue.
     */
    if (current_prio < prio) {
        MUTEX(id_mutex)->prio = current_prio;

        /*
         * The OS_SchedRemoveThread() remove the in_thread thread from the sched
         * queue and add it to the new priority sched queue.
         * In case the in_thread status is TS_BLOCKED means that the thread is
         * waiting for another event, and it is blocked in other queue so it
         * is not necessary to add it to the sched queue
         */
        if(!OS_TaskIsBlocked(MUTEX(id_mutex)->in_thread)) {
            OS_SchedRemoveThread(MUTEX(id_mutex)->in_thread);

            OS_SetTmpPriority(MUTEX(id_mutex)->in_thread, current_prio);

            OS_SchedTaskReady(MUTEX(id_mutex)->in_thread);
        }else
            OS_SetTmpPriority(MUTEX(id_mutex)->in_thread, current_prio);

    }


}

static void OS_MutexPrioCeiling(OS_MutexID_t _mutex, OS_task_t *_task)
{
    OS_MutexID_t id_mutex = GET_MUTEX_ID(_mutex);

    if(MUTEX(id_mutex)->prio < current->priority)
    {
        /*
         * Además la current->base_priority debería de ser mayor
         * que la prioridad techo del mutex
         */
        OS_SetTmpPriority(current->tid, MUTEX(id_mutex)->prio);
    }
}

