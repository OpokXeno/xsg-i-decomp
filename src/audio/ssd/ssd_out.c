/* Original ssd_out.c translation unit. Shared data is defined by its owning FDR. */





typedef struct SsdWorkState SsdWorkState;
typedef struct SsdStereoVolume SsdStereoVolume;
typedef struct SsdVolumeFade SsdVolumeFade;
typedef struct SsdReverbState SsdReverbState;
typedef struct SsdChannel SsdChannel;
typedef struct SsdEffectAttr SsdEffectAttr;

struct SsdStereoVolume { short left, right; };

struct SsdVolumeFade { int value, step, target; short frames; short unmodeled_0e; };

struct SsdReverbState {
    unsigned char unmodeled_00[3];
    signed char mode;
    SsdStereoVolume volume;
    signed char delay, feedback;
    short depth;
    unsigned int work_end;
    unsigned char unmodeled_10[8];
};

struct SsdChannel {
    unsigned int unmodeled_00;
    struct SsdChannel *next;
    unsigned char unmodeled_08[12];
    void *device;
    unsigned char unmodeled_18[20];
    unsigned char output_volume, output_group;
};

struct SsdEffectAttr {
    int core, mode;
    SsdStereoVolume volume;
    int delay, feedback;
};

struct SsdWorkState {
    unsigned char unmodeled_000[0x168];
    SsdChannel *channels;
    unsigned char unmodeled_16c[0x40];
    unsigned int output_flags;
    SsdStereoVolume master_volume;
    short master_depth_left, master_depth_right;
    unsigned char unmodeled_1b8[4];
    SsdStereoVolume cd_volume;
    unsigned int cd_reverb_left, cd_reverb_right;
    unsigned char unmodeled_1c8[12];
    short master_depth, cd_depth;
    unsigned char unmodeled_1d8[4];
    unsigned char sequence_volume, effect_volume, unmodeled_1de, output_mode;
    SsdVolumeFade master_fade, cd_fade;
    unsigned int unmodeled_200;
    SsdReverbState reverb[2];
    unsigned char unmodeled_234[0x3048];
    int reverb_clear_result;
};

extern SsdWorkState SsdWork;
extern void sceSdVoiceTransStatus(short core, int wait);
extern void sceSdSetParam(unsigned short, unsigned short);
extern unsigned short sceSdGetParam(unsigned short);
extern void sceSdSetCoreAttr(unsigned short, unsigned short);
extern void sceSdSetAddr(unsigned short, unsigned int);
extern void sceSdGetEffectAttr(int, SsdEffectAttr *);
extern void sceSdSetEffectAttr(int, SsdEffectAttr *);
extern void sceSdClearEffectWorkArea(int, int, int);
extern void SsdSetReverbModeDepth(int, short, short);
extern void SsdSetChannelAllVoiceEvent(void *, int);
extern void SsdSetMidiDeviceOutputVolume(void *, int);
void SsdSetOutputMode(int mode);
void SsdSetSpuVolume(int volume, int frames);
void SsdSetCDVolume(int volume, int frames);
void SsdSetAudioAttenuator(int value);
void SsdSetCDMixingReverb(int left, int right);
void SsdCheckWideMode(void);
void SsdSetVolumeRec(int volume, SsdStereoVolume *output, int reverb);
void SsdSetGroupOutputVolume(int volume, int unused, int group);
int SsdReverbClearCompleted(int core);
void SsdReverbClear(void);
void SsdReverbClearCallback(void);
void SsdInitOutputControl(void);
void SsdSetSequenceOutputVolume(int volume, int unused);
void SsdSetEffectOutputVolume(int volume, int unused);
int SsdSetReverbParameter(int mode, int depth, int delay, int feedback);
void SsdEnableReverbParam(int core);

void SsdInitOutputControl(void) {
    int core;
    unsigned short parameter;
    SsdWork.master_depth_right = SsdWork.master_depth_left = 0;
    SsdWork.output_flags = 12;
    SsdWork.sequence_volume = 127;
    SsdWork.effect_volume = 127;
    SsdSetOutputMode(1);
    SsdSetSpuVolume(16383, 0);
    SsdSetCDVolume(0, 0);
    SsdSetAudioAttenuator(128);
    SsdSetCDMixingReverb(0, 1);
    sceSdSetCoreAttr(10, 2176);
    for (core = 0; core < 2; core++) {
        parameter = core | 0x800;
        sceSdSetParam(parameter, sceSdGetParam(parameter) & 0xffcf);
    }
}

void SsdSetOutputMode(int mode) {
    SsdChannel *channel;
    int left;
    int right;
    SsdWork.output_mode = mode;
    SsdCheckWideMode();
    SsdSetReverbModeDepth(0, SsdWork.reverb[0].volume.left, SsdWork.reverb[0].volume.right);
    SsdSetReverbModeDepth(1, SsdWork.reverb[1].volume.left, SsdWork.reverb[1].volume.right);
    for (channel = SsdWork.channels; channel; channel = channel->next)
        SsdSetChannelAllVoiceEvent(channel->device, 32);
    if (mode < 2) {
        left = right = 32767;
    } else {
        left = 32767;
        right = -32767;
    }
    sceSdSetParam(3457, left);
    sceSdSetParam(3713, right);
}

void SsdSetSpuVolume(int volume, int frames) {
    int difference;
    SsdWork.master_fade.target = volume;
    if (frames == 0) {
        SsdWork.master_fade.value = volume << 16;
        SsdWork.master_fade.frames = 0;
        SsdWork.master_depth = volume;
        SsdSetVolumeRec((short)volume, &SsdWork.master_volume, 0);
        SsdWork.output_flags |= 3;
    } else {
        difference = (volume << 8) - (SsdWork.master_fade.value >> 8);
        if (difference) {
            SsdWork.master_fade.frames = frames;
            SsdWork.master_fade.step = (difference / frames) << 8;
        }
    }
}

void SsdSetCDVolume(int volume, int frames) {
    int difference;
    SsdWork.cd_fade.target = volume;
    if (frames == 0) {
        SsdWork.cd_fade.value = volume << 16;
        SsdWork.cd_fade.frames = 0;
        SsdWork.cd_depth = volume;
        SsdWork.cd_volume.right = volume;
        SsdWork.cd_volume.left = volume;
        SsdWork.output_flags |= 0xc0;
    } else {
        difference = (volume << 8) - (SsdWork.cd_fade.value >> 8);
        if (difference) {
            SsdWork.cd_fade.frames = frames;
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
    SsdChannel *channel;
    for (channel = SsdWork.channels; channel; channel = channel->next) {
        if (channel->output_group == group) {
            channel->output_volume = volume;
            SsdSetMidiDeviceOutputVolume(channel->device, volume);
        }
    }
}

void SsdSetCDMixingReverb(int left, int right) {
    SsdWork.cd_reverb_left = left;
    SsdWork.cd_reverb_right = right;
    SsdWork.output_flags |= 0x300;
}

void SsdSetAudioAttenuator(int value) {}

int SsdSetReverbParameter(int mode, int depth, int delay, int feedback) {
    SsdEffectAttr attributes;
    int core;
    int changed;
    SsdReverbState *reverb;
    if (mode == -2) return 0;
    changed = 0;
    core = mode >> 8;
    mode &= 0xff;
    reverb = &SsdWork.reverb[core];
    if (mode == 0) {
        feedback = 0;
        delay = 0;
        depth = 0;
    } else if (mode == -1) {
        mode = reverb->mode;
    }
    sceSdGetEffectAttr(core, &attributes);
    if (attributes.mode != mode || mode == 0) {
        changed = 1;
        sceSdSetAddr(core | 0x1d00, reverb->work_end - 1);
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
        attributes.volume.right = 0;
        attributes.volume.left = 0;
    } else {
        attributes.volume.left = reverb->volume.left;
        attributes.volume.right = reverb->volume.right;
    }
    sceSdSetEffectAttr(core, &attributes);
    if (changed) sceSdClearEffectWorkArea(core, core, mode);
    return changed;
}

void SsdEnableReverbParam(int core) {
    SsdEffectAttr attributes;
    attributes.core = core;
    attributes.mode = SsdWork.reverb[core].mode;
    attributes.volume.left = SsdWork.reverb[core].volume.left;
    attributes.volume.right = SsdWork.reverb[core].volume.right;
    attributes.delay = SsdWork.reverb[core].delay;
    attributes.feedback = SsdWork.reverb[core].feedback;
    sceSdSetEffectAttr(core, &attributes);
}

void SsdCheckWideMode(void) {
    SsdSetVolumeRec(SsdWork.master_depth, &SsdWork.master_volume, 0);
    SsdWork.cd_volume.right = SsdWork.cd_depth;
    SsdWork.cd_volume.left = SsdWork.cd_depth;
    SsdSetVolumeRec(SsdWork.reverb[0].depth, &SsdWork.reverb[0].volume, 1);
    SsdSetVolumeRec(SsdWork.reverb[1].depth, &SsdWork.reverb[1].volume, 1);
    SsdWork.output_flags |= 3;
}

void SsdSetVolumeRec(int volume, SsdStereoVolume *output, int reverb) {
    unsigned int mode;
    output->right = volume;
    output->left = volume;
    mode = SsdWork.output_mode;
    if (mode >= 2U) {
        if (mode == 2 && reverb == 1) {
            output->right = -volume;
            return;
        }
        if (mode == 2 || reverb != 0) {
            output->left = -volume;
            return;
        }
        output->right = -volume;
    }
}

void SsdReverbClear(void) {}

void SsdReverbClearCallback(void) {}

int SsdReverbClearCompleted(int core) {
    sceSdVoiceTransStatus((short)core, 1);
    return SsdWork.reverb_clear_result;
}
