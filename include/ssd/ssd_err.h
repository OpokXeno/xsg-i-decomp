#ifndef SSD_ERR_H
#define SSD_ERR_H

#include "ssd/ssd.h"

/* Codes passed to SsdErrorProc, SsdWarringProc and SsdSeqErrorProc, and returned by the API. */
#define SSD_ERROR_BAD_SIGNATURE (-1)
#define SSD_ERROR_BAD_CHECKSUM (-2)
#define SSD_ERROR_NO_SEQUENCE (-5)
#define SSD_ERROR_BAD_EFFECT_DATA (-11)
#define SSD_ERROR_SEQUENCE_NOT_FOUND (-17)
#define SSD_ERROR_EFFECT_SET_NOT_FOUND (-18)
#define SSD_ERROR_EFFECT_INDEX (-19)
#define SSD_ERROR_NO_EFFECT_BANK (-20)
#define SSD_ERROR_BAD_EFFECT_SAMPLE (-21)
#define SSD_WARNING_SEQUENCE_EXISTS (-25)
#define SSD_ERROR_NO_SEQUENCE_MEMORY (-36)
#define SSD_ERROR_NO_TRACK_MEMORY (-37)
#define SSD_ERROR_NO_MIDI_DEVICE (-40)
#define SSD_ERROR_NO_PATCHES (-48)
#define SSD_ERROR_PATCH_INDEX (-49)
#define SSD_SEQ_ERROR_DUMMY_TRAP (-192)
#define SSD_ERROR_WAVE_BANK_NOT_FOUND (-16)
#define SSD_WARNING_WAVE_BANK_LOADED (-24)
#define SSD_WARNING_NO_WAVE_TRANSFER (-28)
#define SSD_ERROR_NO_MEMORY (-32)
#define SSD_ERROR_NO_SPU_MEMORY (-33)
#define SSD_ERROR_WAVE_SPU_FREE (-43)
#define SSD_ERROR_ALREADY_INITIALIZED (-128)
#define SSD_ERROR_NOT_INITIALIZED (-129)
#define SSD_ERROR_DMA_QUEUE_FULL (-225)

/* Last error and warning codes reported by the driver. */
typedef struct SsdErrorState {
    signed char function_code;
    signed char warning_code;
    signed char sequence_code;
    int detail;
} SsdErrorState;

/* Functions and data used by other translation units. */
void SsdResetFuncErrorCode(void);
int SsdCheckSumMusicData(const void *data);
int SsdCheckData(const void *data, int signature);
void SsdErrorProc(int code, int detail);
void SsdWarringProc(int code, int detail);
void SsdSeqErrorProc(int code, int detail);

#endif
