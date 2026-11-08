#include "common.h"
#include "shared.h"
#include "disp_on.h"
#include "main/disp_on.h"
#include "main/map_1.h"
#include "main/res_get_path.h"
#include "main/xgl_thread.h"

const char event_suffix[];

extern char *strcpy(char *destination, const char *source);
extern char *strcat(char *destination, const char *source);
extern int SCRIPT_load(const char *path);
extern int SCRIPT_exec(void);
extern void ACT_init(void);
extern void CallMethod(const char *method_name);

const char event_suffix[8] = ".evt";

/* Counts down a display on/off request; then sets or clears the display flag (bit 1) and removes itself. */
static int disptest(XglTaskPrefix *task)
{
    DispSwitchTask *display_task = (DispSwitchTask *)task;
    GameLoopStateRecord *state = display_task->header.state;

    if ((state->runtime_flags & 0x100u) == 0u)
        return xglTaskRemove(task);

    display_task->delay -= 1;
    if (display_task->delay < 0) {
        if (display_task->request != DISP_REQUEST_ON) {
            if (display_task->request == DISP_REQUEST_OFF)
                state->runtime_flags &= ~2u;
        } else {
            state->runtime_flags |= 2u;
        }
        return xglTaskRemove(task);
    }
}

/* Queues a task that sets the display flag after `countdown` ticks. */
void DISP_on(int countdown)
{
    XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
    DispSwitchTask *task = (DispSwitchTask *)xglTaskEntryNext(
        scheduler, disptest, scheduler != 0 ? scheduler->active_tail : 0);
    if (task != 0) {
        task->header.state = &GameLoopState;
        task->header.flags = 0;
        task->header.next_callback = 0;
    }
    task->delay = countdown;
    task->request = DISP_REQUEST_ON;
}

/* Queues a task that clears the display flag after `countdown` ticks. */
void DISP_off(int countdown)
{
    XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
    DispSwitchTask *task = (DispSwitchTask *)xglTaskEntryNext(
        scheduler, disptest, scheduler != 0 ? scheduler->active_tail : 0);
    if (task != 0) {
        task->header.state = &GameLoopState;
        task->header.flags = 0;
        task->header.next_callback = 0;
    }
    task->delay = countdown;
    task->request = DISP_REQUEST_OFF;
}

/* Map-load task: performs the requested load (map, map with event script, or script only) once, then removes itself. */
static void loader(MapLoadTask *task)
{
    char path[256];
    unsigned int runtime_flags = task->header.state->runtime_flags;

    if ((runtime_flags & 0x200u) == 0u) {
        xglTaskRemove(&task->header.entry);
        return;
    }
    if ((runtime_flags & 0x400u) != 0u)
        return;
    if ((task->header.flags & 1u) != 0)
        return;

    task->header.flags |= 1u;
    if (task->request != MAP_LOAD_SCRIPT_ONLY) {
        if (task->request < MAP_LOAD_MAP_AND_EVENT) {
            if (task->request != MAP_LOAD_MAP_ONLY)
                return;
        } else if (task->request != MAP_LOAD_MAP_AND_EVENT) {
            return;
        }
        if (task->request != MAP_LOAD_MAP_ONLY) {
            xglSleep();
            MapChange(task->map_id);
            GameLoopState.map_event_index = (unsigned short)task->event_index;
            strcpy(path, MapGetName(task->map_id));
            strcat(path, event_suffix);
            SCRIPT_load(path);
            SCRIPT_exec();
        } else {
            xglSleep();
            MapChange(task->map_id);
        }
        xglTaskRemove(&task->header.entry);
    } else {
        strcpy(path, MapGetName(task->map_id));
        strcat(path, event_suffix);
        SCRIPT_load(path);
        SCRIPT_exec();
        xglTaskRemove(&task->header.entry);
    }
}

/* Queues a load of map `map_id` together with its event script. */
void LoadMap(int map_id)
{
    XglTaskScheduler *scheduler;
    MapLoadTask *task;

    GameResourceReset(0);
    ACT_init();
    scheduler = GameLoopState.task_scheduler;
    task = (MapLoadTask *)xglTaskEntryNext(
        scheduler, (int (*)(XglTaskPrefix *))loader,
        scheduler != 0 ? scheduler->active_tail : 0);
    if (task != 0) {
        task->header.state = &GameLoopState;
        task->header.flags = 0;
        task->header.next_callback = 0;
    }
    task->request = MAP_LOAD_MAP_AND_EVENT;
    task->map_id = map_id;
    task->event_index = -1;
}

/* Queues a load of map `map_id` with its event script, starting at `event_index`. */
void LoadMap2(int map_id, int event_index)
{
    XglTaskScheduler *scheduler;
    MapLoadTask *task;

    GameResourceReset(0);
    ACT_init();
    scheduler = GameLoopState.task_scheduler;
    task = (MapLoadTask *)xglTaskEntryNext(
        scheduler, (int (*)(XglTaskPrefix *))loader,
        scheduler != 0 ? scheduler->active_tail : 0);
    if (task != 0) {
        task->header.state = &GameLoopState;
        task->header.flags = 0;
        task->header.next_callback = 0;
    }
    task->event_index = event_index;
    task->map_id = map_id;
    task->request = MAP_LOAD_MAP_AND_EVENT;
}

/* Queues a load of map `map_id` without its event script. */
void LoadMapOnly(int map_id)
{
    XglTaskScheduler *scheduler;
    MapLoadTask *task;

    scheduler = GameLoopState.task_scheduler;
    task = (MapLoadTask *)xglTaskEntryNext(
        scheduler, (int (*)(XglTaskPrefix *))loader,
        scheduler != 0 ? scheduler->active_tail : 0);
    if (task != 0) {
        task->header.state = &GameLoopState;
        task->header.flags = 0;
        task->header.next_callback = 0;
    }
    task->map_id = map_id;
    task->request = MAP_LOAD_MAP_ONLY;
}

/*
 * Counts down an event timer and calls its method when the countdown reaches zero.
 * The countdown pauses while runtime flag bit 0 is set; the task removes itself
 * when flags 0x80000030 are set.
 */
int EventTimerTask(XglTaskPrefix *task)
{
    EventTimerWork *timer_task = (EventTimerWork *)task;
    unsigned int runtime_flags = GameLoopState.runtime_flags;

    if ((runtime_flags & 1u) == 0u) {
        if ((runtime_flags & 0x80000030u) != 0u)
            return xglTaskRemove(task);

        timer_task->countdown -= 1;
        if (timer_task->countdown == 0) {
            CallMethod(timer_task->method_reference);
            xglTaskRemove(task);
        }
    }
}

/* Queues an event timer task that calls `method_reference` after `countdown` ticks. */
void setEventTimerTaskEntry(const char *method_reference, int countdown)
{
    XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
    EventTimerWork *task = (EventTimerWork *)xglTaskEntryNext(
        scheduler, EventTimerTask, scheduler != 0 ? scheduler->active_tail : 0);
    if (task != 0) {
        task->header.state = &GameLoopState;
        task->header.flags = 0;
        task->header.next_callback = 0;
    }
    task->countdown = countdown;
    task->method_reference = method_reference;
}
