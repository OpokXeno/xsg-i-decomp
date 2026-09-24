/*
 * TU-local declarations of main/tu215 (src/main/sdv.c).
 */

#ifndef SRC_MAIN_SDV_H
#define SRC_MAIN_SDV_H

typedef short int16_t;

/*
 * Fresh Spark hypothesis F1: renamed partial camera-offset view.
 * Changed from the predecessor's SdvCameraOffset/offset_mode/offset_source
 * spelling: identical 48-byte layout and else-if branch shape, but distinct
 * type/field/local identifiers (SdvCamOffset/cam_kind/cam_vec, pos/ang) to
 * test the ABI saved-register hypothesis that local-alias lifetime spelling
 * keeps mode in s1 and source in s2. Unnamed 32-bit units preserve the
 * observed +0x10/+0x20 vector offsets and assert no historical semantics.
 */
typedef struct {
    int kind;
    unsigned int : 32;
    unsigned int : 32;
    unsigned int : 32;
    float pos[3];
    unsigned int : 32;
    float ang[3];
    unsigned int : 32;
} SdvCamOffset;

extern int _sdvAmbFrame;
extern int _sdvAmbState;

static void sdvSetAmbStateSub(int state, int effect_no, int force);
extern void *xglStudioGetLight2(void);
extern void xglLightIntensityAmbient(void *light, void *ambient);
extern void func_A2C3D8(void *map_rgb);
extern unsigned char _sdvAmbient[16];
extern unsigned char _sdvMapRgb[16];
extern void xglSoundEffectNormalID(int sound_id, int variant);

extern int srsAnalyzeEftNo(int effectId, unsigned char *charId,
                            int *effectCategory);
extern unsigned char charID_0;
extern int eftCate_1;

/*
 * One 12-byte channel of a camera schedule task: sdvScheduleCamera reads
 * only the last word (nonzero enables the channel for this frame) and hands
 * the whole 12 bytes, or a channel-specific prefix of it, to sdvProgressPrm.
 * The first 8 bytes are otherwise unread by sdvScheduleCamera.
 */
typedef struct SdvCameraPhase {
    unsigned char unmodeled_00[8];
    int active;                    /* +0x08 */
} SdvCameraPhase;

/*
 * A battle camera schedule task (sefExecScheduler's caller data): only the
 * fields sdvScheduleCamera reads are named.
 */
typedef struct SdvCameraTask {
    int active;                    /* +0x00 */
    unsigned char unmodeled_04[0x10];
    SdvCameraPhase pos;             /* +0x14 */
    SdvCameraPhase angle;           /* +0x20 */
    SdvCameraPhase scale;           /* +0x2C */
    SdvCameraPhase offset;          /* +0x38 */
} SdvCameraTask;

extern short _sefBattleMode;
extern void sdvProgressPrm(int kind, void *data, int size);
extern void func_A31920(int kind, float *params);

void sdvScheduleCamera(SdvCameraTask *task);

extern void GameDefocusSet(int index, int, int);
extern short _sdvSpecialBuf[8];

void sdvClearSpecialWork(void);

/*
 * One 0x1280-byte alter record. Only the +0x1278 status halfword that
 * sdvInitAlters, sdvDestroyAlters and sdvExecAlters read is named.
 */
typedef struct SdvAlter {
    unsigned char unmodeled_0000[0x1278];
    unsigned short active;          /* +0x1278 */
    unsigned char unmodeled_127a[6];
} SdvAlter;

extern SdvAlter _sdvAlter[16];
extern void sdvInitAlter(SdvAlter *alter);
extern void sdvDestroyAlter(SdvAlter *alter);
extern unsigned short sdvExecAlter(SdvAlter *alter);

void sdvInitAlters(void);
void sdvDestroyAlters(void);
unsigned short sdvExecAlters(void);

#endif /* SRC_MAIN_SDV_H */
