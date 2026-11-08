#include "common.h"
#include "main/act_1.h"
#include "main/open_close_main.h"
#include "main/e_number_main.h"
#include "shared.h"
#include "main/e_battle_win_open.h"
#include "endou_test.h"
#include "main/control_entry.h"
#include "main/xgl_cd.h"
#include "main/xgl_font.h"
#include "main/end_print.h"
#include "main/e_message.h"
#include "main/act_2.h"
#include "main/data.h"
#include "main/file.h"
#include "main/game_snap_shot.h"
#include "main/menu_model.h"
#include "main/menu_shop.h"
#include "main/nml_model_set.h"
#include "main/party.h"
#include "main/res_get_path.h"
#include "main/window_tex_load.h"
#include "main/xgl_packet.h"
#include "main/xgl_studio.h"
#include "main/xgl_thread.h"

#define ENDOU_CLEAR_ENV (&ClearEnv)
#define NULL ((void *)0)
#define ENDOU_MODEL_PAD ((EndouModelPadState *)&PadData)

/* Held SELECT in the low half and pressed START in the high half of the pad
 * snapshot (PadData +0x28) leave a test screen. The three spellings are the
 * signed, unsigned and 64-bit forms of the same value. */
#define ENDOU_EXIT_CHORD 0x08000100
#define ENDOU_EXIT_CHORD_U 0x08000100U
#define ENDOU_EXIT_CHORD_ULL 0x08000100ULL

/* Analog stick axes read as signed bytes: pushed past this is a deflection. */
#define ENDOU_STICK_DEADZONE 0x40

/* eLineMain states: draw the segments one after another or all at once. */
enum {
    ELINE_STATE_DRAW_SEQUENTIAL = 0x10,
    ELINE_STATE_DRAW_ALL = 0x20
};

/* ShopTest: shopdata.bin holds 20 shop records of 0xb8 halfwords, laid out as
 * nine columns of 20 slots plus four trailing values. */
#define ENDOU_SHOP_COUNT 20
#define ENDOU_SHOP_RECORD_WORDS 0xb8
#define ENDOU_SHOP_SLOT_COUNT 20
#define ENDOU_SHOP_TEST_MONEY_SMALL 100
#define ENDOU_SHOP_TEST_MONEY_LARGE 1000000

/* Party debug menu: the pages of PauseMenuPagePartyDebug, and the party id
 * ranges the pages walk (friends 1..7, twelve characters per toggle page,
 * AGWS ids from 0x11). */
enum {
    PARTY_DEBUG_PAGE_FRIEND,
    PARTY_DEBUG_PAGE_ATTACKER,
    PARTY_DEBUG_PAGE_OUT_FRIEND,
    PARTY_DEBUG_PAGE_LOCK_PARTY,
    PARTY_DEBUG_PAGE_TAKE_AGWS
};
#define PARTY_DEBUG_FRIEND_COUNT 7
#define PARTY_DEBUG_ROSTER_COUNT 12
#define PARTY_DEBUG_AGWS_FIRST_ID 0x11
#define PARTY_DEBUG_ID_MAX 0x1d
#define PARTY_DEBUG_ATTACK_SLOT_COUNT 3
#define PARTY_DEBUG_ATTACK_POSITION_COUNT 10
#define PARTY_DEBUG_ATTACK_POSITION_MAX (PARTY_DEBUG_ATTACK_POSITION_COUNT - 1)
#define PARTY_DEBUG_ATTACKER_ROW_LAST 5

/* Battle window test: 19 commands and a terminator. */
#define ENDOU_BATTLE_COMMAND_COUNT 20

/* Message test: four model-test strings cycled every 0x40 frames; five
 * message-test strings (the last is a terminator). */
#define ENDOU_MODEL_MESSAGE_COUNT 4
#define ENDOU_MODEL_MESSAGE_FRAMES 0x40
#define ENDOU_MESSAGE_TEST_COUNT 5

/* ModelTest floor grid: ENDOU_GRID_ROWS x ENDOU_GRID_COLUMNS projected points. */
#define ENDOU_GRID_ROWS 5
#define ENDOU_GRID_COLUMNS 6
#define ENDOU_GRID_POINT_COUNT (ENDOU_GRID_ROWS * ENDOU_GRID_COLUMNS)

/* eModelTest: menu model pool and its separate memory block. */
#define ENDOU_MENU_MODEL_POOL 0x01000000
#define ENDOU_MENU_MODEL_MEMORY_BASE 0x01100000
#define ENDOU_MENU_MODEL_MEMORY_SIZE 0x00500000

/* Work buffers of the file, JPEG and UML tests. */
#define ENDOU_FILE_LOAD_BUFFER_SIZE 0x100000
#define ENDOU_JPEG_BUFFER_SIZE 0x80000
#define ENDOU_UML_BUFFER_SIZE 0x20000
#define ENDOU_UML_CLEAR_SIZE 0x10000

/* Texture (.xtx) tests: 0x30-byte file header, then the image upload. */
#define ENDOU_XTX_HEADER_SIZE 0x30
#define ENDOU_XTX_PAYLOAD_QWORDS 0x4002

/* Quadword counts of the GIF packets the tests reference. */
#define ENDOU_BG_ENV_QWORDS 10
#define ENDOU_UPLOAD_ENV_QWORDS 6
#define ENDOU_TEX_DRAW_ENV_QWORDS 12
#define ENDOU_FILE_DRAW_ENV_QWORDS 10
#define ENDOU_WINTEX_ENV_QWORDS 7
#define ENDOU_RELOAD_ENV_QWORDS 8

/* VRAM byte offsets where the .xtx test images are uploaded (shifted right by
 * 6 into the GS word address). */
#define ENDOU_TEX_VRAM_BASE 0xe0000U
#define ENDOU_FILE_VRAM_BASE 0xf0000U

/* GS register field positions of the packets the tests build. */
#define GS_TRXREG_RRH_SHIFT 32


typedef struct EndouBattleWindowParams {
    int x;
    int y;
    int style;
    int information_x;
    int information_y;
    struct BattleWinCommand *commands;
    const char *title;
    int repeat;
    int selection;
} EndouBattleWindowParams;

typedef struct EndouBattleMessageParams {
    int x;
    int y;
    int style;
    const char *title;
    const char *text;
} EndouBattleMessageParams;

typedef struct EndouBattleAnalysisParams {
    int x;
    int y;
    int style;
    const char *name;
    short hp;
    short max_hp;
    unsigned short conditions;
    u8 unmodeled_16[2];
    int technique_points;
    int ether_points;
    int skill_points;
    int experience;
    long money;
    unsigned short common_drop;
    unsigned short rare_drop;
} EndouBattleAnalysisParams;

typedef struct EndouBattleRewardParams {
    int x;
    int y;
    int style;
    int width;
    const char *text;
    const char *title;
} EndouBattleRewardParams;

/* ld at +0x28 and lhu/sh at +0x28/+0x2a access the same button storage. */
typedef struct PadDataButtonView {
    u8 unmodeled_00[0x28];
    union {
        u64 button_state;
        struct {
            u16 held;
            u16 pressed;
            u16 repeat;
            u16 released;
        } input;
    } buttons;
} PadDataButtonView;

typedef struct EModelTestState {
    unsigned char unmodeled_00[0x11];
    unsigned char isReady;
    unsigned char unmodeled_12[1];
    unsigned char extFuncFlag;
} EModelTestState;

typedef struct GifPackedTag {
    unsigned int loop_count_and_eop;
    unsigned int primitive_flags_and_register_count;
    u64 register_descriptors;
} GifPackedTag;

typedef struct GifAdWrite {
    u64 value;
    u64 register_id;
} GifAdWrite;

typedef struct GifPackedRgbaq {
    unsigned int red;
    unsigned int green;
    unsigned int blue;
    unsigned int alpha;
} GifPackedRgbaq;

typedef struct GifUv {
    unsigned int u;
    unsigned int v;
    unsigned int q;
    unsigned int reserved;
} GifUv;

typedef struct GifXyz2 {
    unsigned int x;
    unsigned int y;
    unsigned int z;
    unsigned int fog;
} GifXyz2;

typedef struct BgTestRenderState {
    unsigned char unmodeled_00[0x14];
    unsigned short packetControl;
} BgTestRenderState;

typedef struct BgTestEnvironment {
    unsigned int vif_nop[3];
    unsigned int vif_directhl_nine_quadwords;
    GifPackedTag tag;
    GifAdWrite texture_setup;
    GifAdWrite packet_write;
    GifAdWrite primitive_setup;
    GifPackedRgbaq color;
    GifUv first_uv;
    GifXyz2 first_vertex;
    GifUv second_uv;
    GifXyz2 second_vertex;
} BgTestEnvironment;

enum {
    BG_TEST_COMMAND_HIGH = 0xc800,
    BG_TEST_COMMAND_BASE = 0x2412,
    BG_TEST_COMMAND_BASE_SHIFT = 16,
    BG_TEST_CONTROL_SHIFT = 5,
    BG_TEST_COMMAND_HIGH_SHIFT = 19
};

typedef struct WinTexEnvironment {
    unsigned int vif_nop_before_flush;
    unsigned int vif_nop_before_direct;
    unsigned int vif_flushe;
    unsigned int vif_directhl_six_quadwords;
    GifPackedTag gif_tag;
    GifAdWrite texture_flush;
    GifAdWrite bitblt_buffer;
    GifAdWrite transfer_position;
    GifAdWrite transfer_region;
    GifAdWrite transfer_direction;
} WinTexEnvironment;

typedef struct EndouClearEnvironment {
    u8 unmodeled_00[0x20];
    u32 color_r;
    u32 color_g;
    u32 color_b;
    u32 color_a;
} EndouClearEnvironment;

typedef struct ShopDataRecord {
    u16 values[0xb8];
} ShopDataRecord;

/* EMessageParam is 0x44 bytes; this window adds a reserved 0xc-byte tail, so each
 * window is 0x50 bytes. The windows arrays and msg_11 use this size. */
typedef struct EMessageTestWindow {
    EMessageParam message;
    u8 unmodeled_44[0xc];
} EMessageTestWindow;

typedef struct UmnEventTextRecord {
    u32 unmodeled_00;
    const char *message_text;
} UmnEventTextRecord;

/* Partial view of TyaUmlDispParam (include/ov02/tya_uml.h): 0x40 bytes here and used
 * as a windows[2] array, so it does not take the owner's 0x30-byte size. */
typedef struct TyaUmlDispParamMailView {
    int mode;
    u8 unmodeled_04[4];
    u16 texture_x;
    u16 texture_y;
    u16 texture_width;
    u16 texture_height;
    u8 unmodeled_10[8];
    s16 scroll_x;
    s16 scroll_y;
    u16 view_width;
    u16 view_height;
    u8 unmodeled_20[0x20];
} TyaUmlDispParamMailView;

typedef struct JpegTestDecodeRequest {
    void *source;
    void *destination;
    u8 unmodeled_08[8];
    u16 source_width;
    u16 source_height;
    u16 max_width;
    u16 max_height;
    u8 unmodeled_18[8];
} JpegTestDecodeRequest;

typedef struct JpegTestDisplay {
    u16 x;
    u16 y;
    u32 color;
    signed char red;
    signed char green;
    signed char blue;
    signed char alpha;
    void *image_data;
} JpegTestDisplay;

typedef struct FileLoadState {
    int state;
    unsigned char *buffer;
    unsigned char unmodeled_08[0x40];
} FileLoadState;

typedef int s32;

typedef s32 M2C_UNK;

typedef struct StudioLight StudioLight;

typedef struct EndouTestLighting {
    DoublewordAlignedVector4 ambient;
    DoublewordAlignedVector4 parallel_intensity[3];
    Vector4 direction[3];
} EndouTestLighting;

typedef struct EndouModelLineVertex {
    int x;
    int y;
    int z;
    int terminator;
} EndouModelLineVertex;

typedef struct EndouModelLinePoint {
    EndouModelLineVertex vertex;
    unsigned char color[4];
    unsigned char unmodeled_14[12];
} EndouModelLinePoint;

typedef struct EndouOrientationStorage {
    AlignedVector4 rotation;
    AlignedVector4 scale;
} EndouOrientationStorage;

typedef struct EndouModelBackQuad {
    int vertices[4][4];
    float uv[4];
} EndouModelBackQuad;

typedef struct EndouPrintLinePoint {
    int x;
    int y;
    int z;
    u32 unmodeled_0c;
    u8 color[4];
    u8 unmodeled_14[12];
} EndouPrintLinePoint;

typedef struct EndouELinePoint {
    int x;
    int y;
    int work;
    signed char red;
    signed char green;
    signed char blue;
    signed char alpha;
} EndouELinePoint;

typedef struct EndouELineEntry {
    u8 state;
    u8 unmodeled_001[2];
    u8 duration;
    u8 progress[5];
    u8 unmodeled_009[3];
    EndouELinePoint points[5];
    u32 count;
    EndouELinePoint render[5];
} EndouELineEntry;

typedef struct EndouSpriteQuadVertex {
    int x;
    int y;
    int depth;
    unsigned char unmodeled_00c[4];
} EndouSpriteQuadVertex;

typedef struct EndouSpriteQuad {
    unsigned char flags;
    signed char coordinate_mode;
    unsigned short uv;
    unsigned char unmodeled_004[12];
    EndouSpriteQuadVertex vertices[4];
    unsigned char unmodeled_050[16];
} EndouSpriteQuad;

typedef struct EndouPrintPoint {
    short x;
    unsigned short y;
    int depth;
    unsigned char color[4];
} EndouPrintPoint;

typedef struct EndouQuadColor {
    signed char red;
    signed char green;
    signed char blue;
    signed char alpha;
} EndouQuadColor;

typedef struct EndouQuadPosition {
    short x;
    short y;
    int depth;
} EndouQuadPosition;

typedef struct EndouQuadDescriptor {
    EndouQuadPosition points[4];
    EndouQuadColor colors[4];
    unsigned int format;
} EndouQuadDescriptor;

typedef union EndouTagFontViews {
    ETagFontMainData control;
    ETagFontMainData draw;
} EndouTagFontViews;

typedef struct EndouModelPadState {
    unsigned char unmodeled_00[0x28];
    unsigned short held;
    unsigned short pressed;
    unsigned char unmodeled_2c[4];
    unsigned char unmodeled_30[8];
    signed char stick_axis[4];
} EndouModelPadState;

enum FileLoadDrawState {
    FILE_DRAW_UNINITIALIZED = -1,
    FILE_DRAW_ENABLED = 1
};

struct FileLoadInfo {
    enum FileLoadDrawState status;
    unsigned char *buffer;
    char filename[64];
};

struct FileLoadDrawEnvironment {
    unsigned char unmodeled_00[0x30];
    unsigned long long texture_parameters;
};

typedef struct TexturTestHeader {
    u8 unmodeled_00[8];
    int entry_count;
    u32 entries_offset;
} TexturTestHeader;

typedef struct TexturTestEntry {
    u32 width_and_pitch;
    u32 height;
    u32 vram_offset;
    int data_quadwords;
    u32 data_offset;
} TexturTestEntry;

typedef struct TexturTestUploadEnvironment {
    u8 unmodeled_00[0x20];
    u64 destination_parameters;
    u8 unmodeled_28[0x18];
    u64 transfer_dimensions;
    u8 unmodeled_48[0x18];
} TexturTestUploadEnvironment;

typedef struct TexturTestDrawEnvironment {
    u8 unmodeled_00[0x50];
    u64 texture_parameters;
    u8 unmodeled_58[0x68];
} TexturTestDrawEnvironment;

extern const char attackerHeadingLabel[];
extern const char attackerControlsLabel[];
static const char * text02_85[] ;
extern const char partyDebugTitleLabel[];
extern const char partyDebugToggleLabel[];
static const char * tag_16[] ;
static const char * msg_19[] ;
extern const char battleWindowTestLabel[];
extern const char condorPasaName[];
extern const char battleWindowTitleText[];
extern const char padRepeatFormat[];
extern PadDataButtonView PadData;

extern BgTestRenderState sRender;
extern EndouClearEnvironment ClearEnv;
extern const char settlementScreenTestLabel[];
extern const char textTestSquareLabel[];
extern const char messageTestCrossLabel[];
extern const char miscTestCircleLabel[];
extern const char modelTestL1Label[];
extern const char loadTestL2Label[];
extern const char textureTestR1Label[];
extern const char jpegTestR2Label[];
extern const char battleWindowTestUpLabel[];
extern const char saveTestDownLabel[];
extern const char modelTestRightLabel[];
extern const char shopTestLeftLabel[];
extern const char GameEndouTestTitleText[];
extern const char eMessageTestLabel[];
extern const char reLoadTestFloorPath[];
extern const char reLoadTestLabel[];
extern const char kanjiTestText[];
extern const char textTestLabel[];
extern const char umnMailDispTestLabel[];
extern const char jpegTestLabel[];
extern const char takeAgwsHeadingLabel[];
extern const char takeAgwsControlsLabel[];
extern const char lockPartyHeadingLabel[];
extern const char toggleControlsLabel[];
extern const char outFriendHeadingLabel[];
extern const char friendHeadingLabel[];
extern const char friendControlsLabel[];
extern const char menuCursorText[];
static const char * text00_75[] ;
static const char * text00_77[] ;
static const char * text00_79[] ;
static const char * text00_81[] ;
extern const char shopDataPath[];
extern const char shopTestLabel[];
extern const char shopNumberChangeLabel[];
extern const char shopRunQuitLabel[];
extern const char shopNoFormat[];
extern char shopHeadingIText[];
extern const char shopHeadingCwText[];
extern const char shopHeadingCaText[];
extern const char shopHeadingCbText[];
extern const char shopHeadingAwText[];
extern const char shopHeadingAaText[];
extern const char shopHeadingAbText[];
extern const char shopHeadingAfText[];
extern const char shopHeadingAeText[];
extern char shopHeadingAText[];
extern const char shopRowNumberFormat[];
extern const char fileLoadTestMenuPath[];
extern const char fileLoadTestLabel[];
extern void (*xglCdSetCallback(void (*callback)(int, int)))(int, int);
extern const EndouTestLighting modelTestLighting;
extern const char modelTestFpkName[152];
extern const char modelTestMotionName[];
extern const DoublewordAlignedVector4 modelTestQuadWordTable[4];
extern const char modelTestLexExtension[8];
extern const char modelTestXtxExtension[8];
extern const char modelTestJntExtension[];
extern unsigned char modelTestColumnAlphaTable[];
extern unsigned char modelTestRowAlphaTable[];
extern const char eNumberTestLabel[];
extern const char umlTestFilePath[];
extern const char umlTestLabel[];
extern const char umlTestColorControl[];
extern const char textureTestNewdMenuPath[];
extern const char textureTestDataMenuPath[];
extern const char textureTestLabel[];
extern const char modelTestHiraganaKatakanaText[16];
extern const char modelTestKatakanaHiraganaText[16];
extern const char modelTestHiraganaText[16];
extern const char modelTestKatakanaText[16];
extern const char gradeOneTagText[8];
extern const char temptationMessageText[16];
extern const char slowMessageText[40];
extern const char yesText[8];
extern const char endouTestNoText[8];
extern const char offText[8];
extern const char onText[8];
extern const char absentText[8];
extern const char presentText[8];
extern const char a1Text[8];
extern const char p4Text[8];
extern const char a2Text[8];
extern const char p6Text[8];
extern const char nullText[16];
extern const char p1Text[8];
extern const char p3Text[8];
extern const char p5Text[8];
extern const char p2Text[8];
extern const char a3Text[8];
extern const char number3Text[8];
extern const char number2Text[8];
extern const char number1Text[8];
extern const char characterText[8];
extern const char positionText[16];
extern const char grandPrixText[16];
extern const char arimaKinenText[16];
extern const char twoYearOldFilliesChampionText[24];
extern const char hanshinJuvenileFilliesText[16];
extern const char olderHorsesRoyalRoadText[24];
extern const char japanCupText[16];
extern const char autumnMileChampionshipText[24];
extern const char mileChampionshipText[16];
extern const char filliesMaresChampionshipText[16];
extern const char queenElizabethIiCupText[24];
extern const char olderHorsesCursedTennoShoText[24];
extern const char tennoShoAutumnText[16];
extern const char coltsClassicsThirdLegText[24];
extern const char filliesClassicsThirdLegText[24];
extern const char autumnSprintChampionshipText[24];
extern const char sprintersStakesText[24];
extern const char springGrandPrixText[16];
extern const char takarazukaKinenText[16];
extern const char springMileChampionshipText[24];
extern const char yasudaKinenText[16];
extern const char coltsClassicsSecondLegText[32];
extern const char japaneseDerbyText[16];
extern const char filliesClassicsSecondLegText[32];
extern const char oaksText[16];
extern const char foreignBredMilersChampionshipText[32];
extern const char nhkMileCupText[16];
extern const char springLongDistanceChampionshipText[24];
extern const char tennoShoSpringText[16];
extern const char coltsClassicsFirstLegText[24];
extern const char filliesClassicsFirstLegText[24];
extern const char springSprintChampionshipText[24];
extern const char takamatsunomiyaKinenText[16];
extern const char dirtChampionshipText[16];
extern const char februaryStakesText[16];
extern const char horseNameListText[64];
extern const char vowelKanaListText[24];
extern const char winningRaceListText[144];
extern const char deviceListText[144];
extern const char shukaShoText[8];
extern const char satsukiShoText[8];
extern const char okaShoText[8];
extern const char kikukaShoText[8];

extern void xglCdLoadOverlay(int overlayId);
extern void SeisanInit(void);
extern int SeisanMain(void);
extern void SeisanDisp(void);
extern void xglRenderClearFrame(void);
extern void MenuModelUnitOpen(void *unit, int drawType);
extern void MenuModelControl(void *unit);
extern void ACT_init(void);
extern void MenuModelCreate(void **model, int character_id);

extern void MenuModelExtFuncSet(void *model,
                                void (*callback)(EModelTestState *),
                                int argument);

extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);
extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);
extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);
void SeisanTest(void);
void eMessageTest(void);
void eNumberTest(void);
void eModelTest(void);
void TexturTest(void);
void JpegTest(void);
void BattleWindowTest(void);
void ModelTest(void);
void TextTest(void);
void ShopTest(void);
extern int WindowTexLoad(unsigned char *buffer, unsigned int request);
extern void xglFontDebugHex(int x, int y, int value, int digits);


extern u8 *UmnEventTextInit(u8 *work_end);
extern int UmnEventTextMake(int mode);
extern UmnEventTextRecord *UmnEventTextNextGet(int index);
extern void tyaUmlDispInit(void);
extern void tyaUmlDispParamReset(TyaUmlDispParamMailView *params, int mode);
extern void tyaUmlDispMain(TyaUmlDispParamMailView *params);
extern const char *MenuCharNameGet(int characterId);
extern void func_A22628(void);
void GameResourceInit(unsigned int address, int size);
char *strcpy(char *destination, const char *source);

void xglRotTransPersN(void *destination, Matrix4 *matrix,
                      const void *points, int count, int cameraId);

StudioCamera *xglStudioGetActiveCamera(void);

void xglLightDirection(void *light, unsigned int index,
                       const Vector4 *direction);

void xglLightIntensityAmbient(void *light, const void *intensity);

void xglLightIntensityParallel(void *light, unsigned int index,
                               const void *intensity);

void eLineSet(EndouELineEntry *line, int unused);
void eLineMain(EndouELineEntry *line);
void endPrintDirectLine(EndouPrintLinePoint *line);
void endSpriteSet(void *sprite, int mode);
void eSpriteMain(ESpriteMainData *sprite);
void eSpriteSet(ESprite *sprite, int flags);
void xglStudioGetCamera(StudioCamera **camera_out, int camera_index);
extern void eBattleWinOpen(EndouBattleWindowParams *params);
extern int eBattleWinMain(EndouBattleWindowParams *params);
extern void eBattleWinOpen2(EndouBattleRewardParams *params);
extern int eBattleWinMain2(void);
extern void eBattleWinOpen3(EndouBattleAnalysisParams *params);
extern int eBattleWinMain3(void);
extern void eBattleWinOpen4(EndouBattleMessageParams *params);
extern int eBattleWinMain4(void);
extern void xglRenderClearDepth(void);
extern void FlushCache(int mode);

/* EUC-JP bytes for the model-test diagnostic with its leading control byte. */
/* JP: モデルテスト
 * EN: Model test */
const char eModelTestLabel[16] =
    "\x0B\xA5\xE2\xA5\xC7\xA5\xEB\xA5\xC6\xA5\xB9\xA5\xC8";

static const char not_found_format[16] = "%s/NotFound";

static const char read_error_format[24] = "%s/ReadError";

static int Select_82;

static int TopMode_86;

static const char * text00_83[PARTY_DEBUG_ATTACK_POSITION_COUNT];

static const char * text01_84[PARTY_DEBUG_ATTACK_SLOT_COUNT];

static BattleWinCommand data_15[ENDOU_BATTLE_COMMAND_COUNT];

static char text_17[171] ;

static char tag_20[17] ;

static int count_18;

static BgTestEnvironment TestEnv_47 = {
    { 0, 0, 0 }, 0x50000009,
    { 0x00008001, 0x808B4000, 0x0000000053531EEEULL },
    { 0x20, 0x14 },
    { 0, 0x6 },
    { 0x00050000, 0x47 },
    { 0x80, 0x80, 0x80, 0x80 },
    { 0, 0, 0, 0 },
    { 0x6FF8, 0x71F8, 0, 0 },
    { 0x2000, 0x1C00, 0, 0 },
    { 0x8FF8, 0x8DF8, 0, 0 },
};

u8 *test_data = 0;

static unsigned int TestEnv_66[32] = {
    0x00008001, 0x208B4000, 0x000000EE, 0x00000000,
    0x00000000, 0x00000000, 0x0000003F, 0x00000000,
    0xDD343840, 0x20070005, 0x00000006, 0x00000000,
    0x00000080, 0x00000080, 0x00000080, 0x00000080,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00007C00, 0x00007BF8, 0x10000000, 0x00000000,
    0x00000800, 0x00000800, 0x00000000, 0x00000000,
    0x00008400, 0x000083F8, 0x10000000, 0x00000000
};

/* VIF DIRECTHL carries a GIF PACKED tag and five GS A+D register writes. */
static WinTexEnvironment WinTexEnv_65 = {
    0, 0, 0x11000000, 0x51000006,
    { 0x00008005, 0x10000000, 0x000000000000000EULL },
    { 0x0000000000000000ULL, 0x000000000000003FULL },
    { 0x0008380000000000ULL, 0x0000000000000050ULL },
    { 0x0000000000000000ULL, 0x0000000000000051ULL },
    { 0x0000008000000200ULL, 0x0000000000000052ULL },
    { 0x0000000000000000ULL, 0x0000000000000053ULL },
};

static const char * msg_45[ENDOU_MESSAGE_TEST_COUNT];

static char text00_46[176];

static int Select_74;

static int Select_76;

static int Select_78;

static int Select_80;

static FileLoadState files;

static u64 eNumberTexEnv_69[12];

static u64 TestEnv_70[20];

static int x_71;

static float ang_0[4];

static const char * msg00_14[ENDOU_MODEL_MESSAGE_COUNT];

static s32 no_13;

static f32 pipo_2;

static float pos_1[4];

static s32 time_10;

static u8 time_37;

static EndouPrintPoint d_42[7];

static EndouPrintPoint d_43;

static EndouPrintPoint line_34[6];

static char moji2_27[27];

static char moji_26[27];

static TexturTestUploadEnvironment eNumberTexEnv_48;

static TexturTestDrawEnvironment TestEnv_52;

static int x_53;

static float ang_0[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

static float pos_1[4] = { 0.0f, 0.0f, 4.0f, 1.0f };

static const char * msg00_14[ENDOU_MODEL_MESSAGE_COUNT] = { modelTestKatakanaText, modelTestHiraganaKatakanaText, modelTestKatakanaHiraganaText, modelTestHiraganaText };

static const char * tag_16[] = { gradeOneTagText };

static const char * msg_19[] = { slowMessageText, temptationMessageText };

static char tag_20[17] = "BattleWindowTest";

static char moji_26[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static char moji2_27[27] = "abcdefghijklmnopqrstuvwxyz";

static EndouPrintPoint line_34[6] = { { .x = 64, .y = 256, .depth = 33554416, .color = { 96, 96, 128, 0 } }, { .x = 128, .y = 256, .depth = 33554416, .color = { 96, 96, 128, 96 } }, { .x = 256, .y = 256, .depth = 33554416, .color = { 96, 96, 128, 128 } }, { .x = 384, .y = 256, .depth = 33554416, .color = { 96, 96, 128, 96 } }, { .x = 448, .y = 256, .depth = 33554416, .color = { 96, 96, 128, 0 } }, { .x = 0, .y = 0, .depth = 0, .color = { 0, 0, 0, 0 } } };

static EndouPrintPoint d_42[7] = { { .x = 100, .y = 300, .depth = 16777200, .color = { 64, 64, 128, 0 } }, { .x = 100, .y = 316, .depth = 16777200, .color = { 64, 64, 128, 0 } }, { .x = 200, .y = 300, .depth = 16777200, .color = { 64, 64, 128, 112 } }, { .x = 200, .y = 316, .depth = 16777200, .color = { 64, 64, 128, 128 } }, { .x = 400, .y = 300, .depth = 16777200, .color = { 64, 64, 128, 0 } }, { .x = 400, .y = 316, .depth = 16777200, .color = { 64, 64, 128, 0 } }, { .x = 0, .y = 0, .depth = 0, .color = { 0, 0, 0, 0 } } };

static EndouPrintPoint d_43 = { .x = 256, .y = 256, .depth = 268435455, .color = { 255, 255, 255, 128 } };

static u64 eNumberTexEnv_69[12] = { 0ULL, 5836665138832211968ULL, 1152921504606846980ULL, 14ULL, 2317770511351808ULL, 80ULL, 0ULL, 81ULL, 549755814400ULL, 82ULL, 0ULL, 83ULL };

static const char * text00_75[] = { endouTestNoText, yesText };

static const char * text00_77[] = { offText, onText };

static const char * text00_79[] = { offText, onText };

static const char * text00_81[] = { absentText, presentText };

static const char * text00_83[PARTY_DEBUG_ATTACK_POSITION_COUNT] = { nullText, p1Text, p2Text, p3Text, p4Text, p5Text, p6Text, a1Text, a2Text, a3Text };

static const char * text01_84[PARTY_DEBUG_ATTACK_SLOT_COUNT] = { number1Text, number2Text, number3Text };

static const char * text02_85[] = { characterText, positionText };

const char shopRowNumberFormat[8] = "%3d";

const char modelTestJntExtension[8] = ".jnt";

static f32 pipo_2 = 0.0f;

static s32 time_10 = 0;

static s32 no_13 = 0;

const char gradeOneTagText[8] = "G One";

const char battleWindowTitleText[8] = "Test";

static int count_18 = 0;

const char padRepeatFormat[3] = "%d";

static u8 time_37 = 0;

static int x_53 = 56832;

static int x_71 = 52992;

static int Select_74 = 0;

/* JP: あり
 * EN: Yes */
const char yesText[8] = "\244\242\244\352";

/* JP: なし
 * EN: No */
const char endouTestNoText[8] = "\244\312\244\267";

/* JP: ＞
 * EN: > */
const char menuCursorText[4] = "\241\344";

static int Select_76 = 0;

/* JP: オン
 * EN: On */
const char onText[8] = "\245\252\245\363";

/* JP: オフ
 * EN: Off */
const char offText[8] = "\245\252\245\325";

static int Select_78 = 0;

static int Select_80 = 0;

/* JP: いる
 * EN: Present */
const char presentText[8] = "\244\244\244\353";

/* JP: いない
 * EN: Absent */
const char absentText[8] = "\244\244\244\312\244\244";

static int Select_82 = 0;

/* JP: Ａ３
 * EN: A3 */
const char a3Text[8] = "\243\301\243\263";

/* JP: Ａ２
 * EN: A2 */
const char a2Text[8] = "\243\301\243\262";

/* JP: Ａ１
 * EN: A1 */
const char a1Text[8] = "\243\301\243\261";

/* JP: Ｐ６
 * EN: P6 */
const char p6Text[8] = "\243\320\243\266";

/* JP: Ｐ５
 * EN: P5 */
const char p5Text[8] = "\243\320\243\265";

/* JP: Ｐ４
 * EN: P4 */
const char p4Text[8] = "\243\320\243\264";

/* JP: Ｐ３
 * EN: P3 */
const char p3Text[8] = "\243\320\243\263";

/* JP: Ｐ２
 * EN: P2 */
const char p2Text[8] = "\243\320\243\262";

/* JP: Ｐ１
 * EN: P1 */
const char p1Text[8] = "\243\320\243\261";

/* JP: ３．
 * EN: 3. */
const char number3Text[8] = "\243\263\241\245";

/* JP: ２．
 * EN: 2. */
const char number2Text[8] = "\243\262\241\245";

/* JP: １．
 * EN: 1. */
const char number1Text[8] = "\243\261\241\245";

/* JP: キャラ
 * EN: Character */
const char characterText[8] = "\245\255\245\343\245\351";

static int TopMode_86 = 0;

/* Raw words; the layout of this storage is not recovered. */
static unsigned int TestEnv_49[48];

static BgTestEnvironment TestEnv_54;

static unsigned int TestEnv_56[44];

static unsigned int TestEnv_58[48];

static unsigned int TestEnv_59[44];

static unsigned int TestEnv_60[48];

static unsigned int TestEnv_61[44];

static unsigned int TestEnv_63[68];

static unsigned int TestEnv_64[52];

static int x_50;

static int x_51;

static int x_55;

static int x_57;

static int xx_62;

static const char * msg_45[ENDOU_MESSAGE_TEST_COUNT] = { deviceListText, winningRaceListText, vowelKanaListText, horseNameListText, 0 };

static BgTestEnvironment TestEnv_54 = { .vif_nop = { 0U, 0U, 0U }, .vif_directhl_nine_quadwords = 1358954505U, .tag = { .loop_count_and_eop = 32769U, .primitive_flags_and_register_count = 2156609536U, .register_descriptors = 1397956334ULL }, .texture_setup = { .value = 68ULL, .register_id = 66ULL }, .packet_write = { .value = 2308076694620355584ULL, .register_id = 6ULL }, .primitive_setup = { .value = 327680ULL, .register_id = 71ULL }, .color = { .red = 128U, .green = 128U, .blue = 128U, .alpha = 128U }, .first_uv = { .u = 4096U, .v = 0U, .q = 0U, .reserved = 0U }, .first_vertex = { .x = 32760U, .y = 32760U, .z = 262144U, .fog = 0U }, .second_uv = { .u = 8192U, .v = 4096U, .q = 0U, .reserved = 0U }, .second_vertex = { .x = 36856U, .y = 36344U, .z = 262144U, .fog = 0U } };

static unsigned int TestEnv_58[48] = { 0U, 0U, 0U, 1358954507U, 32769U, 2710257664U, 893316846U, 229U, 262624U, 0U, 77U, 0U, 0U, 0U, 25U, 0U, 4278194175U, 0U, 9U, 0U, 1744911360U, 6U, 7U, 0U, 200705U, 0U, 72U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 4096U, 4096U, 0U, 0U, 2048U, 4096U, 0U, 0U, 0U, 0U, 63U, 0U };

static unsigned int TestEnv_61[44] = { 0U, 0U, 0U, 1358954506U, 32769U, 2425044992U, 892464878U, 5U, 68U, 0U, 66U, 0U, 4278194175U, 0U, 8U, 0U, 0U, 0U, 6U, 0U, 327680U, 0U, 71U, 0U, 128U, 128U, 128U, 128U, 0U, 0U, 0U, 0U, 28664U, 32760U, 262144U, 0U, 8192U, 4096U, 0U, 0U, 36856U, 36344U, 262144U, 0U };

static unsigned int TestEnv_63[68] = { 0U, 0U, 0U, 1358954512U, 32770U, 268435456U, 14U, 0U, 68U, 0U, 66U, 0U, 327680U, 0U, 71U, 0U, 32774U, 539410432U, 81U, 0U, 128U, 128U, 128U, 128U, 32768U, 32768U, 16777215U, 0U, 128U, 128U, 128U, 0U, 29696U, 30208U, 16777215U, 0U, 128U, 128U, 128U, 0U, 35840U, 30208U, 16777215U, 0U, 128U, 128U, 128U, 0U, 35840U, 35328U, 16777215U, 0U, 128U, 128U, 128U, 0U, 29696U, 35328U, 16777215U, 0U, 128U, 128U, 128U, 0U, 29696U, 30208U, 16777215U, 0U };

static unsigned int TestEnv_64[52] = { 0U, 0U, 0U, 1358954508U, 32770U, 268435456U, 14U, 0U, 68U, 0U, 66U, 0U, 327680U, 0U, 71U, 0U, 32769U, 2149990400U, 1364283857U, 0U, 128U, 128U, 128U, 0U, 29696U, 30208U, 16777215U, 0U, 128U, 128U, 128U, 0U, 29696U, 35328U, 16777215U, 0U, 128U, 128U, 128U, 0U, 35840U, 30208U, 16777215U, 0U, 128U, 128U, 128U, 0U, 35840U, 35328U, 16777215U, 0U };

const char modelTestLexExtension[8] = ".lex";

const char modelTestXtxExtension[8] = ".xtx";

/* JP: 秋華賞
 * EN: Shuka Sho */
const char shukaShoText[8] = "\275\251\262\332\276\336";

/* JP: 皐月賞
 * EN: Satsuki Sho */
const char satsukiShoText[8] = "\273\251\267\356\276\336";

/* JP: 桜花賞
 * EN: Oka Sho */
const char okaShoText[8] = "\272\371\262\326\276\336";

static int x_50 = 0;

static int x_51 = 56832;

static int x_55 = 57088;

static int x_57 = 56832;

static int xx_62 = 0;

static FileLoadState files = { .state = -1, .buffer = 0, .unmodeled_08 = { 100, 97, 116, 97, 92, 101, 110, 100, 111, 117, 92, 109, 101, 110, 117, 48, 48, 46, 120, 116, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

static BattleWinCommand data_15[ENDOU_BATTLE_COMMAND_COUNT] = {
    { februaryStakesText, dirtChampionshipText, 1, 2 },
    { takamatsunomiyaKinenText, springSprintChampionshipText, 1, 3 },
    { okaShoText, filliesClassicsFirstLegText, 1, 4 },
    { satsukiShoText, coltsClassicsFirstLegText, 1, 4 },
    { tennoShoSpringText, springLongDistanceChampionshipText, 1, 4 },
    { nhkMileCupText, foreignBredMilersChampionshipText, 1, 5 },
    { oaksText, filliesClassicsSecondLegText, 0, 5 },
    { japaneseDerbyText, coltsClassicsSecondLegText, 0, 5 },
    { yasudaKinenText, springMileChampionshipText, 0, 5 },
    { takarazukaKinenText, springGrandPrixText, 0, 6 },
    { sprintersStakesText, autumnSprintChampionshipText, 0, 10 },
    { shukaShoText, filliesClassicsThirdLegText, 0, 10 },
    { kikukaShoText, coltsClassicsThirdLegText, 0, 10 },
    { tennoShoAutumnText, olderHorsesCursedTennoShoText, 0, 10 },
    { queenElizabethIiCupText, filliesMaresChampionshipText, 0, 11 },
    { mileChampionshipText, autumnMileChampionshipText, 0, 11 },
    { japanCupText, olderHorsesRoyalRoadText, 0, 11 },
    { hanshinJuvenileFilliesText, twoYearOldFilliesChampionText, 0, 11 },
    { arimaKinenText, grandPrixText, 0, 11 },
    { 0, 0, 0, 0 }
};

/* JP: あ あああ / い　い  ボタン  いいい  いいいいい /  赤赤赤赤赤赤赤赤赤赤赤 青青青青青青青青青青青 う / えええ ┐┐ おおおおおおおおおおおおおおおおお おおお！！
 * EN: A aaa / I  I  button  iii (test text) Iiiii / Red red red... Blue blue blue... U / Eee (test text) Ooooo... Ooooh!! */
static char text_17[171] = "\244\242\037\244\242\244\242\244\242\012\244\244\241\241\244\244\241\241\036\001\245\334\245\277\245\363\241\241\241\241\244\244\244\244\244\244\241\241\241\241"
    "\241\241\244\244\244\244\244\244\244\244\244\244\012\014\200\000\000\300\326\300\326\300\326\300\326\300\326\300\326\300\326\300\326\300\326\300\326\300\326\014"
    "\000\000\200\300\304\300\304\300\304\300\304\300\304\300\304\300\304\300\304\300\304\300\304\300\304\037\014\200\200\200\244\246\012\244\250\244\250\244\250\244"
    "\250\244\250\244\250\037\244\252\244\252\244\252\244\252\244\252\244\252\244\252\244\252\244\252\244\252\244\252\244\252\244\252\244\252\244\252\244\252\244\252"
    "\244\252\244\252\244\252\241\252\241\252";

/* JP: Ｐｌ :ａｙ / Ｓｔａｔｉｏｎ２ : ;エ < ル =コ >ン ?ド ル  / パ サ ー スペ シャルウィーク グラスワンダー ! "セイウンスカイ キングヘイローーーー ーーーーーー あ
 * EN: PlayStation 2 / (control codes) El Condor / Pasa / Spe(cial) Shall Week / Grass Wonder Seiun Sky / King Halo ------ A */
static char text00_46[176] = "\031\001\243\320\243\354\036:\243\341\243\371\012\243\323\243\364\243\341\243\364\243\351\243\357\243\356\243\262\037\031\000\036:\036;\245\250\036<"
    "\245\353\036=\245\263\036>\245\363\036\077\245\311\036\011\245\353\036\012\245\321\036\013\245\265\036\014\241\274\037\036\001\245\271\245\332\036\033\036"
    "\034\036\035\036\036\245\267\245\343\245\353\245\246\245\243\241\274\245\257\037\036\002\245\260\245\351\245\271\245\357\245\363\245\300\241\274\037\036\037\036"
    " \036!\036\042\245\273\245\244\245\246\245\363\245\271\245\253\245\244\037\245\255\245\363\245\260\245\330\245\244\245\355\241\274\241\274\241\274\241\274"
    "\241\274\241\274\241\274\241\274\241\274\241\274\037\244\242";

static TexturTestUploadEnvironment eNumberTexEnv_48 = {
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x05, 0x00, 0x00, 0x51,
        0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    0x00083c0000000000ULL,
    {
        0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    0x0000008000000400ULL,
    {
        0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

static unsigned int TestEnv_49[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x5100000A,
    0x00008001, 0xB08B4000, 0x31EEEEEE, 0x00000535,
    0x003FFFF0, 0x00000000, 0x00000008, 0x00000000,
    0x00000044, 0x00000000, 0x00000042, 0x00000000,
    0x69343C00, 0x2007E786, 0x00000006, 0x00000000,
    0x00030003, 0x00000000, 0x00000047, 0x00000000,
    0x00000000, 0x00000000, 0x0000003F, 0x00000000,
    0x00000080, 0x00000080, 0x00000080, 0x00000080,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00006FF8, 0x00007FF8, 0x00040000, 0x00000000,
    0x00003000, 0x00001000, 0x00000000, 0x00000000,
    0x00008FF8, 0x00008DF8, 0x00040000, 0x00000000
};

static TexturTestDrawEnvironment TestEnv_52 = {
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x51,
        0x01, 0x80, 0x00, 0x00, 0x00, 0x40, 0x0b, 0xa0, 0xee, 0xee, 0x1e, 0x42, 0x42, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0xbf, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    0x0000000000000000ULL,
    {
        0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
        0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x6f, 0x00, 0x00, 0xf8, 0x71, 0x00, 0x00,
        0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f,
        0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x8f, 0x00, 0x00, 0xf8, 0x8d, 0x00, 0x00,
        0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

static unsigned int TestEnv_56[44] = {
    0x00000000, 0x00000000, 0x00000000, 0x5100000A,
    0x00008001, 0x908B4000, 0x3531EEEE, 0x00000005,
    0x00000044, 0x00000000, 0x00000042, 0x00000000,
    0x003FDFF0, 0x00000000, 0x00000008, 0x00000000,
    0x69343C00, 0x2007EF06, 0x00000006, 0x00000000,
    0x00050000, 0x00000000, 0x00000047, 0x00000000,
    0x00000080, 0x00000080, 0x00000080, 0x00000080,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00006FF8, 0x000071F8, 0x00040000, 0x00000000,
    0x00003000, 0x00001000, 0x00000000, 0x00000000,
    0x00008FF8, 0x00007FF8, 0x00040000, 0x00000000
};

static unsigned int TestEnv_59[44] = {
    0x00000000, 0x00000000, 0x00000000, 0x5100000A,
    0x00008001, 0x908B4000, 0x3531EEEE, 0x00000005,
    0x00000044, 0x00000000, 0x00000042, 0x00000000,
    0x003FDFF0, 0x00000000, 0x00000008, 0x00000000,
    0x69343C00, 0x2007EF06, 0x00000006, 0x00000000,
    0x00050000, 0x00000000, 0x00000047, 0x00000000,
    0x00000080, 0x00000080, 0x00000080, 0x00000080,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00006FF8, 0x000071F8, 0x00040000, 0x00000000,
    0x00002000, 0x00001000, 0x00000000, 0x00000000,
    0x00008FF8, 0x00007FF8, 0x00040000, 0x00000000
};

static unsigned int TestEnv_60[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x5100000B,
    0x00008001, 0xA18B4000, 0x353EEEEE, 0x000000E5,
    0x00000000, 0x00000000, 0x0000004D, 0x00000000,
    0x00000000, 0x00000000, 0x00000019, 0x00000000,
    0x007FDFF0, 0x00000000, 0x00000009, 0x00000000,
    0x69343C00, 0x2007EF06, 0x00000007, 0x00000000,
    0x00031001, 0x00000000, 0x00000048, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00002000, 0x00001000, 0x00000000, 0x00000000,
    0x00002000, 0x00001000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000003F, 0x00000000
};

static u64 TestEnv_70[20] = {
    0x0000000000000000ULL, 0x5100000900000000ULL,
    0x80AB400000008001ULL, 0x0000000053531EEEULL,
    0x0000000000000044ULL, 0x0000000000000042ULL,
    0x2007EF8669343C00ULL, 0x0000000000000006ULL,
    0x0000000000050000ULL, 0x0000000000000047ULL,
    0x0000008000000080ULL, 0x0000008000000080ULL,
    0x0000000000001000ULL, 0x0000000000000000ULL,
    0x00007FF800007FF8ULL, 0x0000000040000000ULL,
    0x0000100000002000ULL, 0x0000000000000000ULL,
    0x00008DF800008FF8ULL, 0x0000000040000000ULL
};

/* Text: {reset}I */
char shopHeadingIText[8] = "\013I ";

/* Text: {reset}CW */
const char shopHeadingCwText[8] = "\013CW\000\000\000\000";

/* Text: {reset}CA */
const char shopHeadingCaText[8] = "\013CA\000\000\000\000";

/* Text: {reset}CB */
const char shopHeadingCbText[8] = "\013CB\000\000\000\000";

/* Text: {reset}AW */
const char shopHeadingAwText[8] = "\013AW\000\000\000\000";

/* Text: {reset}AA */
const char shopHeadingAaText[8] = "\013AA\000\000\000\000";

/* Text: {reset}AB */
const char shopHeadingAbText[8] = "\013AB\000\000\000\000";

/* Text: {reset}AF */
const char shopHeadingAfText[8] = "\013AF\000\000\000\000";

/* Text: {reset}AE */
const char shopHeadingAeText[8] = "\013AE\000\000\000\000";

/* Text: {reset}A */
char shopHeadingAText[8] = "\013A ";

static char fname_67[64];

static float size_68;

static EMessageTestWindow msg_11;

static EndouModelLinePoint line00_3[7];

static EndouModelLinePoint line01_4[7];

static EndouModelLinePoint line02_5[7];

static EndouModelLinePoint line03_6[7];

static EndouModelLinePoint line10_7[7];

static EndouModelLinePoint line11_8[7];

static EndouModelLinePoint line12_9[7];

static EndouModelBackQuad bsp_12;

static ECursolMainData cur_25;

static EndouSpriteQuad d_28;

static ENumberSpriteDescriptor d_33;

static ENumberSpriteDescriptor d_35[2];

static ENumberSpriteDescriptor d_36;

static ENumberSpriteDescriptor d_38;

static ENumberRenderDescriptor d_39;

static ENumberRenderDescriptor d_40;

static ENumberSpriteDescriptor d_41;

static EndouQuadDescriptor d_44;

static EndouELineEntry line_23[4];

static ENumberMainData num_29[1];

static ENumberMainData num_30[1];

static ENumberMainData num_31[1];

static ENumberMainData num_32[1];

static ERibbonMainData rbn_21[4];

static ESprite spr_22[4];

static EndouTagFontViews tgf_24[2];

static unsigned char * buffer_72;

static char mm_73[352];

static inline void endouShopHeading(int y, const char *format, int shop_number)
{
    xglFontDebugPrintf(0, y, format, shop_number);
}

static inline void endModelCameraPosition(StudioCamera *camera,
                                         float x, float y, float z)
{
    camera->position.vector.x = x;
    camera->position.vector.y = y;
    camera->position.vector.z = z;
}

/*
 * Applies a test lighting profile to a studio light: the ambient intensity,
 * then the three parallel lights.
 */
static __inline__ void EndouApplyModelLighting(StudioLight **light_slot,
                                              EndouTestLighting profile)
{
    xglStudioGetLight(light_slot);
    xglLightIntensityAmbient(*light_slot, &profile.ambient);
    xglLightIntensityParallel(*light_slot, 0, &profile.parallel_intensity[0]);
    xglLightDirection(*light_slot, 0, &profile.direction[0]);
    xglLightIntensityParallel(*light_slot, 1, &profile.parallel_intensity[1]);
    xglLightDirection(*light_slot, 1, &profile.direction[1]);
    xglLightIntensityParallel(*light_slot, 2, &profile.parallel_intensity[2]);
    xglLightDirection(*light_slot, 2, &profile.direction[2]);
}

/*
 * Test menu loop: shows the test list and runs the test whose pad button is
 * pressed, until the exit chord is held.
 */
void EndouTest(void)
{
    /* Set the clear color used by each frame. */
    ENDOU_CLEAR_ENV->color_r = 0;
    ENDOU_CLEAR_ENV->color_g = 0x40;
    ENDOU_CLEAR_ENV->color_b = 0;
    ENDOU_CLEAR_ENV->color_a = 0;

    PadData.buttons.input.pressed = 0;
    endPrintInit();

    for (;;) {
        xglRenderClearFrame();
        if ((PadData.buttons.button_state & ENDOU_EXIT_CHORD_ULL) == ENDOU_EXIT_CHORD_ULL) {
            break;
        }

        xglFontDebugPrintf(0, 0, GameEndouTestTitleText);
        xglFontDebugPrintf(0, 0x10, settlementScreenTestLabel);
        if ((PadData.buttons.input.pressed & PAD_TRIANGLE) != 0) {
            SeisanTest();
        }

        xglFontDebugPrintf(0, 0x20, textTestSquareLabel);
        if ((PadData.buttons.input.held & PAD_SQUARE) != 0) {
            TextTest();
        }

        xglFontDebugPrintf(0, 0x30, messageTestCrossLabel);
        if ((PadData.buttons.input.pressed & PAD_CROSS) != 0) {
            eMessageTest();
        }

        xglFontDebugPrintf(0, 0x40, miscTestCircleLabel);
        if ((PadData.buttons.input.pressed & PAD_CIRCLE) != 0) {
            eNumberTest();
        }

        xglFontDebugPrintf(0, 0x50, modelTestL1Label);
        if ((PadData.buttons.input.pressed & PAD_L1) != 0) {
            eModelTest();
        }

        xglFontDebugPrintf(0, 0x60, loadTestL2Label);
        if ((PadData.buttons.input.pressed & PAD_L2) != 0) {
            MenuFileMain(1);
        }

        xglFontDebugPrintf(0, 0x70, textureTestR1Label);
        if ((PadData.buttons.input.pressed & PAD_R1) != 0) {
            TexturTest();
        }

        xglFontDebugPrintf(0, 0x80, jpegTestR2Label);
        if ((PadData.buttons.input.pressed & PAD_R2) != 0) {
            JpegTest();
        }

        xglFontDebugPrintf(0, 0x90, battleWindowTestUpLabel);
        if ((PadData.buttons.input.pressed & PAD_UP) != 0) {
            BattleWindowTest();
        }

        xglFontDebugPrintf(0, 0xa0, saveTestDownLabel);
        if ((PadData.buttons.input.pressed & PAD_DOWN) != 0) {
            MenuFileMain(0);
        }

        xglFontDebugPrintf(0, 0xb0, modelTestRightLabel);
        if ((PadData.buttons.input.pressed & PAD_RIGHT) != 0) {
            ModelTest();
        }

        xglFontDebugPrintf(0, 0xc0, shopTestLeftLabel);
        if ((PadData.buttons.input.pressed & PAD_LEFT) != 0) {
            ShopTest();
        }

        xglSleep();
    }
}

/*
 * Loads the event text overlay and shows its messages in a message window,
 * advancing pages with R1, until the exit chord is held.
 */
void TextTest(void)
{
    EMessageParam message;
    UmnEventTextRecord *record;

    xglCdLoadOverlay(2);
    xglRenderClearFrame();
    WorkEnd = UmnEventTextInit(WorkEnd);
    UmnEventTextMake(1);
    endPrintInit();

    record = UmnEventTextNextGet(0);
    if (record == 0) {
        return;
    }

    eMessageSet(&message, record->message_text);
    message.x = 0x40;
    message.y = 0x40;
    message.draw_parameter = 0x00fffff0;
    message.page_available = 2;
    eMessageModeChange(&message, E_MODE_OPEN_START);

    while (xglFontDebugPrintf(0, 0x10, textTestLabel),
           (PadData.buttons.button_state & ENDOU_EXIT_CHORD_U) != ENDOU_EXIT_CHORD_U) {
        eMessageMain(&message);
        if ((PadData.buttons.input.pressed & PAD_R1) != 0 &&
            eMessageNextPage(&message, 0) == 0) {
            record = UmnEventTextNextGet(1);
            if (record == 0) {
                return;
            }
            eMessageTextChange(&message, record->message_text);
            eMessageModeChange(&message, E_MODE_OPEN_START);
        }

        endPrintExtFunc(0, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_FLUSH, 0);
        xglSleep();
    }
}

/*
 * Shop debug test: shows the shop records for the selected shop and runs
 * MenuShopMain on it. Exits on Cross or the exit chord.
 */
void ShopTest(void)
{
    unsigned int ab_column_offset;
    typedef struct ShopFile {
        u16 records[ENDOU_SHOP_COUNT][ENDOU_SHOP_RECORD_WORDS];
    } ShopFile;
    int row_index;
    int menu_open;
    unsigned int menu_selection = 1;
    ShopFile *shop_file;
    unsigned short pressed;

    xglCdLoadOverlay(1);
    func_A22628();
    ACT_init();

    shop_file = (ShopFile *)(((u32)WorkEnd + 0x7fU) & 0xffffff80U);
    WorkEnd = (u8 *)shop_file + xglCdReadFile(shopDataPath, shop_file, 0, 1);
    PartyFriendOn(3);
    PartyFriendOn(2);
    menu_open = 0;
    PartyFriendOn(7);
    PartyTakeAgwsOn(PARTY_DEBUG_AGWS_FIRST_ID);
    PartyTakeAgwsOn(0x18);
    if ((PadData.buttons.input.held & PAD_SELECT) != 0)
        dataMoneyBoxInc(ENDOU_SHOP_TEST_MONEY_SMALL);
    else
        dataMoneyBoxInc(ENDOU_SHOP_TEST_MONEY_LARGE);
    xglRenderClearFrame();

    for (;;) {
        xglFontDebugPrintf(0, 0x10, shopTestLabel);
        if ((PadData.buttons.button_state & ENDOU_EXIT_CHORD_U) == ENDOU_EXIT_CHORD_U)
            break;
        pressed = PadData.buttons.input.pressed;
        if ((pressed & PAD_CROSS) != 0)
            break;

        /*
         * Mode 0 browses the shop records; mode 1 runs the shop menu for the
         * selected shop.
         */
        switch (menu_open) {
        case 0: {
            u16 repeated;
            u16 (*selected_page)[ENDOU_SHOP_RECORD_WORDS];
            const u16 *entry_values;
            const u16 *neighbor_values;
            const u16 *extra_values;

            repeated = PadData.buttons.input.repeat;
            if ((repeated & PAD_UP) != 0)
                menu_selection = menu_selection == 1 ? ENDOU_SHOP_COUNT : menu_selection - 1;
            if ((repeated & PAD_DOWN) != 0)
                menu_selection = menu_selection == ENDOU_SHOP_COUNT ? 1 : menu_selection + 1;
            if ((pressed & PAD_CIRCLE) != 0)
                menu_open = 1;
            xglFontDebugPrintf(0, 0x20, shopNumberChangeLabel);
            xglFontDebugPrintf(0x80, 0x20, shopRunQuitLabel);
            endouShopHeading(0x30, shopNoFormat, menu_selection);

            {
            u16 (*const page)[ENDOU_SHOP_RECORD_WORDS] =
                &shop_file->records[menu_selection];

            endouShopHeading(0x40, shopHeadingIText, menu_selection);
            endouShopHeading(0x50, shopHeadingCwText, menu_selection);
            selected_page = page - 1;
            endouShopHeading(0x60, shopHeadingCaText, menu_selection);
            endouShopHeading(0x70, shopHeadingCbText, menu_selection);
            endouShopHeading(0x80, shopHeadingAwText, menu_selection);
            endouShopHeading(0x90, shopHeadingAaText, menu_selection);
            endouShopHeading(0xa0, shopHeadingAbText, menu_selection);
            endouShopHeading(0xb0, shopHeadingAfText, menu_selection);
            endouShopHeading(0xc0, shopHeadingAeText, menu_selection);
            endouShopHeading(0xd0, shopHeadingAText, menu_selection);

            row_index = 0;
            extra_values = page[-1] + 4;
            entry_values = selected_page[0];
            neighbor_values = selected_page[0] + 0x8c;

            do {
                xglFontDebugPrintf((row_index % 10) * 0x18 + 0x10,
                                   (row_index / 10) * 8 + 0x40, shopRowNumberFormat,
                                   *entry_values);
                xglFontDebugPrintf((row_index % 10) * 0x18 + 0x10,
                                   (row_index / 10) * 8 + 0x50, shopRowNumberFormat,
                                   neighbor_values[-0x78]);
                xglFontDebugPrintf((row_index % 10) * 0x18 + 0x10,
                                   (row_index / 10) * 8 + 0x60, shopRowNumberFormat,
                                   entry_values[0x28]);
                xglFontDebugPrintf((row_index % 10) * 0x18 + 0x10,
                                   (row_index / 10) * 8 + 0x70, shopRowNumberFormat,
                                   neighbor_values[-0x50]);
                xglFontDebugPrintf((row_index % 10) * 0x18 + 0x10,
                                   (row_index / 10) * 8 + 0x80, shopRowNumberFormat,
                                   entry_values[0x50]);
                xglFontDebugPrintf((row_index % 10) * 0x18 + 0x10,
                                   (row_index / 10) * 8 + 0x90, shopRowNumberFormat,
                                   neighbor_values[-0x28]);
                ab_column_offset = 6 * ENDOU_SHOP_SLOT_COUNT;
                xglFontDebugPrintf((row_index % 10) * 0x18 + 0x10,
                                   (row_index / 10) * 8 + 0xa0, shopRowNumberFormat,
                                   entry_values[ab_column_offset]);
                xglFontDebugPrintf((row_index % 10) * 0x18 + 0x10,
                                   (row_index / 10) * 8 + 0xb0, shopRowNumberFormat,
                                   *neighbor_values++);
                xglFontDebugPrintf((row_index % 10) * 0x18 + 0x10,
                                   (row_index / 10) * 8 + 0xc0, shopRowNumberFormat,
                                   (entry_values++)[0xa0]);
            } while (++row_index < ENDOU_SHOP_SLOT_COUNT);
            for (row_index = 0; row_index < 4; row_index++) {
                int x = (row_index % 10) * 0x18 + 0x10;
                int y = (row_index / 10) * 8;

                xglFontDebugPrintf(x, y + 0xd0, shopRowNumberFormat,
                                   extra_values[0xb0 + row_index]);
            }
            }
            break;
        }
        case 1:
            menu_open = 0;
            MenuShopMain(menu_selection);
            xglRenderClearFrame();
            break;
        }
        xglSleep();
    }
}

void SeisanTest(void)
{
    const int exit_chord = ENDOU_EXIT_CHORD;

    xglCdLoadOverlay(1);
    SeisanInit();
    xglRenderClearFrame();

    while ((PadData.buttons.button_state & exit_chord) != exit_chord) {
        SeisanMain();
        SeisanDisp();
        xglSleep();
    }
}

/*
 * Model test: loads the model with its motion and accessory actor, then runs
 * the pad-driven viewer until the exit chord is held.
 */
void ModelTest(void) {
    StudioLight *light;
    u8 **work_end = &WorkEnd;
    s32 *message_time = &time_10;
    int motion;

    motion = 1;
    xglStudioChange(0);
    EndouApplyModelLighting(&light, modelTestLighting);
    {
    int point_index;
    StudioCamera *temp_17_4;
    StudioCamera *temp_2;
    StudioCamera *temp_2_2;
    f32 temp_f0_21;
    f32 temp_f0_22;
    f32 temp_f0_23;
    f32 temp_f0_24;
    f32 temp_f0_25;
    f32 temp_f0_26;
    f32 temp_f0_27;
    f32 temp_f1;
    f32 temp_f20;
    f32 temp_f2;
    s32 temp_2_11;
    s32 temp_2_14;
    s32 temp_2_16;
    s32 temp_2_18;
    s32 temp_2_20;
    s32 temp_2_22;
    s32 temp_2_24;
    s32 temp_2_26;
    s32 temp_2_5;
    s32 temp_2_7;
    s32 temp_2_9;
    u16 temp_16;
    u16 temp_4;
    u16 var_3_2;
    u8 *temp_3;
    u8 *temp_3_10;
    u8 *temp_3_11;
    u8 *temp_3_2;
    u8 *temp_3_3;
    u8 *temp_3_4;
    u8 *temp_3_5;
    u8 *temp_3_6;
    u8 *temp_3_7;
    u8 *temp_3_8;
    u8 *temp_3_9;
    Actor *temp_23_9;
    Actor *temp_2_12;
    Actor *temp_2_3;
    u8 *resource_buffer;
    u8 *accessory_resource_buffer;

    /* Set up the two studio cameras. */
    temp_2 = xglStudioGetCamera2(0);
    temp_2->rotation.vector.x = 0.0f;
    temp_2->rotation.vector.y = 0.0f;
    temp_2->rotation.vector.z = 0.0f;
    endModelCameraPosition(temp_2, 0.0f, 1.0f, 4.0f);
    temp_2->active = 1;
    temp_2->state = 1;
    temp_2_2 = xglStudioGetCamera2(1);
    temp_2_2->state = 1;
    endModelCameraPosition(temp_2_2, -1.0f, 1.0f, 4.0f);
    temp_2_2->rotation.vector.y = 0.0f;
    temp_2_2->rotation.vector.z = 0.0f;
    temp_2_2->active = 0;
    temp_2_2->rotation.vector.x = 0.0f;
    GameResourceInit(GAME_RESOURCE_BASE_ADDRESS, GAME_RESOURCE_BUFFER_SIZE);
    ACT_init();
    /* Create the main actor and load its model files. */
    temp_2_3 = ACT_create(-1, 0x2101);
    ACT_initMotion(temp_2_3);
    {
    char sp70[128] = "\0";
    char spF0[128] = "\0";
    EndouTestLighting unused_lighting;

    RES_GetMdlFileName(sp70, temp_2_3->inUseId);
    resource_buffer = (u8 *)((u32)((*work_end) + 0x7f) & ~0x7fU);
    strcpy(spF0, sp70);
    strcat(spF0, modelTestLexExtension);
    temp_2_5 = xglCdReadFile(spF0, resource_buffer, 0, 0);
    temp_3 = (*work_end) + temp_2_5;
    temp_2_3->modelResource = resource_buffer;
    (*work_end) = temp_3;
    resource_buffer = (u8 *)((u32)(temp_3 + 0x7f) & ~0x7fU);
    strcpy(spF0, sp70);
    strcat(spF0, modelTestXtxExtension);
    temp_2_7 = xglCdReadFile(spF0, resource_buffer, 0, 0);
    temp_3_2 = (*work_end) + temp_2_7;
    temp_2_3->matrixResource = resource_buffer;
    (*work_end) = temp_3_2;
    resource_buffer = (u8 *)((u32)(temp_3_2 + 0x7f) & ~0x7fU);
    strcpy(spF0, sp70);
    strcat(spF0, modelTestJntExtension);
    temp_2_9 = xglCdReadFile(spF0, resource_buffer, 0, 0);
    }
    temp_3_3 = ((*work_end) + temp_2_9);
    temp_2_3->move = resource_buffer;
    resource_buffer = (u8 *)((u32)(temp_3_3 + 0x7f) & ~0x7fU);
    (*work_end) = temp_3_3;
    temp_2_11 = xglCdReadFile(modelTestFpkName, resource_buffer, 0, 0);
    temp_3_4 = (*work_end) + temp_2_11;
    temp_2_3->animPackTables[0] = resource_buffer;
    (*work_end) = temp_3_4;
    ACT_allocMatrix(temp_2_3, -1);
    ACT_setModelWrapper(temp_2_3, 0);
    ACT_setMotion(temp_2_3, 1);
    /* Create the second actor and load its model files. */
    temp_2_12 = ACT_create(-1, 1);
    ACT_initMotion(temp_2_12);
    {
    char sp1E0[128] = "\0";
    char sp260[128] = "\0";

    RES_GetMdlFileName(sp1E0, temp_2_12->inUseId);
    resource_buffer = (u8 *)((u32)((*work_end) + 0x7f) & ~0x7fU);
    strcpy(sp260, sp1E0);
    strcat(sp260, modelTestLexExtension);
    temp_2_14 = xglCdReadFile(sp260, resource_buffer, 0, 0);
    temp_3_5 = (*work_end) + temp_2_14;
    temp_2_12->modelResource = resource_buffer;
    (*work_end) = temp_3_5;
    resource_buffer = (u8 *)((u32)(temp_3_5 + 0x7f) & ~0x7fU);
    strcpy(sp260, sp1E0);
    strcat(sp260, modelTestXtxExtension);
    temp_2_16 = xglCdReadFile(sp260, resource_buffer, 0, 0);
    temp_3_6 = (*work_end) + temp_2_16;
    temp_2_12->matrixResource = resource_buffer;
    (*work_end) = temp_3_6;
    resource_buffer = (u8 *)((u32)(temp_3_6 + 0x7f) & ~0x7fU);
    strcpy(sp260, sp1E0);
    strcat(sp260, modelTestJntExtension);
    temp_2_18 = xglCdReadFile(sp260, resource_buffer, 0, 0);
    }
    temp_3_7 = (*work_end) + temp_2_18;
    temp_2_12->move = resource_buffer;
    resource_buffer = (u8 *)((u32)(temp_3_7 + 0x7f) & ~0x7fU);
    (*work_end) = temp_3_7;
    temp_2_20 = xglCdReadFile(modelTestMotionName, resource_buffer, 0, 0);
    temp_3_8 = (*work_end) + temp_2_20;
    temp_2_12->animPackTables[0] = resource_buffer;
    (*work_end) = temp_3_8;
    ACT_allocMatrix(temp_2_12, -1);
    ACT_setModelWrapper(temp_2_12, 0);
    ACT_setMotion(temp_2_12, 1);
    /* Create the accessory actor and load its files. */
    temp_23_9 = ACT_create(-1, 0x3003);
    {
    char sp2E0[128] = "\0";
    char sp360[128] = "\0";

    RES_GetMdlFileName(sp2E0, temp_23_9->inUseId);
    accessory_resource_buffer = (u8 *)((u32)((*work_end) + 0x7f) & ~0x7fU);
    strcpy(sp360, sp2E0);
    strcat(sp360, modelTestLexExtension);
    temp_2_22 = xglCdReadFile(sp360, accessory_resource_buffer, 0, 0);
    temp_3_9 = (*work_end) + temp_2_22;
    temp_23_9->modelResource = accessory_resource_buffer;
    (*work_end) = temp_3_9;
    accessory_resource_buffer = (u8 *)((u32)(temp_3_9 + 0x7f) & ~0x7fU);
    strcpy(sp360, sp2E0);
    strcat(sp360, modelTestXtxExtension);
    temp_2_24 = xglCdReadFile(sp360, accessory_resource_buffer, 0, 0);
    temp_3_10 = (*work_end) + temp_2_24;
    temp_23_9->matrixResource = accessory_resource_buffer;
    (*work_end) = temp_3_10;
    accessory_resource_buffer = (u8 *)((u32)(temp_3_10 + 0x7f) & ~0x7fU);
    strcpy(sp360, sp2E0);
    strcat(sp360, modelTestJntExtension);
    if (xglCdGetFileSize(sp360) < 0) {
        temp_23_9->move = NULL;
    } else {
        temp_2_26 = xglCdReadFile(sp360, accessory_resource_buffer, 0, 0);
        temp_3_11 = (*work_end) + temp_2_26;
        temp_23_9->move = accessory_resource_buffer;
        (*work_end) = temp_3_11;
    }
    }
    ACT_allocMatrix(temp_23_9, -1);
    ACT_setModelWrapper(temp_23_9, 0);
    ACT_setArms(temp_23_9, temp_2_3, 0x108U, 0);
    {
        /*
         * Each frame: the pad moves the model and camera, then the model, grid
         * lines and message are drawn.
         */
        while ((PadData.buttons.button_state & ENDOU_EXIT_CHORD) != ENDOU_EXIT_CHORD) {
            var_3_2 = ENDOU_MODEL_PAD->pressed;
            if (var_3_2 & PAD_R1) {
                ACT_setMotion(temp_2_3, ++motion);
                var_3_2 = ENDOU_MODEL_PAD->pressed;
            }
            if (var_3_2 & PAD_R2) {
                ACT_setMotion(temp_2_3, --motion);
            }
            temp_4 = ENDOU_MODEL_PAD->held;
            if (temp_4 & PAD_SELECT) {
                AlignedVector4 reset_position = {0.0f, 0.0f, 0.0f, 1.0f};
                AlignedVector4 reset_rotation = {0.0f, 0.0f, 0.0f, 1.0f};
                AlignedVector4 reset_scale = {1.0f, 1.0f, 1.0f, 1.0f};
                temp_4 = ENDOU_MODEL_PAD->held;
                temp_2_3->position = reset_position;
                temp_2_3->rotation = reset_rotation;
                temp_2_3->scale = reset_scale;
            }
            {
            Matrix4 camera_matrix;
            float world_points[ENDOU_GRID_ROWS][ENDOU_GRID_COLUMNS][4];
            EndouModelLineVertex screen_points[ENDOU_GRID_ROWS][ENDOU_GRID_COLUMNS];
            Matrix4 projection_matrix;
            unsigned char column_alphas[ENDOU_GRID_ROWS];
            unsigned char row_alphas[ENDOU_GRID_COLUMNS];
            DoublewordAlignedVector4 quad_words[4];
            /*
             * Triangle scales the model; without it the same inputs move its
             * position.
             */
            if (temp_4 & PAD_TRIANGLE) {
                if (temp_4 & PAD_RIGHT) {
                    temp_2_3->scale.x += 0.1f;
                }
                if (temp_4 & PAD_LEFT) {
                    temp_2_3->scale.x -= 0.1f;
                }
                if (temp_4 & PAD_UP) {
                    temp_2_3->scale.y += 0.1f;
                }
                if (temp_4 & PAD_DOWN) {
                    temp_2_3->scale.y -= 0.1f;
                }
                if (temp_4 & PAD_L1) {
                    temp_2_3->scale.z += 0.1f;
                }
                if ((unsigned short)(temp_4 & PAD_L2)) {
                    temp_2_3->scale.z -= 0.1f;
                }
            } else {
                if (temp_4 & PAD_RIGHT) {
                    temp_2_3->position.x += 0.1f;
                }
                if (temp_4 & PAD_LEFT) {
                    temp_2_3->position.x -= 0.1f;
                }
                if (temp_4 & PAD_UP) {
                    temp_2_3->position.y += 0.1f;
                }
                if (temp_4 & PAD_DOWN) {
                    temp_2_3->position.y -= 0.1f;
                }
                if (temp_4 & PAD_L1) {
                    temp_2_3->position.z += 0.1f;
                }
                if ((unsigned short)(temp_4 & PAD_L2)) {
                    temp_2_3->position.z -= 0.1f;
                }
            }
            if (ENDOU_MODEL_PAD->stick_axis[0] < -ENDOU_STICK_DEADZONE) {
                temp_2_3->rotation.y += 0.04f;
            }
            if (ENDOU_MODEL_PAD->stick_axis[0] >= ENDOU_STICK_DEADZONE + 1) {
                temp_2_3->rotation.y -= 0.04f;
            }
            if (ENDOU_MODEL_PAD->stick_axis[1] < -ENDOU_STICK_DEADZONE) {
                temp_2_3->rotation.x += 0.04f;
            }
            if (ENDOU_MODEL_PAD->stick_axis[1] >= ENDOU_STICK_DEADZONE + 1) {
                temp_2_3->rotation.x -= 0.04f;
            }
            temp_17_4 = xglStudioGetCamera2(0);
            if (ENDOU_MODEL_PAD->stick_axis[2] < -ENDOU_STICK_DEADZONE) {
                ang_0[1] += 0.04f;
            }
            if (ENDOU_MODEL_PAD->stick_axis[2] >= ENDOU_STICK_DEADZONE + 1) {
                ang_0[1] -= 0.04f;
            }
            if (ENDOU_MODEL_PAD->stick_axis[3] < -ENDOU_STICK_DEADZONE) {
                ang_0[0] += 0.04f;
            }
            if (ENDOU_MODEL_PAD->stick_axis[3] >= ENDOU_STICK_DEADZONE + 1) {
                ang_0[0] -= 0.04f;
            }
            xglMatrixStackUnit();
            xglMatrixStackRotZ(ang_0[2]);
            xglMatrixStackRotY(ang_0[1]);
            xglMatrixStackRotX(ang_0[0]);
            xglMatrixStackTrans(pos_1);
            xglMatrixStackSave(camera_matrix);
            temp_f0_21 = ang_0[0];
            temp_16 = ENDOU_MODEL_PAD->held;
            temp_17_4->rotation.vector.x = temp_f0_21;
            temp_f0_22 = ang_0[1];
            temp_17_4->rotation.vector.y = temp_f0_22;
            temp_f1 = ang_0[2];
            temp_17_4->rotation.vector.z = temp_f1;
            temp_f2 = camera_matrix[3][0];
            temp_17_4->position.vector.x = temp_f2;
            temp_f0_23 = camera_matrix[3][1] + 1.0f;
            temp_17_4->position.vector.y = temp_f0_23;
            temp_f20 = camera_matrix[3][2];
            temp_17_4->position.vector.z = temp_f20;
            if (temp_16 & PAD_R1) {
                temp_f0_24 = pipo_2 + 0.2;
                pipo_2 = temp_f0_24;
            }
            if (temp_16 & PAD_R2) {
                temp_f0_26 = pipo_2 - 0.2;
                pipo_2 = temp_f0_26;
            }
            temp_f0_25 = pipo_2;
            temp_f0_27 = temp_f20 + temp_f0_25;
            temp_17_4->position.vector.z = temp_f0_27;
            temp_2_12->position = temp_2_3->position;
            temp_2_12->position.x += 1.0f;
            temp_2_12->rotation = temp_2_3->rotation;
            temp_2_12->scale = temp_2_3->scale;
            ACT_updateMotion(temp_2_3);
            ACT_updateMotion(temp_2_12);
            xglStudioGetActiveCamera()->active = 0;
            xglStudioGetCamera2(0)->active = 1;
            ACT_modelDraw(temp_2_3);
            xglStudioGetActiveCamera()->active = 0;
            xglStudioGetCamera2(0)->active = 1;
            ACT_modelDraw(temp_2_12);
            nmlModelFlush();
            {
                int row;

                for (point_index = 0; point_index < ENDOU_GRID_COLUMNS; point_index++) {
                    float x = point_index * 0.5f + -1.25f;

                    for (row = 0; row < ENDOU_GRID_ROWS; row++) {
                        world_points[row][point_index][0] = x;
                        world_points[row][point_index][1] = 0.0f;
                        world_points[row][point_index][3] = 1.0f;
                        world_points[row][point_index][2] = row * 0.5f + -1.0f;
                    }
                }
                {
                    for (point_index = 0; point_index < ENDOU_GRID_COLUMNS; point_index++) {
                        world_points[0][point_index][2] -= 0.5f;
                        world_points[4][point_index][2] += 0.5f;
                    }
                    for (point_index = 0; point_index < ENDOU_GRID_ROWS; point_index++) {
                        world_points[point_index][0][0] -= 0.5f;
                        world_points[point_index][5][0] += 0.5f;
                    }
                }
            }
            /*
             * Copy the alpha tables, then project the grid points to screen
             * space.
             */
            __builtin_memcpy(column_alphas, modelTestColumnAlphaTable,
                             sizeof(column_alphas));
            __builtin_memcpy(row_alphas, modelTestRowAlphaTable, sizeof(row_alphas));
            xglMatrixStackUnit();
            xglMatrixStackSave(projection_matrix);
            xglRotTransPersN(screen_points, &projection_matrix, world_points, ENDOU_GRID_POINT_COUNT, 0);
            {
                for (point_index = 0; point_index < ENDOU_GRID_ROWS; point_index++) {
                    unsigned char *color;
                    line00_3[point_index].vertex = screen_points[point_index][1];
                    color = line00_3[point_index].color;
                    color[0] = -0x80;
                    color[2] = -0x10;
                    color[1] = -0x10;
                    color[3] = column_alphas[point_index];
                    line00_3[point_index].vertex.terminator = 1;
                }
                line00_3[5].vertex.terminator = 0;

                {
                for (point_index = 0; point_index < ENDOU_GRID_ROWS; point_index++) {
                    unsigned char *color;
                    line01_4[point_index].vertex = screen_points[point_index][2];
                    color = line01_4[point_index].color;
                    color[0] = -0x80;
                    color[2] = -0x10;
                    color[1] = -0x10;
                    color[3] = column_alphas[point_index];
                    line01_4[point_index].vertex.terminator = 1;
                }
                (line01_4 + point_index)->vertex.terminator = 0;
                }

                {
                for (point_index = 0; point_index < ENDOU_GRID_ROWS; point_index++) {
                    unsigned char *color;
                    line02_5[point_index].vertex = screen_points[point_index][3];
                    color = line02_5[point_index].color;
                    color[0] = -0x80;
                    color[2] = -0x10;
                    color[1] = -0x10;
                    color[3] = column_alphas[point_index];
                    line02_5[point_index].vertex.terminator = 1;
                }
                (line02_5 + point_index)->vertex.terminator = 0;
                }

                {
                for (point_index = 0; point_index < ENDOU_GRID_ROWS; point_index++) {
                    unsigned char *color;
                    line03_6[point_index].vertex = screen_points[point_index][4];
                    color = line03_6[point_index].color;
                    color[0] = -0x80;
                    color[2] = -0x10;
                    color[1] = -0x10;
                    color[3] = column_alphas[point_index];
                    line03_6[point_index].vertex.terminator = 1;
                }
                (line03_6 + point_index)->vertex.terminator = 0;
                }

                {
                for (point_index = 0; point_index < ENDOU_GRID_COLUMNS; point_index++) {
                    unsigned char *color;
                    line10_7[point_index].vertex = screen_points[1][point_index];
                    color = line10_7[point_index].color;
                    color[0] = -0x80;
                    color[2] = -0x10;
                    color[1] = -0x10;
                    color[3] = row_alphas[point_index];
                    line10_7[point_index].vertex.terminator = 1;
                }
                (line10_7 + point_index)->vertex.terminator = 0;
                }

                {
                for (point_index = 0; point_index < ENDOU_GRID_COLUMNS; point_index++) {
                    unsigned char *color;
                    line11_8[point_index].vertex = screen_points[2][point_index];
                    color = line11_8[point_index].color;
                    color[0] = -0x80;
                    color[2] = -0x10;
                    color[1] = -0x10;
                    color[3] = row_alphas[point_index];
                    line11_8[point_index].vertex.terminator = 1;
                }
                (line11_8 + point_index)->vertex.terminator = 0;
                }

                {
                for (point_index = 0; point_index < ENDOU_GRID_COLUMNS; point_index++) {
                    unsigned char *color;
                    line12_9[point_index].vertex = screen_points[3][point_index];
                    color = line12_9[point_index].color;
                    color[0] = -0x80;
                    color[2] = -0x10;
                    color[1] = -0x10;
                    color[3] = row_alphas[point_index];
                    line12_9[point_index].vertex.terminator = 1;
                }
                (line12_9 + point_index)->vertex.terminator = 0;
                }
            }
            /* Submit the seven grid line sets to the print layer. */
            endPrintExtFunc(0xFFFFF0, END_PRINT_CMD_LINE_RAW, line00_3);
            endPrintExtFunc(0xFFFFF1, END_PRINT_CMD_LINE_RAW, line01_4);
            endPrintExtFunc(0xFFFFF2, END_PRINT_CMD_LINE_RAW, line02_5);
            endPrintExtFunc(0xFFFFF3, END_PRINT_CMD_LINE_RAW, line03_6);
            endPrintExtFunc(0xFFFFF4, END_PRINT_CMD_LINE_RAW, line10_7);
            endPrintExtFunc(0xFFFFF5, END_PRINT_CMD_LINE_RAW, line11_8);
            endPrintExtFunc(0xFFFFF6, END_PRINT_CMD_LINE_RAW, line12_9);
            __builtin_memcpy(quad_words, modelTestQuadWordTable, sizeof(quad_words));
            endPrintExtFunc(0, 0x6A, NULL);
            endPrintExtFunc(0, 0x6C, NULL);
            /* Show the next message when the timer runs out. */
            if (*message_time == 0) {
                eMessageSet(&msg_11.message, msg00_14[no_13]);
                msg_11.message.mode = EMESSAGE_MODE_LINE_START;
                *message_time = ENDOU_MODEL_MESSAGE_FRAMES;
                no_13 = (no_13 + 1) & 3;
            }
            (*message_time)--;
            msg_11.message.x = 0;
            msg_11.message.draw_parameter = 0xFFFFF0;
            msg_11.message.y = 0;
            eMessageMain(&msg_11.message);
            endPrintExtFunc(0, 0x6B, NULL);
            bsp_12.uv[0] = 0.0f;
            bsp_12.uv[2] = 0.234375f;
            bsp_12.uv[3] = 0.046875f;
            bsp_12.uv[1] = 0.0f;
            xglRotTransPersN(bsp_12.vertices, NULL, quad_words, 4, 0);
            endPrintExtFunc(0, 0x6D, &bsp_12);
            xglSleep();
            }
        }
    }
    }
}

/*
 * Battle window debug test: opens and closes the battle window variants from
 * the pad, until the exit chord is held.
 */
void BattleWindowTest(void)
{
    EndouBattleWindowParams window;
    EndouBattleMessageParams message;
    EndouBattleAnalysisParams analysis;
    EndouBattleRewardParams reward;

    window.x = 16;
    window.style = 0x00fffffd;
    window.title = tag_16[0];
    window.commands = data_15;
    window.y = 144;
    window.information_y = 160;
    window.information_x = 0;
    xglRenderClearFrame();
    xglSleep();
    xglCdLoadOverlay(1);
    eBattleWinInit();

    if ((PadData.buttons.button_state & ENDOU_EXIT_CHORD_U) != ENDOU_EXIT_CHORD_U) {
    do {
        xglFontDebugPrintf(0, 0, battleWindowTestLabel);
        if ((PadData.buttons.input.pressed & PAD_TRIANGLE) != 0) {
            message.x = 16;
            message.y = 32;
            message.title = battleWindowTitleText;
            message.style = 0x00fffffd;
            message.text = text_17;
            eBattleWinOpen4(&message);
        }
        if ((PadData.buttons.input.pressed & PAD_SQUARE) != 0 &&
            eBattleWinPageCheck4() != 0)
            eBattleWinClose4();
        eBattleWinMain4();
        if ((PadData.buttons.input.pressed & PAD_TRIANGLE) != 0) {
            analysis.x = 64;
            analysis.experience = 0x2107a;
            analysis.y = 256;
            analysis.conditions = 0x208;
            analysis.hp = 100;
            analysis.max_hp = 200;
            analysis.technique_points = 495;
            analysis.ether_points = 689;
            analysis.skill_points = 800;
            analysis.money = 0xdac0;
            analysis.common_drop = 5;
            analysis.rare_drop = 10;
            analysis.style = 0x00fffff0;
            analysis.name = condorPasaName;
            eBattleWinOpen3(&analysis);
        }
        if ((PadData.buttons.input.pressed & PAD_SQUARE) != 0)
            eBattleWinClose3();
        eBattleWinMain3();
        if ((PadData.buttons.input.pressed & PAD_L1) != 0) {
            reward.width = 0x800000b4;
            reward.x = 16;
            reward.y = 32;
            reward.text = msg_19[count_18];
            reward.style = 0x00fffff0;
            reward.title = tag_20;
            eBattleWinOpen2(&reward);
            count_18 ^= 1;
        }
        if ((PadData.buttons.input.pressed & PAD_L2) != 0)
            eBattleWinClose2();
        eBattleWinMain2();
        if ((PadData.buttons.input.pressed & PAD_R1) != 0)
            eBattleWinOpen(&window);
        if ((PadData.buttons.input.pressed & PAD_R2) != 0)
            eBattleWinClose(&window);
        window.repeat = PadData.buttons.input.repeat;
        xglFontDebugPrintf(0, 32, padRepeatFormat, PadData.buttons.input.repeat);
        eBattleWinMain(&window);
        xglSleep();
    } while ((PadData.buttons.button_state & ENDOU_EXIT_CHORD_U) != ENDOU_EXIT_CHORD_U);
    }
}

/*
 * Number, sprite and line display test: sets up the displays and a camera, then
 * redraws them each frame until the exit chord is held.
 */
void eNumberTest(void) {
    EndouPrintLinePoint line[3];
    XglClock sp60;
    s16 sp70[2];
    StudioCamera *camera;
    StudioCamera *studio_camera;
    int primary_order;
    int number_order;
    int sprite_order;
    StudioCamera *second_camera;
    u8 sprite_tick;
    short blink_sprite_id;
    int cursor_pressed;
    EndouPrintPoint *cursor;

    /* Initialize the sprites, ribbons, lines, cursor and text tags. */
    eSpriteSet(&spr_22[0], 0x200);
    eSpriteSet(&spr_22[1], 0x200);
    eSpriteSet(&spr_22[2], 0x200);
    eSpriteSet(&spr_22[3], 0x200);
    eRibbonSet(&rbn_21[0], 1);
    eRibbonSet(&rbn_21[1], 0);
    eRibbonSet(&rbn_21[2], 0);
    eLineSet(&line_23[0], 0);
    eLineSet(&line_23[1], 0);
    eLineSet(&line_23[2], 0);
    eLineSet(&line_23[3], 0);
    eCursolSet(&cur_25, 0);
    eTagFontSet(&tgf_24[0].control, moji_26);
    eTagFontSet(&tgf_24[1].control, moji2_27);
    cursor = &d_43;
    xglRenderClearFrame();
    xglStudioChange(0);
    xglStudioGetCamera(&camera, 0);
    PadData.buttons.input.pressed = 0;
    studio_camera = camera;
    studio_camera->rotation.vector.x = 0.0f;
    studio_camera->rotation.vector.y = 0.0f;
    studio_camera->rotation.vector.z = 0.0f;
    studio_camera->position.vector.x = 0.0f;
    studio_camera->position.vector.y = 0.0f;
    studio_camera->position.vector.z = 1.0f;
    studio_camera->active = 1;
    studio_camera->state = 1;
    second_camera = xglStudioGetCamera2(0);
    second_camera->position.vector.y = 1.0f;
    second_camera->position.vector.z = 4.0f;
    second_camera->rotation.vector.x = 0.0f;
    second_camera->rotation.vector.y = 0.0f;
    second_camera->rotation.vector.z = 0.0f;
    second_camera->position.vector.x = 0.0f;
    second_camera->active = 1;
    second_camera->state = 1;
    WindowTexLoad(NULL, 1U);
    if ((PadData.buttons.button_state & ENDOU_EXIT_CHORD_U) != ENDOU_EXIT_CHORD_U) {
        primary_order = 0xFFFFFF;
        number_order = 0xFFFFFE;
        sprite_order = 0x02FFFFF0;
        do {
        /* Each frame: submit the number, sprite, ribbon and line displays. */
            xglFontDebugPrintf(0, 0, eNumberTestLabel);
            d_28.uv = 0xE00;
            d_28.vertices[0].x = 0x100;
            d_28.vertices[0].y = 0x40;
            d_28.vertices[0].depth = primary_order;
            d_28.coordinate_mode = 0;
            d_28.flags = 1;
            endPrintExtFunc(0, END_PRINT_CMD_SPRITE_QUAD, &d_28);
            line[0].x = 0x70F8;
            line[0].y = 0x84B8;
            line[1].x = 0x8F38;
            line[1].y = 0x7BF8;
            line[0].z = primary_order;
            line[0].color[1] = 0x40;
            line[0].color[0] = 0x40;
            line[0].color[3] = 0x80;
            line[0].color[2] = 0x80;
            line[1].z = primary_order;
            line[1].color[0] = 0x80;
            line[1].color[3] = 0;
            line[1].color[2] = 0;
            line[1].color[1] = 0;
            line[2].z = 0;
            endPrintDirectLine(line);
            eNumberSet(num_29, 0);
            num_29[0].x = 0x10;
            num_29[0].y = 0x80;
            num_29[0].digits = 5;
            num_29[0].work = number_order;
            num_29[0].value = -1;
            num_29[0].format = 0;
            eNumberMain(num_29);
            xglClockRead(&sp60);
            eNumberSet(&num_30[0], 0);
            num_30[0].x = 0x10;
            num_30[0].y = 0xA0;
            __builtin_memcpy(&num_30[0].value, &sp60,
                             sizeof(num_30[0].value));
            num_30[0].work = number_order;
            num_30[0].format = 2;
            eNumberMain(num_30);
            eNumberSet(&num_31[0], 0);
            num_31[0].x = 0x40;
            num_31[0].y = 0xC0;
            num_31[0].work = number_order;
            num_31[0].format = 3;
            sp70[1] = 0xC8;
            sp70[0] = 0;
            num_31[0].value = *(int *)sp70;
            num_31[0].digits = 3;
            num_31[0].colorModeFirst = 1;
            num_31[0].colorModeSecond = 2;
            num_31[0].alignment = 0;
            num_31[0].firstOnly = 0;
            eNumberMain(num_31);
            eNumberSet(&num_32[0], 0);
            num_32[0].x = 0x40;
            num_32[0].y = 0xE0;
            num_32[0].format = 7;
            num_32[0].digits = 4;
            num_32[0].value = 0x4D2;
            num_32[0].work = number_order;
            eNumberMain(num_32);
            d_33.spriteId = 1024;
            endSpriteSet(&d_33, 0xFF);
            d_33.x = 0x64;
            d_33.y = 0x60;
            d_33.work = sprite_order;
            endPrintExtFunc(d_33.work, END_PRINT_CMD_SPRITE, &d_33);
            endPrintExtFunc(0xFFFFFF, END_PRINT_CMD_LINE, line_34);
            d_35[0].spriteId = 0x31C;
            d_35[1].spriteId = 0x31F;
            endSpriteSet(&d_35[0], 0xFF);
            endSpriteSet(&d_35[1], 0xFF);
            d_35[0].x = 0xC8;
            d_35[0].y = 0x60;
            d_35[1].x = d_35[0].x + d_35[0].width - d_35[1].width;
            d_35[1].y = d_35[0].y + d_35[0].height - d_35[1].height;
            d_35[1].work = 0x02FFFFF2;
            d_35[0].work = sprite_order;
            d_35[1].color[0] = 0xFF;
            d_35[1].color[1] = 0x20;
            d_35[1].color[2] = d_35[1].color[1];
            endPrintExtFunc(d_35[0].work, END_PRINT_CMD_SPRITE, &d_35[0]);
            endPrintExtFunc(d_35[1].work, END_PRINT_CMD_SPRITE, &d_35[1]);
            sprite_tick = time_37++;
            blink_sprite_id = ((sprite_tick >> 4) & 1) + 0x10D;
            d_36.spriteId = blink_sprite_id;
            endSpriteSet(&d_36, 0xFF);
            d_36.x = 0x108;
            d_36.y = 0x60;
            d_36.work = sprite_order;
            endPrintExtFunc(d_36.work, END_PRINT_CMD_SPRITE, &d_36);
            d_38.spriteId = 0x609;
            endSpriteSet(&d_38, 0xFF);
            d_38.x = 0x108;
            d_38.y = 0x80;
            d_38.work = sprite_order;
            endPrintExtFunc(d_38.work, END_PRINT_CMD_SPRITE, &d_38);
            rbn_21[0].work = 0xFFFFF0;
            rbn_21[0].x = 0x40;
            rbn_21[0].y = 0x80;
            rbn_21[0].width = 0x100;
            rbn_21[0].height = 0x20;
            eRibbonMain(&rbn_21[0]);
            d_39.payload.number.value = 0x7B;
            d_39.argument1 = 3;
            d_39.x = 0xA;
            d_39.y = 0x40;
            d_39.work = primary_order;
            d_39.payload.number.alpha = -0x80;
            d_39.argument2 = 0;
            d_39.payload.number.blue = -0x80;
            d_39.payload.number.green = -0x80;
            d_39.payload.number.red = -0x80;
            endPrintExtFunc(d_39.work, END_PRINT_CMD_NUMBER, &d_39);
            {
            short c5 = 5, c1 = 1, c80 = 0x50;
            d_40.x = 0xA;
            d_40.work = primary_order;
            d_40.argument1 = c5;
            d_40.y = c80;
            d_40.argument2 = c1;
            d_40.payload.number.alpha = -0x80;
            d_40.payload.number.value = 0;
            d_40.payload.number.blue = -0x80;
            d_40.payload.number.green = -0x80;
            d_40.payload.number.red = -0x80;
            endPrintExtFunc(d_40.work, END_PRINT_CMD_NUMBER, &d_40);
            }
            d_41.spriteId = 258;
            d_41.x = 0xA;
            d_41.y = 0x60;
            d_41.work = primary_order;
            endPrintExtFunc(d_41.work, END_PRINT_CMD_SPRITE, &d_41);
            cursor_pressed = PadData.buttons.input.pressed & PAD_CIRCLE;
            cur_25.x = 0x10;
            cur_25.y = 0xC8;
            cur_25.work = 0xFFFFF4;
            if (cursor_pressed) {
                cur_25.state = E_MODE_SHOW_START;
            }
            eCursolMain(&cur_25);
            endPrintExtFunc(0x01000000, END_PRINT_CMD_RIBBON, d_42);
            if ((PadData.buttons.input.held & PAD_UP) != 0) {
                int cursor_y = cursor->y;

                cursor->y = cursor_y - 2;
            }
            if ((PadData.buttons.input.held & PAD_DOWN) != 0) {
                int cursor_y = cursor->y;

                cursor->y = cursor_y + 2;
            }
            endPrintExtFunc(cursor->depth, END_PRINT_CMD_POINT, cursor);
            if (PadData.buttons.input.pressed & PAD_CIRCLE) {
                spr_22[0].state = 0x22;
                spr_22[1].state = 0x22;
                spr_22[2].state = 0x22;
                spr_22[3].state = 0x22;
            }
            spr_22[0].x = 0x64;
            spr_22[0].y = 0xC8;
            spr_22[0].work = primary_order;
            eSpriteMain((ESpriteMainData *)&spr_22[0]);
            spr_22[1].y = 0xC8;
            spr_22[1].x = 0xA4;
            spr_22[1].work = primary_order;
            eSpriteMain((ESpriteMainData *)&spr_22[1]);
            spr_22[2].x = 0xC8;
            spr_22[2].y = spr_22[2].x;
            spr_22[2].work = primary_order;
            eSpriteMain((ESpriteMainData *)&spr_22[2]);
            spr_22[3].x = 0x190;
            spr_22[3].y = spr_22[3].x;
            spr_22[3].work = primary_order;
            eSpriteMain((ESpriteMainData *)&spr_22[3]);
            tgf_24[0].draw.x = 0x40;
            tgf_24[0].draw.y = 0x12C;
            tgf_24[0].draw.work = primary_order;
            eTagFontMain(&tgf_24[0].draw);
            tgf_24[1].draw.y = 0x140;
            tgf_24[1].draw.x = 0x40;
            tgf_24[1].draw.work = primary_order;
            eTagFontMain(&tgf_24[1].draw);
            d_44.points[0].depth = 0x01FFFFF0;
            d_44.points[1].depth = 0x01FFFFF0;
            d_44.points[2].depth = 0x01FFFFF0;
            d_44.points[3].depth = 0x01FFFFF0;
            d_44.points[0].y = 0x32;
            d_44.points[1].y = 0x64;
            d_44.points[2].y = 0x14;
            d_44.points[3].y = 0x82;
            d_44.colors[3].red = -0x80;
            d_44.colors[2].red = -0x80;
            d_44.colors[1].red = -0x80;
            d_44.colors[0].red = -0x80;
            d_44.colors[3].green = -0x80;
            d_44.colors[2].green = -0x80;
            d_44.colors[1].green = -0x80;
            d_44.colors[0].green = -0x80;
            d_44.colors[3].blue = -0x80;
            d_44.colors[2].blue = -0x80;
            d_44.colors[1].blue = -0x80;
            d_44.colors[0].blue = -0x80;
            d_44.colors[0].alpha = -0x80;
            d_44.colors[1].alpha = -0x80;
            d_44.colors[2].alpha = -0x80;
            d_44.colors[3].alpha = -0x80;
            d_44.points[0].x = 0;
            d_44.points[1].x = 0;
            d_44.points[2].x = 0xA0;
            d_44.points[3].x = 0xA0;
            d_44.format = 0x1A11;
            /*
             * Three line strips, each with four colored points. START and SELECT
             * pick the draw mode of each.
             */
            line_23[1].count = 4;
            line_23[1].points[0].x = 0xB4;
            line_23[1].points[0].y = 0x46;
            line_23[1].points[0].work = 0x7FFFFFFF;
            line_23[1].points[1].x = 0xB4;
            line_23[1].points[1].y = 0x78;
            line_23[1].points[1].work = 0x7FFFFFFF;
            line_23[1].points[2].x = 0x96;
            line_23[1].points[2].y = 0x96;
            line_23[1].points[2].work = 0x7FFFFFFF;
            line_23[1].points[3].x = 0x64;
            line_23[1].points[3].y = 0x96;
            line_23[1].points[3].work = 0x7FFFFFFF;
            line_23[1].points[0].red = -1;
            line_23[1].points[0].green = 0;
            line_23[1].points[0].blue = 0;
            line_23[1].points[0].alpha = -0x80;
            line_23[1].points[1].red = 0;
            line_23[1].points[1].green = 0;
            line_23[1].points[1].blue = -1;
            line_23[1].points[1].alpha = -0x80;
            line_23[1].points[2].red = 0;
            line_23[1].points[2].green = -1;
            line_23[1].points[2].blue = 0;
            line_23[1].points[2].alpha = -0x80;
            line_23[1].points[3].red = -1;
            line_23[1].points[3].green = -1;
            line_23[1].points[3].blue = -1;
            line_23[1].points[3].alpha = -0x80;
            if (PadData.buttons.input.pressed & PAD_START) {
                line_23[1].state = ELINE_STATE_DRAW_ALL;
            }
            if (PadData.buttons.input.pressed & PAD_SELECT) {
                line_23[1].state = ELINE_STATE_DRAW_SEQUENTIAL;
            }
            eLineMain(&line_23[1]);
            line_23[2].count = 4;
            line_23[2].points[0].x = 0x64;
            line_23[2].points[0].y = 0x64;
            line_23[2].points[0].work = 0x7FFFFFFF;
            line_23[2].points[1].y = 0x46;
            line_23[2].points[1].x = 0x82;
            line_23[2].points[1].work = 0x7FFFFFFF;
            line_23[2].points[2].x = 0x82;
            line_23[2].points[2].y = 0x78;
            line_23[2].points[2].work = 0x7FFFFFFF;
            line_23[2].points[3].y = 0x78;
            line_23[2].points[3].x = 0xB4;
            line_23[2].points[3].work = 0x7FFFFFFF;
            line_23[2].points[0].red = -1;
            line_23[2].points[0].green = 0;
            line_23[2].points[0].blue = 0;
            line_23[2].points[0].alpha = -0x80;
            line_23[2].points[1].red = 0;
            line_23[2].points[1].green = 0;
            line_23[2].points[1].blue = -1;
            line_23[2].points[1].alpha = -0x80;
            line_23[2].points[2].red = 0;
            line_23[2].points[2].green = -1;
            line_23[2].points[2].blue = 0;
            line_23[2].points[2].alpha = -0x80;
            line_23[2].points[3].red = -1;
            line_23[2].points[3].green = -1;
            line_23[2].points[3].blue = -1;
            line_23[2].points[3].alpha = -0x80;
            if (ENDOU_MODEL_PAD->pressed & PAD_START) {
                line_23[2].state = ELINE_STATE_DRAW_ALL;
            }
            if (ENDOU_MODEL_PAD->pressed & PAD_SELECT) {
                line_23[2].state = ELINE_STATE_DRAW_SEQUENTIAL;
            }
            eLineMain(&line_23[2]);
            line_23[3].count = 4;
            line_23[3].points[0].x = 0x82;
            line_23[3].points[0].y = 0x78;
            line_23[3].points[0].work = 0x7FFFFFFF;
            line_23[3].points[1].x = 0x64;
            line_23[3].points[1].y = 0x96;
            line_23[3].points[1].work = 0x7FFFFFFF;
            line_23[3].points[2].x = 0x64;
            line_23[3].points[2].y = 0x64;
            line_23[3].points[2].work = 0x7FFFFFFF;
            line_23[3].points[3].x = 0x96;
            line_23[3].points[3].y = 0x64;
            line_23[3].points[3].work = 0x7FFFFFFF;
            line_23[3].points[0].red = -1;
            line_23[3].points[0].green = 0;
            line_23[3].points[0].blue = 0;
            line_23[3].points[0].alpha = -0x80;
            line_23[3].points[1].red = 0;
            line_23[3].points[1].green = 0;
            line_23[3].points[1].blue = -1;
            line_23[3].points[1].alpha = -0x80;
            line_23[3].points[2].red = 0;
            line_23[3].points[2].green = -1;
            line_23[3].points[2].blue = 0;
            line_23[3].points[2].alpha = -0x80;
            line_23[3].points[3].red = -1;
            line_23[3].points[3].green = -1;
            line_23[3].points[3].blue = -1;
            line_23[3].points[3].alpha = -0x80;
            if (ENDOU_MODEL_PAD->pressed & PAD_START) {
                line_23[3].state = ELINE_STATE_DRAW_ALL;
            }
            if (ENDOU_MODEL_PAD->pressed & PAD_SELECT) {
                line_23[3].state = ELINE_STATE_DRAW_SEQUENTIAL;
            }
            eLineMain(&line_23[3]);
            endPrintExtFunc(0xFFFFFF, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_FLUSH, NULL);
            xglSleep();
        } while ((PadData.buttons.button_state & ENDOU_EXIT_CHORD_U) != ENDOU_EXIT_CHORD_U);
    }
}

/*
 * Per-frame model test update: opens the model on first use, then runs its
 * control.
 */
void eModelTestMain(EModelTestState *unit)
{
    if (unit->isReady == 0) {
        return;
    }

    if (unit->extFuncFlag == 0) {
        MenuModelUnitOpen(unit, 2);
        unit->extFuncFlag = 20;
    }

    MenuModelControl(unit);
}

/*
 * Model test: loads the menu model overlay and runs the model test unit until
 * the exit chord is held.
 */
void eModelTest(void)
{
    void *model_id;

    xglCdLoadOverlay(1);
    ACT_init();
    xglRenderClearFrame();
    xglSleep();
    MenuLoadInit();
    MenuModelInit((void *)ENDOU_MENU_MODEL_POOL);
    MenuModelMemoryInit((void *)ENDOU_MENU_MODEL_MEMORY_BASE, ENDOU_MENU_MODEL_MEMORY_SIZE);
    MenuModelMemorySet(0);
    MenuModelCreate(&model_id, 29);
    MenuModelExtFuncSet(model_id, eModelTestMain, 0);

    while ((PadData.buttons.button_state & ENDOU_EXIT_CHORD_U) != ENDOU_EXIT_CHORD_U) {
        xglFontDebugPrintf(0, 0, eModelTestLabel);
        MenuModelMain();
        xglSleep();
    }
}

void ePrintTest(void)
{
}

/*
 * Message window test: shows two message windows and a current message, and
 * lets the pad change its text, page and mode, until the exit chord is held.
 */
void eMessageTest(void)
{
    EMessageTestWindow windows[2];
    EMessageTestWindow current;
    int message_index = 0;

    WindowTexLoad(0, 0);
    endPrintInit();
    eMessageSet(&windows[0].message, msg_45[0]);
    eMessageSet(&windows[1].message, msg_45[0]);
    windows[1].message.mode = E_MODE_SHOW_START;
    eMessageSet(&current.message, text00_46);
    current.message.page_available = 1;
    eMessageTextChange(&current.message, text00_46);

    PadData.buttons.input.pressed = 0;
    current.message.x = 0x40;
    current.message.y = 300;
    current.message.draw_parameter = 0x00fffff0;
    current.message.mode = EMESSAGE_MODE_LINE_START;
    ClearEnv.color_g = 0x40;
    ClearEnv.color_r = 0;
    ClearEnv.color_b = 0;
    ClearEnv.color_a = 0;

    if ((PadData.buttons.button_state & ENDOU_EXIT_CHORD_U) != ENDOU_EXIT_CHORD_U) {
        do {
            xglFontDebugPrintf(0, 0, eMessageTestLabel);
            if ((PadData.buttons.input.pressed & PAD_CIRCLE) != 0) {
                eMessageNextPage(&current.message, 0);
            }
            if ((PadData.buttons.input.pressed & PAD_SELECT) != 0) {
                message_index += 1;
                if (message_index >= ENDOU_MESSAGE_TEST_COUNT) {
                    message_index = 0;
                }
                windows[0].message.text = msg_45[message_index];
            }
            if ((PadData.buttons.input.pressed & PAD_R1) != 0) {
                windows[0].message.mode = E_MODE_OPEN_START;
                windows[1].message.mode = E_MODE_OPEN_START;
                eMessageNextPage(&current.message, 1);
                current.message.mode = E_MODE_OPEN_START;
            }
            if ((PadData.buttons.input.pressed & PAD_R2) != 0) {
                windows[0].message.mode = E_MODE_HIDE_START;
                windows[1].message.mode = E_MODE_HIDE_START;
                current.message.mode = E_MODE_HIDE_START;
            }
            if ((PadData.buttons.input.held & PAD_L1) != 0) {
                windows[0].message.text_y_offset -= 1;
            }
            if ((PadData.buttons.input.held & PAD_L2) != 0) {
                windows[0].message.text_y_offset += 1;
            }
            if ((PadData.buttons.input.held & PAD_UP) != 0) {
                windows[0].message.color_a -= 1;
            }
            if ((PadData.buttons.input.held & PAD_DOWN) != 0) {
                windows[0].message.color_a += 1;
            }

            xglFontDebugHex(0, 0x10, windows[0].message.color_b, 2);
            windows[0].message.draw_parameter = 0x01ffffff;
            windows[1].message.draw_parameter = 0x01ffff00;
            windows[0].message.x = 100;
            windows[0].message.y = 100;
            windows[1].message.x = 0xa4;
            windows[1].message.y = 0xa4;
            eMessageDraw(&windows[1].message);
            eMessageMain(&current.message);
            endPrintExtFunc(0x00ffffff, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_FLUSH, 0);
            xglSleep();
        } while ((PadData.buttons.button_state & ENDOU_EXIT_CHORD_U) != ENDOU_EXIT_CHORD_U);
    }
}

void BgTest(void)
{
    XglPacket *packet;
    int packet_command = BG_TEST_COMMAND_BASE << BG_TEST_COMMAND_BASE_SHIFT;

    packet_command |= sRender.packetControl << BG_TEST_CONTROL_SHIFT;
    TestEnv_47.packet_write.value = packet_command |
        ((u64)BG_TEST_COMMAND_HIGH << BG_TEST_COMMAND_HIGH_SHIFT);
    packet = xglPacketGetCurrent();
    sceVif1PkRef(packet, &TestEnv_47, ENDOU_BG_ENV_QWORDS, 0, 0, 0);
}

/*
 * Loads two texture files into the work area, then uploads the textures and
 * draws them each frame until the exit chord is held.
 */
void TexturTest(void)
{
    u8 *work_start = WorkEnd;
    TexturTestHeader *texture_file =
        (TexturTestHeader *)(((u32)work_start + 15U) & 0xfffffff0U);
    int size;
    u8 *texture_end;
    void *second_file;

    size = xglCdReadFile(textureTestNewdMenuPath, texture_file, 0, 0);
    if (size < 0)
        return;
    texture_end = (u8 *)texture_file + size;
    second_file = (void *)(((u32)texture_end + 15U) & 0xfffffff0U);
    WorkEnd = texture_end;
    size = xglCdReadFile(textureTestDataMenuPath, second_file, 0, 0);
    if (size < 0)
        return;
    WorkEnd = (u8 *)second_file + size;
    xglRenderClearDepth();

    for (;;) {
        int index;
        XglPacket *packet;
        TexturTestEntry *entry;
        u16 repeated;

        BgTest();
        xglFontDebugPrintf(0, 0, textureTestLabel);
        if ((PadData.buttons.button_state & ENDOU_EXIT_CHORD_U) == ENDOU_EXIT_CHORD_U)
            break;
        index = 0;
        packet = xglPacketGetCurrent();
        entry = (TexturTestEntry *)
            ((u8 *)texture_file + texture_file->entries_offset);
        if (texture_file->entry_count > 0) {
            TexturTestUploadEnvironment *upload_environment = &eNumberTexEnv_48;

            do {
                int width = entry->width_and_pitch & 0xffffU;
                u32 pitch = entry->width_and_pitch >> 16;

                if (entry->width_and_pitch == width)
                    pitch = (width + 63) >> 6;
                upload_environment->transfer_dimensions =
                    (u32)width | ((u64)entry->height << GS_TRXREG_RRH_SHIFT);
                upload_environment->destination_parameters =
                    ((u64)((entry->vram_offset + ENDOU_TEX_VRAM_BASE) >> 6) << GS_BITBLTBUF_DBP_SHIFT) |
                    ((u64)pitch << GS_BITBLTBUF_DBW_SHIFT);
                FlushCache(0);
                sceVif1PkRef(packet, upload_environment, ENDOU_UPLOAD_ENV_QWORDS, 0, 0, 0);
                {
                    u8 *transfer_data = (u8 *)texture_file + entry->data_offset;
                    int transfer_quadwords = entry->data_quadwords;

                    entry++;
                    sceVif1PkRef(packet, transfer_data,
                                 transfer_quadwords, 0, 0, 0);
                }
                index++;
            } while (index < texture_file->entry_count);
        }
        xglFontDebugHex(0, 0x20, x_53, 5);
        repeated = PadData.buttons.input.repeat;
        if ((repeated & PAD_RIGHT) != 0)
            x_53 += 0x100;
        if ((repeated & PAD_LEFT) != 0)
            x_53 -= 0x100;
        TestEnv_52.texture_parameters =
            ((u64)(((u32)x_53 + ENDOU_TEX_VRAM_BASE) >> 6) << GS_TEX0_CBP_SHIFT) |
            0x2000000629343800ULL;
        sceVif1PkRef(packet, &TestEnv_52, ENDOU_TEX_DRAW_ENV_QWORDS, 0, 0, 0);
        xglSleep();
    }
    xglRenderClearFrame();
    WorkEnd = work_start;
}

void UmnPrintTest(void)
{
}

/*
 * Mail display test: shows two text windows, scrolls and changes the mode of
 * the first one with the pad, until the exit chord is held.
 */
void UmnMailDispTest(void)
{
    TyaUmlDispParamMailView windows[2];

    tyaUmlDispInit();
    tyaUmlDispParamReset(&windows[0], 0);
    tyaUmlDispInit();
    tyaUmlDispParamReset(&windows[1], 0);

    windows[0].mode = 2;
    windows[0].texture_x = 0x710;
    windows[0].texture_y = 0x730;
    windows[0].texture_width = 0x1e0;
    windows[0].texture_height = 0xc8;
    windows[0].view_width = 0x1e0;
    windows[0].view_height = 0x180;

    windows[1].mode = 3;
    windows[1].texture_x = 0x710;
    windows[1].texture_y = 0x808;
    windows[1].texture_width = 0x1e0;
    windows[1].texture_height = 0xc8;
    windows[1].view_width = 0x1e0;
    windows[1].view_height = 0x180;

    while ((PadData.buttons.button_state & ENDOU_EXIT_CHORD_U) != ENDOU_EXIT_CHORD_U) {
        xglFontDebugPrintf(0, 0, umnMailDispTestLabel);
        if ((PadData.buttons.input.repeat & PAD_UP) != 0) {
            windows[0].scroll_y -= 8;
        }
        if ((PadData.buttons.input.repeat & PAD_DOWN) != 0) {
            windows[0].scroll_y += 8;
        }
        if ((PadData.buttons.input.repeat & PAD_LEFT) != 0) {
            windows[0].scroll_x -= 8;
        }
        if ((PadData.buttons.input.repeat & PAD_RIGHT) != 0) {
            windows[0].scroll_x += 8;
        }
        if ((PadData.buttons.input.pressed & PAD_R1) != 0) {
            windows[0].mode = 0;
        }
        if ((PadData.buttons.input.pressed & PAD_R2) != 0) {
            windows[0].mode = 1;
        }
        if ((PadData.buttons.input.pressed & PAD_TRIANGLE) != 0) {
            windows[0].mode = 2;
        }
        if ((PadData.buttons.input.pressed & PAD_CIRCLE) != 0) {
            windows[0].mode = 3;
        }
        if ((PadData.buttons.input.pressed & PAD_SQUARE) != 0) {
            windows[0].mode = 4;
        }
        if ((PadData.buttons.input.pressed & PAD_CROSS) != 0) {
            windows[0].mode = 5;
        }
        tyaUmlDispMain(&windows[0]);
        tyaUmlDispMain(&windows[1]);
        xglSleep();
    }
}

/* Font draw callback: writes the VIF packet that reloads the test texture. */
static void e_test(FontDrawCursor *cursor, void *argument)
{
    sceVif1PkCloseDirectHLCode(cursor->packet);
    sceVif1PkRef(cursor->packet, &WinTexEnv_65,
                 ENDOU_WINTEX_ENV_QWORDS, 0, 0, 0);
    sceVif1PkRef(cursor->packet, test_data + ENDOU_XTX_HEADER_SIZE, ENDOU_XTX_PAYLOAD_QWORDS, 0, 0, 0);
    sceVif1PkCnt(cursor->packet, 0);
    sceVif1PkOpenDirectHLCode(cursor->packet, 0);
    sceVif1PkAddDirectDataN(cursor->packet, TestEnv_66, ENDOU_RELOAD_ENV_QWORDS);
    xglFontReloadTexture(cursor, 0);
}

/*
 * Texture reload test: loads the floor file, then draws kanji text through the
 * reload callback until the exit chord is held.
 */
void ReLoadTest(void)
{
    xglRenderClearFrame();
    xglSleep();
    test_data = (u8 *)(((u32)WorkEnd + 0x0fU) & 0xfffffff0U);
    WorkEnd = test_data +
        xglCdReadFile(reLoadTestFloorPath, test_data, 0, 0);

    while ((PadData.buttons.button_state & ENDOU_EXIT_CHORD_U) != ENDOU_EXIT_CHORD_U) {
        xglFontDebugPrintf(0, 0, reLoadTestLabel);
        xglFontPrint(0x40, 0x28, 0x00ffffff, kanjiTestText);
        xglFontPrintExtFunc(0x01ffffffU, e_test, 0);
        xglFontPrint(0x40, 0x48, 0x02ffffff, kanjiTestText);
        xglSleep();
    }
}

/*
 * CD event callback: copies a looked-up file name, stores a scaled file size,
 * sleeps while progress events arrive, and stops on a not-found or read error.
 */
static void endCallback(int result, int amount)
{
    switch (result) {
    case CD_EVENT_LOOKUP: {
        const char *source = (const char *)amount;
        char *destination = fname_67;
        do {
            *destination = *source++;
        } while (((unsigned int)*destination++ << 24) != 0);
        break;
    }
    case CD_EVENT_SIZE:
        size_68 = I2F(amount) * progress_percent;
        break;
    case CD_EVENT_PROGRESS:
    case CD_EVENT_PROGRESS_ASYNC:
        if (result != CD_EVENT_PROGRESS_ASYNC)
            xglSleep();
        break;
    case CD_EVENT_NOT_FOUND: {
        for (;;) {
            xglFontDebugPrintf(8, 216, not_found_format, fname_67);
            xglSleep();
        }
    }
    case CD_EVENT_READ_ERROR: {
        for (;;) {
            xglFontDebugPrintf(8, 216, read_error_format, fname_67);
            xglSleep();
        }
    }
    case CD_EVENT_DONE:
        break;
    default:
        break;
    }
}

/*
 * File load test: reads the menu file through the CD callback, then draws the
 * test texture until the exit chord is held.
 */
void FileLoadTest(void)
{
    void (*previous_callback)(int, int);
    unsigned int pad_mask;
    u64 pad_state;
    u64 *pad_words;

    xglRenderClearFrame();
    xglSleep();
    files.buffer = (u8 *)(((u32)WorkEnd + 0x7fU) & 0xffffff80U);
    WorkEnd = files.buffer + ENDOU_FILE_LOAD_BUFFER_SIZE;
    previous_callback = xglCdSetCallback(endCallback);
    xglCdReadFile(fileLoadTestMenuPath, files.buffer, 0, 0);
    files.state = 1U;
    pad_words = (u64 *)&PadData;
    pad_mask = ENDOU_EXIT_CHORD_U;
    pad_state = pad_words[5];

    if ((pad_state & pad_mask) != pad_mask)
    do {
        xglFontDebugPrintf(0, 0, fileLoadTestLabel);
        if (files.state == 1) {
            XglPacket *packet = xglPacketGetCurrent();

            sceVif1PkRef(packet, eNumberTexEnv_69, ENDOU_UPLOAD_ENV_QWORDS, 0, 0, 0);
            sceVif1PkRef(packet, files.buffer + ENDOU_XTX_HEADER_SIZE, ENDOU_XTX_PAYLOAD_QWORDS, 0, 0, 0);
            xglFontDebugHex(0, 0x20, x_71, 5);
            if ((PadData.buttons.input.repeat & PAD_RIGHT) != 0) {
                x_71 += 0x100;
            }
            if ((PadData.buttons.input.repeat & PAD_LEFT) != 0) {
                x_71 -= 0x100;
            }
            TestEnv_70[6] = ((u64)((x_71 + ENDOU_FILE_VRAM_BASE) >> 6) << GS_TEX0_CBP_SHIFT) | 0x2000000669343c00ULL;
            sceVif1PkRef(packet, TestEnv_70, ENDOU_FILE_DRAW_ENV_QWORDS, 0, 0, 0);
        }
        xglSleep();
    } while ((PadData.buttons.button_state & pad_mask) != pad_mask);
    xglCdSetCallback(previous_callback);
}

/*
 * JPEG test: saves a thumbnail into the source buffer, decodes it, and shows
 * the image until the exit chord is held.
 */
void JpegTest(void)
{
    JpegTestDisplay display;
    JpegTestDecodeRequest request;
    u8 *source_buffer;
    u8 *destination_buffer;

    source_buffer = (u8 *)(((u32)WorkEnd + 0x7fU) & 0xffffff80U);
    WorkEnd = source_buffer + ENDOU_JPEG_BUFFER_SIZE;
    memset(source_buffer, 0, ENDOU_JPEG_BUFFER_SIZE);

    destination_buffer = (u8 *)(((u32)WorkEnd + 0x7fU) & 0xffffff80U);
    WorkEnd = destination_buffer + ENDOU_JPEG_BUFFER_SIZE;
    memset(destination_buffer, 0, ENDOU_JPEG_BUFFER_SIZE);

    GameSnapShotSaveThumbnail(100, (unsigned int)source_buffer);

    memset(&request, 0, sizeof(request));
    request.source = source_buffer;
    request.destination = destination_buffer;
    request.source_width = 0;
    request.source_height = 0;
    request.max_width = 0;
    request.max_height = 0;
    xglJpegDecode(&request);

    display.x = 100;
    display.y = 100;
    display.color = 0x00ffffffU;
    display.alpha = -0x80;
    display.blue = -0x80;
    display.green = -0x80;
    display.red = -0x80;
    display.image_data = destination_buffer;

    while ((PadData.buttons.button_state & ENDOU_EXIT_CHORD_U) != ENDOU_EXIT_CHORD_U) {
        xglFontDebugPrintf(0, 0, jpegTestLabel);
        endPrintExtFunc(0, END_PRINT_CMD_THUMBNAIL, &display);
        xglSleep();
    }
}

/*
 * UML test: reads the UML test file, converts its backslash-coded fields into
 * text, and shows the result until the exit chord is held.
 */
void UmlTest(void)
{
    unsigned int source_byte;
    unsigned long character;
    unsigned char *file_buffer;
    const char *text;
    unsigned char *output;
    unsigned char *read_buffer;
    const char *file_name;

    xglRenderClearFrame();
    xglSleep();
    file_buffer = (unsigned char *)(((unsigned int)WorkEnd + 127) ^ 127);
    WorkEnd = file_buffer + ENDOU_UML_BUFFER_SIZE;
    buffer_72 = file_buffer;
    memset(file_buffer, 0, ENDOU_UML_CLEAR_SIZE);
    file_name = umlTestFilePath;
    read_buffer = file_buffer;
    xglCdReadFile(file_name, read_buffer, 0, 0);
    output = (unsigned char *)mm_73;
    text = (const char *)buffer_72;
    source_byte = (unsigned char)*text;
    goto test_character;
    do {
        character &= 0xffUL;
        if (character == '\\') {
            unsigned char digits[16];
            unsigned char value;
            int digit_count;
            int place;
            int index;
            int trailing_index;
            unsigned char trailing_count;
            unsigned char trailing_fields;
            unsigned char trailing_value;

            ++text;
            source_byte = (unsigned char)*text;
            if (source_byte == 't') {
                text += 2;
                digit_count = 0;
                value = 0;
                while ((unsigned char)((unsigned char)*text - '0') < 10) {
                    digits[digit_count++] = (unsigned char)*text++ - '0';
                }
                place = 1;
                for (index = 0; index < digit_count - 1; ++index)
                    place *= 10;
                for (index = 0; index < digit_count; ++index) {
                    value += digits[index] * place;
                    place /= 10;
                }
                *output++ = value;
                ++text;
                *output++ = '\t';
                trailing_fields = 2;
                trailing_count = 0;
                for (; trailing_fields != 0; --trailing_fields) {
                    while ((unsigned char)((unsigned char)*text - '0') < 10) {
                        digits[trailing_count] = (unsigned char)*text - '0';
                        ++trailing_count;
                        ++text;
                    }
                    place = 1;
                    for (trailing_index = 0; trailing_index < trailing_count - 1; ++trailing_index)
                        place *= 10;
                    trailing_value = 0;
                    for (trailing_index = 0; trailing_index < trailing_count; ++trailing_index) {
                        trailing_value += digits[trailing_index] * place;
                        place /= 10;
                    }
                    ++text;
                }
                source_byte = (unsigned char)*text;
            }
        } else {
            *output = source_byte;
            ++output;
            ++text;
            source_byte = (unsigned char)*text;
        }
test_character:
        character = source_byte;
    } while (character != 0);
    *output = 0;
    while ((PadData.buttons.button_state & ENDOU_EXIT_CHORD_U) != ENDOU_EXIT_CHORD_U) {
        xglFontDebugPrintf(0, 0, umlTestLabel);
        xglFontPrint(0, 64, 0x00fffff0, umlTestColorControl);
        xglFontPrint(0, 64, 0x00ffffff, mm_73);
        xglSleep();
    }
}

/*
 * Party debug page: moves the roster cursor, toggles the take-AGWS flag of the
 * selected member on Circle, and draws the roster.
 */
void PauseMenuPagePartyDebugTakeAgws(void)
{
    int i;

    switch (PadData.buttons.input.repeat) {
    case PAD_LEFT:
        Select_74 -= 1;
        break;
    case PAD_RIGHT:
        Select_74 += 1;
        break;
    case PAD_UP:
        Select_74 -= 2;
        break;
    case PAD_DOWN:
        Select_74 += 2;
        break;
    default:
        break;
    }

    if (Select_74 < 0) {
        Select_74 += PARTY_DEBUG_ROSTER_COUNT;
    }
    Select_74 %= PARTY_DEBUG_ROSTER_COUNT;

    if (PadData.buttons.input.pressed & PAD_CIRCLE) {
        if (PartyTakeAgwsCheck(Select_74 + PARTY_DEBUG_AGWS_FIRST_ID) != 0) {
            PartyTakeAgwsOff(Select_74 + PARTY_DEBUG_AGWS_FIRST_ID);
        } else {
            PartyTakeAgwsOn(Select_74 + PARTY_DEBUG_AGWS_FIRST_ID);
        }
    }

    xglFontDebugPrintf(8, 0x20, takeAgwsHeadingLabel);
    xglFontDebugPrintf((Select_74 % 2) * 0x80 + 8, (Select_74 / 2) * 0x10 + 0x30, menuCursorText);
    for (i = 0; i < PARTY_DEBUG_ROSTER_COUNT; i++) {
        xglFontDebugPrintf((i % 2) * 0x80 + 0x10, (i / 2) * 0x10 + 0x30, MenuCharNameGet(i + PARTY_DEBUG_AGWS_FIRST_ID));
        xglFontDebugPrintf((i % 2) * 0x80 + 0x68, (i / 2) * 0x10 + 0x30, text00_75[PartyTakeAgwsCheck(i + PARTY_DEBUG_AGWS_FIRST_ID)]);
    }
    xglFontDebugPrintf(8, 0xc0, takeAgwsControlsLabel);
}

/*
 * Party debug page: moves the roster cursor, toggles the locked-party flag of
 * the selected member on Circle, and draws the roster.
 */
void PauseMenuPagePartyDebugLockParty(void)
{
    int i;

    switch (PadData.buttons.input.repeat) {
    case PAD_LEFT:
        Select_76 -= 1;
        break;
    case PAD_RIGHT:
        Select_76 += 1;
        break;
    case PAD_UP:
        Select_76 -= 2;
        break;
    case PAD_DOWN:
        Select_76 += 2;
        break;
    default:
        break;
    }

    if (Select_76 < 0) {
        Select_76 += PARTY_DEBUG_ROSTER_COUNT;
    }
    Select_76 %= PARTY_DEBUG_ROSTER_COUNT;

    if (PadData.buttons.input.pressed & PAD_CIRCLE) {
        if (PartyLockPartyCheck(Select_76 + 1) != 0) {
            PartyLockPartyOff(Select_76 + 1);
        } else {
            PartyLockPartyOn(Select_76 + 1);
        }
    }

    xglFontDebugPrintf(8, 0x20, lockPartyHeadingLabel);
    xglFontDebugPrintf((Select_76 % 2) * 0x80 + 8, (Select_76 / 2) * 0x10 + 0x30, menuCursorText);
    for (i = 0; i < PARTY_DEBUG_ROSTER_COUNT; i++) {
        xglFontDebugPrintf((i % 2) * 0x80 + 0x10, (i / 2) * 0x10 + 0x30, MenuCharNameGet(i + 1));
        xglFontDebugPrintf((i % 2) * 0x80 + 0x58, (i / 2) * 0x10 + 0x30, text00_77[PartyLockPartyCheck(i + 1)]);
    }
    xglFontDebugPrintf(8, 0xc0, toggleControlsLabel);
}

/*
 * Party debug page: moves the roster cursor, toggles the out-friend flag of the
 * selected member on Circle, and draws the roster.
 */
void PauseMenuPagePartyDebugOutFriend(void)
{
    int i;

    switch (PadData.buttons.input.repeat) {
    case PAD_LEFT:
        Select_78 -= 1;
        break;
    case PAD_RIGHT:
        Select_78 += 1;
        break;
    case PAD_UP:
        Select_78 -= 2;
        break;
    case PAD_DOWN:
        Select_78 += 2;
        break;
    default:
        break;
    }

    if (Select_78 < 0) {
        Select_78 += PARTY_DEBUG_ROSTER_COUNT;
    }
    Select_78 %= PARTY_DEBUG_ROSTER_COUNT;

    if (PadData.buttons.input.pressed & PAD_CIRCLE) {
        if (PartyOutFriendCheck(Select_78 + 1) != 0) {
            PartyOutFriendOff(Select_78 + 1);
        } else {
            PartyOutFriendOn(Select_78 + 1);
        }
    }

    xglFontDebugPrintf(8, 0x20, outFriendHeadingLabel);
    xglFontDebugPrintf((Select_78 % 2) * 0x80 + 8, (Select_78 / 2) * 0x10 + 0x30, menuCursorText);
    for (i = 0; i < PARTY_DEBUG_ROSTER_COUNT; i++) {
        xglFontDebugPrintf((i % 2) * 0x80 + 0x10, (i / 2) * 0x10 + 0x30, MenuCharNameGet(i + 1));
        xglFontDebugPrintf((i % 2) * 0x80 + 0x58, (i / 2) * 0x10 + 0x30, text00_79[PartyOutFriendCheck(i + 1)]);
    }
    xglFontDebugPrintf(8, 0xc0, toggleControlsLabel);
}

/*
 * Party debug page: moves the roster cursor, toggles the friend flag of the
 * selected member on Circle, and draws the roster.
 */
void PauseMenuPagePartyDebugFriend(void)
{
    int i;

    switch (PadData.buttons.input.repeat) {
    case PAD_LEFT:
        Select_80 -= 1;
        break;
    case PAD_RIGHT:
        Select_80 += 1;
        break;
    case PAD_UP:
        Select_80 -= 2;
        break;
    case PAD_DOWN:
        Select_80 += 2;
        break;
    default:
        break;
    }

    if (Select_80 < 0) {
        Select_80 += PARTY_DEBUG_FRIEND_COUNT;
    }
    Select_80 %= PARTY_DEBUG_FRIEND_COUNT;

    if (PadData.buttons.input.pressed & PAD_CIRCLE) {
        if (PartyFriendCheck(Select_80 + 1) != 0) {
            PartyFriendOff(Select_80 + 1);
        } else {
            PartyFriendOn(Select_80 + 1);
        }
    }

    xglFontDebugPrintf(8, 0x20, friendHeadingLabel);
    xglFontDebugPrintf((Select_80 % 2) * 0x80 + 8, (Select_80 / 2) * 0x10 + 0x30, menuCursorText);
    for (i = 0; i < PARTY_DEBUG_FRIEND_COUNT; i++) {
        xglFontDebugPrintf((i % 2) * 0x80 + 0x10, (i / 2) * 0x10 + 0x30, MenuCharNameGet(i + 1));
        xglFontDebugPrintf((i % 2) * 0x80 + 0x58, (i / 2) * 0x10 + 0x30, text00_81[PartyFriendCheck(i + 1)]);
    }
    xglFontDebugPrintf(8, 0xc0, friendControlsLabel);
}

/*
 * Party debug page: Up/Down selects a battle member row, and Left/Right
 * changes the character or attack position of that row.
 */
void PauseMenuPagePartyDebugAttacker(void)
{
    PartyDataPrefix *party;
    int character;
    int position;
    int i;

    if (PadData.buttons.input.repeat & PAD_UP) {
        if (Select_82 == 0) {
            Select_82 = PARTY_DEBUG_ATTACKER_ROW_LAST;
        } else {
            Select_82 -= 1;
        }
    }
    if (PadData.buttons.input.repeat & PAD_DOWN) {
        if (Select_82 == PARTY_DEBUG_ATTACKER_ROW_LAST) {
            Select_82 = 0;
        } else {
            Select_82 += 1;
        }
    }

    if (Select_82 < PARTY_DEBUG_ATTACK_SLOT_COUNT) {
        party = PartyDataGet();
        character = party->attack_slots[Select_82].party_id;
        if (PadData.buttons.input.repeat & PAD_LEFT) {
            character = (character != 0) ? character - 1 : PARTY_DEBUG_ID_MAX;
        }
        if (PadData.buttons.input.repeat & PAD_RIGHT) {
            character = (character != PARTY_DEBUG_ID_MAX) ? character + 1 : 0;
        }
        party = PartyDataGet();
        party->attack_slots[Select_82].party_id = character;
    } else {
        party = PartyDataGet();
        position = party->attack_slots[Select_82 - PARTY_DEBUG_ATTACK_SLOT_COUNT].attack_position;
        if (PadData.buttons.input.repeat & PAD_LEFT) {
            position = (position != 0) ? position - 1 : PARTY_DEBUG_ATTACK_POSITION_MAX;
        }
        if (PadData.buttons.input.repeat & PAD_RIGHT) {
            position = (position != PARTY_DEBUG_ATTACK_POSITION_MAX) ? position + 1 : 0;
        }
        party = PartyDataGet();
        party->attack_slots[Select_82 - PARTY_DEBUG_ATTACK_SLOT_COUNT].attack_position = position;
    }

    party = PartyDataGet();
    xglFontDebugPrintf(8, 0x20, attackerHeadingLabel);
    xglFontDebugPrintf(8, Select_82 * 0x10 + 0x30, menuCursorText);
    for (i = 0; i < PARTY_DEBUG_ATTACK_SLOT_COUNT; i++) {
        xglFontDebugPrintf(0x10, i * 0x10 + 0x30, text01_84[i]);
        xglFontDebugPrintf(0x30, i * 0x10 + 0x30, text02_85[0]);
        xglFontDebugPrintf(0x70, i * 0x10 + 0x30, MenuCharNameGet(party->attack_slots[i].party_id));
    }
    for (i = 0; i < PARTY_DEBUG_ATTACK_SLOT_COUNT; i++) {
        xglFontDebugPrintf(0x10, i * 0x10 + 0x60, text01_84[i]);
        xglFontDebugPrintf(0x30, i * 0x10 + 0x60, text02_85[1]);
        xglFontDebugPrintf(0x70, i * 0x10 + 0x60, text00_83[party->attack_slots[i].attack_position]);
    }
    xglFontDebugPrintf(8, 0xc0, attackerControlsLabel);
}

/*
 * Party debug menu: Square moves to the next page, then runs the current page.
 */
void PauseMenuPagePartyDebug(void)
{
    xglFontDebugPrintf(8, 0x10, partyDebugTitleLabel);
    xglFontDebugPrintf(8, 0xb0, partyDebugToggleLabel);

    if (PadData.buttons.input.repeat & PAD_SQUARE) {
        if (TopMode_86 == PARTY_DEBUG_PAGE_TAKE_AGWS) {
            TopMode_86 = PARTY_DEBUG_PAGE_FRIEND;
        } else {
            TopMode_86 += 1;
        }
    }

    switch (TopMode_86) {
    case PARTY_DEBUG_PAGE_FRIEND:
        PauseMenuPagePartyDebugFriend();
        return;
    case PARTY_DEBUG_PAGE_ATTACKER:
        PauseMenuPagePartyDebugAttacker();
        return;
    case PARTY_DEBUG_PAGE_OUT_FRIEND:
        PauseMenuPagePartyDebugOutFriend();
        return;
    case PARTY_DEBUG_PAGE_LOCK_PARTY:
        PauseMenuPagePartyDebugLockParty();
        return;
    case PARTY_DEBUG_PAGE_TAKE_AGWS:
        PauseMenuPagePartyDebugTakeAgws();
        break;
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* JP: 清算画面テスト  △
 * EN: Settlement screen test  Triangle */
const char settlementScreenTestLabel[24] = "\013\300\266\273\273\262\350\314\314\245\306\245\271\245\310\241\241\241\241\242\244";

/* JP: テキスト　□
 * EN: Text  Square */
const char textTestSquareLabel[16] = "\013\245\306\245\255\245\271\245\310\241\241\242\242";

/* JP: メッセージテスト　×
 * EN: Message test  X */
const char messageTestCrossLabel[24] = "\013\245\341\245\303\245\273\241\274\245\270\245\306\245\271\245\310\241\241\241\337";

/* JP: いろいろテスト  ○
 * EN: Misc test  Circle */
const char miscTestCircleLabel[24] = "\013\244\244\244\355\244\244\244\355\245\306\245\271\245\310\241\241\241\241\241\373";

/* JP: モデルテスト  Ｌ１
 * EN: Model test  L1 */
const char modelTestL1Label[24] = "\013\245\342\245\307\245\353\245\306\245\271\245\310\241\241\241\241\243\314\243\261";

/* JP: ロードテスト  Ｌ２
 * EN: Load test  L2 */
const char loadTestL2Label[24] = "\013\245\355\241\274\245\311\245\306\245\271\245\310\241\241\241\241\243\314\243\262";

/* JP: テクスチャテストＲ１
 * EN: Texture test R1 */
const char textureTestR1Label[24] = "\013\245\306\245\257\245\271\245\301\245\343\245\306\245\271\245\310\243\322\243\261";

/* JP: ＪＰＥＧテスト　Ｒ２
 * EN: JPEG test  R2 */
const char jpegTestR2Label[24] = "\013\243\312\243\320\243\305\243\307\245\306\245\271\245\310\241\241\243\322\243\262";

/* JP: バトルウィンドー　↑
 * EN: Battle window  Up */
const char battleWindowTestUpLabel[24] = "\013\245\320\245\310\245\353\245\246\245\243\245\363\245\311\241\274\241\241\242\254";

/* JP: セーブテスト  ↓
 * EN: Save test  Down */
const char saveTestDownLabel[24] = "\013\245\273\241\274\245\326\245\306\245\271\245\310\241\241\241\241\241\241\242\255";

/* JP: モデルテスト  →
 * EN: Model test  Right */
const char modelTestRightLabel[24] = "\013\245\342\245\307\245\353\245\306\245\271\245\310\241\241\241\241\241\241\242\252";

/* JP: ショップテスト  ←
 * EN: Shop test  Left */
const char shopTestLeftLabel[24] = "\013\245\267\245\347\245\303\245\327\245\306\245\271\245\310\241\241\241\241\242\253";

/* JP: テキストテスト
 * EN: Text test */
const char textTestLabel[16] = "\013\245\306\245\255\245\271\245\310\245\306\245\271\245\310";

const char shopDataPath[32] = "data\\endou\\shop\\shopdata.bin";

/* JP: ショップテスト
 * EN: Shop test */
const char shopTestLabel[16] = "\013\245\267\245\347\245\303\245\327\245\306\245\271\245\310";

/* JP: ナンバーチェンジ　↑↓
 * EN: Number change  Up/Down */
const char shopNumberChangeLabel[24] = "\013\245\312\245\363\245\320\241\274\245\301\245\247\245\363\245\270\241\241\242\254\242\255";

/* JP: 実行　○  終了　×
 * EN: Run Circle  Quit X */
const char shopRunQuitLabel[24] = "\013\274\302\271\324\241\241\241\373\241\241\241\241\275\252\316\273\241\241\241\337";

/* Text: {reset}ShopNo = %2d */
const char shopNoFormat[24] = "\013ShopNo = %2d";

const EndouTestLighting modelTestLighting = { .ambient = { 0.3f, 0.3f, 0.3f, 1.0f }, .parallel_intensity = { { 0.6f, 0.6f, 0.6f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } }, .direction = { { .x = 0.0f, .y = 0.0f, .z = 1.0f, .w = 1.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f } } };

const char modelTestMotionName[32] = "newd\\motion\\chaos_cf.fpk";

const DoublewordAlignedVector4 modelTestQuadWordTable[4] = {
    { -0.5f, 0.1f, 0.0f, 1.0f },
    { -0.5f, -0.1f, 0.0f, 1.0f },
    { 0.5f, 0.1f, 0.0f, 1.0f },
    { 0.5f, -0.1f, 0.0f, 1.0f },
};

/* JP: もでるてすと
 * EN: Model test */
const char modelTestHiraganaText[16] = "\244\342\244\307\244\353\244\306\244\271\244\310";

/* JP: モデルてすと
 * EN: Model test */
const char modelTestKatakanaHiraganaText[16] = "\245\342\245\307\245\353\244\306\244\271\244\310";

/* JP: もでるテスト
 * EN: Model test */
const char modelTestHiraganaKatakanaText[16] = "\244\342\244\307\244\353\245\306\245\271\245\310";

/* JP: モデルテスト
 * EN: Model test */
const char modelTestKatakanaText[16] = "\245\342\245\307\245\353\245\306\245\271\245\310";

/* JP: バトルウィンドーテスト
 * EN: Battle window test */
const char battleWindowTestLabel[24] = "\013\245\320\245\310\245\353\245\246\245\243\245\363\245\311\241\274\245\306\245\271\245\310";

/* JP: Ｅコンドルパサー
 * EN: E. Condor Pasa */
const char condorPasaName[24] = "\243\305\245\263\245\363\245\311\245\353\245\321\245\265\241\274";

/* JP: 誘惑 / 誘惑誘惑
 * EN: Temptation / Temptation temptation */
const char temptationMessageText[16] = "\315\266\317\307\n\315\266\317\307\315\266\317\307";

/* JP: スロウ / data\endou\bustup.xtx
 * EN: Slow / data\endou\bustup.xtx */
const char slowMessageText[40] = "\241\241\245\271\245\355\245\246\n\000\000\000\000\000\000\000data\\endou\\bustup.xtx";

/* JP: いろいろテスト
 * EN: Misc test */
const char eNumberTestLabel[16] = "\013\244\244\244\355\244\244\244\355\245\306\245\271\245\310";

/* JP: メッセージテスト
 * EN: Message test */
const char eMessageTestLabel[24] = "\013\245\341\245\303\245\273\241\274\245\270\245\306\245\271\245\310";

const char textureTestNewdMenuPath[24] = "newd\\endou\\menu01.xtx";

const char textureTestDataMenuPath[24] = "data\\endou\\menu01.xtx";

/* JP: テクスチャーテスト
 * EN: Texture test */
const char textureTestLabel[24] = "\013\245\306\245\257\245\271\245\301\245\343\241\274\245\306\245\271\245\310";

/* JP: Ｕ．Ｍ．Ｎ．ＭａｉｌＤｉｓｐ
 * EN: U.M.N. MailDisp */
const char umnMailDispTestLabel[32] = "\013\243\325\241\245\243\315\241\245\243\316\241\245\243\315\243\341\243\351\243\354\243\304\243\351\243\363\243\360";

const char reLoadTestFloorPath[24] = "data\\endou\\floor.xtx";

/* JP: リロードテスト
 * EN: Reload test */
const char reLoadTestLabel[16] = "\013\245\352\245\355\241\274\245\311\245\306\245\271\245\310";

/* JP: 漢字のテスト０１２３ＡＢＣＤ
 * EN: Kanji test 0123ABCD */
const char kanjiTestText[32] = "\264\301\273\372\244\316\245\306\245\271\245\310\243\260\243\261\243\262\243\263\243\301\243\302\243\303\243\304";

const char fileLoadTestMenuPath[24] = "data\\endou\\menu00.xtx";

/* JP: ロードテスト
 * EN: Load test */
const char fileLoadTestLabel[16] = "\013\245\355\241\274\245\311\245\306\245\271\245\310";

/* JP: ＪＰＥＧテスト
 * EN: JPEG test */
const char jpegTestLabel[16] = "\013\243\312\243\320\243\305\243\307\245\306\245\271\245\310";

const char umlTestFilePath[24] = "data\\endou\\test00.txt";

/* JP: ＵＭＬテスト
 * EN: UML test */
const char umlTestLabel[16] = "\013\243\325\243\315\243\314\245\306\245\271\245\310";

/* Text: {effect 0}{line height 12}{color #AD0000} */
const char umlTestColorControl[16] = "\r\000\025\002\001\014\014\255";

/* JP: ー　ＡＧＷＳ設定　ー
 * EN: - AGWS settings - */
const char takeAgwsHeadingLabel[24] = "\241\274\241\241\243\301\243\307\243\327\243\323\300\337\304\352\241\241\241\274";

/* JP: 十字キー：カーソル移動  ○：あり／なし
 * EN: D-pad: move cursor  Circle: yes/no */
const char takeAgwsControlsLabel[48] = "\275\275\273\372\245\255\241\274\241\247\245\253\241\274\245\275\245\353\260\334\306\260\241\241\241\241\241\241\241\373\241\247\244\242\244\352\241\277\244\312\244\267";

/* JP: ー　ロックパーティー設定　ー
 * EN: - Locked party settings - */
const char lockPartyHeadingLabel[32] = "\241\274\241\241\245\355\245\303\245\257\245\321\241\274\245\306\245\243\241\274\300\337\304\352\241\241\241\274";

/* JP: 十字キー：カーソル移動  ○：オン／オフ
 * EN: D-pad: move cursor  Circle: on/off */
const char toggleControlsLabel[48] = "\275\275\273\372\245\255\241\274\241\247\245\253\241\274\245\275\245\353\260\334\306\260\241\241\241\241\241\241\241\373\241\247\245\252\245\363\241\277\245\252\245\325";

/* JP: ー　アウトフレンド設定　ー
 * EN: - Out-friend settings - */
const char outFriendHeadingLabel[32] = "\241\274\241\241\245\242\245\246\245\310\245\325\245\354\245\363\245\311\300\337\304\352\241\241\241\274";

/* JP: ー　フレンド設定　ー
 * EN: - Friend settings - */
const char friendHeadingLabel[24] = "\241\274\241\241\245\325\245\354\245\363\245\311\300\337\304\352\241\241\241\274";

/* JP: 十字キー：カーソル移動  ○：いる／いない
 * EN: D-pad: move cursor  Circle: present/absent */
const char friendControlsLabel[48] = "\275\275\273\372\245\255\241\274\241\247\245\253\241\274\245\275\245\353\260\334\306\260\241\241\241\241\241\241\241\373\241\247\244\244\244\353\241\277\244\244\244\312\244\244";

/* JP: ＮＵＬＬ
 * EN: NULL */
const char nullText[16] = "\243\316\243\325\243\314\243\314";

/* JP: ポジション
 * EN: Position */
const char positionText[16] = "\245\335\245\270\245\267\245\347\245\363";

/* JP: ー　バトルメンバー設定　ー
 * EN: - Battle member settings - */
const char attackerHeadingLabel[32] = "\241\274\241\241\245\320\245\310\245\353\245\341\245\363\245\320\241\274\300\337\304\352\241\241\241\274";

/* JP: ↑↓：カーソル移動　←→：キャラ／ポジション変更
 * EN: Up/Down: move cursor  Left/Right: change character/position */
const char attackerControlsLabel[56] = "\242\254\242\255\241\247\245\253\241\274\245\275\245\353\260\334\306\260\241\241\242\253\242\252\241\247\245\255\245\343\245\351\241\277\245\335\245\270\245\267\245\347\245\363\312\321\271\271";

/* JP: ＝＝　パーティーデバックメニュー　＝＝
 * EN: == Party debug menu == */
const char partyDebugTitleLabel[40] = "\241\341\241\341\241\241\245\321\241\274\245\306\245\243\241\274\245\307\245\320\245\303\245\257\245\341\245\313\245\345\241\274\241\241\241\341\241\341";

/* JP: □：設定切り替え
 * EN: Square: toggle setting */
const char partyDebugToggleLabel[32] = "\242\242\241\247\300\337\304\352\300\332\244\352\302\330\244\250";

/* JP: グランプリ
 * EN: Grand Prix */
const char grandPrixText[16] = "\245\260\245\351\245\363\245\327\245\352";

/* JP: 有馬記念
 * EN: Arima Kinen */
const char arimaKinenText[16] = "\315\255\307\317\265\255\307\260";

/* JP: ２歳牝馬チャンピオン
 * EN: 2-year-old fillies' champion */
const char twoYearOldFilliesChampionText[24] = "\243\262\272\320\314\306\307\317\245\301\245\343\245\363\245\324\245\252\245\363";

/* JP: 阪神ＪＦ
 * EN: Hanshin Juvenile Fillies */
const char hanshinJuvenileFilliesText[16] = "\272\345\277\300\243\312\243\306";

/* JP: 古馬王道　世界の壁
 * EN: Older horses' royal road: the world's wall */
const char olderHorsesRoyalRoadText[24] = "\270\305\307\317\262\246\306\273\241\241\300\244\263\246\244\316\312\311";

/* JP: ジャパンカップ
 * EN: Japan Cup */
const char japanCupText[16] = "\245\270\245\343\245\321\245\363\245\253\245\303\245\327";

/* JP: 秋のマイル王決定戦
 * EN: Autumn mile championship */
const char autumnMileChampionshipText[24] = "\275\251\244\316\245\336\245\244\245\353\262\246\267\350\304\352\300\357";

/* JP: マイルＣＳ
 * EN: Mile Championship */
const char mileChampionshipText[16] = "\245\336\245\244\245\353\243\303\243\323";

/* JP: 牝馬女王決定戦
 * EN: Fillies' and mares' championship */
const char filliesMaresChampionshipText[16] = "\314\306\307\317\275\367\262\246\267\350\304\352\300\357";

/* JP: エリザベス女王杯
 * EN: Queen Elizabeth II Cup */
const char queenElizabethIiCupText[24] = "\245\250\245\352\245\266\245\331\245\271\275\367\262\246\307\325";

/* JP: 古馬王道　魔の天皇賞
 * EN: Older horses' royal road: the cursed Tenno Sho */
const char olderHorsesCursedTennoShoText[24] = "\270\305\307\317\262\246\306\273\241\241\313\342\244\316\305\267\271\304\276\336";

/* JP: 天皇賞（秋）
 * EN: Tenno Sho (Autumn) */
const char tennoShoAutumnText[16] = "\305\267\271\304\276\336\241\312\275\251\241\313";

/* JP: 牡馬クラシック第三弾
 * EN: Third leg of the colts' classics */
const char coltsClassicsThirdLegText[24] = "\262\264\307\317\245\257\245\351\245\267\245\303\245\257\302\350\273\260\303\306";

/* JP: 牝馬クラシック第三弾
 * EN: Third leg of the fillies' classics */
const char filliesClassicsThirdLegText[24] = "\314\306\307\317\245\257\245\351\245\267\245\303\245\257\302\350\273\260\303\306";

/* JP: 秋の短距離王決定戦
 * EN: Autumn sprint championship */
const char autumnSprintChampionshipText[24] = "\275\251\244\316\303\273\265\367\316\245\262\246\267\350\304\352\300\357";

/* JP: スプリンターズＳ
 * EN: Sprinters Stakes */
const char sprintersStakesText[24] = "\245\271\245\327\245\352\245\363\245\277\241\274\245\272\243\323";

/* JP: 春のグランプリ
 * EN: Spring Grand Prix */
const char springGrandPrixText[16] = "\275\325\244\316\245\260\245\351\245\363\245\327\245\352";

/* JP: 宝塚記念
 * EN: Takarazuka Kinen */
const char takarazukaKinenText[16] = "\312\365\304\315\265\255\307\260";

/* JP: 春のマイル王決定戦
 * EN: Spring mile championship */
const char springMileChampionshipText[24] = "\275\325\244\316\245\336\245\244\245\353\262\246\267\350\304\352\300\357";

/* JP: 安田記念
 * EN: Yasuda Kinen */
const char yasudaKinenText[16] = "\260\302\305\304\265\255\307\260";

/* JP: 牡馬クラシック第二弾　東京優駿
 * EN: Second leg of the colts' classics: Tokyo Yushun */
const char coltsClassicsSecondLegText[32] = "\262\264\307\317\245\257\245\351\245\267\245\303\245\257\302\350\306\363\303\306\241\241\305\354\265\376\315\245\275\331";

/* JP: 日本ダービー
 * EN: Japanese Derby */
const char japaneseDerbyText[16] = "\306\374\313\334\245\300\241\274\245\323\241\274";

/* JP: 牝馬クラシック第二弾　牝馬優駿
 * EN: Second leg of the fillies' classics: Yushun Himba */
const char filliesClassicsSecondLegText[32] = "\314\306\307\317\245\257\245\351\245\267\245\303\245\257\302\350\306\363\303\306\241\241\314\306\307\317\315\245\275\331";

/* JP: オークス
 * EN: Oaks */
const char oaksText[16] = "\245\252\241\274\245\257\245\271";

/* JP: 外国産馬３歳マイル王決定戦
 * EN: Championship for foreign-bred 3-year-old milers */
const char foreignBredMilersChampionshipText[32] = "\263\260\271\361\273\272\307\317\243\263\272\320\245\336\245\244\245\353\262\246\267\350\304\352\300\357";

/* JP: ＮＨＫマイルＣ
 * EN: NHK Mile Cup */
const char nhkMileCupText[16] = "\243\316\243\310\243\313\245\336\245\244\245\353\243\303";

/* JP: 春の古馬長距離王決定戦
 * EN: Spring long-distance championship for older horses */
const char springLongDistanceChampionshipText[24] = "\275\325\244\316\270\305\307\317\304\271\265\367\316\245\262\246\267\350\304\352\300\357";

/* JP: 天皇賞（春）
 * EN: Tenno Sho (Spring) */
const char tennoShoSpringText[16] = "\305\267\271\304\276\336\241\312\275\325\241\313";

/* JP: 牡馬クラシック第一弾
 * EN: First leg of the colts' classics */
const char coltsClassicsFirstLegText[24] = "\262\264\307\317\245\257\245\351\245\267\245\303\245\257\302\350\260\354\303\306";

/* JP: 牝馬クラシック第一弾
 * EN: First leg of the fillies' classics */
const char filliesClassicsFirstLegText[24] = "\314\306\307\317\245\257\245\351\245\267\245\303\245\257\302\350\260\354\303\306";

/* JP: 春の短距離王決定戦
 * EN: Spring sprint championship */
const char springSprintChampionshipText[24] = "\275\325\244\316\303\273\265\367\316\245\262\246\267\350\304\352\300\357";

/* JP: 高松宮記念
 * EN: Takamatsunomiya Kinen */
const char takamatsunomiyaKinenText[16] = "\271\342\276\276\265\334\265\255\307\260";

/* JP: ダート王決定戦
 * EN: Dirt championship */
const char dirtChampionshipText[16] = "\245\300\241\274\245\310\262\246\267\350\304\352\300\357";

/* JP: フェブラリーＳ
 * EN: February Stakes */
const char februaryStakesText[16] = "\245\325\245\247\245\326\245\351\245\352\241\274\243\323";

const char modelTestFpkName[152] = { 110, 101, 119, 100, 92, 121, 97, 109, 97, 109, 111, 116, 111, 92, 102, 112, 107, 92, 97, 103, 48, 50, 46, 102, 112, 107, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -51, -52, 76, 62, -51, -52, 76, 62, -51, -52, 76, 62, 0, 0, -128, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -128, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -128, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -128, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -128, 63, 0, 0, -128, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -128, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -128, 63 };

/* JP: スペシャルウィーク /  グラスワンダー / エルコンドルパサー
 * EN: Special Week / Grass Wonder / El Condor Pasa */
const char horseNameListText[64] = "\r\002\n\245\271\245\332\245\267\245\343\245\353\245\246\245\243\241\274\245\257\n\014\200\000\000\245\260\245\351\245\271\245\357\245\363\245\300\241\274\n\245\250\245\353\245\263\245\363\245\311\245\353\245\321\245\265\241\274";

/* JP: ?あ / い / う / え / お
 * EN: A / I / U / E / O */
const char vowelKanaListText[24] = "\r\002\031\001\014\?\000\000\244\242\n\244\244\n\244\246\n\244\250\n\244\252";

/* JP: 天皇賞春秋連覇 / スペシャルウィーク / グランプリ３連覇 / グラスワンダー / エルコンドルパサー / サンデーサイレンス / 皐月賞 / 日本ダービー / 菊花賞
 * EN: Tenno Sho spring/autumn double / Special Week / Grand Prix triple / Grass Wonder / El Condor Pasa / Sunday Silence / Satsuki Sho / Japanese Derby / Kikuka Sho */
const char winningRaceListText[144] = "\r\002\014\037\037\037\305\267\271\304\276\336\275\325\275\251\317\242\307\306\n\245\271\245\332\245\267\245\343\245\353\245\246\245\243\241\274\245\257\n\245\260\245\351\245\363\245\327\245\352\243\263\317\242\307\306\n\245\260\245\351\245\271\245\357\245\363\245\300\241\274\n\245\250\245\353\245\263\245\363\245\311\245\353\245\321\245\265\241\274\n\245\265\245\363\245\307\241\274\245\265\245\244\245\354\245\363\245\271\n\273\251\267\356\276\336\n\306\374\313\334\245\300\241\274\245\323\241\274\n\265\306\262\326\276\336";

/* JP: ＰｌａｙＳｔａｔｉｏｎ２ /  ゼノサ 璽  /  ＭＥＭＯＲＹＣＡＲＤ /  差込口 ＭＥＭＯＲＹＣＡＲＤ /  差込口２ ハードディスク
 * EN: PlayStation 2 / Xenosa(ga) (seal) / MEMORY CARD / slot MEMORY CARD / slot 2 Hard disk */
const char deviceListText[144] = "\015\002\031\001\031\002\243\320\243\354\243\341\243\371\243\323\243\364\243\341\243\364\243\351\243\357\243\356\243\262\012\031\000\245\274\245\316\245\265\241"
    "\274\245\254\012\031\001\014\200\000\000\243\315\243\305\243\315\243\317\243\322\243\331\243\303\243\301\243\322\243\304\012\031\000\272\271\271\376\270\375\243"
    "\261\012\031\001\014\000\200\000\243\315\243\305\243\315\243\317\243\322\243\331\243\303\243\301\243\322\243\304\012\031\000\272\271\271\376\270\375\243\262\037"
    "\031\000\014\000\000\200\245\317\241\274\245\311\245\307\245\243\245\271\245\257\000\000\000";

unsigned char modelTestColumnAlphaTable[8] = { 0, 96, 128, 96, 0, 0, 0, 0 };

unsigned char modelTestRowAlphaTable[8] = { 0, 96, 128, 128, 96, 0, 0, 0 };

/* JP: 菊花賞
 * EN: Kikuka Sho */
const char kikukaShoText[8] = "\265\306\262\326\276\336";
