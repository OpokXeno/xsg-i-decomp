#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "main/xgl_studio.h"
#include "main/sef.h"
#include "main/data.h"
#include "main/nml_model_set.h"
#include "main/party.h"
#include "main/srs.h"
#include "main/xgl_font.h"
#include "main/xgl_light.h"
#include "main/xgl_render.h"
#include "main/xgl_sound.h"
#include "main/xgl_thread.h"
#include "vu0_macro.h"

#define SCAM_TAKE_DEFAULT_RATE 0.1f
#define SCAM_TAKE_DEFAULT_RANGE 2.6f

/* Effect viewer exit chord on PadData.buttons.packed: SELECT held (bit 8 of
 * the held halfword) while START is pressed (bit 11 of the pressed halfword). */
#define EFFECT_VIEWER_EXIT_CHORD 0x08000100

/* Fixed EE RAM areas of the effect viewer. */
#define EFFECT_VIEWER_FILE_LIST_ADDR 0x1000000
#define EFFECT_VIEWER_FILE_LIST_SIZE 0x160000
#define EFFECT_VIEWER_GRIDLEX_ADDR 0x1160000
#define EFFECT_VIEWER_GRIDXTX_ADDR 0x1167200
#define EFFECT_VIEWER_HAMALEX_ADDR 0x1168700
#define EFFECT_VIEWER_HAMAXTX_ADDR 0x116AA00
#define EFFECT_VIEWER_CF_SCRIPT_ADDR 0x1172F00

/* File browser layout (rows on screen, margins). */
#define EFFECT_VIEWER_FILE_ROWS 20
#define EFFECT_VIEWER_FILE_TOP 20
#define EFFECT_VIEWER_FILE_LEFT 8

/* Effect number steps: L1/R1 move by 10, L2/R2 by 50, LEFT/RIGHT by 1. */
#define EFFECT_SELECT_STEP_MEDIUM 10
#define EFFECT_SELECT_STEP_COARSE 50

/* Effect id ranges. */
#define DB_EFFECT_BASE 3041       /* debug "db" effects are numbered from here */
#define DB_EFFECT_MIN 1
#define DB_EFFECT_MAX 30
#define CF_EFFECT_FIRST 601       /* CF effect numbers: 601 .. CF_EFFECT_END - 1 */
#define CF_EFFECT_END 2000
#define EFFECT_WEAPON_BASE 2800   /* weapon effects: 200 ids from here */
#define EFFECT_WEAPON_COUNT 200
#define PLAYER_CHARA_COUNT 32     /* effect charaId 1..32 are player characters */

/* Menu entries returned by debugEffectConfig, in menu_3 order (the
 * ColliTest*Label strings). */
enum EffectViewerMode {
    EFFECT_VIEWER_BATTLE = 0,       /* ColliTestBattleLabel */
    EFFECT_VIEWER_BATTLE_DEBUG = 1, /* ColliTestBattleDebugLabel */
    EFFECT_VIEWER_CF_DEBUG = 2,     /* ColliTestCfDebugLabel */
    EFFECT_VIEWER_DEBUG = 3,        /* ColliTestDebugLabel */
    EFFECT_VIEWER_TITLE = 4         /* ColliTestTitleLabel */
};

/* The effect request block sefCreateEffect reads: the debug viewers fill the
 * actor links with zero and select the effect and its variant. */
typedef struct SetPathEffectParams {
    void *actor;
    void *target;
    void *source;
    unsigned char unmodeled_0c[4];
    short charaId;
    unsigned char unmodeled_12[2];
    short effectId;
    short variant;
    unsigned char unmodeled_18[0x24 - 0x18];
} SetPathEffectParams;

typedef struct SetPathDummyActor {
    unsigned char unmodeled_00[0x50];
    Vector4 rotation;
    Vector4 scale;
    unsigned char unmodeled_70[0x86 - 0x70];
    short enabled;
    unsigned char unmodeled_88[0xa70 - 0x88];
} SetPathDummyActor;

typedef struct SetPathEffectVector {
    float x;
    float y;
    float z;
} SetPathEffectVector;

typedef struct CfSelectState {
    int horizontalOrigin; /* +0x0 */
    int verticalOrigin; /* +0x4 */
    int visibleRows; /* +0x8 */
    int phase; /* +0xc */
    char basePath[256]; /* +0x10 */
    char *listBuffer; /* +0x110 */
    int listSize; /* +0x114 */
    void (*printFunc)(int x, int y, const char *format, ...); /* +0x118 */
    /* +0x11c: the selected entry's path, filled by xglCdFileSelect and
     * handed to setPath() by execFileSelect; its extent past this offset
     * is unmodeled. */
    char selectedPath[1];
    unsigned char unmodeled_11d[0x268 - 0x11d];
} CfSelectState;

/* Two 0x68-byte controller records. xglPadRead's CPU stores evidence
 * the overlapping held/pressed/repeat/released words at +0x28..+0x2e. */
typedef struct PadDataMenuLayout {
    u8 unmodeled_00[0x28];
    /* debugEffectConfig and debugCfSelect test the +0x28 doubleword as
     * one packed mask, then read its +0x2a and +0x2c halfwords separately.
     * Both representations are evidenced by their original ld/lhu reads. */
    union {
        u64 packed;
        struct {
            u16 held;
            u16 pressed;
            u16 repeat;
            u16 released;
        } halfwords;
    } buttons;
    u8 unmodeled_30[0x68 - 0x30];
} PadDataMenuLayout;

typedef struct SetPathClearEnv {
    unsigned char unmodeled_00[0x20];
    unsigned int color_r;
    unsigned int color_g;
    unsigned int color_b;
    unsigned int color_a;
    unsigned char unmodeled_30[0x30];
} SetPathClearEnv;

/* The studio camera as SinitCamera writes it. */
typedef struct SetPathCamera {
    unsigned char unmodeled_00[0xd0];
    Vector4 position;
    unsigned char unmodeled_e0[0x20];
    Vector4 target;
    Vector4 unmodeled_110;
    Vector4 up;
    unsigned char unmodeled_130[0x4d0 - 0x130];
} SetPathCamera;

extern s16 _sdrawFloor;
extern const char nowLoadingText[];
extern const char pressConfirmToStartText[];
extern const char enemyZSelectFormat[];
extern const char comboEnemyEffectSelectText[];
char pauseText[];
char enemySelectFormat[];
char senLexName[];
char senXtxName[];
extern PadDataMenuLayout PadData[2];
const char cfEffectSelectText[];
const char hamaLexName[];
const char hamaXtxName[];
extern Matrix4 _defMatLcMod[];
extern Matrix4 _defMatLn[];
extern const char debugVersionFormat[16];
extern const char buildDateText[16];
extern const char buildTimeText[16];
extern const char setPathDebugCursorText[];
extern SetPathClearEnv ClearEnv;
extern const char debugDbSelectFormat[];
extern const char effectSelectionText[];

extern const char setPathLoadingText[];

extern char ColliTestTitleLabel[16];
extern char ColliTestBattleLabel[16];
extern char ColliTestCfDebugLabel[16];
extern char ColliTestBattleDebugLabel[24];
extern char ColliTestDebugLabel[16];

extern void sefLoadEffect(int effect, int variant);
static void dummyMapLoad(void);
extern void sefCreateEffect(SetPathEffectParams *params);
extern unsigned int strlen(const char *string);
static void splitName(char *filename, char *path, char *name);
void srsSetViewPath(char *path);
extern int xglCdFileSelect(CfSelectState *state);
extern void func_A31438(void);
extern int sprintf(char *destination, const char *format, ...);
extern char *srsGetEsdData2(int index);
extern void func_A31688(void);
extern void func_A31920(int kind, float *params);
extern void func_A318B8(void);
extern void *memset(void *destination, int value, unsigned int count);
extern void func_A2F6A8(void);
extern void func_A22668(void);
extern void func_A1D1C0(int level);
extern void func_A2C3A0(int level);
extern int srsFileLoad(void *buffer, const char *name, int mode);
extern int func_A1A760(void);
extern void func_A2C488(int level);
extern void nmlModelEntry(int entry);
extern int sefCheckLoad(void);
extern StudioLight *xglStudioGetLight2(void);
extern void initFileSelect(void);
extern void func_A22628(void);
extern void func_A31840(void);
static void seffectDebugBattle(void);
static void seffectDebugCf(void);
static void seffectDebugDb(void);
extern void sefInitEffectBattle(void);
static void dummyMapDraw(void);
extern void xglStudioGetCamera(SetPathCamera **camera, int index);
extern void SinitCamera(void);
static int debugEnemySelect(void);
extern int srsEftNo2WeaponID(int effect);
extern void sefSetupPlayer(int charaId, int weaponId, int setupParam1, int setupParam2);
extern void sefSetupEnemy(int charaId);
extern void sefHitEffect(void);

static char * cfScript;

static char basepath[23];

static int cur_6;

static SchedulerState *handle_9;

static Vector4 point_10;

static Vector4 angle_11;

/* Initialized storage, in the original .sdata/.data order. */
static char *filelist;

static char *gridlexTop;

static char *gridxtxTop;

static char *hamalexTop;

static char *hamaxtxTop;

static s16 loaded;

static s16 mapdisp;

static s16 casdisp;

static s16 tgtdisp;

s16 _debugPause;

static int cur_7;

static Vector4 pos_14;

static Vector4 caspos_15;

static Vector4 tgtpos_16;

static Vector4 scale_17;

/* Initialized objects recovered for this TU. */
/* PadData's storage export belongs in the defining TU's generated header. */
static int cur_2;

static int menumax_4;

static char * menu_3[5];

static int sftMode;

static int cur_5;

static char basepath[23] = "host0:/home/xeno/work/";

static char * menu_3[5] = { ColliTestBattleLabel, ColliTestBattleDebugLabel, ColliTestCfDebugLabel, ColliTestDebugLabel, ColliTestTitleLabel };

static Vector4 point_10 = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f };

static Vector4 angle_11 = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f };

static Vector4 pos_14 = {0.0f, 0.0f, 0.0f, 1.0f};

static Vector4 caspos_15 = {0.0f, 0.0f, 0.0f, 1.0f};

static Vector4 tgtpos_16 = {0.0f, 0.0f, -4.0f, 1.0f};

static Vector4 scale_17 = {0.1f, 0.1f, 0.1f, 1.0f};

static char path_0[0x80];

static char name_1[0x40];

static SetPathEffectParams prm_8;

static SetPathDummyActor dmyActor_12 __attribute__((aligned(16)));

static SetPathEffectParams prm_13;

static CfSelectState cfs;

/* Initialized objects recovered for this TU. */
static void splitName(char *filename, char *path, char *name)
{
    int len;
    int slash;
    int i;
    int nameLen;

    len = strlen(filename);
    slash = 0;
    for (i = len; i >= 0; i--) {
        if (filename[i] == '/') {
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

void setPath(char *filename)
{
    splitName(filename, path_0, name_1);
    srsSetViewPath(path_0);
}

void initFileSelect(void)
{
    char **directorySlot = &filelist;

    if (*directorySlot != 0)
        return;
    {
        CfSelectState *selector = &cfs;
        int *visibleRowLimit = &selector->visibleRows;
        char *gridxtxStart = (char *) EFFECT_VIEWER_GRIDXTX_ADDR;
        unsigned int gridlexAddress = EFFECT_VIEWER_GRIDLEX_ADDR;
        char **resourceSlot = &gridlexTop;

        selector->verticalOrigin = EFFECT_VIEWER_FILE_TOP;
        selector->phase = 0;
        *resourceSlot = (char *) gridlexAddress;
        resourceSlot = &gridxtxTop;
        *resourceSlot = gridxtxStart;
        resourceSlot = &hamalexTop;
        *resourceSlot = (char *) EFFECT_VIEWER_HAMALEX_ADDR;
        resourceSlot = &hamaxtxTop;
        *resourceSlot = (char *) EFFECT_VIEWER_HAMAXTX_ADDR;
        resourceSlot = &cfScript;
        *resourceSlot = (char *) EFFECT_VIEWER_CF_SCRIPT_ADDR;
        *directorySlot = (char *) EFFECT_VIEWER_FILE_LIST_ADDR;
        selector->horizontalOrigin = EFFECT_VIEWER_FILE_LEFT;
        *visibleRowLimit = EFFECT_VIEWER_FILE_ROWS;
        strcpy(selector->basePath, basepath);
        selector->listBuffer = *directorySlot;
        selector->listSize = EFFECT_VIEWER_FILE_LIST_SIZE;
        selector->printFunc = xglFontDebugPrintf;
    }
}

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

static void sdbExecEffect(void)
{
    PadDataMenuLayout *pad = &PadData[0];
    u16 toggled;
    u16 debugPaused;

    if (pad->buttons.halfwords.pressed & PAD_START) {
        toggled = (_debugPause == 0);
        _debugPause = toggled;
        debugPaused = toggled;
    } else {
        debugPaused = _debugPause;
    }

    if (debugPaused == 0 || (pad->buttons.halfwords.repeat & PAD_SELECT)) {
        sefExecEffect();
    } else {
        xglFontDebugPrintf(0x10, 0x18, pauseText);
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

static int debugEffectConfig(void)
{
    PadDataMenuLayout *pad = &PadData[0];
    char versionText[0x100];
    int lineSpacing = 0x10;
    int pressed;

    while (1) {
        xglRenderClearFrame();
        sprintf(versionText, debugVersionFormat, buildDateText, buildTimeText);
        xglFontDebugPrintf(0x10, 0x10, versionText);
        xglFontDebugPrintf(0x10, cur_2 * lineSpacing + 0x1c, setPathDebugCursorText);
        sdebufPrintMenu(0x10, 0x10, 0x10, menu_3, menumax_4);

        if ((pad->buttons.packed & EFFECT_VIEWER_EXIT_CHORD) == EFFECT_VIEWER_EXIT_CHORD) {
            cur_2 = -1;
            break;
        }

        pressed = pad->buttons.halfwords.pressed;
        if (pressed & PAD_UP) {
            cur_2 = cur_2 - 1;
        }
        if (pressed & PAD_DOWN) {
            cur_2++;
        }
        if (cur_2 < 0) {
            cur_2 = menumax_4 - 1;
        }
        if (cur_2 >= menumax_4) {
            cur_2 = 0;
        }
        if (pressed & PAD_CIRCLE) {
            break;
        }
        xglSleep();
    }

    xglSleep();
    xglRenderClearFrame();
    return cur_2;
}

static int debugEffectSelect(void)
{
    char text[0x100];
    u16 repeat;

    while (1) {
        xglRenderClearFrame();
        sprintf(text, debugDbSelectFormat, cur_5 / 10, cur_5 % 10, cur_5);
        xglFontDebugPrintf(0x10, 0x10, effectSelectionText);
        xglFontDebugPrintf(0x10, 0x20, text);
        if ((PadData[0].buttons.packed & EFFECT_VIEWER_EXIT_CHORD) == EFFECT_VIEWER_EXIT_CHORD) {
            cur_5 = -1;
            break;
        }
        repeat = PadData[0].buttons.halfwords.repeat;
        if (repeat & PAD_L1) cur_5 -= EFFECT_SELECT_STEP_MEDIUM;
        if (repeat & PAD_L2) cur_5 -= EFFECT_SELECT_STEP_COARSE;
        if (repeat & PAD_LEFT) cur_5--;
        if (repeat & PAD_R2) cur_5 += EFFECT_SELECT_STEP_COARSE;
        if (repeat & PAD_R1) cur_5 += EFFECT_SELECT_STEP_MEDIUM;
        if (repeat & PAD_RIGHT) cur_5++;
        if (cur_5 < DB_EFFECT_MIN) cur_5 = DB_EFFECT_MAX;
        if (cur_5 >= DB_EFFECT_MAX + 1) cur_5 = DB_EFFECT_MIN;
        if (PadData[0].buttons.halfwords.pressed & PAD_CIRCLE) break;
        xglSleep();
    }
    xglSleep();
    xglRenderClearFrame();
    return cur_5;
}

static int debugEnemySelect(void)
{
    char text[0x100];
    char *esd;
    int previous;
    u16 repeat;

    while (1) {
        xglRenderClearFrame();
        if ((unsigned int) (cur_6 - 2900) < 99) {
            esd = srsGetEsdData2(cur_6 - 100);
            if (esd != 0 && *esd != 0) {
                sprintf(text, enemyZSelectFormat, esd, cur_6);
            } else {
                sprintf(text, enemySelectFormat, esd, cur_6);
            }
        } else {
            esd = srsGetEsdData2(cur_6);
            sprintf(text, enemySelectFormat, esd, cur_6);
        }
        xglFontDebugPrintf(0x10, 0x10, comboEnemyEffectSelectText);
        xglFontDebugPrintf(0x10, 0x20, text);
        if ((PadData[0].buttons.packed & EFFECT_VIEWER_EXIT_CHORD) == EFFECT_VIEWER_EXIT_CHORD) {
            cur_6 = -1;
            break;
        }
        previous = cur_6;
        repeat = PadData[0].buttons.halfwords.repeat;
        if (repeat & PAD_L1) cur_6 -= EFFECT_SELECT_STEP_MEDIUM;
        if (repeat & PAD_L2) cur_6 -= EFFECT_SELECT_STEP_COARSE;
        if (repeat & PAD_LEFT) cur_6--;
        if (repeat & PAD_R2) cur_6 += EFFECT_SELECT_STEP_COARSE;
        if (repeat & PAD_R1) cur_6 += EFFECT_SELECT_STEP_MEDIUM;
        if (repeat & PAD_RIGHT) cur_6++;
        if (cur_6 <= 0) {
            cur_6 = 2998;
        } else if (cur_6 >= 600 && cur_6 <= 2000) {
            if (cur_6 < previous) {
                cur_6 = 599;
            } else {
                cur_6 = 2001;
            }
        } else if (cur_6 >= 2999) {
            cur_6 = 1;
        }
        if (PadData[0].buttons.halfwords.pressed & PAD_CIRCLE) break;
        xglSleep();
    }
    xglSleep();
    xglRenderClearFrame();
    return cur_6;
}

static int debugCfSelect(void)
{
    char text[0x100];
    u16 repeat;
    char *esd;

    for (;;) {
        esd = srsGetEsdData2(cur_7);
        xglRenderClearFrame();
        sprintf(text, enemySelectFormat, esd, cur_7);
        xglFontDebugPrintf(0x10, 0x10, cfEffectSelectText);
        xglFontDebugPrintf(0x10, 0x20, text);
        if ((PadData[0].buttons.packed & EFFECT_VIEWER_EXIT_CHORD) == EFFECT_VIEWER_EXIT_CHORD) {
            cur_7 = -1;
            break;
        }
        repeat = PadData[0].buttons.halfwords.repeat;
        if (repeat & PAD_L1) cur_7 -= EFFECT_SELECT_STEP_MEDIUM;
        if (repeat & PAD_L2) cur_7 -= EFFECT_SELECT_STEP_COARSE;
        if (repeat & PAD_LEFT) cur_7--;
        if (repeat & PAD_R2) cur_7 += EFFECT_SELECT_STEP_COARSE;
        if (repeat & PAD_R1) cur_7 += EFFECT_SELECT_STEP_MEDIUM;
        if (repeat & PAD_RIGHT) cur_7++;
        if (cur_7 < CF_EFFECT_FIRST) cur_7 = CF_EFFECT_END - 1;
        if (cur_7 >= CF_EFFECT_END) cur_7 = CF_EFFECT_FIRST;
        if (PadData[0].buttons.halfwords.pressed & PAD_CIRCLE) break;
        xglSleep();
    }
    xglSleep();
    xglRenderClearFrame();
    return cur_7;
}

static void SCamTake(void)
{
    float params[44];

    func_A31688();
    memset(params, 0, sizeof(params));
    params[0] = 0.0f;
    params[4] = SCAM_TAKE_DEFAULT_RATE;
    params[5] = SCAM_TAKE_DEFAULT_RANGE;
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

void SinitCamera(void)
{
    SetPathCamera *camera;
    SetPathCamera *view;
    StudioLight *light;
    float focus = 1.4898003f;
    float lookHeight = 9.9308424f;

    xglStudioChange(0);
    xglStudioGetCamera(&camera, 0);
    view = camera;
    view->up.y = 1.0f;
    view->up.z = 0.0f;
    view->position.z = 4.87f;
    view->up.w = 1.0f;
    view->target.x = focus;
    view->target.y = lookHeight;
    view->position.w = 1.0f;
    view->position.x = 0.435f;
    view->position.y = 2.0f;
    view->up.x = 0.0f;
    xglStudioGetLight(&light);
    xglLightSetDefault(light);
}

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

static void seffectDebugDb(void)
{
    int effect;

    SinitCamera();
    effect = debugEffectSelect();
    if (effect < 0) {
        _sdrawFloor = 0;
        sresFreeReloaderMemory(1);
        xglSleep();
        return;
    }

    sefInitEffectBattle();
    effect += DB_EFFECT_BASE;
    dummyMapLoad();
    sefLoadEffect(effect, 1);
    prm_8.actor = 0;
    prm_8.target = 0;
    prm_8.effectId = effect;

    while ((PadData[0].buttons.packed & EFFECT_VIEWER_EXIT_CHORD) != EFFECT_VIEWER_EXIT_CHORD) {
        xglRenderClearFrame();
        if (sefCheckLoad() == 0) {
            xglFontDebugPrintf(8, 8, pressConfirmToStartText);
            if (PadData[0].buttons.halfwords.pressed & PAD_CIRCLE) {
                sefCreateEffect(&prm_8);
            }
            _sdrawFloor = 1;
            if (PadData[0].buttons.halfwords.pressed & PAD_L1) {
                casdisp = (casdisp == 0);
            }
            if (PadData[0].buttons.halfwords.pressed & PAD_L2) {
                tgtdisp = (tgtdisp == 0);
            }
            dummyMapDraw();
            sdbExecEffect();
        } else {
            xglFontDebugPrintf(0x10, 0x10, nowLoadingText);
        }
        xglSleep();
    }

    _sdrawFloor = 0;
    sresFreeReloaderMemory(1);
}

static void seffectDebugCf(void)
{
    int effect;

    SinitCamera();
    effect = debugCfSelect();
    if (effect < 0) {
        xglSleep();
        xglRenderClearFrame();
        return;
    }

    handle_9 = 0;
    memset(&dmyActor_12, 0, sizeof(dmyActor_12));
    Vu0SetOrigin(&dmyActor_12.scale);
    dmyActor_12.scale.x = 1.0f;
    dmyActor_12.scale.w = 1.0f;
    dmyActor_12.scale.z = 1.0f;
    dmyActor_12.scale.y = 1.0f;
    dmyActor_12.rotation.w = 1.0f;
    dmyActor_12.enabled = 1;
    xglSleep();
    xglRenderClearFrame();
    sefInitEffectCf();
    dummyMapLoad();
    sefLoadEffectCf((int)cfScript, effect);

    while ((PadData[0].buttons.packed & EFFECT_VIEWER_EXIT_CHORD) != EFFECT_VIEWER_EXIT_CHORD) {
        xglRenderClearFrame();
        if (sefCheckLoad() == 0) {
            xglFontDebugPrintf(8, 8, pressConfirmToStartText);
            if (PadData[0].buttons.halfwords.pressed & PAD_CIRCLE) {
                handle_9 = sefCreateEffectCf(
                    effect, &point_10, &angle_11);
            }
            if (PadData[0].buttons.halfwords.pressed & PAD_SQUARE) {
                sefKillEffect(effect);
            }
            if (PadData[0].buttons.halfwords.pressed & PAD_R1) {
                mapdisp = (mapdisp == 0);
            }
            if (PadData[0].buttons.halfwords.repeat & PAD_R2) {
                if (handle_9 != 0) {
                    handle_9->display = (handle_9->display == 0);
                }
            }
            if (PadData[0].buttons.halfwords.pressed & PAD_L1) {
                casdisp = (casdisp == 0);
            }
            if (PadData[0].buttons.halfwords.pressed & PAD_L2) {
                tgtdisp = (tgtdisp == 0);
            }
            _sdrawFloor = 1;
            dummyMapDraw();
            sdbExecEffect();
        } else {
            xglFontDebugPrintf(0x10, 0x10, nowLoadingText);
        }
        xglSleep();
    }

    _sdrawFloor = 0;
    sefDestroyEffectCf();
}

static void seffectDebugBattle(void)
{
    int effect;
    struct {
        int kind;
        int variant;
    } eftInfo;
    int weapon;

    eftInfo.kind = 0;
    SinitCamera();
    effect = debugEnemySelect();
    if (effect < 0) {
        _sdrawFloor = 0;
        sefDestroyEffectCf();
        xglSleep();
        return;
    }

    xglSleep();
    xglRenderClearFrame();
    sefInitEffectBattle();
    dummyMapLoad();
    srsAnalyzeEftNo(effect, &eftInfo.kind, &eftInfo.variant);
    if (eftInfo.kind > 0) {
        weapon = 0;
        if ((unsigned int) (effect - EFFECT_WEAPON_BASE) < EFFECT_WEAPON_COUNT) {
            weapon = srsEftNo2WeaponID(effect);
            eftInfo.kind = 17;
        }
        if ((unsigned int) (eftInfo.kind - 1) < PLAYER_CHARA_COUNT) {
            sefSetupPlayer(eftInfo.kind, weapon, 0, 0);
        } else if (eftInfo.kind > PLAYER_CHARA_COUNT) {
            sefSetupEnemy(eftInfo.kind);
        }
        sefSetupEffect();
        while (sefCheckLoad() != 0) {
            xglRenderClearFrame();
            xglFontDebugPrintf(8, 8, setPathLoadingText);
            xglSleep();
        }
    }

    memset(&prm_13, 0, sizeof(prm_13));
    prm_13.actor = 0;
    prm_13.charaId = (u16) eftInfo.kind;
    prm_13.target = 0;
    prm_13.effectId = effect;
    prm_13.source = 0;
    prm_13.variant = 1;
    if ((u16) (effect - 2027) < 3) {
        prm_13.charaId = 2;
    }
    if ((u16) (prm_13.effectId - 2040) < 2) {
        prm_13.charaId = 1;
    }
    sefLoadEffect(effect, prm_13.charaId);

    while ((PadData[0].buttons.packed & EFFECT_VIEWER_EXIT_CHORD) != EFFECT_VIEWER_EXIT_CHORD) {
        if (PadData[0].buttons.halfwords.pressed & PAD_R1) {
            mapdisp = (mapdisp == 0);
        }
        if (PadData[0].buttons.halfwords.pressed & PAD_L1) {
            casdisp = (casdisp == 0);
        }
        if (PadData[0].buttons.halfwords.pressed & PAD_L2) {
            tgtdisp = (tgtdisp == 0);
        }
        if (PadData[0].buttons.halfwords.pressed & PAD_TRIANGLE) {
            SCamTake();
        }
        if (PadData[0].buttons.halfwords.pressed & PAD_SQUARE) {
            func_A31840();
        }
        xglRenderClearFrame();
        _sdrawFloor = 1;
        if (sefCheckLoad() == 0) {
            xglFontDebugPrintf(8, 8, pressConfirmToStartText);
            if (PadData[0].buttons.halfwords.pressed & PAD_CIRCLE) {
                sefCreateEffect(&prm_13);
            }
            if (PadData[0].buttons.halfwords.pressed & PAD_CROSS) {
                sefHitEffect();
            }
            dummyMapDraw();
            sdbExecEffect();
        } else {
            xglFontDebugPrintf(0x10, 0x10, nowLoadingText);
        }
        xglSleep();
    }

    _sdrawFloor = 0;
    sefDestroyEffectCf();
}

static void dummyMapLoad(void)
{
    if (loaded == 0) {
        func_A2C3A0(10);
        func_A1D1C0(10);
        srsFileLoad(gridlexTop, senLexName, 1);
        srsFileLoad(gridxtxTop, senXtxName, 1);
        srsFileLoad(hamalexTop, hamaLexName, 1);
        srsFileLoad(hamaxtxTop, hamaXtxName, 1);
        loaded = 1;
    }
}

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
        nmlModelSetPlace(&sp0);
        nmlModelSetLight(_defMatLcMod, _defMatLn);
        nmlModelEntry((int) gridlexTop);
    }
    if (casdisp != 0) {
        xglMatrixStackUnit();
        xglMatrixStackTrans(&caspos_15.x);
        xglMatrixStackScale(&scale_17.x);
        xglMatrixStackSave(sp0);
        nmlModelSetTexture(hamaxtxTop);
        nmlModelSetPlace(&sp0);
        nmlModelSetLight(_defMatLcMod, _defMatLn);
        nmlModelEntry((int) hamalexTop);
    }
    if (tgtdisp != 0) {
        xglMatrixStackUnit();
        xglMatrixStackTrans(&tgtpos_16.x);
        xglMatrixStackScale(&scale_17.x);
        xglMatrixStackSave(sp0);
        nmlModelSetTexture(hamaxtxTop);
        nmlModelSetPlace(&sp0);
        nmlModelEntry((int) hamalexTop);
    }
}

void SimajiriTest(void)
{
    _debugPause = 0;
    filelist = 0;
    loaded = 0;
    xglRenderClearFrame();
    ClearEnv.color_r = 0;
    ClearEnv.color_g = 4;
    ClearEnv.color_b = 12;
    ClearEnv.color_a = 0;
    srsSetLoadMode(1);

    for (;;) {
        xglSleep();
        xglRenderClearFrame();
        initFileSelect();
        execFileSelect();
        xglSleep();
        xglRenderClearFrame();
        func_A22628();
        sftMode = debugEffectConfig();
        if (sftMode < 0) {
            sefDestroyEffect();
            xglSleep();
            xglRenderClearFrame();
            return;
        }
        xglSleep();
        xglRenderClearFrame();
        if (sftMode == EFFECT_VIEWER_BATTLE) {
            sbattleDebug();
        } else if (sftMode == EFFECT_VIEWER_BATTLE_DEBUG) {
            seffectDebugBattle();
        } else if (sftMode == EFFECT_VIEWER_CF_DEBUG) {
            seffectDebugCf();
        } else if (sftMode == EFFECT_VIEWER_DEBUG) {
            seffectDebugDb();
        }
        func_A31840();
        sefDestroyEffect();
        xglRenderClearFrame();
        if (sftMode == EFFECT_VIEWER_TITLE) return;
    }
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const char debugVersionFormat[16] = "ver: %s %s";

const char buildDateText[16] = "Sep  5 2002";

const char buildTimeText[16] = "12:51:35";

const char debugDbSelectFormat[16] = "db%d%d : %d";

/* JP: エフェクト選択\n
 * EN: Effect selection\n */
const char effectSelectionText[16] = "\245\250\245\325\245\247\245\257\245\310\301\252\302\362\n";

const char enemyZSelectFormat[16] = "%sz : %d";

/* JP: コンボ＆雑魚敵領域テスト用のエフェクト選択
 * EN: Effect selection for combo and regular enemy region tests */
const char comboEnemyEffectSelectText[48] = "\245\263\245\363\245\334\241\365\273\250\265\373\305\250\316\316\260\350\245\306\245\271\245\310\315\321\244\316\245\250\245\325\245\247\245\257\245\310\301\252\302\362";

/* JP: ＣＦ領域テスト用のエフェクト選択
 * EN: Effect selection for CF region tests */
const char cfEffectSelectText[40] =
    "\xA3\xC3\xA3\xC6\xCE\xCE\xB0\xE8\xA5\xC6\xA5\xB9\xA5\xC8\xCD\xD1"
    "\xA4\xCE\xA5\xA8\xA5\xD5\xA5\xA7\xA5\xAF\xA5\xC8\xC1\xAA\xC2\xF2";

/* JP: 決定ボタンを押すとエフェクト起動\n
 * EN: Press the confirm button to start the effect.\n */
const char pressConfirmToStartText[40] = "\267\350\304\352\245\334\245\277\245\363\244\362\262\241\244\271\244\310\245\250\245\325\245\247\245\257\245\310\265\257\306\260\n";

/* JP: Now Loading…\n
 * EN: Now Loading...\n */
const char nowLoadingText[16] = "Now Loading\241\304\n";

/* JP: ロード中\n
 * EN: Loading\n */
const char setPathLoadingText[16] = "\245\355\241\274\245\311\303\346\n";

const char hamaLexName[16] = "hama.lex";

const char hamaXtxName[24] = "hama.xtx";

static char *filelist = 0;

static char *gridlexTop = 0;

static char *gridxtxTop = 0;

static char *hamalexTop = 0;

static char *hamaxtxTop = 0;

static char * cfScript = 0;

static s16 loaded = 0;

static s16 mapdisp = 1;

static s16 casdisp = 0;

static s16 tgtdisp = 0;

static int sftMode = 0;

s16 _sdrawFloor = 0;

s16 _debugPause = 0;

char pauseText[8] = "PAUSE";

static int cur_2 = 0;

static int menumax_4 = 5;

const char setPathDebugCursorText[4] = ">";

static int cur_5 = 1;

static int cur_6 = 1;

char enemySelectFormat[8] = "%s : %d";

static int cur_7 = CF_EFFECT_FIRST;

static SchedulerState *handle_9 = 0;

char senLexName[8] = "sen.lex";

char senXtxName[8] = "sen.xtx";
