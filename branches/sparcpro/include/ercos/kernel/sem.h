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
 * \file include/kernel/sem.h
 * \brief Kernel layer MACROS and functions related to the management of the
 *        binary semaphores.
 */

#ifndef _KERNEL_SEM_H_
#define _KERNEL_SEM_H_

#include <public/glue.h>
#include <ercos/kernel/types.h>
#include <ercos/kernel/task.h>
#include <ercos/kernel/list.h>

/** Binary Semaphore Structure */
typedef struct semaphore
{
    /** Semaphore identifier  */
    OS_SemID_t sem_id;

    /** ID of the task blocked in the semaphore   */
    tid_t tid_in;

    /** Semaphore counter value */
    int8_t sem_value;

    /** Semaphore queue. In this queue the task will be blocked */
    OS_List_t list;

    /** Neccesary to implement semaphore queues */
    OS_Node_t node;

}OS_Sem_t;

/**
 * \brief Identifies the semaphore value when it is free.
 */
#define SEM_FREE    255

/* the semaphore vector, in kernel/sem.c */
extern OS_Sem_t semaphores[CONFIG_ERCOS_MAX_BINSEM];

//------------------------------------------------------------------------------

/**
 * \brief Initializes a certain binary semaphore.
 *
 * \param _sem    Reference to the semaphore that is being initialized.
 * \param _value  Initial value of the semaphore.
 * \return        Always returns zero.
 */
int OS_SemInit(OS_SemID_t *_sem, uint32_t _value);

/**
 * \brief Resets a certain binary semaphore.
 *
 * \param _sem  Reference to the semaphore.
 * return       Always returns zero.
 */
int OS_SemDestroy(OS_SemID_t *_sem);

/**
 * \brief Performs a post (unlock) to a certain binary semaphore.
 *
 * \param _sem  The semaphore to unlock.
 * \return      Always returns zero.
 */
int OS_SemPost(OS_SemID_t _sem);

/**
 * \brief Performs a wait (lock) to a certain binary semaphore.
 *
 * \param _sem  The semaphore to lock.
 * \return      Returns zero if the semaphore was free, -1 otherwise.
 *
 * In this case, if the semaphore was already catched, the function inserts
 * the current thread into the semaphore's waiting queue.
 */
int OS_SemWait(OS_SemID_t _sem);

/**
 * \brief Performs a non-blocking wait to a certain binary semaphore.
 *
 * \param _sem  The semaphore to lock.
 * \return      Returns zero if the thread obtains the semaphore, -1
 *              otherwise.
 *
 * In this case, the current thread is never blocked, even if the semaphore was
 * already catched.
 */
int OS_SemTryWait(OS_SemID_t _sem);

/**
 * \brief Checks if the semaphore has already been catched.
 *
 * \param _sem  The semaphore to check.
 * \return      True if the semaphore was catched, false if not.
 */
int OS_SemIsCatched(OS_SemID_t _sem);

void OS_SemExtracThreadFromList(OS_SemID_t _sem, OS_task_t *task);

#endif

