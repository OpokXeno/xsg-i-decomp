#include "common.h"

/*
 * Returns the SPU-DMA busy bit of RssdWork; a nonzero wait first spins until
 * it clears (main:0x002409b0).  Same prototype as src/ov01/snd.h.
 */
extern int SsdSpuDmaCompleted(int wait);
extern void xglSoundSequenceNormal3(int channel, int volume, int time);
extern void xglSoundSequenceFadeOut2(int channel, int time);
extern void xglSoundSequenceStop2(int channel);

void xglSoundWaitDma(void)
{
    SsdSpuDmaCompleted(0);
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundSendSwd);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundSendSmd2);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundSendSmd);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundSendSed);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundLoadSwd);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundSendEffect);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundLoadEffect);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundLoadRequestSmd);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundSequenceNormal3);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundSequenceNormal2);

void xglSoundSequenceNormal(int volume)
{
    xglSoundSequenceNormal3(0, volume, 0);
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundSequenceFadeOut2);

void xglSoundSequenceFadeOut(int time)
{
    xglSoundSequenceFadeOut2(0, time);
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundSequenceStop2);

void xglSoundSequenceStop(void)
{
    xglSoundSequenceStop2(0);
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundEffectNormalDirect);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundEffectNormalID);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundEffectParamDirect);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundEffectParamID);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundEffectPosID);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundEffectStopDirect);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundEffectStopBank);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundEffectStopID);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundEffectCheckID);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundStreamOpenPcm);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundStreamOpenVagStereoParam);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundStreamOpenVagStereo);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundStreamOpenVagMultiParam);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundStreamOpenVagMulti);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundStreamStop);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundStreamMute);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", stream_check);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundStreamMain);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglMakeSePacket);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSendSePacket);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundReset);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundInitial);
