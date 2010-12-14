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
 * \file arch/sparc/common/cpu.c
 * \brief Definition of the SPARC common task context init routine.
 */

//------------------------------------------------------------------------------

#include <cpu.h>
#include <arch.h>
#ifdef CONFIG_MP_ERC32
#include <erc32/platform.h>
#endif
#ifdef CONFIG_MP_LEON2
#include <leon2/platform.h>
#endif
#ifdef CONFIG_MP_LEON3
#include <leon3/platform.h>
#endif

//------------------------------------------------------------------------------

extern uint32_t __ERC32_MEC_Timer_Control_Mirror;
extern uint32_t ERC32_RAM_START[];
extern uint32_t CODE_END[];
//extern char bss_start[];
//extern char end[];
//------------------------------------------------------------------------------

void sparcpro_context_init(sparcpro_context_t  *the_context,
                     uint8_t          *stack_base,
                     uint32_t         size,
                     uint32_t         level,
                     void           *entry_point)
{
    uint32_t   stack_high;  /* highest "stack aligned" address */
    uint32_t   the_size;
    uint32_t   tmp_psr;

    /*
     *  On CPUs with stacks which grow down (i.e. SPARC), we build the stack
     *  based on the stack_high address.
     */

    stack_high = (uint32_t)((stack_base) + size);
    stack_high &= ~(CPU_STACK_ALIGNMENT - 1);

    the_size = size & ~(CPU_STACK_ALIGNMENT - 1);

    /*
     *  See the README in this directory for a diagram of the stack.
     */

//    the_context->i6_fp = stack_high - CPU_MINIMUM_STACK_FRAME_SIZE;
//    the_context->o6_sp = the_context->i6_fp - CPU_MINIMUM_STACK_FRAME_SIZE;
    the_context->i6_fp = stack_high - CPU_MINIMUM_STACK_FRAME_SIZE;


    the_context->l1 = ((uint32_t) entry_point);
    the_context->l2 = ((uint32_t) entry_point) + 4;

    /*  In case the task has a level bigger than the maximum number of register
     *  windows sets, we assign the upper level. Otherwise the fiven level
     */
    /*  FIXME: check the level  */
    the_context->regwin_level = level;
//    the_context->regwin_level = 
//        (level > CONFIG_SPARCPRO_HAL_TASK_REG_WINDOWS) ?
//        CONFIG_SPARCPRO_HAL_TASK_REG_WINDOWS : level;

    /*
     *  Build the PSR for the task.  Most everything can be 0 and the
     *  CWP is corrected during the context switch.
     *
     *  The EF bit determines if the floating point unit is available.
     *  The FPU is ONLY enabled if the context is associated with an FP task
     *  and this SPARC model has an FPU.
     */

    sparc_get_psr( tmp_psr );
    tmp_psr &= ~PSR_PIL;
    tmp_psr |= (0 << 8) & PSR_PIL;
    tmp_psr &= ~PSR_EF;      /* disabled by default */

    the_context->psr = tmp_psr;
}

//void clear_bss(void)
//{
//    char * p = bss_start;
//    for (p = bss_start; p != end; p++)
//        *p = 0;
//}
