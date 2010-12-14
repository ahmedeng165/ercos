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
 * $Id: cputypes.h 1025 2006-06-19 14:19:24 +0000 (lun, 19 jun 2006) aitor $
 *
 * $Revision: 1025 $
 * $Date: 2006-06-19 14:19:24 +0000 (lun, 19 jun 2006) $
 * Last CVS commit by $Author: aitor $
 *
 * Registro de cambios
 *
 * $Log$
 * Revision 1.3  2005/10/10 12:56:59  aitor
 * NULL value redefinition
 *
 * Revision 1.2  2005/09/22 11:30:22  aitor
 * CVS Problems
 *
 * Revision 1.7  2005/08/15 12:45:29  aitor
 * Change the OS_context_t name by CPU_context_t
 *
 * Revision 1.6  2005/07/21 10:59:46  aitor
 * Added the OS_context_t struture in this file.
 *
 * Revision 1.5  2005/07/05 16:06:29  aitor
 * Added some comments and other kernel types.
 *
 * Revision 1.4  2005/07/22 09:00:55  aitor
 * Delete the BIG_ENDIAN label.
 *
 * Revision 1.3  2005/07/19 07:58:44  aitor
 * Define the NULL value.
 *
 * Revision 1.2  2005/07/14 15:33:55  aitor
 * Added the TRUE and FALSE types
 *
 * Revision 1.1  2005/07/12 13:43:57  aitor
 * Se cambia de nombre el archivo fuente.
 *
 * Revision 1.1  2005/06/23 12:08:37  aitor
 * Nueva estructura de directorios.
 *
 * Revision 1.2  2005/06/23 10:54:33  aitor
 * Correcci�n de los tipos de 64 bits.
 *
 * Revision 1.1  2005/06/21 10:28:15  aitor
 * Alta del mini sistema ERCOS-RT
 *
 */

#ifndef _SPARC_TYPES_H_
#define _SPARC_TYPES_H_

#include <public/glue.h>
/**
 * Contexts
 *
 *  Generally there are 2 types of context to save.
 *     1. Interrupt registers to save
 *     2. Task level registers to save
 *
 *  This means we have the following 3 context items:
 *     1. task level context stuff::  Context_Control
 *     2. floating point task stuff:: Context_Control_fp
 *     3. special interrupt level context :: Context_Control_interrupt
 *
 *  On the SPARC, we are relatively conservative in that we save most
 *  of the CPU state in the context area.  The ET (enable trap) bit and
 *  the CWP (current window pointer) fields of the PSR are considered
 *  system wide resources and are not maintained on a per-thread basis.
 */

typedef struct context
{
    /*
     *  Using a double g0_g1 will put everything in this structure on a
     *  double word boundary which allows us to use double word loads
     *  and stores safely in the context switch.
     */
    uint64_t     g0_g1;
    uint32_t g2;
    uint32_t g3;
    uint32_t g4;
    uint32_t g5;
    uint32_t g6;
    uint32_t g7;

    uint32_t l0;
    uint32_t l1;
    uint32_t l2;
    uint32_t l3;
    uint32_t l4;
    uint32_t l5;
    uint32_t l6;
    uint32_t l7;

    uint32_t i0;
    uint32_t i1;
    uint32_t i2;
    uint32_t i3;
    uint32_t i4;
    uint32_t i5;
    uint32_t i6_fp;
    uint32_t i7;

    uint32_t o0;
    uint32_t o1;
    uint32_t o2;
    uint32_t o3;
    uint32_t o4;
    uint32_t o5;
    uint32_t o6_sp;
    uint32_t o7;

    uint32_t psr;
    uint32_t y;
}CPU_context_t;


#endif

