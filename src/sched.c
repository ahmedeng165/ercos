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
 * $Id: sched.c 1285 2008-06-23 15:36:09Z cesar $
 *
 * $Revision: 1285 $
 * $Date: 2008-06-23 17:36:09 +0200 (lun, 23 jun 2008) $
 * Last CVS commit by $Author: cesar $
 */

#include <private/ercos-hal.h>
#include <ercos/kernel/sched.h>
#include <ercos/kernel/types.h>
#include <ercos/kernel/ktracer.h>
#include <ercos/kernel/list.h>
#include <ercos/kernel/task.h>
#include <ercos/config.h>

//------------------------------------------------------------------------------

extern volatile uint32_t schedule_active;

/**
 * This pointer is the current thread
 *
 * The current thread is initialized to the idle thread.
 */
OS_task_t *current;
OS_task_t *old_current;

/**
 * This is the sched queue.
 *
 * In this queue the task is been ready. We have one queue for each priority.
 * The task is store in the queue that match with its priority.
 */
static OS_List_t sched_list[CONFIG_ERCOS_MAX_PRIO];

/**
 * The sched queue index.
 *
 * This index points to the mayor priority and non empty queue.
 */
static uint16_t sched_queue_i;

/** This macro verifies if a task y already ready   */
#define IS_TASK_READY(task) (TS_READY == task->status)

//------------------------------------------------------------------------------

/**
 * \todo Documentar esta funcion
 */
static void OS_SchedAddQueueHead(OS_task_t *_task);

/**
 * Inserts a task into the sched queue from the tail.
 *
 * \param _task:    This is a pointer to the TCB to be inserted.
 */
static void OS_SchedAddQueueTail(OS_task_t *_task);

//------------------------------------------------------------------------------


/*
 * Function: void OS_SchedInit(void)
 *
 * This function initializes the schedule parameters
 */
void OS_SchedInit(void)
{
    int i;

    /*  Current task points to the first entry in the task table. After calling
     *  to OS_Schedule, the current task will be the main task (the second
     *  entry) and the old_current will become this task. In fact, this slot is
     *  never used (really?) */
    current = (task_table + 0);

    /*  The sched queue index points to the min priority queue  */
    sched_queue_i = SCHED_MIN_PRIORITY;

    for (i = 0; i < CONFIG_ERCOS_MAX_PRIO; i++)
        OS_ListReset(&sched_list[i]);
}

/*
 * Function:    void OS_SchedCurrentReady(void)
 */
void OS_SchedCurrentReady(void)
{
    OS_SchedAddQueueHead(current);
}

/*
 * Function:    void OS_SchedTaskReady(tid_t _task)
 */
void OS_SchedTaskReady(tid_t _task)
{
        /* DEBUG ("(_task=%d); priority=%d", _task, task_table[_task].priority); */
        KTRACE (KTRACE_READY, _task, 0);
        OS_SchedAddQueueTail(task_table + _task);
}

/*
 * Function: void OS_Schedule(void)
 *
 * This function implements the system scheduler
 */
void OS_Schedule(void)
{
    OS_task_t *current_aux;
    OS_Node_t *n;

    /* Ensure interrupts are disable before using the queue */
    OS_DisableIRQ();

    while(OS_ListGetNum(&(sched_list[sched_queue_i])) == 0) {
        /*  Increment the queue index in order to obtain the next thread    */
        sched_queue_i++;
        sched_queue_i %= (CONFIG_ERCOS_MAX_PRIO);
    }

    current_aux = current;

    /*  Extract the new current task    */
    n = OS_ListGetNode(&sched_list[sched_queue_i]);
    current = GET_ELEMENT(OS_task_t, n, node);

    current->status = TS_RUNNING;

    old_current = current_aux;

    /* Enable interrupts */
    OS_EnableIRQ();

    /* DEBUG ("Selected %d as current task (prio %d); leaving %d as old_current",
        current - task_table, current->priority, old_current - task_table); */
    if (current != old_current) KTRACE (KTRACE_EXEC, current->tid, old_current->tid);

    schedule_active = 0;
}

void OS_SetPriority(tid_t _tid, OS_Priority_t _priority)
{
    ASSERT (_priority <= SCHED_MIN_PRIORITY);
    ASSERT (_priority >= SCHED_MAX_PRIORITY);

    KTRACE (KTRACE_PRIO, _tid, _priority);
    task_table[_tid].base_priority = _priority;

    if((task_table[_tid].pmutex) && (_priority < task_table[_tid].pmutex->prio))
        task_table[_tid].priority = task_table[_tid].pmutex->prio;
    else
        task_table[_tid].priority = _priority;
}

void OS_SetTmpPriority(tid_t _tid, OS_Priority_t _priority)
{
    ASSERT (_priority <= SCHED_MIN_PRIORITY);
    ASSERT (_priority >= SCHED_MAX_PRIORITY);

    KTRACE (KTRACE_PRIO, _tid, _priority);
    task_table[_tid].priority = _priority;
}

tid_t OS_GetCurrentTid(void)
{
    return current->tid;
}

void OS_SchedRemoveThread(tid_t _tid)
{
    OS_task_t *task = task_table + _tid;

    /* DEBUG ("Removing task %d from prq. %d", _tid, task->priority); */
    ASSERT (_tid != 1);
    OS_ListExtract(&sched_list[task->priority], &(task->node));
}

OS_Priority_t OS_GetPriority(tid_t _tid)
{
    return task_table[_tid].priority;
}

void OS_RestorePriority(tid_t _tid)
{
    KTRACE (KTRACE_PRIO, _tid, task_table[_tid].base_priority);
    task_table[_tid].priority = task_table[_tid].base_priority;
}

OS_Priority_t OS_GetCurrentPrio(void)
{
    return current->priority;
}

//------------------------------------------------------------------------------

static void OS_SchedAddQueueTail(OS_task_t *_task)
{
    ASSERT (_task->priority <= SCHED_MIN_PRIORITY);

    /*  Task ready  */
    _task->status = TS_READY;

    /*  The task priority is greater than the queue index   */
    if (_task->priority < sched_queue_i) {
        sched_queue_i = _task->priority;
    }

    /*  Add task from the tail of the list  */
    /* DEBUG ("Appending task %d to the tail of prq. %d", _task - task_table, _task->priority); */
    OS_ListAppend(&sched_list[_task->priority], &(_task->node));
}

/*
 * Function:    void OS_SchedAddQueueHead(OS_task_t *_task)
 */
static void OS_SchedAddQueueHead(OS_task_t *_task)
{
    ASSERT (_task->priority <= SCHED_MIN_PRIORITY);

    /*  Task ready  */
    _task->status = TS_READY;

    /*  The task priority is greater than the queue index   */
    if (_task->priority < sched_queue_i)
        sched_queue_i = _task->priority;

    /*  Add task from the head of the list  */
    /* DEBUG ("Appending task %d to the head of prq. %d", _task - task_table, _task->priority); */
    OS_ListInsert(&sched_list[_task->priority], &(_task->node));
}

// vim:ts=4:sw=4:et:

