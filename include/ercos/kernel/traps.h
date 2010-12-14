/*
    Copyright 2004-2005 (c) by Aitor Viana Sanchez,
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://atc1.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT on ERC-32.
*/

/*!
 * \file include/kernel/traps.h
 * \brief Kernel level functions and MACROS related to the trap services.
 */

#ifndef _KERNEL_TRAPS_H_
#define _KERNEL_TRAPS_H_

/**
 * \brief Data structure to manage the interrupt handler routines.
 *
 * This structure provides the mechanism to carry out the event propagation to
 * the upper levels.
 */
typedef struct interrupt_control
{
    /** Current interrupt service routine   */
    void(*isr)(int _level);

    /** Old interrupt service routine   */
    void(*old_isr)(int _level);

}OS_Interrupt_Control_t;

#ifndef CONFIG_NDEBUG

/**
 * \brief Traces a trap that has occurred in the system.
 *
 * \param _vec  Vector of the trap.
 * \param l1    Value of the l1 register after the trap has occurred (i. e. the
 *              IP of the instruction that was being executed when the trap
 *              occurred).
 * \param l2    Value of the l2 register after the trap has occurred (i. e. the
 *              instruction following to the one that was being executed when the
 *              trap occurred).
 */
void CPU_TraceTraps (unsigned long _vec, int l1, int l2);
#endif

/**
 * \brief Register IRQ handler.
 *
 * \param _vec     The interrupt number.
 * \param handler  The IRQ handler rountine.
 * \return         The function returns 0 in case of success registration and
 *                 returns -1 in any other case.
 *
 * This function registers a irq handler.
 */
void OS_RegisterIRQ(unsigned int _vec, void (*handler)(int level));

/**
 * \brief Entry point for all the trap handlers.
 *
 * \param vector  Vector of the trap.
 */
void OS_TrapEntry (unsigned int vector);

/**
 * \brief Restores the previous handler routine of a given IRQ.
 *
 * \param vector  Vector of the trap.
 * \return        Zero if the operation performs correctly, -1 otherwise.
 */
void OS_RestoreIRQ(int vector);


/**
 * \brief Inits all the trap handlers.
 */
void OS_TrapInit(void);

/**
 * \brief Uninstall a trap hanlder for a given IRQ.
 *
 * \param trap     Vector of the trap.
 * \param handler  Current IRQ handler routine.
 * \return         Zero if the operation performs correctly, -1 otherwise.
 */
int OS_FreeIRQ(int trap,void(*handler)(int level));

#endif  //_TRAPS_H_

