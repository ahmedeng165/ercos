/*
    Copyright 2004-2005 (c) by Aitor Viana Sanchez,
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://atc1.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT.
*/

#include <private/ercos-hal.h>
#include <public/glue.h>

#include <ercos/config.h>
#include <ercos/kernel/task.h>
#include <ercos/kernel/ktracer.h>
#include <ercos/kernel/sched.h>
#include <ercos/kernel/nomm.h>
#include <ercos/kernel/list.h>
#include <ercos/kernel/sem.h>

#include <ercos/syscall.h> /* For SYS_ThreadExit !! */


/**
 * This is the array that stores all the Threads Control Blocks that the system
 * has (plus the main and the idle task)
 */
OS_task_t task_table[CONFIG_ERCOS_MAX_THREADS + 2];

/**
 * This queue maintains the free TCBs
 */
static OS_List_t tcb_list;

/** This is the main thread identifier  */
#define MAIN_THREAD_TID 1

/** This array stores the kernel stack which is used in the interrupts  */
static uint8_t kernel_stack[CONFIG_ERCOS_KERNEL_STACK_SIZE];

/** This is the pointer to the kernel_stack which is used from the asm code */
uint8_t *kstack = (kernel_stack + CONFIG_ERCOS_KERNEL_STACK_SIZE);
//------------------------------------------------------------------------------

/** this is the idle task */
static void OS_IdleTask (void);

/**
 * This funcion is the thread wrapper.
 *
 * This function is the first "entry point" for all threads. The function
 * invokes the correct thread entry point and then frees all the thread TCB
 * entries.
 */
static void OS_ThreadWrapper(void);

/* This is the main function header */
void main (void);

//------------------------------------------------------------------------------

/*
 * Function:    static void OS_IdleTask(void)
 *
 * This is the IDLE task
 */
static void OS_IdleTask (void)
{
    /* execute the trace reader thread or enter into low-power mode */
#ifdef CONFIG_ERCOS_TRACER
    OS_TracerReaderThread ();
#else
    while (1) {
    	ercos_hal_power_down ();
    }
#endif
}

/*
 * Function: void OS_TCBInit(void)
 *
 * This function initializes all the Thread Control Blocks to the init state
 */
void OS_TCBInit(void)
{
    int i;

    OS_ListReset(&tcb_list);

    /* Assure that the priority of the task 0 is 0. This is important if ERCOS
     * is linked to C++ stuff; constructors (called from OS_CPPInit) may do
     * system calls and the task 0 will store the context of OS_Start function
     * before the first official dispatch (at the end of the function). Until
     * now, priority was 0 because task_table is allocated in the bss which is
     * zeroed at startup time (!!) */
    task_table[0].priority = 0;

    for (i = MAIN_THREAD_TID; i < CONFIG_ERCOS_MAX_THREADS; ++i)
    {
        /*  Init the thread status  */
        task_table[i].status = 0;

        /*  Traps off, set S, PS and FP unit    */
        //task_table[i].context.psr = 0x000010c0;

        /*  Init the thread identifier  */
        task_table[i].tid = i;

        /*  Reset all the nodes associated with each thread */
        OS_ListNodeReset(&task_table[i].node);
        OS_ListNodeReset(&task_table[i].time_node);
        OS_ListNodeReset(&task_table[i].resource_node);

        /*  Add the TCB into the free TCBs queue   */
        OS_ListAppend(&tcb_list, &task_table[i].node);
    }

    /* create the idle task, with the minimum priority */
    OS_TaskCreate (NULL, 0x600, OS_IdleTask, NULL, SCHED_MIN_PRIORITY);

    /* create the main task, with the maximum priority */
    OS_TaskCreate (NULL, CONFIG_ERCOS_MAIN_STACK_SIZE, (void *) main, NULL, SCHED_MAX_PRIORITY);
}

void OS_ThreadExit(void)
{
    int tid = current->tid;

    DEBUG ("(); tid=%d", tid);
    KTRACE (KTRACE_EXIT, current->tid, 0);

    /*  Init the thread status  */
    task_table[tid].status = 0;
    /*  Traps off, set S, PS and FP unit    */
    //task_table[tid].context.psr = 0x000010c0;

    /*  Free the thread stack   */
    OS_Free(task_table[tid].stack.addr);

    /*  Add the TCB into the free TCB queue   */
    OS_ListAppend(&tcb_list, &task_table[tid].node);
}

int OS_TaskGetStatus(tid_t _tid)
{
    return task_table[_tid].status;
}

static void OS_ThreadWrapper(void)
{
    /*  Execute the thread entry point  */
    current->entry_point(current->arg);

    /*  The thread is finished so we free all the TCB entries   */
    SYS_ThreadExit();
}

/*
 * Function: tid_t OS_TaskCreate(uint32_t  _stack_size, void *_ip, uint8 _priority)
 *
 * This routine creates a new thread and returns the TCB created
 */
tid_t OS_TaskCreate(uint32_t *_stackaddr, uint32_t  _stacksize, void *_ip, void *_arg, uint8_t _priority)
{
    OS_task_t *task;
    OS_Node_t *n;

  //  DEBUG ("(_stackaddr=%p, _stacksize=%p, _ip=%p, _arg=%p, priority=%d)",
   //     _stackaddr, _stacksize, _ip, _arg, _priority);

    ASSERT (_priority <= SCHED_MIN_PRIORITY);

    /* get a free thread control block */
    n = (OS_Node_t *) OS_ListGetNode (&tcb_list);

    /* assert that we really have a free tcb */
    ASSERT (n != NULL);
    if (n == NULL) return -1;

    task = GET_ELEMENT(OS_task_t, n, node);
    TRACE (task->tid, "d");
    KTRACE (KTRACE_CREATE, task->tid, _priority);

    /*  Stablish the thread stack */
    if (_stackaddr == NULL)
    {
        if (_stacksize < CONFIG_ERCOS_MIN_STACK) _stacksize = CONFIG_ERCOS_MIN_STACK;
        _stackaddr = OS_Malloc (_stacksize);
    } else {
        ASSERT (_stacksize >= CONFIG_ERCOS_MIN_STACK);
    }
    task->stack.size = _stacksize;
    task->stack.addr = (uint8_t *)_stackaddr;

    /*  Fill the stack with the 0x5a mark */
#ifndef CONFIG_NDEBUG
//    unsigned i;
//    DEBUG ("Stack @ %p, %p bytes", _stackaddr, _stacksize);
//   for (i = 0; i < _stacksize; i++) ((uint8_t *) _stackaddr)[i] = 0x5a;
#endif

    /*  Init the new thread context */
    ercos_hal_hwcontext_init (&(task->context), task->stack.addr, task->stack.size, 0, OS_ThreadWrapper);

    /*  Stablish the thread status  */
    task->status = TS_READY;

    /*  Stablish the thread entry point  */
    task->entry_point = _ip;
    task->arg = _arg;

    /*  Init the pointer to the catched mutex   */
    task->pmutex =(OS_Mutex_t*) NULL;

    /*  Set the thread base and temporal priorities   */
    task->priority = _priority;
    task->base_priority = _priority;

    /*  Add the task to the correspond sched queue  */
    OS_SchedTaskReady(task->tid);

    return task->tid;
}

void OS_TaskGetStackParameters(uint32_t *_stackaddr, uint32_t *_stacksize)
{
    ASSERT (_stackaddr != NULL && _stacksize != NULL);
    *_stackaddr = (uint32_t)current->stack.addr;
    *_stacksize = current->stack.size;

}

void OS_CheckCurrentStackGuard (void)
{
    uint32_t * guard;

    guard = (uint32_t *) current->stack.addr;
    if (*guard != 0x5a5a5a5a) {
        TRACE (current - task_table, "d");
        TRACE (old_current - task_table, "d");
        TRACE (current->stack.addr, "p");
        TRACE (current->stack.size, "p");
        TRACE (guard, "p");
        TRACE (*guard, "p");
	/* TRACE (current->context.usp, "p"); */
    }
    ASSERT (*guard == 0x5a5a5a5a);
}

int OS_TaskIsBlocked(tid_t _tid)
{
    return (task_table[_tid].status & (TS_SEM_BLOCKED | TS_TIME_BLOCKED));
}

