
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

#include <ercos/posix/resource.h>
#include <ercos/syscall.h>

int getpriority(int which, int who)
{
    if (which != PRIO_PROCESS) return -1;
    return SYS_GetPriority (SYS_GetCurrentTid ());
}

int setpriority(int which, int who, int prio)
{

    /* check parameters */
    if (which != PRIO_PROCESS) return -1;
    if (who != 0) return -1;
    	
    /* update the priority of the current task */
    SYS_SetPriority(SYS_GetCurrentTid (), prio);
    return 0;
}

