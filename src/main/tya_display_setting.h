#ifndef XENO_MAIN_TYA_DISPLAY_SETTING_H
#define XENO_MAIN_TYA_DISPLAY_SETTING_H

#include "main/xgl_mc.h"
#include "main/xgl_jpeg.h"
#include "main/xgl_thread.h"
#include "main/xgl_dma.h"

typedef struct TyaDisplayEnvironment {
    u64 flags;
    unsigned char unmodeled_08[0x10];
    struct {
        unsigned int dx : 12;
        unsigned int dy : 11;
        unsigned int magh : 4;
        unsigned int magv : 2;
        unsigned int unmodeled_29 : 3;
        unsigned int dw : 12;
        unsigned int dh : 11;
        unsigned int unmodeled_55 : 9;
    } display;
    unsigned char unmodeled_20[0x30];
} TyaDisplayEnvironment;

typedef struct TyaSaveDataRecord {
    unsigned char unmodeled_00[0x30];
    short horizontal_display_offset;
    short vertical_display_offset;
} TyaSaveDataRecord;

typedef struct TyaJpegRequest {
    u8 *source;
    u8 *destination;
    unsigned char unmodeled_08[0x18];
} TyaJpegRequest;

extern TyaDisplayEnvironment DispEnv;
extern const char displaySettingImagePath[];
extern const char displayHalfwordFormat[];

extern void sceGsSetDefDispEnv(TyaDisplayEnvironment *environment,
    int psm, int width, int height, int x_offset, int y_offset);

extern void xglSoundEffectNormalDirect(int effect_id);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
void tyaDisplaySetting(u8 *framebuffer_or_mode);

#endif
