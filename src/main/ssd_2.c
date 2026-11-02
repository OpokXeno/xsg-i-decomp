#include "common.h"

/*
 * RssdWork, RssdRequest and RssdCallFunc: the one spelling lives in main/tu110's
 * own header, src/main/ssd_init.h. The generated include/main/ssd_init.h cannot
 * carry RssdWork's type yet, so that
 * header is included directly.
 */
#include "ssd_init.h"
#include "main/ssd_2.h"

typedef struct SsdSequenceData {
    unsigned char _unmodeled_00[8];
    int size;
} SsdSequenceData;

typedef struct SsdTrackPatchData {
    unsigned char _unmodeled_00[2];
    unsigned short size_units;
} SsdTrackPatchData;

typedef struct SsdTimeCode {
    unsigned char _unmodeled_00[2];
    unsigned short hours;
    unsigned char minutes;
    unsigned char seconds;
    unsigned char frames;
    unsigned char _unmodeled_07;
    unsigned char _unmodeled_08[4];
    unsigned short residual_samples;
} SsdTimeCode;

/* Sequencer (SMD) commands of the RSSD RPC. */
enum {
    RSSD_CMD_PLAY_SEQUENCE            = 48,
    RSSD_CMD_ADD_SEQUENCE_DATA        = 49,
    RSSD_CMD_DISPOSE_SEQUENCE         = 50,
    RSSD_CMD_START_SEQUENCE           = 52,
    RSSD_CMD_STOP_SEQUENCE            = 54,
    RSSD_CMD_PAUSE_SEQUENCE           = 55,
    RSSD_CMD_ADD_SEQ_EFFECT_DATA      = 56,
    RSSD_CMD_GET_SEQ_PLAY_STATUS      = 64,
    RSSD_CMD_SET_SEQ_MASTER_TEMPO     = 65,
    RSSD_CMD_SET_SEQ_MASTER_VOLUME    = 66,
    RSSD_CMD_SET_SEQ_MASTER_NOTE      = 67,
    RSSD_CMD_SET_SEQ_MASTER_PANPOT    = 68,
    RSSD_CMD_SET_SEQ_MUTE             = 69,
    RSSD_CMD_SET_SEQ_NOTE_FADER       = 72,
    RSSD_CMD_SET_SEQ_VOLUME_FADER     = 73,
    RSSD_CMD_SET_SEQ_PANPOT_FADER     = 74,
    RSSD_CMD_SET_SEQUENCE_OUTPUT_VOLUME = 75,
    RSSD_CMD_SET_EFFECT_OUT_VOLUME    = 76,
    RSSD_CMD_SET_SEQ_PATCH_NUMBER     = 80,
    RSSD_CMD_SET_TRACK_PATCH_SET      = 81,
    RSSD_CMD_SET_SEQ_SIGNAL           = 88
};

/* SsdGetTimeCode: the sample position is split into hours:minutes:seconds:frames. */
#define SSD_TIMECODE_UNITS_PER_MINUTE 60
#define SSD_TIMECODE_FRAMES_PER_SECOND 30

void SsdSetSeqMasterVolume(int sequence, int volume, int duration);

int SsdPlaySequence(SsdSequenceData *sequence, int request_value1, int request_value2)
{
    RssdRequest request;
    int size = sequence->size;

    request.arg[0].pointer = sequence;
    request.arg[1].value = request_value1;
    request.arg[2].value = request_value2;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_PLAY_SEQUENCE,&request, sequence, size);
    return RssdWork.response.value;
}

int SsdAddSequenceData(SsdSequenceData *data)
{
    RssdRequest request;
    int size = data->size;

    request.arg[0].pointer = data;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_ADD_SEQUENCE_DATA,&request, data, size);
    return RssdWork.response.value;
}

int SsdAddSeqEffectData(SsdSequenceData *data)
{
    RssdRequest request;
    int size = data->size;

    request.arg[0].pointer = data;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_ADD_SEQ_EFFECT_DATA,&request, data, size);
    return RssdWork.response.value;
}

int SsdDisposeSequence(void *sequence)
{
    RssdRequest request;

    request.arg[0].pointer = sequence;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_DISPOSE_SEQUENCE,&request, 0, 0);
    return 0;
}

void SsdStartSequence(int sequence, int start, int loop)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = start;
    request.arg[2].value = loop;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_START_SEQUENCE,&request, 0, 0);
}

void SsdResumeSequence(int sequence, int volume, int duration)
{
    SsdSetSeqMasterVolume(sequence, volume, duration);
}

void SsdStopSequence(int sequence)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_STOP_SEQUENCE,&request, 0, 0);
}

void SsdPauseSequence(int sequence, int position)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = position;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_PAUSE_SEQUENCE,&request, 0, 0);
}

int SsdGetSeqPlayStatus(int sequence)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_GET_SEQ_PLAY_STATUS,&request, 0, 0);
    return RssdWork.response.value;
}

void SsdSetSeqMasterTempo(int sequence, int tempo, int duration)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = tempo;
    request.arg[2].value = duration;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_SEQ_MASTER_TEMPO,&request, 0, 0);
}

void SsdSetSeqMasterVolume(int sequence, int volume, int duration)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = volume;
    request.arg[2].value = duration;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_SEQ_MASTER_VOLUME,&request, 0, 0);
}

void SsdSetSeqMasterNote(int sequence, int note, int duration)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = note;
    request.arg[2].value = duration;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_SEQ_MASTER_NOTE,&request, 0, 0);
}

void SsdSetSeqMasterPanpot(int sequence, int panpot, int duration)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = panpot;
    request.arg[2].value = duration;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_SEQ_MASTER_PANPOT,&request, 0, 0);
}

void SsdSetSeqNoteFader(int sequence, int note, int duration)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = note;
    request.arg[2].value = duration;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_SEQ_NOTE_FADER,&request, 0, 0);
}

void SsdSetSeqVolumeFader(int sequence, int volume, int duration)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = volume;
    request.arg[2].value = duration;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_SEQ_VOLUME_FADER,&request, 0, 0);
}

void SsdSetSeqPanpotFader(int sequence, int panpot, int duration)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = panpot;
    request.arg[2].value = duration;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_SEQ_PANPOT_FADER,&request, 0, 0);
}

void SsdSetSequenceOutputVolume(int sequence, int volume)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = volume;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_SEQUENCE_OUTPUT_VOLUME,&request, 0, 0);
}

void SsdSetEffectOutVolume(int effect, int volume)
{
    RssdRequest request;

    request.arg[0].value = effect;
    request.arg[1].value = volume;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_EFFECT_OUT_VOLUME,&request, 0, 0);
}

void SsdSetSeqPatchNumber(int sequence, int patch)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = patch;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_SEQ_PATCH_NUMBER,&request, 0, 0);
}

void SsdSetTrackPatchSet(int track, SsdTrackPatchData *patch)
{
    RssdRequest request;

    request.arg[0].value = track;
    request.arg[1].value = 0;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_TRACK_PATCH_SET,&request, patch, (patch->size_units << 3) + 4);
}

void SsdSetSeqMute(int sequence, int mute)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = mute;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_SEQ_MUTE,&request, 0, 0);
}

void SsdGetTimeCode(unsigned int sample_position, SsdTimeCode *time_code)
{
    unsigned int total_units;
    unsigned int residual_samples;
    unsigned int minutes;
    unsigned int hours;
    unsigned int seconds;
    unsigned int frame_units;
    unsigned int frames;

    total_units = sample_position / RssdWork.sample_rate;
    residual_samples = sample_position % RssdWork.sample_rate;
    minutes = total_units / SSD_TIMECODE_UNITS_PER_MINUTE;
    frame_units = residual_samples * SSD_TIMECODE_FRAMES_PER_SECOND;
    time_code->residual_samples = residual_samples;
    hours = minutes / SSD_TIMECODE_UNITS_PER_MINUTE;
    time_code->hours = hours;
    seconds = total_units % SSD_TIMECODE_UNITS_PER_MINUTE;
    time_code->seconds = seconds;
    time_code->minutes = minutes % SSD_TIMECODE_UNITS_PER_MINUTE;
    frames = frame_units / RssdWork.sample_rate;
    time_code->_unmodeled_07 = 0;
    time_code->frames = frames;
}

void SsdSetSeqSignal(int sequence, int signal)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = signal;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_SEQ_SIGNAL,&request, 0, 0);
}
