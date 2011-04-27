/*
    Copyright 2004-2005 (c) by Aitor Viana Sanchez,
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://atc1.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT.
*/

/*
 * File:    $Source$
 * 
 * $Id:irqev.c 240 2005-12-14 01:03:49 +0100 (mié, 14 dic 2005) aitor $
 *  
 * $Revision:240 $
 * $Date:2005-12-14 01:03:49 +0100 (mié, 14 dic 2005) $
 * Last CVS commit by $Author:aitor $
 */

#include <ercos/kernel/irqev.h>
#include <ercos/kernel/types.h>
#include <ercos/kernel/sem.h>
#include <ercos/kernel/list.h>
#include <ercos/kernel/sched.h>
#include <ercos/kernel/task.h>
#include <ercos/kernel/ktracer.h>
#include <ercos/kernel/sem.h>

//------------------------------------------------------------------------------

/** 
 * This flag is modified when an event marked. It is used in the IRQ epilogue 
 */
static uint32_t event_pending = 0;

/** This list maintains the pending events  */
static OS_List_t event_list;

/**  This flag is necessary to notify a clock event  */
static uint8_t clock_event = 0;

/** This macro increments the event_pending flag  */
#define INCREASE_PENDING_EVENTS() (event_pending++)

/** This macro decrease the event_pending flag  */
#define DECREASE_PENDING_EVENTS() (event_pending--)

/** This macro clears the event_pending flag  */
#define CLEAR_PENDING_EVENTS() (event_pending = 0)

/** This macro verifies that the event_pending flag is set */
#define IS_EVENT_PENDING() (0 < event_pending)

//------------------------------------------------------------------------------

#define SEM(_id_sem)    (&(semaphores[_id_sem]))

//------------------------------------------------------------------------------

/**
 * This routine carries out all the semaphore post operation over the IRQ 
 * semaphores. 
 * 
 * @param _sem: This parameter is the semaphore that must be signaled.
 * 
 * When a thread is blocked waitting for an IRQ it is blocked in
 * an event semaphore. The IRQ handler must signal the event semaphore but this
 * operation is not performed, only is marked. All of this operations are 
 * performed later when the OS_IRQHandlerEvent routine is executed, and the 
 * OS_IRQSemPost routine perform all the signal operations.
 */
static void OS_IRQSemPost(OS_Sem_t *_sem);

//------------------------------------------------------------------------------


void OS_IRQInitEvents(void)
{
    
    OS_ListReset(&event_list);
    
}

void OS_IRQHandleEvent(void)
{
    OS_Sem_t *sem;
    OS_Node_t *pos;

    if(IS_EVENT_PENDING())
    {

        KTRACE (KTRACE_HANDLEEVENTS, 0, 0);

        list_for_each(pos, &event_list)
        {
            sem = GET_ELEMENT(OS_Sem_t, OS_ListGetNode(&event_list), node);

            DECREASE_PENDING_EVENTS();

            OS_EnableIRQ();
            OS_IRQSemPost(sem);
            OS_DisableIRQ();

        }
        /*  The event list must be reset    */
        OS_ListReset(&event_list);

        /*  
         * We clear the clock_event flag.
         * We use the while instead of the if sentence because it is possible
         * that another clock events was triggered when the OS_ClockBottomISR()
         * routine programs the next timer.
         */
        while(clock_event) {
            clock_event = 0;

            DECREASE_PENDING_EVENTS();

            OS_ClockBottomISR();

        }
    }
    
    CLEAR_PENDING_EVENTS();

}



void OS_IRQMarkClockEvent(void)
{
    /*
     * This routine must be executed in mutual exclusion with the INTERRUPTS 
     * DISABLED
     */

    /*  We notify the clock event   */
    clock_event = 1;

    /*  Signal the event to ensure the correct re-scheduling   */
    INCREASE_PENDING_EVENTS();
}

void OS_IRQMarkEvent(OS_SemID_t _sem)
{
    OS_SemID_t id_sem = _sem;
    
    OS_DisableIRQ();
    
    /*  Signal the event to ensure the correct re-scheduling   */
    INCREASE_PENDING_EVENTS();

    OS_ListAppend(&event_list, &(SEM(id_sem)->node));

    OS_EnableIRQ();
        
}

static void OS_IRQSemPost(OS_Sem_t *_sem)
{
    OS_task_t *task;

    if(OS_ListGetNum(&(_sem->list)) != 0)
    {

        /*  Outs the task from the semaphore queue  */
        task = GET_ELEMENT(OS_task_t, OS_ListGetNode(&(_sem->list)), resource_node);

        /*  Store what is the blocked thread in the semaphore structure */
        _sem->tid_in = task->tid;
        
        /*  Add the blocked task to the ready task queue    */
        OS_SchedTaskReady(task->tid);
    }
    else {
        /*  Increment the semaphore counter */
        _sem->sem_value = 1;
    }
}

