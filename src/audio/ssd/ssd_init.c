/* Original ssd_init.c translation unit. */



typedef struct SsdWorkState SsdWorkState;
typedef struct SsdProc SsdProc;
typedef struct SsdSequence SsdSequence;
typedef struct SsdChannel SsdChannel;
typedef struct SsdReverbState SsdReverbState;
typedef struct SsdMidiState SsdMidiState;
typedef struct IopSysClock IopSysClock;

typedef int (*SsdInterruptHandler)(int core, SsdWorkState *work);

typedef void (*SsdProcCallback)(SsdWorkState *work, void *argument);

struct SsdProc {
    unsigned short kind;
    unsigned short flags;
    SsdProcCallback callback;
    void *argument;
    struct SsdProc *next;
};

typedef void (*SsdUserCallback)(int channel, void *argument);

struct SsdReverbState {
    unsigned char unmodeled_00[3];
    signed char mode;
    short left, right;
    signed char delay, feedback;
    short depth;
    unsigned int work_end;
    unsigned int work_handle;
    unsigned int unmodeled_14;
};

struct SsdMidiState { short flags; unsigned char unmodeled_002[526]; };

struct IopSysClock { unsigned int low, high; };

struct SsdWorkState {
    int flags;
    unsigned char control_byte_04, unmodeled_05, control_byte_06, unmodeled_07;
    unsigned int process_state, unmodeled_0c;
    void *memory_base;
    unsigned char unmodeled_014[0x134];
    unsigned int stream_buffer, stream_buffer_size;
    int timer_id;
    unsigned int timer_interval;
    unsigned int sequence_ticks, wave_interrupts, stream_interrupts, irq_interrupts;
    SsdChannel *channels;
    SsdSequence *effect_sequence;
    void *list_170, *list_174, *list_178;
    unsigned char unmodeled_17c[0x18];
    SsdUserCallback stream_callback;
    void *stream_argument;
    SsdUserCallback callback_19c;
    void *argument_1a0;
    SsdUserCallback irq_callback;
    void *irq_argument;
    unsigned char unmodeled_1ac[0x58];
    SsdReverbState reverb[2];
    unsigned char unmodeled_234[0x3048];
    int reverb_clear_result;
    SsdProc *processes;
    int random_state;
    unsigned char unmodeled_3288[0x18];
};

SsdWorkState SsdWork;
extern int StopHardTimer(int timer);
extern int printf(const char *format, ...);
extern int SsdIrqInterruptProcess(int core, SsdWorkState *work);
extern int SsdWaveTransferInterruptProcess(int core, SsdWorkState *work);
extern SsdInterruptHandler sceSdSetSpu2IntrHandler(SsdInterruptHandler handler, SsdWorkState *work);
extern SsdInterruptHandler sceSdSetTransIntrHandler(int core, SsdInterruptHandler handler, SsdWorkState *work);
extern int CpuEnableIntr(void);
extern int EnableIntr(int interrupt);
extern void CpuDisableIntr(void);
extern void SsdSuspendDeviceAll(void);
extern void SsdSuspendPcmStream(void);
extern void SsdSuspendVagStream(void);
extern void sceSdSetParam(unsigned short parameter, unsigned short value);
extern void SsdSeqInterruptProcess(SsdWorkState *work);
extern int StartHardTimer(int timer_id);
extern int DisableIntr(int interrupt, int *previous_state);
extern SsdMidiState SsdMidi;
extern unsigned int SpuReverbWorkSize[];
extern void SsdErrorProc(int code, int detail);
extern void SsdInitMainMemoryManager(void *memory, unsigned int size);
extern void SsdInitSpuMemoryManager(void);
extern void SsdInitDmaManager(void);
extern void SsdInitVoiceManager(void);
int SsdInitTimer(void);
int SsdInitInterrupt(void);
extern void SsdInitOutputControl(void);
extern SsdSequence *SsdMakeEffectSequenceRec(int size, int flags);
extern void SsdLinkSequence(SsdSequence *sequence);
extern unsigned int SsdSpuMallocAdrs(unsigned int size, unsigned int address, int tag);
extern int SsdSetReverbParameter(int mode, int depth, int delay, int feedback);
extern void SsdSetReverb(int core, int enabled);
extern int SsdReverbClearCompleted(int core, int wait);
extern void SsdEnableReverbParam(int core);
extern unsigned int SsdSpuMalloc(unsigned int size, int tag);
extern struct SsdExportLibrary ssd_entry;
extern int RegisterLibraryEntries(struct SsdExportLibrary *library);
extern void USec2SysClock(unsigned int microseconds, IopSysClock *clock);
extern int AllocHardTimer(int source, int size, int prescale);
unsigned int SsdMainInterruptProcess(SsdWorkState *work);
extern int SetTimerHandler(int timer, unsigned int interval, unsigned int (*handler)(SsdWorkState *), SsdWorkState *work);
extern int SetupHardTimer(int timer, int source, int mode, int prescale);
extern void SsdResumeDeviceAll(void);
extern void SsdResumePcmStream(void);
extern void SsdResumeVagStream(void);
void SsdQuitInterrupt(void);
void SsdQuitTimer(void);
extern void SsdCompellingKeyoff(void);
void SsdSuspend(void);
SsdProc *SsdLinkProc(SsdProc *process, SsdProcCallback callback, void *argument);
void SsdUnlinkProc(SsdProc *process);
void SsdEnable(void);
void *SsdInit(void *memory, unsigned int size, int mode);
int SsdStart(int argc, char **argv);
int SsdGetRSRandom(void);
void SsdDisable(void);
void SsdResume(void);
SsdWorkState *SsdGetGlobalWorkPtr(void);
void *SsdQuit(void);

/* IRX module identity read by the loader: name and version 2.5. */
typedef struct ModuleInfo {
    const char *name;
    unsigned short version;
} ModuleInfo;

ModuleInfo Module = { "PS2_SuzukiSoundDriver", 0x0205 };

int SsdStart(int argc, char **argv)
{
    printf("SSD Ver 2.5.0.020108,%04x.%02x\n", 96, 1);
    RegisterLibraryEntries(&ssd_entry);
    return 0;
}

SsdWorkState *SsdGetGlobalWorkPtr(void)
{
    return &SsdWork;
}

void *SsdInit(void *memory, unsigned int size, int mode) {
    int flags;
    SsdReverbState *reverb;
    unsigned int reverb_size;
    printf("Ssd init  Suzuki sound driver  version 2.5\n");
    if (SsdWork.flags < 0) {
        SsdErrorProc(-128, 0);
        return 0;
    }
    flags = 0x8e01;
    if (mode & 1) flags = 0xae01;
    SsdWork.flags = flags;
    SsdWork.random_state = 0x12345678;
    SsdWork.channels = 0;
    SsdWork.effect_sequence = 0;
    SsdWork.list_170 = 0;
    SsdWork.list_174 = 0;
    SsdWork.list_178 = 0;
    SsdWork.sequence_ticks = 0;
    SsdWork.wave_interrupts = 0;
    SsdWork.stream_interrupts = 0;
    SsdWork.irq_interrupts = 0;
    SsdWork.stream_callback = 0;
    SsdWork.callback_19c = 0;
    SsdWork.irq_callback = 0;
    SsdMidi.flags = 0;
    SsdInitMainMemoryManager(memory, size);
    reverb = &SsdWork.reverb[0];
    SsdInitSpuMemoryManager();
    SsdInitDmaManager();
    SsdInitVoiceManager();
    SsdInitTimer();
    SsdInitInterrupt();
    SsdInitOutputControl();
    SsdWork.effect_sequence = SsdMakeEffectSequenceRec(44, 0);
    SsdLinkSequence(SsdWork.effect_sequence);
    reverb_size = SpuReverbWorkSize[4];
    SsdWork.reverb_clear_result = 0;
    reverb->work_end = 0x200000;
    reverb->mode = -1;
    reverb->work_handle = SsdSpuMallocAdrs(reverb_size, 0x200000 - reverb_size, 4);
    reverb = &SsdWork.reverb[1];
    reverb_size = SpuReverbWorkSize[4];
    reverb->work_end = 0x1e0000;
    reverb->mode = -1;
    reverb->work_handle = SsdSpuMallocAdrs(reverb_size, 0x1e0000 - reverb_size, 5);
    SsdSetReverbParameter(4, 10240, 0, 0);
    SsdSetReverbParameter(260, 10240, 0, 0);
    SsdSetReverb(0, 1);
    SsdSetReverb(1, 1);
    SsdReverbClearCompleted(1, 1);
    SsdEnableReverbParam(0);
    SsdEnableReverbParam(1);
    SsdSpuMalloc(4032, 2);
    SsdWork.stream_buffer = SsdSpuMalloc(8192, 3);
    SsdWork.stream_buffer_size = 32768;
    return memory;
}

void *SsdQuit(void) {
    void *memory;
    if (SsdWork.flags & 0x8000) {
        SsdWork.flags = 0;
        SsdQuitInterrupt();
        SsdQuitTimer();
        SsdCompellingKeyoff();
        memory = SsdWork.memory_base;
        SsdWork.control_byte_04 = 0;
        SsdWork.control_byte_06 = 0;
        SsdWork.process_state = 0;
    } else {
        SsdErrorProc(-129, 0);
        memory = 0;
    }
    return memory;
}

int SsdInitInterrupt(void) {
    sceSdSetSpu2IntrHandler(SsdIrqInterruptProcess, &SsdWork);
    sceSdSetTransIntrHandler(0, SsdWaveTransferInterruptProcess, &SsdWork);
    CpuEnableIntr();
    EnableIntr(36);
    EnableIntr(40);
    EnableIntr(9);
    return 0;
}

int SsdInitTimer(void) {
    IopSysClock clock;
    int timer;
    USec2SysClock(2000000, &clock);
    SsdWork.timer_interval = clock.low / 1000;
    timer = AllocHardTimer(1, 32, 1);
    SsdWork.timer_id = timer;
    SetTimerHandler(timer, SsdWork.timer_interval, SsdMainInterruptProcess, &SsdWork);
    SetupHardTimer(timer, 1, 0, 1);
    StartHardTimer(timer);
    return 0;
}

void SsdQuitInterrupt(void) {
    int previous_state;
    DisableIntr(36, &previous_state);
    DisableIntr(40, &previous_state);
    DisableIntr(9, &previous_state);
}

void SsdQuitTimer(void) {
    StopHardTimer(SsdWork.timer_id);
    printf("SSD2 - Release timer interrupt.\n");
}

void SsdResume(void) {
    CpuDisableIntr();
    SsdResumeDeviceAll();
    SsdResumePcmStream();
    SsdResumeVagStream();
    sceSdSetParam(2432, 16383);
    sceSdSetParam(2688, 16383);
    sceSdSetParam(2433, 16383);
    sceSdSetParam(2689, 16383);
    SsdWork.flags &= ~0x80U;
    CpuEnableIntr();
    printf("Ssd resume\n");
}

void SsdSuspend(void) {
    CpuDisableIntr();
    SsdSuspendDeviceAll();
    SsdSuspendPcmStream();
    SsdSuspendVagStream();
    sceSdSetParam(2432, 0);
    sceSdSetParam(2688, 0);
    sceSdSetParam(2433, 0);
    sceSdSetParam(2689, 0);
    SsdWork.flags |= 0x80;
    CpuEnableIntr();
    printf("Ssd suspend\n");
}

void SsdEnable(void) {
    if (!(SsdWork.flags & 0x2000)) {
      if (!(SsdWork.flags & 1)) {
        StartHardTimer(SsdWork.timer_id);
        SsdWork.flags |= 1;
      }
    }
}

void SsdDisable(void) {
    if (!(SsdWork.flags & 0x2000)) {
        if (SsdWork.flags & 1) {
            StopHardTimer(SsdWork.timer_id);
            SsdWork.flags &= ~1U;
        }
    }
}

int SsdGetRSRandom(void) {
    int state = SsdWork.random_state;
    state ^= (unsigned int)state << 17;
    state ^= state >> 15;
    SsdWork.random_state = state;
    return state & 0x7fff;
}

unsigned int SsdMainInterruptProcess(SsdWorkState *work) {
    SsdSeqInterruptProcess(work);
    return work->timer_interval;
}

SsdProc *SsdLinkProc(SsdProc *process, SsdProcCallback callback, void *argument) {
    SsdProc *tail;
    process->kind = 3;
    process->flags = 0;
    process->callback = callback;
    process->argument = argument;
    process->next = 0;
    tail = SsdWork.processes;
    if (tail) {
        while (tail->next) tail = tail->next;
        tail->next = process;
    } else {
        SsdWork.processes = process;
    }
    return process;
}

void SsdUnlinkProc(SsdProc *process) {
    SsdProc *previous = SsdWork.processes;
    if (previous == process) {
        SsdWork.processes = process->next;
    } else {
        while (previous->next != process) previous = previous->next;
        previous->next = process->next;
    }
    process->kind = 0;
}
