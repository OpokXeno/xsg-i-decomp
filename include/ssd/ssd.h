#ifndef SSD_SSD_H
#define SSD_SSD_H

/* IOP kernel, libsd (SPU2) and libc declarations used by the SSD sound driver. */

/* SPU2 register entries of libsd: a core number (0 or 1) is ORed in. */
#define SD_VP_VOLL 0x0000
#define SD_VP_VOLR 0x0100
#define SD_VP_PITCH 0x0200
#define SD_VP_ADSR1 0x0300
#define SD_VP_ADSR2 0x0400
#define SD_S_PMON 0x1300
#define SD_S_NON 0x1400
#define SD_S_KON 0x1500
#define SD_S_KOFF 0x1600
#define SD_S_VMIXL 0x1800
#define SD_S_VMIXEL 0x1900
#define SD_S_VMIXR 0x1a00
#define SD_S_VMIXER 0x1b00
#define SD_VA_SSA 0x2040

typedef struct IopSysClock {
    unsigned int low;
    unsigned int high;
} IopSysClock;

/* Effect (reverb) attributes of one SPU2 core. */
typedef struct SceSdEffectAttr {
    unsigned int core;
    int mode;
    short depth_left;
    short depth_right;
    int delay;
    int feedback;
} SceSdEffectAttr;

typedef int (*SceSdIntrHandler)(int core, void *data);
typedef unsigned int (*IopTimerHandler)(void *data);

struct SsdExportLibrary;

/* Driver records: each is defined in the owner header named beside it. */
typedef struct SsdGlobalWork SsdGlobalWork;   /* ssd_init.h */
typedef struct SsdVoice SsdVoice;             /* ssd_voic.h */
typedef struct SsdLfoState SsdLfoState;       /* ssd_lfo.h */
typedef struct SsdChannel SsdChannel;         /* ssd_chan.h */
typedef struct SsdMidiDevice SsdMidiDevice;   /* ssd_chan.h */
typedef struct SsdSequence SsdSequence;       /* ssd_seq.h */
typedef struct SsdTrack SsdTrack;             /* ssd_seq.h */
typedef struct SsdWaveBank SsdWaveBank;       /* ssd_wave.h */
typedef struct SsdEffectData SsdEffectData;   /* ssd_sec.h */
typedef struct SsdProc SsdProc;               /* ssd_init.h */
typedef struct SsdDmaBuffer SsdDmaBuffer;     /* ssd_dma.h */

int printf(const char *format, ...);
int Kprintf(const char *format, ...);
int sprintf(char *buffer, const char *format, ...);

void CpuDisableIntr(void);
void CpuEnableIntr(void);
int EnableIntr(int interrupt);
int DisableIntr(int interrupt, int *previous_state);
void *AllocSysMemory(int mode, unsigned int size, void *address);
int FreeSysMemory(void *memory);
int RegisterLibraryEntries(struct SsdExportLibrary *library);

void USec2SysClock(unsigned int microseconds, IopSysClock *clock);
int AllocHardTimer(int source, int size, int prescale);
int SetTimerHandler(int timer, unsigned int interval, IopTimerHandler handler, void *data);
int SetupHardTimer(int timer, int source, int mode, int prescale);
int StartHardTimer(int timer);
int StopHardTimer(int timer);
unsigned int GetTimerCounter(int timer);

void sceSdSetParam(unsigned short entry, unsigned short value);
unsigned short sceSdGetParam(unsigned short entry);
void sceSdSetAddr(unsigned short entry, unsigned int address);
void sceSdSetCoreAttr(unsigned short entry, unsigned short value);
void sceSdSetSwitch(unsigned short entry, unsigned int bits);
void sceSdGetEffectAttr(int core, SceSdEffectAttr *attr);
void sceSdSetEffectAttr(int core, SceSdEffectAttr *attr);
void sceSdClearEffectWorkArea(int core, int channel, int mode);
int sceSdVoiceTrans(int channel, int mode, void *memory, unsigned int spu_address, unsigned int size);
int sceSdVoiceTransStatus(short channel, short wait);
int sceSdBlockTrans(int channel, int mode, void *memory, int size);
int sceSdBlockTransStatus(int channel, int wait);
SceSdIntrHandler sceSdSetSpu2IntrHandler(SceSdIntrHandler handler, void *data);
SceSdIntrHandler sceSdSetTransIntrHandler(int channel, SceSdIntrHandler handler, void *data);

/* libsd parameter and address entries: OR the core number (SD_CORE_*) and, for a voice, voice * 2. */
#define SD_CORE_0 0
#define SD_CORE_1 1
#define SD_VP_ENVX 0x0500
#define SD_VA_LSAX 0x2140
#define SD_P_MMIX 0x0800
#define SD_P_MVOLL 0x0980
#define SD_P_MVOLR 0x0a80
#define SD_P_AVOLL 0x0d80
#define SD_P_AVOLR 0x0e80
#define SD_P_BVOLL 0x0f80
#define SD_P_BVOLR 0x1080
#define SD_A_EEA 0x1d00
#define SD_A_IRQA 0x1f00

/* sceSdSetCoreAttr entries (core number in bit 0). */
#define SD_C_EFFECT_ENABLE (1 << 1)
#define SD_C_IRQ_ENABLE (2 << 1)
#define SD_C_SPDIF_MODE (5 << 1)

/* Reverb (effect) modes. */
#define SD_EFFECT_MODE_STUDIO_C 4

/* sceSdBlockTrans / sceSdVoiceTrans modes. */
#define SD_TRANS_MODE_WRITE 0
#define SD_TRANS_MODE_READ 1
#define SD_TRANS_MODE_STOP 2
#define SD_BLOCK_LOOP (1 << 4)

#endif
