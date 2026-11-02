#ifndef SSD_INIT_H
#define SSD_INIT_H

#include "ssd/ssd.h"
#include "ssd/ssd_dma.h"
#include "ssd/ssd_err.h"
#include "ssd/ssd_main.h"
#include "ssd/ssd_mem.h"
#include "ssd/ssd_out.h"
#include "ssd/ssd_voic.h"
#include "ssd/ssd_wave.h"

/* SsdGlobalWork.flags */
#define SSD_FLAG_ENABLED 0x1
#define SSD_FLAG_DMA_BUSY 0x2
#define SSD_FLAG_ERROR 0x8
#define SSD_FLAG_IN_TRANSFER_INTR 0x10
#define SSD_FLAG_IN_IRQ_INTR 0x20
#define SSD_FLAG_SUSPENDED 0x80
#define SSD_FLAG_TIMER_LOCKED 0x2000
#define SSD_FLAG_INITIALIZED 0x8000

/* IOP interrupt lines enabled by the driver (SPU2 DMA0, DMA1 and the SPU2 IRQ). */
#define SSD_INTR_SPU2_DMA0 36
#define SSD_INTR_SPU2_DMA1 40
#define SSD_INTR_SPU2 9

/* SsdInit mode bits */
#define SSD_INIT_TIMER_LOCKED 0x1

/* SsdProc.kind of a process on the SsdWork.processes list (0 once unlinked). */
#define SSD_PROC_LINKED 3

/* Process run once per sequencer interrupt after the voices are updated. */
typedef void (*SsdProcCallback)(SsdGlobalWork *work, void *argument);

struct SsdProc {
    unsigned short kind;
    unsigned short flags;
    SsdProcCallback callback;
    void *argument;
    SsdProc *next;
};

/* The driver's global state, allocated once as SsdWork. */
struct SsdGlobalWork {
    int flags;
    SsdErrorState errors;
    unsigned int unmodeled_0c;
    SsdMainMemoryManager main_memory;
    SsdSpuMemoryManager spu_memory;
    SsdSegmentAllocMode segment_modes[SSD_SEGMENT_MODE_COUNT];
    unsigned char unmodeled_144[4];
    unsigned int stream_buffer;
    unsigned int stream_buffer_size;
    int timer_id;
    unsigned int timer_interval;
    unsigned int interrupt_count;
    unsigned int wave_interrupt_count;
    unsigned int stream_interrupt_count;
    unsigned int irq_interrupt_count;
    SsdSequence *sequences;
    SsdSequence *effect_sequence;
    SsdEffectData *effect_sets;
    SsdWaveBank *wave_banks;
    SsdMidiDevice *devices;
    SsdDmaBuffer *dma_buffers;
    SsdDmaBuffer *dma_queue;
    SsdWaveTransferState wave_transfer;
    SsdDmaCallback pcm_callback;
    void *pcm_argument;
    SsdDmaCallback vag_callback;
    void *vag_argument;
    SsdDmaCallback irq_callback;
    void *irq_argument;
    unsigned int output_flags;
    SsdStereoVolume master_volume;
    short master_depth_left;
    short master_depth_right;
    unsigned char unmodeled_1b8[4];
    SsdStereoVolume cd_volume;
    unsigned int cd_reverb_left;
    unsigned int cd_reverb_right;
    unsigned char unmodeled_1c8[12];
    short master_depth;
    short cd_depth;
    unsigned char unmodeled_1d8[4];
    unsigned char sequence_volume;
    unsigned char effect_volume;
    unsigned char unmodeled_1de;
    unsigned char output_mode;
    SsdRamp master_fade;
    SsdRamp cd_fade;
    unsigned int unmodeled_200;
    /*
     * The reverb records are 0x18 bytes apart and the last word of the second
     * is key_on[0], so one C struct cannot hold both arrays: they are views of
     * one region.
     */
    union {
        SsdReverbState reverb[2];
        struct {
            unsigned char reverb_prefix[0x2c];
            unsigned int key_on[2];
            unsigned int key_off[2];
            unsigned int cutoff[2];
            unsigned int pitch_modulation[2];
            unsigned int noise[2];
            unsigned int reverb_mix[2];
            unsigned int dry_mix[2];
            unsigned int active_voices;
        } switches;
    } cores;
    SsdVoice voices[SSD_VOICE_COUNT];
    unsigned char unmodeled_326c[16];
    int reverb_clear_result;
    SsdProc *processes;
    int random_state;
    unsigned char unmodeled_3288[16];
    unsigned int processing_ticks;
    unsigned int measured_interrupts;
};

extern SsdGlobalWork SsdWork;

/* Functions and data used by other translation units. */
void *SsdInit(void *memory, unsigned int size, int mode);
void SsdResume(void);
void SsdSuspend(void);
SsdGlobalWork *SsdGetGlobalWorkPtr(void);
int SsdGetRSRandom(void);
SsdProc *SsdLinkProc(SsdProc *process, SsdProcCallback callback, void *argument);
void SsdUnlinkProc(SsdProc *process);

#endif
