#ifndef RSSD_RSSD_H
#define RSSD_RSSD_H

/* IOP SDK declarations used by the RSSD module (sysmem, intrman, thbase,
 * sifrpc, libsd); sysmem, intrman and printf come from ssd/ssd.h. */

#include "ssd/ssd.h"

/* AllocSysMemory modes. */
#define SMEM_Low 0

typedef struct SifRpcClientData {
    unsigned int opaque[9];
    void *server;
} SifRpcClientData;

typedef struct SifRpcQueueData { unsigned int opaque[6]; } SifRpcQueueData;

typedef struct SifRpcServerData { unsigned int opaque[17]; } SifRpcServerData;

typedef void *(*SifRpcFunction)(int function, void *buffer, int size);

typedef struct IopThreadParameter {
    unsigned int attributes;
    unsigned int option;
    int (*entry)(void *argument);
    int stackSize;
    int priority;
    unsigned int unmodeled_014;
} IopThreadParameter;

typedef struct IopSemaphoreParameter {
    unsigned int attributes;
    unsigned int option;
    int initialCount;
    int maximumCount;
} IopSemaphoreParameter;

extern int sceSdInit(int flag);

extern int sceSifBindRpc(SifRpcClientData *client, unsigned int id, int mode);

extern int sceSifCheckStatRpc(SifRpcClientData *client);

extern int sceSifCallRpc(SifRpcClientData *client, int command, int mode,
    void *send, int sendSize, void *receive, int receiveSize,
    void (*callback)(void *argument), void *argument);

extern int sceSifCheckInit(void);

extern void sceSifInit(void);

extern void sceSifInitRpc(int mode);

extern void sceSifSetRpcQueue(SifRpcQueueData *queue, int thread);

extern void sceSifRegisterRpc(SifRpcServerData *server, int id,
    SifRpcFunction function, void *buffer, SifRpcFunction callback,
    void *callbackBuffer, SifRpcQueueData *queue);

extern void sceSifRpcLoop(SifRpcQueueData *queue);

extern int CreateSema(IopSemaphoreParameter *parameter);

extern int CreateThread(IopThreadParameter *parameter);

extern int StartThread(int thread, void *argument);

extern int GetThreadId(void);

extern int SleepThread(void);

extern int WakeupThread(int thread);

extern int iWakeupThread(int thread);

extern int WaitSema(int semaphore);

extern int SignalSema(int semaphore);

extern int iSignalSema(int semaphore);

#endif
