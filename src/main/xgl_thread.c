#include "common.h"
#include "shared.h"
#include "main/xgl_thread.h"
#include "xgl_thread.h"
#include "main/xgl_studio.h"

/*
 * TU-local thread descriptor. This TU uses only the prefix through `entry`; the original
 * allocation is 48 bytes. The fields after `entry` follow the EE kernel thread-status layout as a
 * partial hypothesis. The original declaration and use of the type are unresolved, so these names
 * do not establish its semantic type.
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
extern void ControlEntry(void *argument);

/* The four system threads: the renderer, the CD control thread, the studio
 * and the main control thread. xglSleep() hands control from one to the next
 * in this order. */
#define XGL_THREAD_COUNT 4
#define XGL_THREAD_STACK_SIZE 0x1000
#define XGL_CONTROL_THREAD_STACK_SIZE 0x10000
#define XGL_THREAD_PRIORITY_RENDER 9
#define XGL_THREAD_PRIORITY_CD 10
#define XGL_THREAD_PRIORITY_CONTROL 11
#define XGL_THREAD_PRIORITY_STUDIO 12
/* The threads' stacks are carved downward from the top of main memory's
 * reserved area. */
#define XGL_THREAD_STACK_TOP 0x1f8000

static system_thread_entry_t asSystemThreadList[XGL_THREAD_COUNT] = {
    { xglRenderEntry, 0, XGL_THREAD_STACK_SIZE, XGL_THREAD_PRIORITY_RENDER, 0 },
    { xglCdControlThread, 0, XGL_THREAD_STACK_SIZE, XGL_THREAD_PRIORITY_CD, 0 },
    { xglStudioEntry, 0, XGL_THREAD_STACK_SIZE, XGL_THREAD_PRIORITY_STUDIO, 0 },
    { ControlEntry, 0, XGL_CONTROL_THREAD_STACK_SIZE, XGL_THREAD_PRIORITY_CONTROL, 0 },
};

static int iRotateSignal;

static unsigned int iCurrentThread;

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/*
 * Storage for the original asActiveThreadList. Only the first four records are used by the
 * consumers below, so this capacity does not establish twenty logical threads.
 */
static active_thread_t asActiveThreadList[20];

static int xglCreateSignal(void)
{
    static ee_sema_t sSemaParam;

    sSemaParam.max_count = 1;
    sSemaParam.init_count = 0;
    return CreateSema(&sSemaParam);
}

/*
 * Creates the rotate signal, then loops forever: wakes the current system thread, waits for the
 * signal, and advances to the next thread in the rotation.
 */
void xglThreadRotate(void)
{
    iRotateSignal = xglCreateSignal();
    for (;;) {
        WakeupThread(asActiveThreadList[iCurrentThread].id);
        WaitSema(iRotateSignal);
        iCurrentThread++;
        if (iCurrentThread >= XGL_THREAD_COUNT)
            iCurrentThread = 0;
    }
}

/*
 * Cancels the caller's pending wakeup, signals the rotate semaphore so the next thread runs, and
 * puts the caller to sleep.
 */
void xglSleep(void)
{
    int thread_id = GetThreadId();
    CancelWakeupThread(thread_id);
    SignalSema(iRotateSignal);
    SleepThread();
}

/*
 * Creates and starts the system threads in asSystemThreadList, carving their stacks downward
 * from XGL_THREAD_STACK_TOP.
 */
void xglThreadInitial(void)
{
    static ee_thread_t sThreadParam;

    active_thread_t *active = asActiveThreadList;
    ee_thread_t *thread = &sThreadParam;
    unsigned int count = 0;
    int stack = XGL_THREAD_STACK_TOP;

    do {
        int stack_size = asSystemThreadList[count].stack_size;
        thread_entry_t entry = asSystemThreadList[count].entry;
        int priority = asSystemThreadList[count].priority;
        void *stack_base;

        stack_base = (void *)(stack - stack_size);
        active->stack_size = stack_size;
        thread->stack_size = stack_size;
        asActiveThreadList[count].priority = priority;
        active->entry = entry;
        active->stack = stack_base;
        thread->entry = entry;
        thread->stack = stack_base;
        thread->gp_reg = &_gp;
        thread->initial_priority = priority;
        active->id = CreateThread(thread);
        StartThread(active->id, 0);
        stack -= asSystemThreadList[count].stack_size;
        count++;
        active++;
    } while (count < XGL_THREAD_COUNT);

    iCurrentThread = 0;
}
