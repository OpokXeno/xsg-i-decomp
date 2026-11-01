#include "common.h"
#include "shared.h"
#include "main/xgl_thread.h"
#include "xgl_thread.h"

/*
 * TU-local thread descriptor view (config/header-canon.json: ee_thread_t is
 * tu_local; xglThreadInitial reproduces only with the `entry` member).
 * The original allocation is 48 bytes, but this TU uses only its known prefix.
 * The suffix follows the documented EE kernel thread-status layout as a partial
 * storage hypothesis. Its original declaration and application semantics remain
 * unresolved; these field names do not establish that original semantic type.
 */
typedef struct {
    int status;
    thread_entry_t entry;
    void *stack;
    int stack_size;
    void *gp_reg;
    int initial_priority;
    int current_priority;
    u32 attr;
    u32 option;
    u32 waitType;
    u32 waitId;
    u32 wakeupCount;
} ee_thread_t;

/*
 * xglCreateSignal is file-local (LOCAL in the original symbol table).  Declare
 * it static before the shared header's `extern` prototype so the static
 * definition below does not follow a non-static declaration.
 */
static int xglCreateSignal(void);

extern int CreateThread(ee_thread_t *thread);
extern void xglRenderEntry(void *argument);
extern void xglCdControlThread(void *argument);
extern void xglStudioEntry(void *argument);
extern void ControlEntry(void *argument);

static system_thread_entry_t asSystemThreadList[4] = {
    { xglRenderEntry, 0, 0x1000, 9, 0 },
    { xglCdControlThread, 0, 0x1000, 10, 0 },
    { xglStudioEntry, 0, 0x1000, 12, 0 },
    { ControlEntry, 0, 0x10000, 11, 0 },
};

static int iRotateSignal;

static unsigned int iCurrentThread;

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/*
 * Storage-capacity hypothesis for original asActiveThreadList (0x00880fe8).
 * Its original ELF size is unspecified. A 400-byte allocation followed by
 * eight alignment bytes ends at the next TU's FS storage (0x00881180).
 * Only the first four records are used by the original consumers below;
 * this capacity does not establish twenty logical or initialized threads.
 */
static active_thread_t asActiveThreadList[20];

static int xglCreateSignal(void)
{
    static ee_sema_t sSemaParam;

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
    static ee_thread_t sThreadParam;

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
