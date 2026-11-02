#include "ssd/ssd_out.h"
#include "ssd/ssd_chan.h"
#include "ssd/ssd_dev.h"
#include "ssd/ssd_init.h"
#include "ssd/ssd_seq.h"

/* SPDIF mode register value written at start-up. */
#define SSD_SPDIF_MODE_DEFAULT 0x880

void SsdSetAudioAttenuator(int value);
void SsdSetCDMixingReverb(int left, int right);
void SsdCheckWideMode(void);
void SsdSetGroupOutputVolume(int volume, int unused, int group);
void SsdReverbClear(void);
void SsdReverbClearCallback(void);

void SsdInitOutputControl(void) {
    int core;
    unsigned short parameter;
    SsdWork.master_depth_right = SsdWork.master_depth_left = 0;
    SsdWork.output_flags = 12;
    SsdWork.sequence_volume = SSD_MIDI_VALUE_MAX;
    SsdWork.effect_volume = SSD_MIDI_VALUE_MAX;
    SsdSetOutputMode(SSD_OUTPUT_STEREO);
    SsdSetSpuVolume(SSD_MASTER_VOLUME_DEFAULT, 0);
    SsdSetCDVolume(0, 0);
    SsdSetAudioAttenuator(128);
    SsdSetCDMixingReverb(0, 1);
    sceSdSetCoreAttr(SD_C_SPDIF_MODE, SSD_SPDIF_MODE_DEFAULT);
    for (core = 0; core < SSD_CORE_COUNT; core++) {
        parameter = core | SD_P_MMIX;
        sceSdSetParam(parameter, sceSdGetParam(parameter) & 0xffcf);
    }
}

void SsdSetOutputMode(int mode) {
    SsdSequence *sequence;
    int left;
    int right;
    SsdWork.output_mode = mode;
    SsdCheckWideMode();
    SsdSetReverbModeDepth(0, SsdWork.cores.reverb[0].volume.left, SsdWork.cores.reverb[0].volume.right);
    SsdSetReverbModeDepth(1, SsdWork.cores.reverb[1].volume.left, SsdWork.cores.reverb[1].volume.right);
    for (sequence = SsdWork.sequences; sequence; sequence = sequence->next)
        SsdSetChannelAllVoiceEvent(sequence->device, SSD_VOICE_EVENT_VOLUME);
    if (mode < SSD_OUTPUT_SURROUND) {
        left = right = SSD_SPU_VOLUME_MAX;
    } else {
        left = SSD_SPU_VOLUME_MAX;
        right = -SSD_SPU_VOLUME_MAX;
    }
    sceSdSetParam(SD_P_AVOLL | SD_CORE_1, left);
    sceSdSetParam(SD_P_AVOLR | SD_CORE_1, right);
}

void SsdSetSpuVolume(int volume, int frames) {
    int difference;
    SsdWork.master_fade.target = volume;
    if (frames == 0) {
        SsdWork.master_fade.current.value = volume << 16;
        SsdWork.master_fade.remaining = 0;
        SsdWork.master_depth = volume;
        SsdSetVolumeRec((short)volume, &SsdWork.master_volume, 0);
        SsdWork.output_flags |= SSD_OUTPUT_MASTER_VOLUME;
    } else {
        difference = (volume << 8) - (SsdWork.master_fade.current.value >> 8);
        if (difference) {
            SsdWork.master_fade.remaining = frames;
            SsdWork.master_fade.step = (difference / frames) << 8;
        }
    }
}

void SsdSetCDVolume(int volume, int frames) {
    int difference;
    SsdWork.cd_fade.target = volume;
    if (frames == 0) {
        SsdWork.cd_fade.current.value = volume << 16;
        SsdWork.cd_fade.remaining = 0;
        SsdWork.cd_depth = volume;
        SsdWork.cd_volume.right = volume;
        SsdWork.cd_volume.left = volume;
        SsdWork.output_flags |= SSD_OUTPUT_CD_VOLUME;
    } else {
        difference = (volume << 8) - (SsdWork.cd_fade.current.value >> 8);
        if (difference) {
            SsdWork.cd_fade.remaining = frames;
            SsdWork.cd_fade.step = (difference / frames) << 8;
        }
    }
}

void SsdSetSequenceOutputVolume(int volume, int unused) {
    SsdWork.sequence_volume = volume;
    SsdSetGroupOutputVolume(volume, unused, 0);
}

void SsdSetEffectOutputVolume(int volume, int unused) {
    SsdWork.effect_volume = volume;
    SsdSetGroupOutputVolume(volume, unused, 1);
}

void SsdSetGroupOutputVolume(int volume, int unused, int group) {
    SsdSequence *sequence;
    for (sequence = SsdWork.sequences; sequence; sequence = sequence->next) {
        if (sequence->is_effect == group) {
            sequence->output_volume = volume;
            SsdSetMidiDeviceOutputVolume(sequence->device, volume);
        }
    }
}

void SsdSetCDMixingReverb(int left, int right) {
    SsdWork.cd_reverb_left = left;
    SsdWork.cd_reverb_right = right;
    SsdWork.output_flags |= SSD_OUTPUT_CD_REVERB;
}

void SsdSetAudioAttenuator(int value) {}

int SsdSetReverbParameter(int mode, int depth, int delay, int feedback) {
    SceSdEffectAttr attributes;
    int core;
    int changed;
    SsdReverbState *reverb;
    if (mode == SSD_REVERB_MODE_SKIP) return 0;
    changed = 0;
    core = mode >> 8;
    mode &= 0xff;
    reverb = &SsdWork.cores.reverb[core];
    if (mode == SSD_REVERB_MODE_OFF) {
        feedback = 0;
        delay = 0;
        depth = 0;
    } else if (mode == SSD_REVERB_MODE_NONE) {
        mode = reverb->mode;
    }
    sceSdGetEffectAttr(core, &attributes);
    if (attributes.mode != mode || mode == SSD_REVERB_MODE_OFF) {
        changed = 1;
        sceSdSetAddr(core | SD_A_EEA,reverb->work_end - 1);
    }
    reverb->mode = mode;
    reverb->delay = delay;
    reverb->feedback = feedback;
    reverb->depth = depth;
    SsdCheckWideMode();
    attributes.core = core;
    attributes.mode = mode;
    attributes.delay = delay;
    attributes.feedback = feedback;
    if (changed) {
        attributes.depth_right = 0;
        attributes.depth_left = 0;
    } else {
        attributes.depth_left = reverb->volume.left;
        attributes.depth_right = reverb->volume.right;
    }
    sceSdSetEffectAttr(core, &attributes);
    if (changed) sceSdClearEffectWorkArea(core, core, mode);
    return changed;
}

void SsdEnableReverbParam(int core) {
    SceSdEffectAttr attributes;
    attributes.core = core;
    attributes.mode = SsdWork.cores.reverb[core].mode;
    attributes.depth_left = SsdWork.cores.reverb[core].volume.left;
    attributes.depth_right = SsdWork.cores.reverb[core].volume.right;
    attributes.delay = SsdWork.cores.reverb[core].delay;
    attributes.feedback = SsdWork.cores.reverb[core].feedback;
    sceSdSetEffectAttr(core, &attributes);
}

void SsdCheckWideMode(void) {
    SsdSetVolumeRec(SsdWork.master_depth, &SsdWork.master_volume, 0);
    SsdWork.cd_volume.right = SsdWork.cd_depth;
    SsdWork.cd_volume.left = SsdWork.cd_depth;
    SsdSetVolumeRec(SsdWork.cores.reverb[0].depth, &SsdWork.cores.reverb[0].volume, 1);
    SsdSetVolumeRec(SsdWork.cores.reverb[1].depth, &SsdWork.cores.reverb[1].volume, 1);
    SsdWork.output_flags |= SSD_OUTPUT_MASTER_VOLUME;
}

void SsdSetVolumeRec(int volume, SsdStereoVolume *output, int reverb) {
    unsigned int mode;
    output->right = volume;
    output->left = volume;
    mode = SsdWork.output_mode;
    if (mode >= SSD_OUTPUT_SURROUND) {
        if (mode == SSD_OUTPUT_SURROUND && reverb == 1) {
            output->right = -volume;
            return;
        }
        if (mode == SSD_OUTPUT_SURROUND || reverb != 0) {
            output->left = -volume;
            return;
        }
        output->right = -volume;
    }
}

void SsdReverbClear(void) {}

void SsdReverbClearCallback(void) {}

int SsdReverbClearCompleted(int core, int wait) {
    sceSdVoiceTransStatus(core, 1);
    return SsdWork.reverb_clear_result;
}
