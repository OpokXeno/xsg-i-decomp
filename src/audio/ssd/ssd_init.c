#include "ssd/ssd_init.h"
#include "ssd/ssd_chan.h"
#include "ssd/ssd_data.h"
#include "ssd/ssd_dev.h"
#include "ssd/ssd_midi.h"
#include "ssd/ssd_pcm.h"
#include "ssd/ssd_sec.h"
#include "ssd/ssd_seq.h"
#include "ssd/ssd_vag.h"
#include "ssd/ssd_voic.h"

#define SSD_RANDOM_SEED 0x12345678
#define SSD_REVERB_DEPTH 10240
#define SSD_REVERB_END_CORE1 0x1e0000

extern struct SsdExportLibrary ssd_entry;

SsdGlobalWork SsdWork;

unsigned int SsdMainInterruptProcess(void *argument);
int SsdInitInterrupt(void);
int SsdInitTimer(void);
void SsdQuitInterrupt(void);
void SsdQuitTimer(void);

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

SsdGlobalWork *SsdGetGlobalWorkPtr(void)
{
    return &SsdWork;
}

void *SsdInit(void *memory, unsigned int size, int mode) {
    int flags;
    SsdReverbState *reverb;
    unsigned int reverb_size;
    printf("Ssd init  Suzuki sound driver  version 2.5\n");
    if (SsdWork.flags < 0) {
        SsdErrorProc(SSD_ERROR_ALREADY_INITIALIZED, 0);
        return 0;
    }
    flags = SSD_FLAG_INITIALIZED | 0xe00 | SSD_FLAG_ENABLED;
    if (mode & SSD_INIT_TIMER_LOCKED) flags |= SSD_FLAG_TIMER_LOCKED;
    SsdWork.flags = flags;
    SsdWork.random_state = SSD_RANDOM_SEED;
    SsdWork.sequences = 0;
    SsdWork.effect_sequence = 0;
    SsdWork.effect_sets = 0;
    SsdWork.wave_banks = 0;
    SsdWork.devices = 0;
    SsdWork.interrupt_count = 0;
    SsdWork.wave_interrupt_count = 0;
    SsdWork.stream_interrupt_count = 0;
    SsdWork.irq_interrupt_count = 0;
    SsdWork.pcm_callback = 0;
    SsdWork.vag_callback = 0;
    SsdWork.irq_callback = 0;
    SsdMidi.enabled = 0;
    SsdInitMainMemoryManager((unsigned int)memory, size);
    reverb = &SsdWork.cores.reverb[0];
    SsdInitSpuMemoryManager();
    SsdInitDmaManager();
    SsdInitVoiceManager();
    SsdInitTimer();
    SsdInitInterrupt();
    SsdInitOutputControl();
    SsdWork.effect_sequence = SsdMakeEffectSequenceRec(44, 0);
    SsdLinkSequence(SsdWork.effect_sequence);
    reverb_size = SpuReverbWorkSize[SD_EFFECT_MODE_STUDIO_C];
    SsdWork.reverb_clear_result = 0;
    reverb->work_end = SSD_SPU_RAM_SIZE;
    reverb->mode = SSD_REVERB_MODE_NONE;
    reverb->work_handle = SsdSpuMallocAdrs(reverb_size, SSD_SPU_RAM_SIZE - reverb_size, SSD_SPU_BLOCK_REVERB_CORE0);
    reverb = &SsdWork.cores.reverb[1];
    reverb_size = SpuReverbWorkSize[SD_EFFECT_MODE_STUDIO_C];
    reverb->work_end = SSD_REVERB_END_CORE1;
    reverb->mode = SSD_REVERB_MODE_NONE;
    reverb->work_handle = SsdSpuMallocAdrs(reverb_size, SSD_REVERB_END_CORE1 - reverb_size, SSD_SPU_BLOCK_REVERB_CORE1);
    SsdSetReverbParameter(SD_EFFECT_MODE_STUDIO_C, SSD_REVERB_DEPTH, 0, 0);
    SsdSetReverbParameter(SSD_REVERB_MODE_CORE1 | SD_EFFECT_MODE_STUDIO_C, SSD_REVERB_DEPTH, 0, 0);
    SsdSetReverb(0, 1);
    SsdSetReverb(1, 1);
    SsdReverbClearCompleted(1, 1);
    SsdEnableReverbParam(0);
    SsdEnableReverbParam(1);
    SsdSpuMalloc(SSD_SPU_STREAM_BASE - SSD_SPU_SYSTEM_SIZE, SSD_SPU_BLOCK_DRIVER);
    SsdWork.stream_buffer = SsdSpuMalloc(SSD_SPU_STREAM_SIZE, SSD_SPU_BLOCK_STREAM);
    SsdWork.stream_buffer_size = 32768;
    return memory;
}

void *SsdQuit(void) {
    void *memory;
    if (SsdWork.flags & SSD_FLAG_INITIALIZED) {
        SsdWork.flags = 0;
        SsdQuitInterrupt();
        SsdQuitTimer();
        SsdCompellingKeyoff();
        memory = SsdWork.main_memory.first;
        SsdWork.errors.function_code = 0;
        SsdWork.errors.sequence_code = 0;
        SsdWork.errors.detail = 0;
    } else {
        SsdErrorProc(SSD_ERROR_NOT_INITIALIZED, 0);
        memory = 0;
    }
    return memory;
}

int SsdInitInterrupt(void) {
    sceSdSetSpu2IntrHandler(SsdIrqInterruptProcess, &SsdWork);
    sceSdSetTransIntrHandler(0, SsdWaveTransferInterruptProcess, &SsdWork);
    CpuEnableIntr();
    EnableIntr(SSD_INTR_SPU2_DMA0);
    EnableIntr(SSD_INTR_SPU2_DMA1);
    EnableIntr(SSD_INTR_SPU2);
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
    DisableIntr(SSD_INTR_SPU2_DMA0, &previous_state);
    DisableIntr(SSD_INTR_SPU2_DMA1, &previous_state);
    DisableIntr(SSD_INTR_SPU2, &previous_state);
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
    sceSdSetParam(SD_P_MVOLL | SD_CORE_0, SSD_MASTER_VOLUME_DEFAULT);
    sceSdSetParam(SD_P_MVOLR | SD_CORE_0, SSD_MASTER_VOLUME_DEFAULT);
    sceSdSetParam(SD_P_MVOLL | SD_CORE_1, SSD_MASTER_VOLUME_DEFAULT);
    sceSdSetParam(SD_P_MVOLR | SD_CORE_1, SSD_MASTER_VOLUME_DEFAULT);
    SsdWork.flags &= ~SSD_FLAG_SUSPENDED;
    CpuEnableIntr();
    printf("Ssd resume\n");
}

void SsdSuspend(void) {
    CpuDisableIntr();
    SsdSuspendDeviceAll();
    SsdSuspendPcmStream();
    SsdSuspendVagStream();
    sceSdSetParam(SD_P_MVOLL | SD_CORE_0, 0);
    sceSdSetParam(SD_P_MVOLR | SD_CORE_0, 0);
    sceSdSetParam(SD_P_MVOLL | SD_CORE_1, 0);
    sceSdSetParam(SD_P_MVOLR | SD_CORE_1, 0);
    SsdWork.flags |= SSD_FLAG_SUSPENDED;
    CpuEnableIntr();
    printf("Ssd suspend\n");
}

void SsdEnable(void) {
    if (!(SsdWork.flags & SSD_FLAG_TIMER_LOCKED)) {
      if (!(SsdWork.flags & SSD_FLAG_ENABLED)) {
        StartHardTimer(SsdWork.timer_id);
        SsdWork.flags |= SSD_FLAG_ENABLED;
      }
    }
}

void SsdDisable(void) {
    if (!(SsdWork.flags & SSD_FLAG_TIMER_LOCKED)) {
        if (SsdWork.flags & SSD_FLAG_ENABLED) {
            StopHardTimer(SsdWork.timer_id);
            SsdWork.flags &= ~SSD_FLAG_ENABLED;
        }
    }
}

int SsdGetRSRandom(void) {
    int state = SsdWork.random_state;
    state ^= state << 17;
    state ^= state >> 15;
    SsdWork.random_state = state;
    return state & 0x7fff;
}

unsigned int SsdMainInterruptProcess(void *argument) {
    SsdGlobalWork *work = argument;
    SsdSeqInterruptProcess(work);
    return work->timer_interval;
}

SsdProc *SsdLinkProc(SsdProc *process, SsdProcCallback callback, void *argument) {
    SsdProc *tail;
    process->kind = SSD_PROC_LINKED;
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
