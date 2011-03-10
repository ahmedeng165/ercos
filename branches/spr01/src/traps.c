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
 * $Id: traps.c 1281 2008-06-17 18:24:57Z cesar $
 *
 * $Revision: 1281 $
 * $Date: 2008-06-17 20:24:57 +0200 (mar, 17 jun 2008) $
 * Last CVS commit by $Author: cesar $
 */

#include <public/glue.h>
#include <private/ercos-hal.h>
#include <ercos/lib/stdio.h>

#include <ercos/kernel/traps.h>
#include <ercos/kernel/sched.h>
#include <ercos/kernel/time.h>
#include <ercos/kernel/panic.h>
#include <ercos/kernel/irq.h>

/**
 * The trap list handler structure
 */
OS_Interrupt_Control_t interrupt_handler[ERCOS_HAL_MAX_INTR];

//----------------------------------------------------------------------------

/*
 * Function: int OS_RegisterIRQ(unsigned int _vec, void (*handler)(int level))
 *
 * Register a IRQ interrupt
 */
void OS_RegisterIRQ(unsigned int _vec, void (*handler)(int level))
{
    DEBUG ("_vec=%d, handler=%p", _vec, handler);
    /* assert that _vec is a valid interrupt number */
    ASSERT (_vec < ERCOS_HAL_MAX_INTR);

    /* never register interrupt handler for the time management    */
    ASSERT (IS_TIMER_INTERRUPT(_vec) ? interrupt_handler[_vec].isr == NULL : 1);

    /* update the table in mutual exclusion */
    OS_DisableIRQ ();

    /* register the handler */
    interrupt_handler[_vec].old_isr = interrupt_handler[_vec].isr;
    interrupt_handler[_vec].isr = handler;

    /* update the table in mutual exclusion */
    OS_EnableIRQ ();

    /* umask the interrupt in case it is an irq (the primitive will
     * silently return in case the vector is not an interrupt) */
    OS_EnableInterrupt (_vec);
}

/*
 * Function:  int OS_FreeIRQ(int vector,void(*handler)(int level)))
 *
 * FIXME -- dead code
 * Frees the irq handler interrupt
 */
int OS_FreeIRQ(int vector,void(*handler)(int level))
{
    /* assert that vector is a valid interrupt number */
    ASSERT (vector < ERCOS_HAL_MAX_INTR);

    if (interrupt_handler[vector].isr == handler)
    {
        /*  No interrupt handler    */
        interrupt_handler[vector].isr = NULL;

        /*  No previous interrupt handler  */
        interrupt_handler[vector].old_isr = NULL;

        /*  Mask the irq (in case it is an irq)  */
        OS_MaskInterrupt (vector);

        return 0;   /*  OK  */
    }

    return -1;  /*  ERROR   */
}

void OS_RestoreIRQ(int vector)
{
    DEBUG ("vector=%d", vector);

    /* assert that vector is a valid interrupt number */
    ASSERT (vector < ERCOS_HAL_MAX_INTR);

    /* begin mutual exclusion */
    OS_DisableIRQ();

    /* assure that this is not a erratic call */
    ASSERT (interrupt_handler[vector].isr != NULL);

    /* restore the handler */
    interrupt_handler[vector].isr = interrupt_handler[vector].old_isr;
    interrupt_handler[vector].old_isr = NULL;

    /* mask the irq (in case it is an irq)  */
    OS_MaskInterrupt (vector);

    /* end mutual exclusion */
    OS_EnableIRQ();
}

/*
 * Function:  void OS_TrapInit(void)
 *
 * This function initializes the interrupts and traps.
 */
void OS_TrapInit(void)
{
    unsigned int i;

    /*  Initialize all the interrupt handlers to NULL   */
    for (i = 0; i < ERCOS_HAL_MAX_INTR ; i++) {
        /*  No interrupt handler    */
        interrupt_handler[i].isr = NULL;

        /*  No previous interrupt handler  */
        interrupt_handler[i].old_isr = NULL;
    }
}

/*
 * Function:  void OS_TrapEntry(int vector, int addr1, int addr2)
 *
 * Trap entry function.
 */
void OS_TrapEntry (unsigned int vector)
{
    /* illegal vector number ? */
    if (vector >= ERCOS_HAL_MAX_INTR) {
        TRACE (vector, "u");
        OS_KernelPanic ("Illegal interrupt!!");
    }

    /* have we got a registered interrupt handler ? */
    if (interrupt_handler[vector].isr == 0) {
        TRACE (vector, "u");
        OS_KernelPanic ("No interrupt handler registered!");
    }

    /* execute the handler */
    interrupt_handler[vector].isr(vector);

    /* clear the "interrupt pending" bit */
    OS_ClearInterrupt (vector);
}

