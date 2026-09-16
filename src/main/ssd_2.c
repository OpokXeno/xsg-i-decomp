#include "common.h"

/*
 * RssdWork, RssdRequest and RssdCallFunc: the one spelling lives in main/tu110's
 * own header, src/main/ssd_init.h. The generated include/main/ssd_init.h cannot
 * carry RssdWork's type yet, so that
 * header is included directly.
 */
#include "ssd_init.h"

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

void SsdSetSeqMasterVolume(int sequence, int volume, int duration);

int SsdPlaySequence(SsdSequenceData *sequence, int arg1, int arg2)
{
    RssdRequest request;
    int size = sequence->size;

    request.arg[0].pointer = sequence;
    request.arg[1].value = arg1;
    request.arg[2].value = arg2;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(48, &request, sequence, size);
    return RssdWork.response.value;
}

int SsdAddSequenceData(SsdSequenceData *data)
{
    RssdRequest request;
    int size = data->size;

    request.arg[0].pointer = data;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(49, &request, data, size);
    return RssdWork.response.value;
}

int SsdAddSeqEffectData(SsdSequenceData *data)
{
    RssdRequest request;
    int size = data->size;

    request.arg[0].pointer = data;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(56, &request, data, size);
    return RssdWork.response.value;
}

int SsdDisposeSequence(void *sequence)
{
    RssdRequest request;

    request.arg[0].pointer = sequence;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(50, &request, 0, 0);
    return 0;
}

void SsdStartSequence(int sequence, int start, int loop)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = start;
    request.arg[2].value = loop;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(52, &request, 0, 0);
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
    RssdCallFunc(54, &request, 0, 0);
}

void SsdPauseSequence(int sequence, int position)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = position;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(55, &request, 0, 0);
}

int SsdGetSeqPlayStatus(int sequence)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(64, &request, 0, 0);
    return RssdWork.response.value;
}

void SsdSetSeqMasterTempo(int sequence, int tempo, int duration)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = tempo;
    request.arg[2].value = duration;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(65, &request, 0, 0);
}

void SsdSetSeqMasterVolume(int sequence, int volume, int duration)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = volume;
    request.arg[2].value = duration;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(66, &request, 0, 0);
}

void SsdSetSeqMasterNote(int sequence, int note, int duration)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = note;
    request.arg[2].value = duration;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(67, &request, 0, 0);
}

void SsdSetSeqMasterPanpot(int sequence, int panpot, int duration)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = panpot;
    request.arg[2].value = duration;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(68, &request, 0, 0);
}

void SsdSetSeqNoteFader(int sequence, int note, int duration)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = note;
    request.arg[2].value = duration;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(72, &request, 0, 0);
}

void SsdSetSeqVolumeFader(int sequence, int volume, int duration)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = volume;
    request.arg[2].value = duration;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(73, &request, 0, 0);
}

void SsdSetSeqPanpotFader(int sequence, int panpot, int duration)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = panpot;
    request.arg[2].value = duration;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(74, &request, 0, 0);
}

void SsdSetSequenceOutputVolume(int sequence, int volume)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = volume;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(75, &request, 0, 0);
}

void SsdSetEffectOutVolume(int effect, int volume)
{
    RssdRequest request;

    request.arg[0].value = effect;
    request.arg[1].value = volume;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(76, &request, 0, 0);
}

void SsdSetSeqPatchNumber(int sequence, int patch)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = patch;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(80, &request, 0, 0);
}

void SsdSetTrackPatchSet(int track, SsdTrackPatchData *patch)
{
    RssdRequest request;

    request.arg[0].value = track;
    request.arg[1].value = 0;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(81, &request, patch, (patch->size_units << 3) + 4);
}

void SsdSetSeqMute(int sequence, int mute)
{
    RssdRequest request;

    request.arg[0].value = sequence;
    request.arg[1].value = mute;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(69, &request, 0, 0);
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
    minutes = total_units / 60;
    frame_units = residual_samples * 30;
    time_code->residual_samples = residual_samples;
    hours = minutes / 60;
    time_code->hours = hours;
    seconds = total_units % 60;
    time_code->seconds = seconds;
    time_code->minutes = minutes % 60;
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
    RssdCallFunc(88, &request, 0, 0);
}
