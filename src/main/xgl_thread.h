/*
 * TU-local declarations of main/tu086 (src/main/xgl_thread.c).
 */

#ifndef SRC_MAIN_XGL_THREAD_H
#define SRC_MAIN_XGL_THREAD_H

#include "shared.h"
#include "main/xgl_thread.h"

typedef struct {
    int count;
    int max_count;
    int init_count;
    int wait_threads;
    u32 attr;
    u32 option;
} ee_sema_t;

typedef void (*thread_entry_t)(void *);

typedef struct system_thread_entry_t {
    thread_entry_t entry;
    int unmodeled_04;
    int stack_size;
    int priority;
    int unmodeled_10;
} system_thread_entry_t;

typedef struct {
    thread_entry_t entry;
    void *stack;
    int stack_size;
    int priority;
    int id;
} active_thread_t;

extern void *_gp;
extern ee_sema_t sSemaParam;
extern int iRotateSignal;
extern active_thread_t asActiveThreadList[];
extern unsigned int iCurrentThread;

extern int WaitSema(int sema_id);
extern int CreateSema(ee_sema_t *sema);
extern int StartThread(int thread_id, void *args);
extern int GetThreadId(void);
extern void SleepThread(void);
extern int CancelWakeupThread(int thread_id);
extern int SignalSema(int sema_id);
#endif /* SRC_MAIN_XGL_THREAD_H */
