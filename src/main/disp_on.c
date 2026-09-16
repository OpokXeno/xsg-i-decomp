#include "common.h"
#include "shared.h"
#include "disp_on.h"

extern void xglSleep(void);
extern void MapChange(int map_id);
extern char *MapGetName(int map_id);
extern char *strcpy(char *destination, const char *source);
extern char *strcat(char *destination, const char *source);
extern int SCRIPT_load(const char *path);
extern int SCRIPT_exec(void);
extern const char event_suffix[];
extern void GameResourceReset(int reset_mode);
extern void ACT_init(void);
extern void CallMethod(const char *method_name);

INCLUDE_ASM("asm/main/nonmatchings/disp_on", disptest);

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

INCLUDE_ASM("asm/main/nonmatchings/disp_on", LoadMap);

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

INCLUDE_ASM("asm/main/nonmatchings/disp_on", EventTimerTask);

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
