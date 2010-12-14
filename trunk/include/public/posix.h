
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
 * POSIX interface for ERCOS
 */

#ifndef _PUBLIC_POSIX_H_
#define _PUBLIC_POSIX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ercos/posix/intr.h>
#include <ercos/posix/pthread.h>
#include <ercos/posix/resource.h>
#include <ercos/posix/semaphore.h>
#include <ercos/posix/time.h>
#include <ercos/posix/trace.h>


/* FIXME -- this primitives shouldn't be provided by ercos */
#include <ercos/lib/crc32.h>
#include <ercos/lib/math.h>
#include <ercos/lib/stdarg.h>
#include <ercos/lib/stdio.h>
#include <ercos/lib/stdlib.h>
#include <ercos/lib/string.h>

#ifdef __cplusplus
}
#endif

#endif

