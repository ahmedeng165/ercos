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
 * \file include/kernel/irq.h
 * \brief Contains all the MACRO definitions and functions related to the
 *        IRQ management service
 */

#ifndef _KERNEL_IRQ_H_
#define _KERNEL_IRQ_H_

#include <private/ercos-hal.h>

/**
 * \brief This is the irq number for the real time clock.
 */
#define TIMER_TRAP  ERCOS_HAL_PIT_INTR

/**
 * \brief This is the irq number for the general purpose timer.
 */
#define CLOCK_TRAP  ERCOS_HAL_CLOCK_INTR

/**
 * \brief This macro return TRUE in case of nested interrupt.
 */
#define OS_IS_IN_TRAP_HANDLER()  (ercos_hal_intr_ongoing ())

/**
 * \brief This macro enables the interrupts.
 */
#define OS_EnableIRQ()  ercos_hal_intr_enable ()

/**
 * \brief This macro disables the interrupts.
 */
#define OS_DisableIRQ() ercos_hal_intr_disable ()

/**
 * \brief This macro mask the interrupt vector passed as parameter.
 *
 * \param vector  The interrupt vector
 */
#define OS_MaskInterrupt(vector)    ercos_hal_intr_mask (vector)

/**
 * \brief This macro clears the interrupt vector passed as parameter, after the
 *        interrupt has been triggered.
 *
 * \param vector  The interrupt vector
 */
#define OS_ClearInterrupt(vector)   ercos_hal_intr_clear (vector)

/**
 * \brief This macro enables the interrupt vector passed as parameter.
 *
 * \param vector  The interrupt vector
 */
#define OS_EnableInterrupt(vector)  ercos_hal_intr_umask (vector)

#endif  // #ifndef _IRQ_H_

