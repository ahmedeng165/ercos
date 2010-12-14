
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
 * Kernel data types
 */

#ifndef _KERNEL_TYPES_H_
#define _KERNEL_TYPES_H_

#include <public/glue.h> /* basic types */

typedef uint16_t event_t;
typedef uint32_t tid_t; /* FIXME -- this type should follow the naming "OS_" naming convention */
typedef uint32_t OS_MutexID_t;
typedef int OS_Priority_t;
typedef uint32_t OS_SemID_t;
typedef uint64_t OS_Time_t;

#endif

