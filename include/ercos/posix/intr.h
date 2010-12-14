
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

#ifndef _POSIX_INTR_H_
#define _POSIX_INTR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <public/glue.h>

typedef uint32_t intr_t;

int intr_disable (void);
int intr_enable (void);
int intr_lock (intr_t  intr);
int intr_unlock (intr_t  intr);
int intr_capture (void (* intr_handler) (int _level), int level);
int intr_restore (int level);

#ifdef __cplusplus
}
#endif

#endif

