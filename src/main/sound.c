#include "common.h"

/*
 * The script VM's per-thread context, recovered as `JThread` in
 * src/main/chr.h (main's chr TU). Every Java native receives
 * (thread, arguments, result) as the sibling natives elsewhere in the TU
 * map (src/main/runtime.c, src/main/window.c) show; these wrappers only
 * need the tag.
 */
typedef struct JThread JThread;

/* xglSoundSequenceNormal is defined in main/tu098 (src/main/xgl_sound.c). */
extern void xglSoundSequenceNormal(int volume);

void Java_xeno_Sound_sequencePlay__I(JThread *thread, int *arguments,
                                     unsigned int *result)
{
    xglSoundSequenceNormal(*arguments);
}

/* The call block of Sound.sequencePlay(int, int): the channel to play the
   sequence on and its playback volume. */
typedef struct SoundChannelVolumeCall {
    int channel;
    int volume;
} SoundChannelVolumeCall;

/* xglSoundSequenceNormal2 is defined in main/tu098 (src/main/xgl_sound.c). */
extern void xglSoundSequenceNormal2(int channel, int volume);

void Java_xeno_Sound_sequencePlay__II(JThread *thread,
                                      SoundChannelVolumeCall *arguments,
                                      unsigned int *result)
{
    xglSoundSequenceNormal2(arguments->channel, arguments->volume);
}

/* xglSoundSequenceFadeOut is defined in main/tu098 (src/main/xgl_sound.c). */
extern void xglSoundSequenceFadeOut(int time);

void Java_xeno_Sound_sequenceStop__I(JThread *thread, int *arguments,
                                     unsigned int *result)
{
    xglSoundSequenceFadeOut(*arguments);
}

/* The call block of Sound.sequenceStop(int, int): the channel to fade out
   and the fade-out time. */
typedef struct SoundChannelTimeCall {
    int channel;
    int time;
} SoundChannelTimeCall;

/* xglSoundSequenceFadeOut2 is defined in main/tu098 (src/main/xgl_sound.c). */
extern void xglSoundSequenceFadeOut2(int channel, int time);

void Java_xeno_Sound_sequenceStop__II(JThread *thread,
                                      SoundChannelTimeCall *arguments,
                                      unsigned int *result)
{
    xglSoundSequenceFadeOut2(arguments->channel, arguments->time);
}

/* The call block of Sound.effectPlay(int, int, int): the effect sound ID,
   its volume and its pan; the engine call's trailing flags argument is
   fixed to 0 by this native. */
typedef struct SoundEffectPlayCall {
    int sound_id;
    int volume;
    int pan;
} SoundEffectPlayCall;

/*
 * xglSoundEffectParamID is not yet recovered (still INCLUDE_ASM in
 * main/tu098, src/main/xgl_sound.c); declared with the (sound_id, volume,
 * pan, flags) shape src/ov12/xrg_sound.h and this call site agree on.
 */
extern void xglSoundEffectParamID(int sound_id, int volume, int pan, int flags);

void Java_xeno_Sound_effectPlay__III(JThread *thread,
                                     SoundEffectPlayCall *arguments,
                                     unsigned int *result)
{
    xglSoundEffectParamID(arguments->sound_id, arguments->volume,
                          arguments->pan, 0);
}

/*
 * xglSoundEffectStopID is not yet recovered (still INCLUDE_ASM in
 * main/tu098, src/main/xgl_sound.c); declared with the (sound_id, flags)
 * shape src/ov12/xrg_sound.h and this call site agree on.
 */
extern void xglSoundEffectStopID(int sound_id, int flags);

void Java_xeno_Sound_effectStop__I(JThread *thread, int *arguments,
                                   unsigned int *result)
{
    xglSoundEffectStopID(*arguments, 0);
}

INCLUDE_ASM("asm/main/nonmatchings/sound", Java_xeno_Sound_streamPlay__IIII);
