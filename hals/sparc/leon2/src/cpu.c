/*
    Copyright 2004-2005 (c) by Aitor Viana Sanchez,
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://atc1.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT.
*/

/*!
 * \file arch/sparc/LEON3/cpu.c
 * \brief CPU Initialization function definition.
 */

//------------------------------------------------------------------------------

#include <leon2/platform.h>
#include <public/glue.h>

//------------------------------------------------------------------------------

/*
 * Function:    void CPU_Init(void)
 */
void CPU_Init(void)
{
    REGMAP.timer_1_control = 0;
    REGMAP.timer_2_control = 0;
}
