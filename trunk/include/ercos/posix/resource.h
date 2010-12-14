
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

#ifndef _POSIX_RESOURCE_H_
#define _POSIX_RESOURCE_H_

#define PRIO_PROCESS 0

int getpriority(int which, int who);
int setpriority(int which, int who, int prio);

#endif

