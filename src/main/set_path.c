#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "main/xgl_studio.h"

static unsigned char path_0[0x80];
static unsigned char name_1[0x40];


extern unsigned int strlen(const char *string);

static void splitName(unsigned char *filename, unsigned char *path, unsigned char *name)
{
    int len;
    int slash;
    int i;
    int nameLen;

    len = strlen((const char *) filename);
    slash = 0;
    for (i = len; i >= 0; i--) {
        if ((signed char) filename[i] == '/') {
            slash = i;
            break;
        }
    }

    for (i = 0; i <= slash; i++) {
        path[i] = filename[i];
    }
    path[slash + 1] = 0;

    nameLen = 0;
    for (i = slash + 1; i <= len; i++) {
        name[nameLen] = filename[i];
        nameLen++;
    }
    name[nameLen] = 0;
}

void splitName(unsigned char *filename, unsigned char *path, unsigned char *name);
void srsSetViewPath(unsigned char *path);
void setPath(unsigned char *filename)
{
    splitName(filename, path_0, name_1);
    srsSetViewPath(path_0);
}

INCLUDE_ASM("asm/main/nonmatchings/set_path", initFileSelect);

typedef struct CfSelectState {
    unsigned char unmodeled_00[0x11c];
    /* +0x11c: the selected entry's path, filled by xglCdFileSelect and
     * handed to setPath() by execFileSelect; its extent past this offset
     * is unmodeled. */
    unsigned char selectedPath[1];
    unsigned char unmodeled_11d[0x268 - 0x11d];
} CfSelectState;
static CfSelectState cfs;
extern void xglRenderClearFrame(void);
extern void xglSleep(void);
extern int xglCdFileSelect(CfSelectState *state);

int execFileSelect(void)
{
    int result;

    for (;;) {
        xglRenderClearFrame();
        result = xglCdFileSelect(&cfs);
        switch (result) {
        case 0:
            break;
        case 1:
            setPath(cfs.selectedPath);
            return 1;
        case 2:
            setPath(cfs.selectedPath);
            return 2;
        }
        xglSleep();
    }
}

/*
 * PARTIAL ACCESSED PREFIX of PadData (0xd0-byte object at 0x490d90).
 * sdbExecEffect reads only the halfword at +0x2a (masked with 0x800; the
 * accepted "pressed" name from src/main/game.c) and the halfword at +0x2c
 * (masked with 0x100), one halfword past the shared include/shared.h
 * PadPrefix view (evidenced only to +0x2c).
 *
 * header_divergence (recorded, allowed): a further local extension of
 * PadData past PadPrefix, the same situation documented in
 * src/main/game.h's PadDataDebugLayout.
 */
typedef struct PadDataEffectLayout {
    u8 unmodeled_00[0x28];
    u16 half_28;
    u16 pressed;
    u16 half_2c;
} PadDataEffectLayout;

extern PadDataEffectLayout PadData;
extern s16 _debugPause;
extern char D_004DBA08[];
extern void sefExecEffect(void);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
extern void func_A31438(void);
extern void xglSoundStreamMain(void);

static void sdbExecEffect(void)
{
    PadDataEffectLayout *pad = &PadData;
    u16 toggled;
    u16 debugPaused;

    if (pad->pressed & 0x800) {
        toggled = (_debugPause == 0);
        _debugPause = toggled;
        debugPaused = toggled;
    } else {
        debugPaused = _debugPause;
    }

    if (debugPaused == 0 || (pad->half_2c & 0x100)) {
        sefExecEffect();
    } else {
        xglFontDebugPrintf(0x10, 0x18, D_004DBA08);
    }

    func_A31438();
    xglSoundStreamMain();
}

void sdbDebugCamera(void) {

}

static void sdebufPrintMenu(int x, int y, int dy, char **list, int count)
{
    char **cursor;
    char *label;
    int printY;
    int lineY;
    int lineX;
    int remaining;
    int step;

    step = dy;
    if (count > 0) {
        lineX = x + 8;
        cursor = list;
        lineY = y;
        remaining = count;
        do {
            label = *cursor;
            cursor++;
            printY = lineY;
            lineY += step;
            remaining--;
            xglFontDebugPrintf(lineX, printY, label);
        } while (remaining != 0);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/set_path", debugEffectConfig);

INCLUDE_ASM("asm/main/nonmatchings/set_path", debugEffectSelect);

INCLUDE_ASM("asm/main/nonmatchings/set_path", debugEnemySelect);

typedef struct PadDataMenuLayout {
    u8 unmodeled_00[0x28];
    u64 buttons;
} PadDataMenuLayout;

extern int sprintf(char *destination, const char *format, ...);
extern void xglRenderClearFrame(void);
extern void xglSleep(void);
static int cur_7;
extern const char D_004CBD68[];
extern char D_004DBA28[];
extern int srsGetEsdData2(int index);

static int debugCfSelect(void)
{
    char text[0x100];
    u16 held;
    int esd;

    for (;;) {
        esd = srsGetEsdData2(cur_7);
        xglRenderClearFrame();
        sprintf(text, D_004DBA28, esd, cur_7);
        xglFontDebugPrintf(0x10, 0x10, D_004CBD68);
        xglFontDebugPrintf(0x10, 0x20, text);
        if ((((PadDataMenuLayout *) &PadData)->buttons & 0x08000100) == 0x08000100) {
            cur_7 = -1;
            break;
        }
        held = PadData.half_2c;
        if (held & 0x4) cur_7 -= 10;
        if (held & 0x1) cur_7 -= 50;
        if (held & 0x8000) cur_7--;
        if (held & 0x2) cur_7 += 50;
        if (held & 0x8) cur_7 += 10;
        if (held & 0x2000) cur_7++;
        if (cur_7 < 601) cur_7 = 1999;
        if (cur_7 >= 2000) cur_7 = 601;
        if (PadData.pressed & 0x20) break;
        xglSleep();
    }
    xglSleep();
    xglRenderClearFrame();
    return cur_7;
}

extern void func_A31688(void);
extern void func_A31920(int kind, float *params);
extern void func_A318B8(void);
#define D_004D8298 0.1f
#define D_004D829C 2.6f
extern void *memset(void *destination, int value, unsigned int count);

static void SCamTake(void)
{
    float params[44];

    func_A31688();
    memset(params, 0, sizeof(params));
    params[0] = 0.0f;
    params[4] = D_004D8298;
    params[5] = D_004D829C;
    params[6] = 5.0f;
    params[7] = 1.0f;
    func_A31920(0, params);

    params[4] = 0.0f;
    params[5] = 2.0f;
    params[6] = 0.0f;
    params[7] = 1.0f;
    func_A31920(1, params);

    params[41] = 40.0f;
    func_A31920(3, params);

    func_A318B8();
}

INCLUDE_ASM("asm/main/nonmatchings/set_path", SinitCamera);

extern void PartyDataInit2(void);
extern int dataItmBoxChk(int id);
extern int dataItmBoxInc(int id);
extern void func_A2F6A8(void);
extern void func_A22668(void);
static int filelist;
static s16 loaded;

static void sbattleDebug(void)
{
    int i;

    filelist = 0;
    loaded = 0;
    PartyDataInit2();

    for (;;) {
        xglRenderClearFrame();
        i = 0;
        func_A2F6A8();
        while (i < 0x63) {
            i++;
            if (dataItmBoxChk(0x15) >= 0x63) {
                break;
            }
            dataItmBoxInc(0x15);
        }
        func_A22668();
    }
}

INCLUDE_ASM("asm/main/nonmatchings/set_path", seffectDebugDb);

INCLUDE_ASM("asm/main/nonmatchings/set_path", seffectDebugCf);

INCLUDE_ASM("asm/main/nonmatchings/set_path", seffectDebugBattle);

extern void func_A1D1C0(int level);
extern void func_A2C3A0(int level);
extern int srsFileLoad(void *buffer, const char *name, int mode);
static char *gridlexTop;
static char *gridxtxTop;
static char *hamalexTop;
static char *hamaxtxTop;
extern const char D_004CBDD8[];
extern const char D_004CBDE8[];
extern char D_004DBA38[];
extern char D_004DBA40[];

static void dummyMapLoad(void)
{
    if (loaded == 0) {
        func_A2C3A0(10);
        func_A1D1C0(10);
        srsFileLoad(gridlexTop, D_004DBA38, 1);
        srsFileLoad(gridxtxTop, D_004DBA40, 1);
        srsFileLoad(hamalexTop, D_004CBDD8, 1);
        srsFileLoad(hamaxtxTop, D_004CBDE8, 1);
        loaded = 1;
    }
}

extern int func_A1A760(void);
extern void func_A2C488(int level);
extern void nmlModelEntry(int entry);
extern void nmlModelSetLight(void *colorMod, void *normal);
extern void nmlModelSetPlace(Matrix4 matrix);
extern void nmlModelSetTexture(const char *texture);
extern int sefCheckLoad(void);
extern void xglLightCalcMatrix(StudioLight *light);
extern void xglLightSetDefault(StudioLight *light);
extern StudioLight *xglStudioGetLight2(void);
extern unsigned char _defMatLcMod[];
extern unsigned char _defMatLn[];
static s16 casdisp;
static Vector4 caspos_15;
static s16 mapdisp;
static Vector4 pos_14;
static Vector4 scale_17;
static s16 tgtdisp;
static Vector4 tgtpos_16;

static void dummyMapDraw(void)
{
    Matrix4 sp0;
    StudioLight *light;

    if (func_A1A760() != 0) {
        return;
    }
    if (sefCheckLoad() != 0) {
        return;
    }
    if (loaded == 0) {
        return;
    }

    light = xglStudioGetLight2();
    xglLightSetDefault(light);
    xglLightCalcMatrix(light);

    if (mapdisp != 0) {
        xglMatrixStackUnit();
        xglMatrixStackTrans(&pos_14.x);
        xglMatrixStackSave(sp0);
        func_A2C488(10);
        xglMatrixStackUnit();
        xglMatrixStackTrans(&pos_14.x);
        xglMatrixStackScale(&scale_17.x);
        xglMatrixStackSave(sp0);
        nmlModelSetTexture(gridxtxTop);
        nmlModelSetPlace(sp0);
        nmlModelSetLight(_defMatLcMod, _defMatLn);
        nmlModelEntry((int) gridlexTop);
    }
    if (casdisp != 0) {
        xglMatrixStackUnit();
        xglMatrixStackTrans(&caspos_15.x);
        xglMatrixStackScale(&scale_17.x);
        xglMatrixStackSave(sp0);
        nmlModelSetTexture(hamaxtxTop);
        nmlModelSetPlace(sp0);
        nmlModelSetLight(_defMatLcMod, _defMatLn);
        nmlModelEntry((int) hamalexTop);
    }
    if (tgtdisp != 0) {
        xglMatrixStackUnit();
        xglMatrixStackTrans(&tgtpos_16.x);
        xglMatrixStackScale(&scale_17.x);
        xglMatrixStackSave(sp0);
        nmlModelSetTexture(hamaxtxTop);
        nmlModelSetPlace(sp0);
        nmlModelEntry((int) hamalexTop);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/set_path", SimajiriTest);

/* Initialized objects recovered for this TU. */
const char D_004CBD68[40] =
    "\xA3\xC3\xA3\xC6\xCE\xCE\xB0\xE8\xA5\xC6\xA5\xB9\xA5\xC8\xCD\xD1"
    "\xA4\xCE\xA5\xA8\xA5\xD5\xA5\xA7\xA5\xAF\xA5\xC8\xC1\xAA\xC2\xF2";
static Vector4 pos_14 = {0.0f, 0.0f, 0.0f, 1.0f};
static Vector4 caspos_15 = {0.0f, 0.0f, 0.0f, 1.0f};
static Vector4 tgtpos_16 = {0.0f, 0.0f, -4.0f, 1.0f};
static Vector4 scale_17 = {0.1f, 0.1f, 0.1f, 1.0f};
static int filelist = 0;
static char *gridlexTop = 0;
static char *gridxtxTop = 0;
static char *hamalexTop = 0;
static char *hamaxtxTop = 0;
static s16 loaded = 0;
static s16 mapdisp = 1;
static s16 casdisp = 0;
static s16 tgtdisp = 0;
s16 _debugPause = 0;
char D_004DBA08[8] = "PAUSE";
char D_004DBA28[8] = "%s : %d";
static int cur_7 = 0x259;
char D_004DBA38[8] = "sen.lex";
char D_004DBA40[8] = "sen.xtx";
const char D_004CBDD8[16] = "hama.lex";
const char D_004CBDE8[24] = "hama.xtx";
