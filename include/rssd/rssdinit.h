#ifndef RSSD_RSSDINIT_H
#define RSSD_RSSDINIT_H

#include "rssd/rssd.h"

/* RPC request ids handled by RssdExecFunc (EE -> IOP, SIF RPC port 0x900). */
enum {
    RSSD_CMD_INIT = 0x1,
    RSSD_CMD_RESUME = 0x5,
    RSSD_CMD_SUSPEND = 0x6,
    RSSD_CMD_SET_OUTPUT_MODE = 0x10,
    RSSD_CMD_SET_SPU_VOLUME = 0x12,
    RSSD_CMD_SET_CD_VOLUME = 0x13,
    RSSD_CMD_SET_REVERB_PARAMETER = 0x14,
    RSSD_CMD_SPU_DIRECT_READ = 0x1b,
    RSSD_CMD_ADD_WAVE_DATA = 0x20,
    RSSD_CMD_NEXT_WAVE_DATA = 0x21,
    RSSD_CMD_DISPOSE_WAVE_BANK = 0x22,
    RSSD_CMD_CHECK_WAVE_DATA = 0x24,
    RSSD_CMD_SET_SEGMENT_ALLOC_MODE = 0x2c,
    RSSD_CMD_RESET_SEGMENT_ALLOC_MODE = 0x2d,
    RSSD_CMD_PLAY_SEQUENCE_DATA = 0x30,
    RSSD_CMD_ADD_SEQUENCE_DATA = 0x31,
    RSSD_CMD_DISPOSE_SEQUENCE = 0x32,
    RSSD_CMD_CHECK_SEQUENCE = 0x33,
    RSSD_CMD_START_SEQUENCE = 0x34,
    RSSD_CMD_RESUME_SEQUENCE = 0x35,
    RSSD_CMD_STOP_SEQUENCE = 0x36,
    RSSD_CMD_PAUSE_SEQUENCE = 0x37,
    RSSD_CMD_ADD_SEQ_EFFECT_DATA = 0x38,
    RSSD_CMD_GET_SEQ_PLAY_STATUS = 0x40,
    RSSD_CMD_SET_SEQ_MASTER_TEMPO = 0x41,
    RSSD_CMD_SET_SEQ_MASTER_VOLUME = 0x42,
    RSSD_CMD_SET_SEQ_MASTER_NOTE = 0x43,
    RSSD_CMD_SET_SEQ_MASTER_PANPOT = 0x44,
    RSSD_CMD_SET_SEQ_MUTE = 0x45,
    RSSD_CMD_SET_SEQ_NOTE_FADER = 0x48,
    RSSD_CMD_SET_SEQ_VOLUME_FADER = 0x49,
    RSSD_CMD_SET_SEQ_PANPOT_FADER = 0x4a,
    RSSD_CMD_SET_SEQUENCE_OUTPUT_VOLUME = 0x4b,
    RSSD_CMD_SET_EFFECT_OUTPUT_VOLUME = 0x4c,
    RSSD_CMD_SET_SEQ_PATCH_NUMBER = 0x50,
    RSSD_CMD_SET_TRACK_PATCH_SET = 0x51,
    RSSD_CMD_SET_SEQ_SIGNAL = 0x58,
    RSSD_CMD_ADD_EFFECT_DATA = 0x60,
    RSSD_CMD_DISPOSE_EFFECT_DATA = 0x61,
    RSSD_CMD_CHECK_EFFECT_DATA = 0x62,
    RSSD_CMD_PLAY_EFFECT_NORMAL = 0x70,
    RSSD_CMD_PLAY_EFFECT_PARAM = 0x71,
    RSSD_CMD_STOP_ALL_EFFECT = 0x78,
    RSSD_CMD_FADEOUT_EFFECT = 0x79,
    RSSD_CMD_FADEOUT_EFFECT_FILE_ID = 0x7a,
    RSSD_CMD_SET_EFFECT_PARAM = 0x7b,
    RSSD_CMD_SET_PLAY_EFFECT_PARAM = 0x7c,
    RSSD_CMD_CONTINUE_ONE_EFFECT = 0x7d,
    RSSD_CMD_FADEOUT_EFFECT_FROM = 0x7e,
    RSSD_CMD_FADEOUT_EFFECT_FILE_ID_FROM = 0x7f,
    RSSD_CMD_CHECK_PLAY_EFFECT_ALL = 0x80,
    RSSD_CMD_CHECK_PLAY_EFFECT = 0x81,
    RSSD_CMD_CHECK_PLAY_EFFECT_FILE_ID = 0x82,
    RSSD_CMD_PLAY_WAVE_NORMAL = 0x90,
    RSSD_CMD_PLAY_WAVE_PARAM = 0x91,
    RSSD_CMD_STOP_WAVE = 0x92,
    RSSD_CMD_INIT_PCM_STREAM = 0xa0,
    RSSD_CMD_DISPOSE_PCM_STREAM = 0xa1,
    RSSD_CMD_PLAY_PCM_STREAM = 0xa2,
    RSSD_CMD_STOP_PCM_STREAM = 0xa3,
    RSSD_CMD_SET_PCM_STREAM_DATA = 0xa4,
    RSSD_CMD_GET_PCM_STREAM_STATUS = 0xa5,
    RSSD_CMD_SET_PCM_STREAM_VOLUME = 0xa6,
    RSSD_CMD_CLEAR_RING_BUFFER = 0xa8,
    RSSD_CMD_INIT_VAG_STREAM_STEREO = 0xb0,
    RSSD_CMD_INIT_VAG_STREAM_MONO = 0xb1,
    RSSD_CMD_DISPOSE_VAG_STREAM = 0xb2,
    RSSD_CMD_PLAY_VAG_STREAM = 0xb3,
    RSSD_CMD_STOP_VAG_STREAM = 0xb4,
    RSSD_CMD_SET_VAG_STREAM_DATA_STEREO = 0xb5,
    RSSD_CMD_SET_VAG_STREAM_DATA_MONO = 0xb6,
    RSSD_CMD_GET_VAG_STREAM_STATUS_STEREO = 0xb7,
    RSSD_CMD_GET_VAG_STREAM_STATUS_MONO = 0xb8,
    RSSD_CMD_SET_VAG_STREAM_PITCH = 0xba,
    RSSD_CMD_SET_VAG_STREAM_VOLUME = 0xbb,
    RSSD_CMD_SET_VAG_STREAM_PANPOT = 0xbc,
    RSSD_CMD_SET_VAG_STREAM_PARAM = 0xbd,
    RSSD_CMD_GET_VAG_STREAM_STATUS_ALL = 0xbe,
    RSSD_CMD_INIT_SAMPLING = 0xd0,
    RSSD_CMD_DISPOSE_SAMPLING = 0xd1,
    RSSD_CMD_TRANSFER_SAMPLING = 0xd2,
    RSSD_CMD_TRANSFER_SAMPLING_NEXT = 0xd3,
    RSSD_CMD_PLAY_SAMPLING = 0xd4,
    RSSD_CMD_STOP_SAMPLING = 0xd5,
    RSSD_CMD_SET_SAMPLING_PARAM = 0xd6,
    RSSD_CMD_SET_SAMPLING_EFFECT = 0xd7,
    RSSD_CMD_CHECK_SPU_MEMORY = 0x100,
    RSSD_CMD_GET_VOICE_ENV = 0x102,
    RSSD_CMD_GET_SEQ_PLAY_INFO = 0x103,
    RSSD_CMD_DEB_OPEN_DEVICE = 0x200,
    RSSD_CMD_DEB_CLOSE_DEVICE = 0x201,
    RSSD_CMD_DEB_WAVE_KEYON = 0x202,
    RSSD_CMD_DEB_WAVE_KEYOFF = 0x203,
    RSSD_CMD_MIDI_DEV_SET_MIDI_EVENT = 0x204,
    RSSD_CMD_MIDI_DEV_SET_HW_EFFECT = 0x205,
    RSSD_CMD_DEB_ADD_EFFECT = 0x300,
    RSSD_CMD_DEB_PLAY_EFFECT = 0x301,
    RSSD_CMD_COMPELLING_KEYOFF = 0x400
};

/* A batch request: the arguments[1] packets that follow the header packet
 * are executed one after another. */
#define RSSD_RPC_BATCH 4

/* Notifications sent to the EE with RssdCallFunc/iRssdCallFunc. */
#define RSSD_EE_STREAM_FINISHED 10
#define RSSD_EE_WAVE_TRANSFER_DONE 37
#define RSSD_EE_SAMPLING_DMA_DONE 218
#define RSSD_EE_SAMPLING_KEYOFF 219
#define RSSD_EE_INIT_NOTIFY 0x1000

/* SIF RPC ports: the IOP serves 0x900 and binds to the EE client at 0x901. */
#define RSSD_RPC_SERVER_ID 0x900
#define RSSD_RPC_EE_CLIENT_ID 0x901

/* RPC buffer: a 32-byte packet header plus 64 KB of data. */
#define RSSD_RPC_BUFFER_SIZE 0x10020

/* SSD DMA callback status that means "called from thread context". */
#define RSSD_CALLBACK_THREAD_CONTEXT (-2)

/* RssdGlobalWork.flags */
#define RSSD_FLAG_RPC_BUSY 0x1

typedef void (*RssdFunctionCallback)(void *);

typedef struct RssdRpcPacket {
    short command;
    short error;
    int parameter;
    int value;
    int dataSize;
    int arguments[4];
    unsigned char data[0];
} RssdRpcPacket;

typedef struct RssdRequestNode {
    struct RssdRequestNode *next;
    int unmodeled_004;
    RssdFunctionCallback callback;
    void *callbackArgument;
    RssdRpcPacket packet;
    unsigned char data[0];
} RssdRequestNode;

typedef struct EnvelopeCache {
    int frames;
    int processingTicks;
    int interrupts;
} EnvelopeCache;

typedef struct RssdGlobalWork {
    unsigned int flags;
    unsigned char *allocation;
    unsigned char *clientBuffer;
    unsigned char *clientData;
    unsigned char unmodeled_010[0x38];
    void *serverBuffer;
    unsigned char unmodeled_04c[0x14];
    RssdRpcPacket lastPacket;
    SifRpcClientData client;
    int serverThread;
    int clientThread;
    int completionSemaphore;
    int serverCallCount;
    RssdRequestNode *firstRequest;
    EnvelopeCache envelopeCache;
    /* SPU direct read / wave upload transfer state. */
    struct {
        RssdRpcPacket *transferBuffer;
        unsigned int spuAddress;
        unsigned int chunkSize;
        unsigned int remaining;
    } spuRead;
    RssdFunctionCallback nextCallback;
    void *nextCallbackArgument;
    RssdFunctionCallback completionCallback;
    void *completionArgument;
    unsigned char unmodeled_0e8[0x18];
} RssdGlobalWork;

/* Functions and data used by other translation units. */
extern RssdGlobalWork RssdWork;
extern void RssdCallFunc(int command, const void *requestData, const void *data, int size);
extern void iRssdCallFunc(int command, const void *requestData, const void *data, int size);
extern void RssdSetFuncCallback(RssdFunctionCallback callback, void *argument);

#endif
