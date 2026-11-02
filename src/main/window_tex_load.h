/*
 * TU-local declarations of main/tu160 (src/main/window_tex_load.c).
 */

#ifndef SRC_MAIN_WINDOW_TEX_LOAD_H
#define SRC_MAIN_WINDOW_TEX_LOAD_H

#include "xgl_render.h"
#include "main/xgl_studio.h"
#include "main/xgl_pad.h"
#include "main/menu_1.h"
#include "main/end_print.h"
#include "main/game.h"
#include "main/game_snap_shot.h"
#include "main/look_at_player.h"
#include "main/menu_model.h"
#include "main/nml_model_set.h"
#include "main/res_get_path.h"
#include "main/window_tex_load.h"
#include "main/xgl_camera.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"

/* SAVE_UMN_MAIL_DATA (main/party.h) addresses the UMN mail table. */
/* The saved mailbox table begins at the UMN database-state window. */
#define SAVE_UMN_MAIL_BOXES 0x1025c

#define MENU_BG_CAMERA_ROTATION_X (-0.42479878664016724f)
#define MENU_BG_CAMERA_ROTATION_Z (2.784193754196167f)
#define MENU_BG_CAMERA_POSITION_Z (7.0559511184692383f)

/* Only the menu's flags word is accessed here. The defining game TU models
 * the complete GameLoopState object. */
typedef struct GameLoopStateLayout {
    unsigned char unmodeled_00[0x10];
    int flags;
} GameLoopStateLayout;

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
    StudioCamera saved_camera;
} MenuBgTaskParameters;

typedef struct {
    char path[26];
} MenuModelPath;

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

extern UmnDataBase UmnDataBaseStateData[1];
extern int MenuLoadCount;
extern unsigned char MenuBibrationAct;
extern unsigned char MenuBibrationCount;
extern unsigned char MenuBibrationPad;
extern unsigned char MenuBibrationSpeed;
extern short UmnKosmosSpecialBox[4];
extern XglPadRecord PadData[2];
extern GameLoopStateLayout GameLoopState;
extern XglRenderFadeCallback MenuCfTaiki[8];
extern const char *file_name_0[3];
extern unsigned char *MenuWinAddr;
extern int original_font_no_1;
extern MenuBgTaskParameters *MenuBgParam;
extern int MenuBgCameraId;
extern StudioCamera *MenuBgKeepCamera;
extern const MenuModelPath tairetuTexturePath;
extern unsigned char *MenuTairetuModelXtxAddr;
extern unsigned char *MenuTairetuModelLexAddr;
extern unsigned char *MenuTairetuModelPointerXtxAddr;
extern unsigned char *MenuTairetuModelPointerLexAddr;
extern int mini_game_no;
extern int MenuDrillCall;
extern XglTaskScheduler *MenuTask;
extern unsigned char *MenuModelWorkTop;
extern int UmnSimulationNo;
extern int MenuScenarioNo;
extern UmnGunoRecord *UmnGunoDataBaseTop;
extern XglTaskScheduler *MenuTask_XMX;
extern unsigned char *MainMenuWorkEnd;
extern void *MenuModelOut[4];
extern const UmnTextPageMap menuTextPageTable;
extern unsigned char defaultCharacterBuffer[];

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
void MenuLoadEnd(void);

/* Defined in main/xgl_cd.c (main:0x0021e330). */
extern void xglCdReadCancel(void);

/*
 * The vibration request MenuBibrationMain replays: while count is non-zero it
 * decrements it and writes speed to actuator act of pad slot pad in PadData.
 * All four are unsigned bytes (MenuBibrationMain reads them with lbu, and
 * subMenuSystemMain passes speed 0x80).
 */
void MenuBibrationSet(unsigned char pad, unsigned char act, unsigned char speed,
                      unsigned char count);

void MenuBibrationInit(void);
void UmnkosmosSpecialInit(void);
extern int xglFontLoad(int font, void (*callback)(int));
extern void *xglTaskInitial(void *pool, int capacity, int flags);
extern StudioCamera *xglStudioGetActiveCamera(void);
extern void tyaMenuBgEntry(XglTaskScheduler *scheduler,
                           MenuBgTaskParameters *work);

extern void GameCFSoundMenuPurge(int);
extern void *xglStudioGetLight2(void);
extern void xglLightIntensityAmbient(void *, void *);
extern void xglLightIntensityParallel(void *, int, void *);
extern void xglLightDirection(void *, int, void *);
extern unsigned char *MenuBackModelSet(unsigned char *);
extern unsigned char *MenuMapExTextLoad(unsigned char *, int);
extern unsigned char *MenuTairetuLoad(unsigned char *, int);
extern void TopMenu(void);
extern int MenuItem(void);
extern void MenuEther(void);
extern void MenuCharactor(int);
extern void MenuTec(void);
extern void MenuSkill(void);
extern void MenuAgws(void);
extern void UmnInterface(void);
extern void MenuSystem(void);
extern void xglFontDebugHex(int, int, int, int);
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
extern void func_A19750(int category, int item);
extern UmnUnitInitRecord *func_A19270(int unit_id);
extern UmnUnitTextRecord *func_A1A698(int unit_id);
extern const char *const *func_A2C828(int unit_id);
extern char *strcpy(char *destination, const char *source);
extern void UmnMain(void);

#endif /* SRC_MAIN_WINDOW_TEX_LOAD_H */
