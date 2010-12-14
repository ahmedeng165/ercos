
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
 * POSIX Thread data types
 */

#ifndef _POSIX_PTHREAD_H_
#define _POSIX_PTHREAD_H_

#include <public/glue.h>
#include <ercos/kernel/mutex.h>

#define PTHREAD_PRIO_NONE	MUTEX_PRIO_NONE
#define PTHREAD_PRIO_NORMAL	PTHREAD_PRIO_NONE /* ?? */
#define PTHREAD_PRIO_INHERIT	MUTEX_PRIO_INHERIT
#define PTHREAD_PRIO_PROTECT	MUTEX_PRIO_CEILING

#define PTHREAD_MUTEX_NORMAL	0

#define SCHED_OTHER		0

/** Thread attribute information */
typedef struct {
	uint32_t stacksize;
	void * stackaddr;
	unsigned thread_prio;
} pthread_attr_t;

/** Mutex attribute information */
typedef struct {
	unsigned prio_type;
	unsigned mutex_type;
	int prioceiling;
} pthread_mutexattr_t;

/** Mutex structure */
typedef struct {
	OS_MutexID_t id;
	unsigned prio_type;
	unsigned mutex_type;
} pthread_mutex_t;

/** Thread identifier: an alias of the ERCOS internal thread identifier */
typedef tid_t pthread_t;

/** Thread specific key */
typedef uint32_t pthread_key_t;

/** Contains the scheduling parameters required for implementation of each
 * supported scheduling policy. */
struct sched_param {
	int sched_priority;
};

int pthread_attr_init(pthread_attr_t *attr);
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void*), void *arg);
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr);
int pthread_attr_setschedparam(pthread_attr_t *attr,
		const struct sched_param *param);
int pthread_getschedparam(pthread_t thread, int *policy,
		struct sched_param *param);
int pthread_setschedparam(pthread_t thread, int policy,
		const struct sched_param *param);
int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int kind);
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int kind);
int pthread_mutex_init(pthread_mutex_t *mutex,
		const pthread_mutexattr_t *mutexattr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_key_create(pthread_key_t *key, void (*destructor)(void *));
int pthread_setspecific(pthread_key_t key, const void *value);
void *pthread_getspecific(pthread_key_t key);
int pthread_key_delete(pthread_key_t key);
int pthread_mutexattr_setprioceiling (pthread_mutexattr_t *attr,
		int prioceiling);
int pthread_mutexattr_getprioceiling (pthread_mutexattr_t *attr,
		int *prioceiling);

#endif
