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
 * \file include/kernel/mutex.h
 * \brief This file includes all the functions related to the mutex semaphores.
 *        This functions belong to the kernel level of the operating system.
 */

#ifndef _KERNEL_MUTEX_H_
#define _KERNEL_MUTEX_H_

#include <public/glue.h>
#include <ercos/kernel/list.h>
#include <ercos/kernel/types.h>

/** \brief Mutex structure */
typedef struct mutex
{
    /** Semaphore identifier  */
    uint8_t mutex_id;

    /** Priority mutex FIFO queue    */
    OS_List_t list;

    /** Mutex counter value */
    int8_t counter;

    /** ID of the thread that has catched the mutex */
    tid_t in_thread;

    /** Priority policy of the mutex */
    uint8_t type;

    /** Inherit maximum priority  */
    OS_Priority_t prio;

    /** Pointer to the next mutex in case a thread catch more than one mutex  */
    struct mutex *next;

}OS_Mutex_t;

/** This label identifies the inherit policy in the mutex   */
#define MUTEX_PRIO_NONE       0

/** This label identifies the inherit policy in the mutex   */
#define MUTEX_PRIO_INHERIT    1

/** This label identifies the priority ceiling policy in the mutex   */
#define MUTEX_PRIO_CEILING    2

/** Number of defined mutex priority policies */
#define MUTEX_PRIO_TYPES      MUTEX_PRIO_CEILING


/**
 * \brief Initializes a certain mutex semaphore.
 *
 * \param _mutex  Id of the mutex to initialize.
 * \param _type   Priority policy of the mutex.
 * \return        Zero in any case.
 *
 */
int OS_MutexInit(OS_MutexID_t *_mutex, int _type);

/**
 * \brief Locks a certain mutex semaphore.
 *
 * \param mutex  Id of the mutex to lock.
 * \return       Zero if the semaphore was free, -1 if not.
 *
 */
int OS_MutexLock(OS_MutexID_t mutex);

/**
 * \brief Unlocks a certain mutex semaphore.
 *
 * \param mutex  Id of the mutex to unlock.
 * \return       Zero in any case.
 */
int OS_MutexUnlock(OS_MutexID_t mutex);

/**
 * \brief Tries to lock a certain mutex semaphore.
 *
 * \param mutex  Id of the mutex to lock.
 * \return       Zero if the semaphore was free, -1 if not.
 *
 * This function is the unblockable version of \a OS_MutexLock().
 */
int OS_MutexTryLock(OS_MutexID_t mutex);

/**
 * \brief Resets a certain mutex semaphore.
 *
 * \param mutex  Id of the mutex semaphore to destroy.
 * \return       Zero in any case.
 *
 */
int OS_MutexDestroy(OS_MutexID_t *mutex);

/**
 * \brief Sets the priority ceiling parameter of a certain semaphore.
 *
 * \param _mutex        Id of the mutex.
 * \param _prioceiling  Priority value.
 * \return              Zero if the mutex semaphore uses the priority ceiling
 *                      policy, -1 if not.
 *
 * This function sets the priority ceiling of a semaphore with this
 * priority policy.
 */
int OS_MutexSetPrioCeiling(OS_MutexID_t _mutex, int _prioceiling);

/**
 * \brief Obtains the priority ceiling parameter of a certain semaphore.
 *
 * \param _mutex        Id of the mutex.
 * \param _prioceiling  Integer where the value of the priority is to be
 *                      stored.
 * \return              Zero if the mutex semaphore uses the priority ceiling
 *                      policy, -1 if not.
 *
 * This function returns the priority ceiling value of a semaphore with
 * this priority policy.
 */
int OS_MutexGetPrioCeiling(OS_MutexID_t _mutex, int *_prioceiling);

#endif

