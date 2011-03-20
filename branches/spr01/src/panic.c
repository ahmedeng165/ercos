/*
    Copyright 2004-2005 (c) by Aitor Viana Sanchez,
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://atc1.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT on ERC-32.
*/

/*
 * File:    $Source$
 *
 * $Id: panic.c 1280 2008-06-03 11:49:54Z cesar $
 *
 * $Revision: 1280 $
 * $Date: 2008-06-03 13:49:54 +0200 (mar, 03 jun 2008) $
 * Last CVS commit by $Author: cesar $
 */

#include <public/glue.h>

#include <ercos/kernel/panic.h>
#include <ercos/kernel/task.h>
#include <ercos/lib/stdio.h>

void OS_KernelPanic (const char * str)
{
    DEBUG ("AIEEE!! kernel panic: %s\n", str);

    DEBUG ("current 0x%x", current - task_table);
    DEBUG ("old_current 0x%x", old_current - task_table);
    DEBUG ("abs_timeout %x %d", (uint32_t)(current->abs_timeout >> 32), (uint32_t)current->abs_timeout);
    DEBUG ("abs_timeout dir 0x%x", &(current->abs_timeout));

    DEBUG ("AIEEE!! endless loop()\n");
    while(1);
}

