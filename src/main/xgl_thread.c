#include "common.h"
#include "shared.h"
#include "main/xgl_thread.h"
#include "xgl_thread.h"

/*
 * xglCreateSignal is file-local (LOCAL in the original symbol table).  Declare
 * it static before the shared header's `extern` prototype so the static
 * definition below does not follow a non-static declaration.
 */
static int xglCreateSignal(void);

/*
 * TU-local thread descriptor view (config/header-canon.json: ee_thread_t is
 * tu_local; xglThreadInitial reproduces only with the `entry` member).
 */
typedef struct {
    int status;
    thread_entry_t entry;
    void *stack;
    int stack_size;
    void *gp_reg;
    int initial_priority;
} ee_thread_t;

extern ee_thread_t sThreadParam;
extern int CreateThread(ee_thread_t *thread);

static int xglCreateSignal(void)
{
    sSemaParam.max_count = 1;
    sSemaParam.init_count = 0;
    return CreateSema(&sSemaParam);
}

void xglThreadRotate(void)
{
    iRotateSignal = xglCreateSignal();
    for (;;) {
        WakeupThread(asActiveThreadList[iCurrentThread].id);
        WaitSema(iRotateSignal);
        iCurrentThread++;
        if (iCurrentThread >= 4)
            iCurrentThread = 0;
    }
}

void xglSleep(void)
{
    int thread_id = GetThreadId();
    CancelWakeupThread(thread_id);
    SignalSema(iRotateSignal);
    SleepThread();
}

void xglThreadInitial(void)
{
    active_thread_t *active = asActiveThreadList;
    system_thread_entry_t *system_record = asSystemThreadList;
    /* The evidenced stack-size/priority tail begins at byte 8 of each record. */
    system_thread_tail_t *thread_config =
        (system_thread_tail_t *)((char *)system_record + 8);
    /* Keep an addressed reference to the real stack_size member. */
    int *stack_size_reference = &thread_config->stack_size;
    ee_thread_t *thread = &sThreadParam;
    unsigned int count = 0;
    int active_offset = 0;
    int stack = 0x1f8000;

    do {
        int stack_size = *stack_size_reference;
        thread_entry_t entry =
            ((system_thread_entry_t *)((char *)thread_config - 8))->entry;
        int priority = thread_config->priority;
        /* Address the known priority member in the parallel active record. */
        int *priority_slot =
            (int *)((char *)&asActiveThreadList[0].priority + active_offset);
        void *stack_base;

        count++;
        stack_base = (void *)(stack - stack_size);
        active->stack_size = stack_size;
        thread->stack_size = stack_size;
        *priority_slot = priority;
        active->entry = entry;
        active->stack = stack_base;
        thread->entry = entry;
        thread->stack = stack_base;
        thread->gp_reg = &_gp;
        thread->initial_priority = priority;
        active->id = CreateThread(thread);
        StartThread(active->id, 0);
        active_offset += sizeof(active_thread_t);
        stack -= *stack_size_reference;
        thread_config =
            (system_thread_tail_t *)((char *)thread_config + 20);
        stack_size_reference = &thread_config->stack_size;
        active++;
    } while (count < 4);

    iCurrentThread = 0;
}
