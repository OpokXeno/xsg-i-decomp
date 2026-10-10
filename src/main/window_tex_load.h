/*
 * TU-local declarations of main/tu160 (src/main/window_tex_load.c).
 */

#ifndef SRC_MAIN_WINDOW_TEX_LOAD_H
#define SRC_MAIN_WINDOW_TEX_LOAD_H

/*
 * The UMN database's saved state: the 0x9c-byte window of the save record
 * that build/main/aliases.ld names `UmnDataBaseStateData = SaveData + 66140`
 * (0x1025c).  Two bit sets of the same 0x1d monster ids (ids 0x22..0x3e, the
 * `monster_id - 0x22` and `< 0x1d` test both functions of this pair share)
 * are evidenced, each read and written one byte at a time as
 * `bits[id / 8] & (1 << (id % 8))`:
 * - +0x88, whether the monster has been met: written by
 *   UmnDataBaseMonsterSet (main:0x002761f0, `lbu/sb 0x0($5)` with
 *   $5 = base + id/8 + 0x88) and read by UmnDataBaseMonsterCheck
 *   (main:0x00276248, `lbu $2,0x88($3)`).
 * - +0x98, whether it has been scanned: the same pair of functions one
 *   displacement further on, UmnDataBaseAnalisisSet (main:0x00276298,
 *   `addiu $5,$3,0x98`) and UmnDataBaseAnalisisCheck (main:0x002762f0,
 *   `lbu $2,0x98($3)`), which this candidate recovers as C.  It is what the
 *   0x9c extent of the published UmnDataBaseStateData declaration ends on.
 * The spans around them stay unmodeled byte ranges with no meaning claimed
 * (docs/naming.md), and this is what was searched to leave them so. Every
 * access through this window's base in main and the overlays was enumerated:
 * the head (+0x00, +0x02, +0x03, +0x10..+0x12, +0x2a, +0x31, +0x56, +0x59,
 * +0x84, +0x86) belongs to the UMN mail and plugin screens (UmnMail,
 * UmnTopMenu, UmnMailBoxSet, UmnMailFolderSet, UmnMailAttachSet, UmnPlugin,
 * Java_xeno_util_Runtime_mailFlag), which this allocation does not recover,
 * and +0x8c..+0x97 is touched by nothing at all. The nearby +0x10254,
 * +0x102dc, +0x102e2 and +0x10304 accesses (MenuSaveDataGet, MenuBoxMoneyGet,
 * tyaUmlDispLoad, UmnMailDataGet) address the save record directly rather
 * than this alias, so they witness neighbours of the window, not members.
 */
typedef struct UmnDataBase {
    unsigned char _unmodeled_00[0x88];      /* +0x00..+0x87 */
    unsigned char monster_discovered[4];    /* +0x88: one bit per monster id */
    unsigned char _unmodeled_8c[0x0c];      /* +0x8c..+0x97 */
    unsigned char monster_analysed[4];      /* +0x98: one bit per monster id */
} UmnDataBase;

/*
 * The fourth argument of xglCdReadFile is the completion-callback slot that
 * xglCdReadFilePart stores at +0x0c of the read request (main:0x0021DFAC).
 * It selects a default callback for the small values it tests first - 0 takes
 * the global default, 1 takes xglCdDummyCallback (0x0021D698) and 2 takes
 * xglCdDefaultCallback (0x0021D690), main:0x0021DEFC..0x0021DF50 - and uses
 * any other value as the callback address itself.  The slot is an int, so a
 * caller that supplies its own callback passes its address as one.
 */
void MenuLoadFile(const char *name, void *buffer);

static void menuCallback(int event);

void UmnDataBaseMonsterSet(int monster_id);

int UmnDataBaseMonsterCheck(int monster_id);

extern unsigned char UmnDataBaseStateData[0x9c];

int MenuLoadSync(void);

extern int MenuLoadCount;

void MenuLoadInit(void);

void MenuLoadEnd(void);

/* Defined in main/xgl_cd.c (main:0x0021e330). */
extern void xglCdReadCancel(void);

void MenuLoadCancel(void);

extern unsigned char MenuBibrationAct;

extern unsigned char MenuBibrationCount;

extern unsigned char MenuBibrationPad;

extern unsigned char MenuBibrationSpeed;

/*
 * The vibration request MenuBibrationMain replays: while count is non-zero it
 * decrements it and writes speed to actuator act of pad slot pad in PadData.
 * All four are unsigned bytes (MenuBibrationMain reads them with lbu, and
 * subMenuSystemMain passes speed 0x80).
 */
void MenuBibrationSet(unsigned char pad, unsigned char act, unsigned char speed,
                      unsigned char count);

void MenuBibrationInit(void);

extern short UmnKosmosSpecialBox[4];

void UmnkosmosSpecialInit(void);

/*
 * Save-record offset of the UMN mail table: one 2-byte entry per mail box,
 * addressed directly as SaveData + 0x10304 (the same region src/main/party.h
 * documents as SAVE_UMN_MAIL_DATA).
 */
#define SAVE_UMN_MAIL_DATA 0x10304

/* The saved mailbox table begins at the UMN database-state window. */
#define SAVE_UMN_MAIL_BOXES 0x1025c

unsigned char *UmnMailDataGet(int box_id);

int MenuModelInit(int work_start);

#include "xgl_render.h"
#include "main/xgl_studio.h"

/* Controller records are 0x68 bytes. The menu reads the trigger and repeat
 * halfwords at +0x32/+0x34 and writes the actuator bytes at +0x50. */
typedef struct XglPadRecord {
    unsigned char unmodeled_00[0x32];
    unsigned short trigger;
    unsigned short repeat;
    unsigned char unmodeled_36[0x12];
    signed char repeat_delay;
    signed char repeat_interval;
    unsigned short repeat_mask;
    signed char repeat_wait;
    signed char repeat_count;
    unsigned char horizontal_dead_zone;
    unsigned char vertical_dead_zone;
    unsigned char actuator[6];
    unsigned char actuator_pending;
    unsigned char unmodeled_57;
    unsigned char button_map[8];
    unsigned char axis_dead_zone[4];
    signed char axis[4];
} XglPadRecord;
extern XglPadRecord PadData[2];

/* Only the menu's flags word is accessed here. The defining game TU models
 * the complete GameLoopState object. */
typedef struct GameLoopStateLayout {
    unsigned char unmodeled_00[0x10];
    int flags;
} GameLoopStateLayout;
extern GameLoopStateLayout GameLoopState;

extern XglRenderFadeCallback MenuCfTaiki[8];
extern const char *file_name_0[3];
extern void endPrintInit(void);
extern unsigned char *MenuWinAddr;
extern int xglFontLoad(int font, void (*callback)(int));
extern int original_font_no_1;
extern void xglSoundEffectNormalID(int sound, int channel);

typedef struct {
    u64 initial_word;
    u64 palette_packet;
    u64 color_packet;
    u64 trailing_word;
    u32 colors[512];
    int camera_id;
    u32 unmodeled_824;
    u32 tasks_finished;
    unsigned char initialization_values[4];
    float rotation_x;
    u32 reset_value;
    float rotation_z;
    float camera_position_z;
    u64 saved_camera_words[0xbe];
} MenuBgTaskParameters;
extern MenuBgTaskParameters *MenuBgParam;
extern int MenuBgCameraId;
extern u64 *MenuBgKeepCamera;
extern void *xglTaskInitial(void *pool, int capacity, int flags);
extern StudioCamera *xglStudioGetActiveCamera(void);
extern void xglCameraInit(StudioCamera *camera);
extern const float D_004D7D58;
extern const float D_004D7D5C;
extern const float D_004D7D60;
extern void tyaMenuBgEntry(XglTaskScheduler *scheduler,
                           MenuBgTaskParameters *work);

typedef struct {
    char path[26];
} MenuModelPath;
extern const MenuModelPath D_004C3220;
extern unsigned char *MenuTairetuModelXtxAddr;
extern unsigned char *MenuTairetuModelLexAddr;
extern unsigned char *MenuTairetuModelPointerXtxAddr;
extern unsigned char *MenuTairetuModelPointerLexAddr;

extern int mini_game_no;
extern int MenuDrillCall;
extern XglTaskScheduler *MenuTask;
extern int MenuModelWorkTop;
extern int UmnSimulationNo;
extern int MenuScenarioNo;
extern void *UmnGunoDataBaseTop;
extern XglTaskScheduler *MenuTask_XMX;
extern unsigned char *MainMenuWorkEnd;
extern int MenuModelOut[4];
extern void endPrintDirectFrameCopy(XglPacket *, int, int);
extern void MenuGameDataPush(void);
extern void MenuGameDataPop(void);
extern void GameCFSoundMenuPurge(int);
extern void xglStudioChange(int);
extern void *xglStudioGetLight2(void);
extern void xglLightIntensityAmbient(void *, void *);
extern void xglLightIntensityParallel(void *, int, void *);
extern void xglLightDirection(void *, int, void *);
extern void *GameResourceWorkAlloc(int);
extern void MenuModelMemoryInit(void *, int);
extern void MenuModelMemorySet(int);
extern unsigned char *MenuBgTaskInit(unsigned char *, int);
extern unsigned char *MenuBackModelSet(unsigned char *);
extern unsigned char *MenuMapExTextLoad(unsigned char *, int);
extern unsigned char *MenuTairetuLoad(unsigned char *, int);
extern void MenuBgTaskBreak(void);
extern int MenuScenarioNoGet(void);
extern int MenuEquipStealMaskCheck(void);
extern void GameSnapShotCheck(void);
extern void TopMenu(void);
extern int MenuItem(void);
extern void MenuEther(void);
extern void MenuCharactor(int);
extern void MenuTec(void);
extern void MenuSkill(void);
extern void MenuAgws(void);
extern void UmnInterface(void);
extern void MenuSystem(void);
extern void endPrintExtFunc(int, int, int);
extern void nmlModelFlush(void);
extern void MenuModelMain(void);
extern void xglFontDebugPrintf(int, int, const char *, ...);
extern void xglFontDebugHex(int, int, int, int);
extern void MenuModelAllBreak(void);
extern void xglPadSetRepeat(int, unsigned short, int, int);
extern void xglCullingIgnoreOff(void);
extern void nmlModelSendMenuEnd(void);
extern void GameStateRestoreCameraLight(void);
extern void Game_Data_Push(void);
extern void Game_Data_Pop(void);
extern void ACT_init(void);
extern void xglCdLoadOverlay(int);
extern void func_00A01EA8(void);
extern void func_00A00338(void);
extern void func_00A00070(void);
extern void xglRenderClearDepth(void);
extern void nmlModelInit(void);
extern void GameResourceWorkReload(void);
extern void GameCfPlayerLoadResource(int);
extern void func_A19750(int category, int item);

typedef struct {
    short menu_page[11];
} UmnTextPageMap;
typedef struct {
    unsigned short unit_type;
    unsigned char unmodeled_02[0x16];
    unsigned short text_type;
} UmnUnitInitRecord;
typedef struct {
    int unit_id;
    unsigned short value;
    unsigned char property_a;
    unsigned char property_b;
    unsigned char property_c;
    unsigned char unmodeled_09[4];
    unsigned char name_page;
    unsigned char description_page;
    unsigned char unmodeled_0f;
    unsigned char name_id;
    unsigned char description_id;
} UmnUnitTextRecord;
typedef struct {
    const char *text;
} UmnTextLookup;
typedef struct {
    char unit_name[0x20];
    short unit_type;
    short duplicate_unit_type;
    short text_type;
    unsigned char unmodeled_26[2];
    unsigned int property_a;
    unsigned int property_b;
    unsigned int property_c;
    int unit_id;
    unsigned long long value;
    char name[0x12];
    char description[0x16];
} UmnGunoRecord;
extern UmnTextPageMap D_004C32F8;
extern unsigned char D_004DA810[];
extern UmnUnitInitRecord *func_A19270(int unit_id);
extern UmnUnitTextRecord *func_A1A698(int unit_id);
extern const char *const *func_A2C828(int unit_id);
extern UmnTextLookup *MenuTextGet(int text_id);
extern char *strcpy(char *destination, const char *source);
extern void UmnMain(void);

#endif /* SRC_MAIN_WINDOW_TEX_LOAD_H */
