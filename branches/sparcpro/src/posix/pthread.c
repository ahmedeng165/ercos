
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
 * $Id: pthread.c 1280 2008-06-03 11:49:54Z cesar $
 *
 * $Revision: 1280 $
 * $Date: 2008-06-03 13:49:54 +0200 (mar, 03 jun 2008) $
 * Last CVS commit by $Author: cesar $
 */

#include <public/glue.h>
#include <public/posix.h>

#include <ercos/posix/pthread.h>
#include <ercos/syscall.h>

//------------------------------------------------------------------------------

static uint32_t ts_bitmap = 0;

/**
 * The size of the minimun stack storage.
 */
#define PTHREAD_STACK_MIN   (2048)

//------------------------------------------------------------------------------

/**********************************************
 * THREAD CREATION
 *********************************************/

/*
 * Function:    int pthread_attr_init(pthread_attr_t *attr)
 */
int pthread_attr_init(pthread_attr_t *attr)
{
    attr->stacksize = 0;
    attr->stackaddr = NULL;

    return 0;
}

/*
 * Function: int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
 */
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
    if (stacksize >= PTHREAD_STACK_MIN)
        attr->stacksize = stacksize;
    else if (stacksize < PTHREAD_STACK_MIN)
        attr->stacksize = PTHREAD_STACK_MIN;
    else
        return -1;


    return 0;
}

/*
 * Function: int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr)
 */
int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr)
{
    if (stackaddr)
        attr->stackaddr = stackaddr;
    else
        return -1;


    return 0;
}

/*
 * Function: int pthread_create(pthread_t  *thread, const pthread_attr_t *attr,
 */
int pthread_create(pthread_t  *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void*), void  *arg)
{
    pthread_t tid;

    tid = SYS_TaskCreate(attr->stackaddr,
                        attr->stacksize,    /*  The thread stack size   */
                        start_routine,      /*  The Instruction Pointer */
                        arg,
                        attr->thread_prio); /*  The thread priority     */

    *(thread) = tid;

    return 0;
}

/**********************************************
 * THREAD PARAMAETERS
 *********************************************/

/*
 * Function: int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param)
 */
int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param)
{
    if (param->sched_priority >= 0 && param->sched_priority <= SCHED_MIN_PRIORITY)
        attr->thread_prio = param->sched_priority;
    else
        return -1;

    return 0;
}

int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param)
{
    OS_Priority_t priority;

    *policy = SCHED_OTHER;

    if (param != NULL) {
        priority = SYS_GetPriority(thread);
        param->sched_priority = priority;
    }

    return 0;
}

int pthread_setschedparam(pthread_t thread, int policy, const
                          struct sched_param *param)
{
    if (policy == SCHED_OTHER)
        SYS_SetPriority(thread, param->sched_priority);
    else
        return -1;

    return 0;
}

/**********************************************
 * THREAD MUTEX
 *********************************************/

int pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
    attr->mutex_type = attr->prio_type = 0;

    return 0;
}

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int kind)
{
    if (kind < 0 || kind > PTHREAD_MUTEX_NORMAL)
        return -1;
    else
        attr->mutex_type = kind;
    return 0;
}

int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int kind)
{
    if (kind < 0 || kind > PTHREAD_PRIO_PROTECT)
        return -1;
    else
        attr->prio_type = kind;
    return 0;
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr)
{
    /*  Init the pthread_mutex parameters   */
    mutex->prio_type = mutexattr->prio_type;
    mutex->mutex_type = mutexattr->mutex_type;

    /*  Init the operating system mutex */
    SYS_MutexInit(&mutex->id, mutexattr->prio_type);

    if(mutexattr->prio_type == PTHREAD_PRIO_PROTECT)
    {
        /*  The kernel highest priority is the 0 value   */
        int priority = SCHED_MIN_PRIORITY - mutexattr->prioceiling;

        priority = (priority < 0) ? 0 : priority;

        /*  init the Priority ceiling attribute  */
        SYS_MutexSetPrioCeiling(&mutex->id, priority);
    }
    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    return SYS_MutexDestroy(&mutex->id);
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    return SYS_MutexLock(mutex->id);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    return SYS_MutexUnLock(mutex->id);
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    return SYS_MutexTryLock(mutex->id);
}


int pthread_mutexattr_setprioceiling (pthread_mutexattr_t *attr, int prioceiling)
{
    int retval = 0;

    if(attr->prio_type == PTHREAD_PRIO_PROTECT) {
        attr->prioceiling = prioceiling;
        retval = 0;
    }else
        retval = -1;

    return retval;
}

/**********************************************
 * THREAD SPECIFIC STORAGE
 *********************************************/

static uint32_t pthread_get_tskey(void)
{
    unsigned int i;

    for (i = 0; i < 32; ++i) {
        if(((1 << i) & ts_bitmap) == 0) {
            ts_bitmap |= (1 << i);
            return i;
        }
    }

    return -1;
}
int pthread_key_create(pthread_key_t *key, void (*destructor)(void *))
{
    *key = pthread_get_tskey();

    if(key < 0) return -1;

    return 0;

}

int pthread_setspecific(pthread_key_t key, const void *value)
{
    uint32_t stackaddr, stacksize;
    uint32_t *stackend;

    if(((1 << key) & ts_bitmap) == 0)
        return -1;

    /*  Get the caller stack parameters */
    SYS_TaskGetStackParameters(&stackaddr, &stacksize);

    /*  End stack address   */
    stackend = ((uint32_t*)stackaddr - stacksize);
    //DEBUG ("stackaddr=%p; stacksize=%p; stackend=%p; offset=%p", stackaddr, stacksize, stackend, key * sizeof (void *));

    /*  Init the thread specific storage area   */
    *(stackend + (key * sizeof(void *))) = (uint32_t)value;

    return 0;
}

void *pthread_getspecific(pthread_key_t key)
{
    uint32_t stackaddr, stacksize;
    uint32_t *stackend;

    /*  Get the caller stack parameters */
    SYS_TaskGetStackParameters(&stackaddr, &stacksize);

    stackend = ((uint32_t*)stackaddr - stacksize);
   //DEBUG ("stackaddr=%p; stacksize=%p; stackend=%p; offset=%p", stackaddr, stacksize, stackend, key * sizeof (void *));

#ifndef CONFIG_NDEBUG
    struct timespec tp;
    ASSERT (clock_gettime (CLOCK_REALTIME, &tp) == 0);
    //DEBUG ("Time: s=%u ns=%u", tp.tv_sec, tp.tv_nsec);
#endif

    return (void *)(*(stackend + (sizeof(void *) * key)));

}

int pthread_key_delete(pthread_key_t key)
{
    ts_bitmap &= ~(1 << key);

    return 0;
}

