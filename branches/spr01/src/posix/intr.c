/*
    Copyright 2004-2005 (c) by Aitor Viana Sanchez,
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://atc1.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT project.
*/

/*
 * File:    $Source$
 *
 * $Id: intr.c 1285 2008-06-23 15:36:09Z cesar $
 *
 * $Revision: 1285 $
 * $Date: 2008-06-23 17:36:09 +0200 (lun, 23 jun 2008) $
 * Last CVS commit by $Author: cesar $
 */

#include <ercos/posix/intr.h>
#include <ercos/syscall.h>

//----------------------------------------------------------------------------

int intr_disable (void)
{
    SYS_Disable ();

    return 0;
}

int intr_enable (void)
{
    SYS_Enable ();

    return 0;
}

int intr_lock(intr_t  intr)
{
    SYS_IntrLock(intr);

    return 0;
}

int intr_unlock(intr_t  intr)
{
    SYS_IntrUnlock(intr);

    return 0;
}

int intr_capture(void (*intr_handler)(int _level), int level)
{
    SYS_RegisterIRQ (level, intr_handler);
    return 0;
}


int intr_restore(int level)
{
    SYS_RestoreIRQ(level);
    return 0;
}


