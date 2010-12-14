/*
    Copyright 2004-2005 (c) by Aitor Viana Sanchez,
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://atc1.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT on ERC-32.
*/

#include <public/glue.h> /* DEBUG and integer types */
#include <private/ercos-hal.h>

#include <ercos/config.h>
#include <ercos/kernel/cppinit.h>
#include <ercos/kernel/task.h>
#include <ercos/kernel/tracer.h>
#include <ercos/kernel/traps.h>
#include <ercos/kernel/nomm.h>
#include <ercos/kernel/irqev.h>
#include <ercos/lib/stdio.h>
#include <ercos/syscall.h>

#ifdef CONFIG_ERCOS_GDB_MONITOR
void gdb_monitor(void);
#endif

/** This array stores the kernel stack which is used in the interrupts  */
static uint8_t kernel_stack[CONFIG_ERCOS_KERNEL_STACK_SIZE];

/* variables for the <public/ecos-lah.h> interface */
const void * ercos_lah_stack_base = kernel_stack;
const void * ercos_lah_stack_top = kernel_stack + CONFIG_ERCOS_KERNEL_STACK_SIZE;
volatile uint32_t * ercos_lah_schedule_active = &schedule_active;

//------------------------------------------------------------------------------

/*
 * Kernel start entry point
 */
void OS_Start(void)
{

    /* If the GDB Monitor is active, enter in debug mode */
#ifdef CONFIG_ERCOS_GDB_MONITOR
    int car;
    printf("Run debugger? (y/n): ");
    car=ercos_hal_uart_rx();
    printf("%c\n", car);
    if (car == 'y') gdb_monitor();
#endif

#ifdef CONFIG_ERCOS_TRACER
    OS_TracerInit();
#endif

    /*
     * Init the Trap Table
     */
    OS_TrapInit();

    /*
     * Init the stack memory pool
     */
    OS_MMInitMemoryPool (ERCOS_MM_MAX_CHUNK, ERCOS_MM_MIN_CHUNK, (void *) CONFIG_ERCOS_STACK_BEGIN_ADDRESS);

    /*
     * init the Schedule
     */
    OS_SchedInit();

    /*
     * Init the General Purpose Timer and the Real Time Clock
     */
    OS_TimeInit();

    /*
     * Init all the Thread Control Blocks
     *
     * The routine also init the idle task
     */

    OS_TCBInit();

    OS_IRQInitEvents();

    /*
     * Init all the C++ Constructors in case of need
     */
    DEBUG ("calling c++ ctors");
    OS_CPPInit();

    /*  First Schedule  */
    DEBUG ("ercos initialization done; first dispatch!");
    OS_Schedule();
    ercos_hal_dispatch ();

    /*  NEVER REACH HERE    */
}

