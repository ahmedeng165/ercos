/*
    Copyright 2004-2005 (c) by Aitor Viana Sanchez,
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://atc1.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT.
*/

/*!
 * \file include/kernel/task.h
 * \brief Kernel level thread management functions.
 */

#ifndef _ERCOS_KERNEL_TASK_H_
#define _ERCOS_KERNEL_TASK_H_

#include <public/glue.h>
#include <private/ercos-hal.h>

#include <ercos/kernel/types.h>
#include <ercos/kernel/sched.h>
#include <ercos/kernel/list.h>

#include <ercos/config.h>

/**
 * \brief Task state. Indicates that the task is currently in control of the
 *        CPU.
 */
#define TS_RUNNING          0x00
/**
 * \brief Task state. Indicates that the task is currently blocked in a
 *        semaphore queue.
 */
#define TS_SEM_BLOCKED      0x01
/**
 * \brief Task state. Indicates that the task is currently blocked in the time
 *        queue.
 */
#define TS_TIME_BLOCKED     0x02
/**
 * \brief Task state. Indicates that the task is currently ready to enter to
 *        the CPU and inserted in the ready queue.
 */
#define TS_READY            0x04

/**
 * \brief Stack structure.
 *
 * This structure is neccesary to implement the dynamic stack allocation when
 * in the creationthread creation the stack pointer is set to a NULL value.
 */
typedef struct stack {
	/** Initial address of the stack memory block */
    uint8_t *addr;
    /** Size of the stack memory block */
    uint32_t size;
    /** Node of the list of the stack memory blocks */
    OS_Node_t node;
}OS_Stack_t;

/** \brief Task structure */
typedef struct task
{
    /** Hardware context. Must be the first field   */
    ercos_hal_hwcontext_t context;

    /** Task status */
    uint32_t status;

    /** Thread priority   */
    OS_Priority_t priority;

    /** Thread base priority   */
    OS_Priority_t base_priority;

    /** Entry point to the task code */
    void (*entry_point)(void *);

    /** Entry point function argument   */
    void *arg;

    /** Task identifier */
    tid_t tid;

    /** Stack information structure  */
    OS_Stack_t stack;

    /** Absolute task timeout */
    OS_Time_t abs_timeout;

    /** Resource semaphore pointer */
    struct mutex *pmutex;

    /** Stores the semaphore identifier for which the thread is waitting. */
    OS_SemID_t wait_sem;

    /** Resource queue node */
    OS_Node_t resource_node;

    /** Sched queue node */
    OS_Node_t node;

    /** Time queue node */
    OS_Node_t time_node;

}OS_task_t;

/* This table stores the Task Control Block for all the running tasks (plus the
 * idle and main tasks) */
extern OS_task_t task_table[CONFIG_ERCOS_MAX_THREADS + 2];

/* This variable points to the task currently running */
extern OS_task_t *current;

/* This variable points to the task that was running before the current task
 * was updated (see OS_Schedule) */
extern OS_task_t *old_current;


//------------------------------------------------------------------------------

/**
 * \brief This function initializes all the system TCBs.
 *
 * The routine also init the idle thread TCB.
 */
void OS_TCBInit(void);

/**
 * \brief Returns the \a status parameter of a given thread.
 *
 * \param _tid  ID of the thread.
 * \return      \a Status word.
 *
 * The \a status parameter can take the following values:
 * - TS_READY: The thread is ready to get the processor and is enqueued in the
 *             ready queue.
 * - TS_RUNNING: The thread is currently running and not present in the ready
 *               queue.
 * - TS_SEM_BLOCKED: The thread is currently blocked in a semaphore queue.
 * - TS_TIME_BLOCKED: The thread is currently blocked in the time queue.
 */
int OS_TaskGetStatus(tid_t _tid);

/**
 * \brief Frees all the TCB entries of the current thread and erases all its
 *        information from the system.
 */
void OS_ThreadExit(void);

/**
 * Creates a thread.
 *
 * \param _stack_size  This parameter is the thread stack size.
 * \param _ip          This parameter is the thread entry point.
 * \param _priority    This parameter is the thread priority.
 * \return             The function returns the thread identifier.
 *
 * The routine init the thread TCB:
 *  - init the thread hardware context.
 *  - stablish the thread priority.
 *  - stablish the thread identifier.
 *  - stablish the thread status to TS_READY.
 *
 */
tid_t OS_TaskCreate(uint32_t *_stackaddr, uint32_t  _stacksize, void *_ip,
		void *_arg, uint8_t _priority);

/**
 * \brief Obtains the stack parameters (address and size) of the current
 *        thread.
 *
 * \param _stackaddr  Initial stack address.
 * \param _stacksize  Size of the stack.
 */
void OS_TaskGetStackParameters(uint32_t *_stackaddr, uint32_t *_stacksize);

/**
 * \brief Returns true if the task specified in the parameter is blocked.
 *
 * \param _tid  This parameter is the thread identifier.
 * \return      The routine returns true in case the thread is blocked and
 *              false in any other case.
 */
int OS_TaskIsBlocked(tid_t _tid);

void OS_CheckCurrentStackGuard (void);

#endif

