#include "common.h"
#include "ssd_3.h"

enum {
    RSSD_CMD_INIT_PCM_STREAM              = 0xa0,
    RSSD_CMD_DISPOSE_PCM_STREAM           = 0xa1,
    RSSD_CMD_PLAY_PCM_STREAM              = 0xa2,
    RSSD_CMD_STOP_PCM_STREAM              = 0xa3,
    RSSD_CMD_GET_PCM_STREAM_STATUS        = 0xa5,
    RSSD_CMD_SET_PCM_STREAM_VOLUME        = 0xa6,
    RSSD_CMD_CLEAR_STREAM_RING_BUFFER     = 0xa8,
    RSSD_CMD_INIT_VAG_STREAM_STEREO       = 0xb0,
    RSSD_CMD_INIT_VAG_STREAM_MONO         = 0xb1,
    RSSD_CMD_DISPOSE_VAG_STREAM           = 0xb2,
    RSSD_CMD_PLAY_VAG_STREAM              = 0xb3,
    RSSD_CMD_STOP_VAG_STREAM              = 0xb4,
    RSSD_CMD_GET_VAG_STREAM_STATUS_STEREO = 0xb7,
    RSSD_CMD_GET_VAG_STREAM_STATUS_MONO   = 0xb8,
    RSSD_CMD_GET_VAG_STREAM_PARAM         = 0xb8,
    RSSD_CMD_SET_VAG_STREAM_PITCH         = 0xba,
    RSSD_CMD_SET_VAG_STREAM_VOLUME        = 0xbb,
    RSSD_CMD_SET_VAG_STREAM_PANPOT        = 0xbc,
    RSSD_CMD_GET_VAG_STREAM_STATUS_ALL    = 0xbe
};

/*
 * channel is cached at RssdStrWork.pcm_channel right after the call (see
 * src/main/ssd_3.h); volume forwards to the sound server as the stream's
 * initial output level, the same role SsdSetPcmStreamVolume's own second
 * argument plays later.
 */
void SsdInitPcmStream(int channel, int volume)
{
    RssdRequest request;

    request.arg[0].value = channel;
    request.arg[1].value = volume;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_INIT_PCM_STREAM, &request, 0, 0);
    RssdStrWork.pcm_channel = channel;
}

void SsdDisposePcmStream(void)
{
    RssdRequest request;

    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_DISPOSE_PCM_STREAM, &request, 0, 0);
}

void SsdPlayPcmStream(int channel, int loop)
{
    RssdRequest request;

    request.arg[0].value = channel;
    request.arg[1].value = loop;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_PLAY_PCM_STREAM, &request, 0, 0);
}

void SsdStopPcmStream(int channel)
{
    RssdRequest request;

    request.arg[0].value = channel;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_STOP_PCM_STREAM, &request, 0, 0);
}

INCLUDE_ASM("asm/main/nonmatchings/ssd_3", SsdSetPcmStreamData);

int SsdGetPcmStreamStatus(void)
{
    RssdRequest request;

    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_GET_PCM_STREAM_STATUS, &request, 0, 0);
    return 0;
}

void SsdSetPcmStreamVolume(int channel, int volume)
{
    RssdRequest request;

    request.arg[0].value = channel;
    request.arg[1].value = volume;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_PCM_STREAM_VOLUME, &request, 0, 0);
}

/*
 * channel is cached at RssdStrWork.vag_channel right after the call. See
 * SsdInitVagStreamMono below for the mono counterpart, which additionally
 * takes a pan value; a stereo stream already spans both output channels and
 * needs no explicit pan.
 */
void SsdInitVagStreamStereo(int channel, int volume)
{
    RssdRequest request;

    request.arg[0].value = channel;
    request.arg[1].value = volume;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_INIT_VAG_STREAM_STEREO, &request, 0, 0);
    RssdStrWork.vag_channel = channel;
}

void SsdInitVagStreamMono(int channel, int volume, int pan)
{
    RssdRequest request;

    request.arg[0].value = channel;
    request.arg[1].value = volume;
    request.arg[2].value = pan;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_INIT_VAG_STREAM_MONO, &request, 0, 0);
    RssdStrWork.vag_channel = channel;
}

void SsdDisposeVagStream(void)
{
    RssdRequest request;

    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_DISPOSE_VAG_STREAM, &request, 0, 0);
}

void SsdPlayVagStream(int channel, int start, int loop)
{
    RssdRequest request;

    request.arg[0].value = channel;
    request.arg[1].value = start;
    request.arg[2].value = loop;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_PLAY_VAG_STREAM, &request, 0, 0);
}

void SsdStopVagStream(int channel)
{
    RssdRequest request;

    request.arg[0].value = channel;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_STOP_VAG_STREAM, &request, 0, 0);
}

INCLUDE_ASM("asm/main/nonmatchings/ssd_3", SsdSetVagStreamDataStereo);

INCLUDE_ASM("asm/main/nonmatchings/ssd_3", SsdSetVagStreamDataMono);

int SsdGetVagStreamStatusStereo(void)
{
    RssdRequest request;

    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_GET_VAG_STREAM_STATUS_STEREO, &request, 0, 0);
    return 0;
}

int SsdGetVagStreamStatusMono(int channel)
{
    RssdRequest request;

    request.arg[0].value = channel;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_GET_VAG_STREAM_STATUS_MONO, &request, 0, 0);
    return 0;
}

/* Same command byte (0xb8) and body shape as SsdGetVagStreamStatusMono. */
int SsdGetVagStreamParam(int channel)
{
    RssdRequest request;

    request.arg[0].value = channel;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_GET_VAG_STREAM_PARAM, &request, 0, 0);
    return 0;
}

int SsdGetVagStreamStatusAll(void)
{
    RssdRequest request;

    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_GET_VAG_STREAM_STATUS_ALL, &request, 0, 0);
    return 0;
}

/*
 * duration follows this TU's own RSSD_CMD_SET_SEQ_* fade convention
 * (src/main/ssd_2.c: SsdSetSeqMasterVolume/Tempo/Note/Panpot and the
 * SsdSetSeqNoteFader/VolumeFader/PanpotFader family all pair a value with a
 * transition length). SsdSetVagStreamVolume below has its own accepted
 * prototype (src/main/game_over.h) confirming its third argument is `pan`
 * instead; this function's third argument has no further reader in this TU.
 */
void SsdSetVagStreamPitch(int channel, int pitch, int duration)
{
    RssdRequest request;

    request.arg[0].value = channel;
    request.arg[1].value = pitch;
    request.arg[2].value = duration;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_VAG_STREAM_PITCH, &request, 0, 0);
}

/* channel, volume, pan: matches the accepted prototype in src/main/game_over.h. */
void SsdSetVagStreamVolume(int channel, int volume, int pan)
{
    RssdRequest request;

    request.arg[0].value = channel;
    request.arg[1].value = volume;
    request.arg[2].value = pan;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_VAG_STREAM_VOLUME, &request, 0, 0);
}

/* duration: see SsdSetVagStreamPitch above for the fade-pair convention this follows. */
void SsdSetVagStreamPanpot(int channel, int panpot, int duration)
{
    RssdRequest request;

    request.arg[0].value = channel;
    request.arg[1].value = panpot;
    request.arg[2].value = duration;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_VAG_STREAM_PANPOT, &request, 0, 0);
}

INCLUDE_ASM("asm/main/nonmatchings/ssd_3", SsdSetVagStreamParam);

void SsdClearStreamRingBuffer(int channel)
{
    RssdRequest request;

    request.arg[0].value = channel;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_CLEAR_STREAM_RING_BUFFER, &request, 0, 0);
}
