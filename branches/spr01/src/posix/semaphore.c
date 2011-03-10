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
 * $Id: semaphore.c 1280 2008-06-03 11:49:54Z cesar $
 *  
 * $Revision: 1280 $
 * $Date: 2008-06-03 13:49:54 +0200 (mar, 03 jun 2008) $
 * Last CVS commit by $Author: cesar $
 */

#include <ercos/posix/semaphore.h>
#include <ercos/kernel/irq.h>
#include <ercos/syscall.h>

//------------------------------------------------------------------------------

int sem_init(sem_t *sem, int pshared, unsigned int value)
{
    return SYS_SemInit(sem, value);
}

int sem_destroy(sem_t * sem)
{
    return SYS_SemDestroy(sem);
}

int sem_wait(sem_t * sem)
{
    return SYS_SemWait(*sem);
}

int sem_trywait(sem_t * sem)
{
    return SYS_SemTryWait(*sem);
}

int sem_post(sem_t * sem)
{
    if(OS_IS_IN_TRAP_HANDLER()) {
        SYS_IRQMarkEvent(*sem);
    }
    else
        return SYS_SemPost(*sem);
    
    return 0;
}

int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout)
{
    OS_Time_t ticks;

    _POSIX_timespec_to_ticks(abs_timeout, ticks);
    return SYS_SemTimedWait(*sem, ticks);
}

