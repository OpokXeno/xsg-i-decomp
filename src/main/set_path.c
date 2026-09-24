#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "main/xgl_studio.h"

INCLUDE_ASM("asm/main/nonmatchings/set_path", splitName);

void splitName(unsigned char *filename, unsigned char *path, unsigned char *name);
void srsSetViewPath(unsigned char *path);
extern unsigned char name_1[];
extern unsigned char path_0[];

void setPath(unsigned char *filename)
{
    splitName(filename, path_0, name_1);
    srsSetViewPath(path_0);
}

INCLUDE_ASM("asm/main/nonmatchings/set_path", initFileSelect);

INCLUDE_ASM("asm/main/nonmatchings/set_path", execFileSelect);

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
    u8 unmodeled_00[0x2a];
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

INCLUDE_ASM("asm/main/nonmatchings/set_path", debugCfSelect);

INCLUDE_ASM("asm/main/nonmatchings/set_path", SCamTake);

INCLUDE_ASM("asm/main/nonmatchings/set_path", SinitCamera);

INCLUDE_ASM("asm/main/nonmatchings/set_path", sbattleDebug);

INCLUDE_ASM("asm/main/nonmatchings/set_path", seffectDebugDb);

INCLUDE_ASM("asm/main/nonmatchings/set_path", seffectDebugCf);

INCLUDE_ASM("asm/main/nonmatchings/set_path", seffectDebugBattle);

extern void func_A1D1C0(int level);
extern void func_A2C3A0(int level);
extern int srsFileLoad(void *buffer, const char *name, int mode);
extern char *gridlexTop;
extern char *gridxtxTop;
extern char *hamalexTop;
extern char *hamaxtxTop;
extern s16 loaded;
extern char D_004CBDD8[];
extern char D_004CBDE8[];
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
extern s16 casdisp;
extern Vector4 caspos_15;
extern s16 mapdisp;
extern Vector4 pos_14;
extern Vector4 scale_17;
extern s16 tgtdisp;
extern Vector4 tgtpos_16;

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
