
/*
    Copyright 2008 (c) by Cesar Rodriguez Ortega,
    Copyright 2004-2005 (c) by Aitor Viana Sanchez,
    Space Research Group,
    University of Alcala.

    For further information, please visit http://srg.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT project.
*/

/*
 * POSIX semaphore management functions
 */

#ifndef _POSIX_SEMAPHORE_H_
#define _POSIX_SEMAPHORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ercos/kernel/sem.h>
#include <ercos/posix/time.h>

typedef OS_SemID_t sem_t;

int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t * sem);
int sem_wait(sem_t * sem);
int sem_trywait(sem_t * sem);
int sem_post(sem_t * sem);
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);

#ifdef __cplusplus
}
#endif


#endif

