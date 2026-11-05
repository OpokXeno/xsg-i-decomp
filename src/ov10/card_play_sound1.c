/*
 * OV10 original TU 1: 0x00a00410..0x00a08670 (15 functions)
 */
#include "common.h"
#include "shared.h"
#include "card_play_sound1.h"
#include "ov10/card_id.h"
#include "ov10/card_fread.h"
#include "ov10/card_disp.h"
#include "ov10/card_enemy.h"
#include "main/xgl_mc.h"
#include "main/xgl_sound.h"
#include "ssd/ssd_dma.h"

/*
 * Card master file (CARDDATA, 0x00a007d0): CardFread loads it to 0x01000000.
 * Layout: u16 card count at +0 (lhu 0x00a00824), 12-character version tag at +4
 * (ori 0x00a0082c/0x00a0086c), CardDefinition table at +0x10 (ori 0x00a008a0).
 */
#define CARD_DATA_HEADER ((CardDataHeader *) CARD_DATA_BUFFER)
#define CARD_DATA_TAG_OFFSET 4        /* ori 0x00a0082c / 0x00a0086c */
#define CARD_DATA_TAG ((u8 *) (CARD_DATA_BUFFER + CARD_DATA_TAG_OFFSET))
#define CARD_DATA_TAG_LENGTH 12      /* slti 0x00a00848 / 0x00a00880 */
#define CARD_DATA_HEADER_SIZE 0x10    /* ori 0x00a008a0: the definitions start after the header */
#define CARD_DATA_TABLE ((CardDefinition *) (CARD_DATA_BUFFER + CARD_DATA_HEADER_SIZE))
typedef struct CardDataHeader {
    u16 count;                      /* +0x00: number of CardDefinition entries */
    u8 unmodeled_02[2];
} CardDataHeader;

/* "CardData1.10": the version tag the file must carry (.data 0x00a42040, 13 bytes). */
static u8 IDdata[13];

#define CARD_RESULT_WAIT_FRAMES 90        /* resultWait after a game ends before input is read */

/* Buffers the sound loaders and the temporary layout files stream through. */
/* Layout CardMainProc gives the deck screens' card list windows (cld0 on the left, cld1 and cldtmp on the right). */
#define CARD_LIST_BASE_COLOR 7
#define CARD_LIST_CURSOR_COLOR 15
#define CARD_LIST_TEXT_LENGTH 16
#define CARD_LIST_ROWS 9
#define CARD_LIST_LEFT_X 12
#define CARD_LIST_RIGHT_X 312

#define CARD_SCRATCH_BUFFER ((void *) 0x01900000)   /* sound banks and sequences, battle music, the deck editor's type1_B.lex */
#define CARD_JINGLE_BUFFER ((void *) 0x01C60000)     /* JINGLE1.SMD / JINGLE2.SMD */
#define CARD_BACKGROUND_LEX ((void *) 0x01E40000)     /* <BGFileNameList entry>.lex */
#define CARD_BACKGROUND_XTX ((void *) 0x01FB0300)     /* <BGFileNameList entry>.xtx */

/* Slots of CardGameWork.models[] and the resource CardMainInit gives each (address of the model store). */
enum CardModelSlot {
    CARD_MODEL_TITLE = 0,        /* title\NewTitle.lex, 0x00a0197c */
    CARD_MODEL_VIEWER = 1,       /* deckmake\viewer.lex, 0x00a01984 */
    CARD_MODEL_DECKLIST = 2,     /* deckmake\decklist.lex, 0x00a01978 */
    CARD_MODEL_DECKMAKE = 3,     /* deckmake\deckmake.lex, 0x00a01988 */
    CARD_MODEL_MAT_FG = 4,       /* game\mat_fg.lex, 0x00a01990 */
    CARD_MODEL_MAT_MOJI = 5,     /* game\mat_moji.lex, or mat_mojiJ.lex without CARD_SAVE_LARGE_CARDS, 0x00a019bc / 0x00a019dc */
    CARD_MODEL_MAT_FG_2 = 6,     /* game\mat_fg.lex again, 0x00a01a00 */
    CARD_MODEL_MAT_MOJI_2 = 7,   /* the same file as slot 5, 0x00a019b8 / 0x00a019d8 */
    CARD_MODEL_BACKGROUND = 15   /* <BGFileNameList entry>.lex, 0x00a01a10 */
};

#define CARD_TITLE_MODEL_SLOT 14             /* the title logo model: matrix addiu 0x00a00960, stores 0x00a0095c..0x00a0096c */
#define CARD_TITLE_MODEL_ADDR ((void *) 0x01DA0000)     /* lui 0x00a00954, stored 0x00a0095c */
#define CARD_TITLE_TEXTURE_ADDR ((void *) 0x01DB5800)   /* lui/ori 0x00a00950/0x00a00958, stored 0x00a0096c */
#define CARD_TITLE_MODEL_Y (-1.37f)          /* 0xBFAF5C29, lui/ori 0x00a00988/0x00a0098c */
#define CARD_TITLE_MODEL_Z 0.1f              /* 0x3DCCCCCD, lui/ori 0x00a00994/0x00a00998 */
#define CARD_TITLE_LEX_BASE ((void *) 0x01900000)       /* m_game.lex, m_card.lex, m_help.lex: lui 0x00a009b4, ori 0x00a009c8/0x00a009dc */
#define CARD_TITLE_LEX_SIZE 0x1000           /* distance between the three layout files */
#define CARD_TITLE_XTX_GAME ((void *) 0x01A10050)       /* lui/ori 0x00a009e4/0x00a009f0 */
#define CARD_TITLE_XTX_CARD ((void *) 0x01A200A0)       /* lui/ori 0x00a009f8/0x00a00a04 */
#define CARD_TITLE_XTX_HELP ((void *) 0x01A300E0)       /* lui/ori 0x00a00a0c/0x00a00a20 */
struct CardSoundPath {
    char name[34];
};

union CardSoundPathStorage {
    char bytes[40];
    struct CardSoundPath path;
};

static s16 MenuNo_1;
static s16 SubNo_2;
static s16 SubMenu_3;
static s16 ModeExit_4;
static u8 CCtimer_5;
static s16 Message_6;
static s16 MessageCnt_7;
static s16 p0num_17;
static s16 p1num_18;
static s16 vol_19;
static char *CardErrorList[6];

extern const union CardSoundPathStorage cardBattle1SwdPath;
extern const union CardSoundPathStorage cardBattle1SmdPath;
extern const union CardSoundPathStorage cardJingle1SmdPath;
extern const union CardSoundPathStorage cardBattle2SwdPath;
extern const union CardSoundPathStorage cardBattle2SmdPath;
extern const union CardSoundPathStorage cardJingle2SmdPath;
extern char BGFileNameList[6][16];
extern char cardLexExtension[];
extern char cardXtxExtension[];

void CardPlaySound1(void)
{
    struct CardSoundPath swdFile = cardBattle1SwdPath.path;
    struct CardSoundPath smdFile = cardBattle1SmdPath.path;

    if (xglCdReadFile(swdFile.name, CARD_SCRATCH_BUFFER, 0, 0) > 0) {
        xglSoundSendSwd(CARD_SCRATCH_BUFFER, -1);
        while (SsdSpuDmaCompleted(0) != 0) {
        }
    }

    if (xglCdReadFile(smdFile.name, CARD_SCRATCH_BUFFER, 0, 0) > 0)
        xglSoundSendSmd2(CARD_SCRATCH_BUFFER, 0);

    xglSoundSequenceNormal2(0, 127);
}

void CardPlaySound3(void)
{
    struct CardSoundPath smdFile = cardJingle1SmdPath.path;

    if (xglCdReadFile(smdFile.name, CARD_JINGLE_BUFFER, 0, 1) > 0)
        xglSoundSendSmd2(CARD_JINGLE_BUFFER, 0);

    xglSoundSequenceNormal2(0, 127);
}

void CardPlaySound2(void)
{
    struct CardSoundPath swdFile = cardBattle2SwdPath.path;
    struct CardSoundPath smdFile = cardBattle2SmdPath.path;

    if (xglCdReadFile(swdFile.name, CARD_SCRATCH_BUFFER, 0, 0) > 0) {
        xglSoundSendSwd(CARD_SCRATCH_BUFFER, -1);
        while (SsdSpuDmaCompleted(0) != 0) {
        }
    }

    if (xglCdReadFile(smdFile.name, CARD_SCRATCH_BUFFER, 0, 0) > 0)
        xglSoundSendSmd2(CARD_SCRATCH_BUFFER, 0);

    xglSoundSequenceNormal2(0, 127);
}

void CardPlaySound4(void)
{
    struct CardSoundPath smdFile = cardJingle2SmdPath.path;

    if (xglCdReadFile(smdFile.name, CARD_JINGLE_BUFFER, 0, 1) > 0)
        xglSoundSendSmd2(CARD_JINGLE_BUFFER, 0);

    xglSoundSequenceNormal2(0, 127);
}

/*
 * CardDataLoad (0x00a007d0): loads card.dat and checks its 12-character version tag
 * (slti 12 at 0x00a00848 and 0x00a00880; IDdata is a 13-byte object, its NUL is not
 * compared).  The tag compare keeps the original's spelling, the one offset cast in
 * the claimed code: the readable alternatives all change the code.  Compiled in private
 * copies and compared with the original (whole-file gate): `*tag++ != IDdata[j]` gives
 * 5 diff ranges (0x00a00855, 0x00a0085C, 0x00a00860, 0x00a00866, 0x00a0086A);
 * `u8 *expectedId = IDdata` with `expectedId[j]` gives 2 ranges (0x00a00872 and
 * 0x00a0089A, one byte each, a7 against e5: register choice); `*expectedId++` gives 17
 * ranges (0x00a007F6 through 0x00a00896).
 */
CardDefinition *CardDataLoad(u16 *cardCount)
{
    char loadedId[CARD_DATA_TAG_LENGTH + 1];
    u8 *tag;
    u32 expectedId;
    int i;
    int j;

    if (CardFread(CARD_DATA_BUFFER, "card.dat") < 0) {
        printf("Card Data File Not Found!!\n");
        return 0;
    }

    *cardCount = CARD_DATA_HEADER->count;

    for (i = 0, tag = CARD_DATA_TAG; i < CARD_DATA_TAG_LENGTH; i++)
        loadedId[i] = *tag++;
    expectedId = (u32) IDdata;
    loadedId[i] = '\0';

    tag = CARD_DATA_TAG;
    for (j = 0; j < CARD_DATA_TAG_LENGTH; j++) {
        if (*tag++ != *(u8 *) (j + expectedId)) {
            printf("ID Not Mutch!!\n");
            return 0;
        }
    }

    return CARD_DATA_TABLE;
}

extern char cardGraphicFileName[];

/* CARDGRAP.BIN (cardGraphicFileName) is read right behind the card master file buffer: lui/ori 0x00a008bc/0x00a008d0. */
#define CARD_GRAPHIC_BUFFER ((void *) 0x0100C000)

/* The result is 1 when CardFread did not fail: nor 0x00a008d4 and srl 31 at 0x00a008dc take the sign bit of ~rc. */
u32 CardGraphicLoad(CardDefinition *definitions, u16 cardCount) {
    return (u32) ~CardFread(CARD_GRAPHIC_BUFFER, cardGraphicFileName) >> 0x1F;
}

/*
 * CardTitleInit (0x00a008e8, 0x144 bytes): hides every card model, resets the
 * title logo model slot (matrix, position, model and texture addresses and the
 * rotation state bytes at +0x70F0) and loads the three title layout files and
 * their textures. Called from CardMainProc when the title screen is entered
 * (CardMainInit 0x00a01aac, CardMainProc 0x00a03cac).
 */
void CardTitleInit(CardGameWork *work)
{
    int i;

    for (i = CARD_MODEL_COUNT - 1; i >= 0; i--)
        work->models[i].visible = 0;

    work->models[CARD_MODEL_TITLE].visible = 1;
    work->models[CARD_MODEL_BACKGROUND].visible = 1;
    xglMatrixUnit(work->models[CARD_MODEL_MAT_FG].matrix);
    xglMatrixUnit(work->models[CARD_MODEL_MAT_MOJI].matrix);
    xglMatrixUnit(work->models[CARD_MODEL_BACKGROUND].matrix);

    work->rotStageCooldown = 0;
    work->rotStageVector[1] = work->rotStageVector[2] = work->rotStageVector[0] = 0.0f;
    work->models[CARD_TITLE_MODEL_SLOT].model = CARD_TITLE_MODEL_ADDR;
    work->models[CARD_TITLE_MODEL_SLOT].visible = 1;
    work->models[CARD_TITLE_MODEL_SLOT].texture = CARD_TITLE_TEXTURE_ADDR;
    work->rotStageFlag = 0;
    work->rotStageId = 0;
    work->rotStageActive = 0;

    xglMatrixUnit(work->models[CARD_TITLE_MODEL_SLOT].matrix);
    work->models[CARD_TITLE_MODEL_SLOT].matrix[3][1] = CARD_TITLE_MODEL_Y;
    work->models[CARD_TITLE_MODEL_SLOT].matrix[3][2] = CARD_TITLE_MODEL_Z;

    CardFread(CARD_TITLE_LEX_BASE, "title\\m_game.lex");
    CardFread(CARD_TITLE_LEX_BASE + CARD_TITLE_LEX_SIZE, "title\\m_card.lex");
    CardFread(CARD_TITLE_LEX_BASE + 2 * CARD_TITLE_LEX_SIZE, "title\\m_help.lex");
    CardFread(CARD_TITLE_XTX_GAME, "title\\m_game.xtx");
    CardFread(CARD_TITLE_XTX_CARD, "title\\m_card.xtx");
    CardFread(CARD_TITLE_XTX_HELP, "title\\m_help.xtx");
}

void CMIBGInit(CardGameWork *work)
{
    char filename[32];

    strcpy(filename, BGFileNameList[work->save->background]);
    strcat(filename, cardLexExtension);
    CardFread(CARD_BACKGROUND_LEX, filename);

    strcpy(filename, BGFileNameList[work->save->background]);
    strcat(filename, cardXtxExtension);
    CardFread(CARD_BACKGROUND_XTX, filename);
}

/* Save data block of the card game inside SaveData (the 0x300 bytes before the thumbnail). */
#define CARD_SAVE_OFFSET 0x15090     /* SaveData 0x004917B0 + this = 0x004A6840, built at 0x00a00ad4/0x00a00ae0 (lui/addiu) and stored at 0x00a00b00 */
#define CARD_SAVE_INITIALIZED 0x01   /* CardSaveData.flags: the initial cards and decks were written; andi 0x00a00b08, ori 0x00a00b18 */
#define CARD_DECK_EMPTY 0xFF         /* li 255 at 0x00a00c30, stored with sh into both deck arrays */
#define CARD_BACKGROUND_COUNT 6      /* BGFileNameList entries, sltiu at 0x00a00c20 */
#define CARD_CURSOR_KINDS 4          /* cursorLex / cursorXtx entries, count-down loop li 3 at 0x00a00cac, bgez at 0x00a00cf4 */

/* Cursor kinds in cursorLex[] / cursorXtx[], named after the files CardMainInit stores (lex 0x00a00f58, 0x00a00f64,
 * 0x00a00f54; texture 0x00a00f50, 0x00a00f40, 0x00a00f48). Entry 3 stays 0. */
enum CardCursorKind {
    CARD_CURSOR_2D_11 = 0,       /* game\cur2D_11.lex frames, game\cur2D.xtx */
    CARD_CURSOR_2D_01 = 1,       /* game\cur2D_01.lex frames, the same texture */
    CARD_CURSOR_FRAME_YELLOW = 2 /* game\Cur_WakuY.lex, game\Cur_Waku.xtx */
};
#define CARD_LIGHT_COUNT 4           /* ambient light plus three parallel lights, slti at 0x00a01ba8 */
#define CARD_FADE_LEVEL_FULL 0x80    /* li -128 at 0x00a01af8, stored with sb at 0x00a01b08..0x00a01b2c */

/* Where CardMainInit reads the resources it names again below. The other files are read
 * to fixed addresses too, in the order of their use: cursors, title windows, deck editor,
 * card faces, digits, help, battle windows, models and the playing mat. Files that sit at a
 * constant distance from each other are written as base + n * size; the destinations that
 * have no further role are plain addresses, each with the address of its jal CardFread. */
#define CARD_CURSOR_LEX_FIRST ((void *) 0x01D00000)   /* game\cur2D_11.lex */
#define CARD_CURSOR_LEX_SECOND ((void *) 0x01D02000)  /* game\cur2D_01.lex */
#define CARD_FRAME_LEX_YELLOW ((void *) 0x01D08000)   /* game\Cur_WakuY.lex */
#define CARD_CURSOR_LEX_SIZE 0x510         /* distance between the six frames of a cursor set (game\cur2D_11..16.lex, cur2D_01..06.lex) */
#define CARD_BLACK_LEX ((void *) 0x01D06000)          /* game\black.lex, drawn last by NewCMPGameModeExit */
#define CARD_CURSOR_XTX ((void *) 0x01D20000)         /* game\cur2D.xtx */
#define CARD_FRAME_XTX ((void *) 0x01D60000)          /* game\Cur_Waku.xtx */
#define CARD_MENU_ITEM_LEX ((void *) 0x0170B600)      /* title\menu1.lex: the item plates of the START menu */
#define CARD_MENU_OPTION_LEX ((void *) 0x0170D600)    /* title\menu2.lex: the option row plates */
#define CARD_MENU_TEXTURE ((void *) 0x0170F600)       /* title\menu.bin: texture of both */
#define CARD_TITLE_WINDOW_SIZE 0x2B00      /* distance between title\window1..8.lex and title\window.bin (CARD_TITLE_MODEL_ADDR, CARD_TITLE_TEXTURE_ADDR) */
#define CARD_SMALL_LEX_BASE ((void *) 0x01C80000)     /* o0..o9, s0..s9, Ftype_*, Btype_*, Atype_*, L0..L9, R0..R9 and field.lex: 0x400 bytes each, in this order */
#define CARD_SMALL_LEX_SIZE 0x400
#define CARD_SMALL_LEX(n) (CARD_SMALL_LEX_BASE + (n) * CARD_SMALL_LEX_SIZE)
#define CARD_LINE_LEX ((void *) 0x01713600)           /* line1..4.lex */
#define CARD_LINE_LEX_SIZE 0x1000
#define CARD_BATTLE_WINDOW_LEX ((void *) 0x01930000)  /* six battle window files, 0x1800 bytes each (CardLoadBattleWindows reads the same ones) */
#define CARD_BATTLE_WINDOW_SIZE 0x1800
#define CARD_BATTLE_WINDOW(n) (CARD_BATTLE_WINDOW_LEX + (n) * CARD_BATTLE_WINDOW_SIZE)
#define CARD_BATTLE_WINDOW_TEXTURE CARD_BATTLE_WINDOW(5)   /* deckmake\battlewin.bin, the sixth file */
#define CARD_TITLE_LEX ((void *) 0x01DE0000)          /* title\NewTitle.lex */
#define CARD_TITLE_XTX ((void *) 0x01EF0000)          /* title\NewTitle.xtx */
#define CARD_VIEWER_LEX ((void *) 0x01DF0400)         /* deckmake\viewer.lex */
#define CARD_DECKLIST_LEX ((void *) 0x01DF6000)       /* deckmake\decklist.lex */
#define CARD_DECKMAKE_LEX ((void *) 0x01E00000)       /* deckmake\deckmake.lex */
#define CARD_DECK_GRA_BIN ((void *) 0x01F30100)       /* deckmake\deck_gra.bin: texture of the three deck screens */
#define CARD_MAT_FG_LEX ((void *) 0x01E10000)         /* game\mat_fg.lex */
#define CARD_MAT_FG_XTX ((void *) 0x01F70200)         /* game\mat_fg.xtx */
#define CARD_MAT_MOJI_LEX ((void *) 0x01E20000)       /* game\mat_moji.lex */
#define CARD_MAT_MOJI_J_LEX ((void *) 0x01996000)     /* game\mat_mojiJ.lex */
#define CARD_MAT_MOJI_J_XTX ((void *) 0x0199A000)     /* game\mat_mojiJ.xtx */
#define CARD_FRAME_LEX_GREEN   ((void *) 0x01D0C000)   /* game\Cur_WakuG.lex */
#define CARD_FRAME_LEX_RED     ((void *) 0x01D10000)   /* game\Cur_WakuR.lex */
#define CARD_FRAME_LEX_WHITE   ((void *) 0x01D12000)   /* game\Cur_WakuW.lex */
#define CARD_WAKU6_LEX         ((void *) 0x01D14000)   /* game\waku6.lex */
#define CARD_PLAYER1_LEX       ((void *) 0x01D18000)   /* game\1p.lex */
#define CARD_PLAYER2_LEX       ((void *) 0x01D1C000)   /* game\2p.lex */
#define CARD_PLAYER_NUMBER_XTX ((void *) 0x01D80000)   /* game\1p2p.xtx */
#define CARD_COMP_LEX          ((void *) 0x01DDF800)   /* deckmake\comp.lex */
#define CARD_COMP_XTX          ((void *) 0x01DDFC00)   /* deckmake\comp.xtx */
#define CARD_BASE_BIN          ((void *) 0x016E0000)   /* NewCardBase.bin */
#define CARD_NAME_LEX          ((void *) 0x016F8100)   /* name.lex */
#define CARD_CARDGRAP_LEX      ((void *) 0x016F8500)   /* cardgrap.lex */
#define CARD_NEWCARD_B_LEX     ((void *) 0x016F8900)   /* NewCard_B.lex */
#define CARD_NEWCARD_E_LEX     ((void *) 0x016FD440)   /* NewCard_E.lex */
#define CARD_NEWCARD_S_LEX     ((void *) 0x01701F80)   /* NewCard_S.lex */
#define CARD_KAGE_LEX          ((void *) 0x01706AC0)   /* card_kage.lex */
#define CARD_LINE_XTX          ((void *) 0x01717600)   /* line.xtx */
#define CARD_XG_URA_UP_LEX     ((void *) 0x01C73000)   /* deckmake\xg_ura_up.lex */
#define CARD_XG_URA_UP_XTX     ((void *) 0x01C74000)   /* deckmake\xg_ura_up.xtx */
#define CARD_MARK_C_LEX        ((void *) 0x01C8F800)   /* card_m_c.lex */
#define CARD_MARK_U_LEX        ((void *) 0x01C8FD10)   /* card_m_u.lex */
#define CARD_MARK_R_LEX        ((void *) 0x01C90220)   /* card_m_r.lex */
#define CARD_MARK_P_LEX        ((void *) 0x01C90730)   /* card_m_p.lex */
#define CARD_INFO0_BIN         ((void *) 0x01C91000)   /* CardInfo0.bin */
#define CARD_INFO1_BIN         ((void *) 0x01C94050)   /* CardInfo1.bin */
#define CARD_INFO2_BIN         ((void *) 0x01C990A0)   /* CardInfo2.bin */
#define CARD_RARE_XTX          ((void *) 0x01C9BB70)   /* card_Rare.xtx */
#define CARD_HELP1_LEX         ((void *) 0x01CA1000)   /* help\help1.lex */
#define CARD_HELP2_LEX         ((void *) 0x01CA1C00)   /* help\help2.lex */
#define CARD_HELP3_LEX         ((void *) 0x01CA2B00)   /* help\help3.lex */
#define CARD_HELP_MENU1_LEX    ((void *) 0x01CA4000)   /* help\Hmenu1.lex */
#define CARD_HELP_MENU2_LEX    ((void *) 0x01CA6000)   /* help\Hmenu2.lex */
#define CARD_HELP1_XTX         ((void *) 0x01CB0000)   /* help\help1.xtx */
#define CARD_HELP2_XTX         ((void *) 0x01CC0050)   /* help\help2.xtx */
#define CARD_MAT_INFO_LEX      ((void *) 0x01E30000)   /* game\mat_info.lex */
#define CARD_MAT_HPATK_LEX     ((void *) 0x01E38000)   /* game\mat_hpatk.lex */

/* The deck editor's type icon layouts: ten 0x400-byte files (type1_B, type1_E, type1_S, type2_0..type2_6) from the
 * start of the scratch buffer, and four textures (type1_B, type1_E, type1_S, type2_0) 0x10100 bytes apart. */
#define CARD_TYPE_LEX_SIZE 0x400
#define CARD_TYPE_LEX(n) (CARD_SCRATCH_BUFFER + (n) * CARD_TYPE_LEX_SIZE)
#define CARD_TYPE_XTX_BASE ((void *) 0x01908000)
#define CARD_TYPE_XTX_SIZE 0x10100
#define CARD_TYPE_XTX(n) (CARD_TYPE_XTX_BASE + (n) * CARD_TYPE_XTX_SIZE)

extern u8 CardInitialDataPack[CARD_DECK_SIZE];
static float s_asIntensity[4][4];
static Vector4 s_asDirection[4];
extern void xglLightIntensityParallel(void *light, int index, void *intensity);

/*
 * CardMainInit (0x00a00ad0, 0x110c bytes): one-time setup of the card game, called from
 * CardGameRoot with its function-static work area. Writes the initial cards and five
 * stored decks into the save data the first time, unpacks the owned-card counts, clears the
 * work area, loads the card data and every resource file, places the models and sets the
 * camera and lights. Returns 1, or 0 after printing an error when the card data or card
 * graphics cannot be loaded.
 */
int CardMainInit(CardGameWork *work)
{
    char filename[32];
    float modelScale[4];
    float eyePosition[4];
    StudioLight *light;
    u8 checksum;
    int i;
    int lightNo;

    work->save = (CardSaveData *) (SaveData + CARD_SAVE_OFFSET);
    if (!(work->save->flags & CARD_SAVE_INITIALIZED)) {
        work->save->flags |= CARD_SAVE_INITIALIZED;
        work->save->flags |= CARD_SAVE_LARGE_CARDS;
        checksum = 0;
        for (i = 0; i < CARD_DECK_SIZE; i++) {
            u8 id = CardInitialDataPack[i];

            /* The five stored decks are written one after the other, not in an inner loop. */
            work->save->decks[0].cards[i] = id;
            checksum += id;
            work->save->decks[1].cards[i] = id;
            work->save->decks[2].cards[i] = id;
            work->save->decks[3].cards[i] = id;
            work->save->decks[4].cards[i] = id;
            CardAddList(work, id);
        }
        checksum = ~checksum;
        work->save->decks[0].checksum = checksum;
        work->save->decks[1].checksum = checksum;
        work->save->decks[2].checksum = checksum;
        work->save->decks[3].checksum = checksum;
        work->save->decks[4].checksum = checksum;
    }
    for (i = 0; i < CARD_ID_COUNT; i++) {
        work->ownedCount[i] = (work->save->cardCounts[i / CARD_COUNTS_PER_BYTE] >>
                                (i % CARD_COUNTS_PER_BYTE * CARD_COUNT_BITS)) & CARD_COUNT_MAX;
    }
    if (work->save->background >= CARD_BACKGROUND_COUNT)
        work->save->background = 0;
    for (i = 0; i < CARD_DECK_SIZE; i++) {
        work->playerDeck[i] = CARD_DECK_EMPTY;
        work->enemyDeck[i] = CARD_DECK_EMPTY;
    }
    {
        /* Two walking pointers: the original steps the placement matrix separately from the entry. */
        float (*matrix)[4] = work->models[CARD_MODEL_TITLE].matrix;
        CardModelEntry *model = work->models;

        for (i = 0; i < CARD_MODEL_COUNT; i++) {
            model->visible = 0;
            model->model = 0;
            model->texture = 0;
            xglMatrixUnit(matrix);
            model++;
            matrix += sizeof(CardModelEntry) / sizeof(matrix[0]);
        }
    }
    work->commandIds = CardCommandPlay;
    work->cursor.selectionMode = 0;
    work->cursorZone = 0;
    work->cursorIndex = 0;
    work->highlightX = work->highlightY = work->highlightScaleX = work->highlightScaleY = 1.0f;
    work->cursorPos[0] = work->cursorPos[1] = work->cursorPos[2] = 0.0f;
    work->cursorPos[3] = 1.0f;
    for (i = 0; i < CARD_CURSOR_KINDS; i++) {
        work->cursorLex[i] = 0;
        work->cursorXtx[i] = 0;
    }
    work->fadeOutFlag = 0;
    work->rotStageFlag = 0;
    work->rotStageCooldown = 0;
    work->rotStageActive = 0;
    work->rotStageId = 0;
    work->fase = CARD_FASE_TITLE;
    work->definitions = CardDataLoad(&work->cardCount);
    if (work->definitions == 0) {
        printf("Card Data Load ERROR!!!\n");
        return 0;
    }
    if (CardGraphicLoad(work->definitions, work->cardCount) == 0) {
        printf("Card Graphic Load ERROR!!!\n");
        return 0;
    }
    CardFread(CARD_CURSOR_LEX_FIRST, "game\\cur2D_11.lex");
    CardFread(CARD_CURSOR_LEX_FIRST + 1 * CARD_CURSOR_LEX_SIZE, "game\\cur2D_12.lex");
    CardFread(CARD_CURSOR_LEX_FIRST + 2 * CARD_CURSOR_LEX_SIZE, "game\\cur2D_13.lex");
    CardFread(CARD_CURSOR_LEX_FIRST + 3 * CARD_CURSOR_LEX_SIZE, "game\\cur2D_14.lex");
    CardFread(CARD_CURSOR_LEX_FIRST + 4 * CARD_CURSOR_LEX_SIZE, "game\\cur2D_15.lex");
    CardFread(CARD_CURSOR_LEX_FIRST + 5 * CARD_CURSOR_LEX_SIZE, "game\\cur2D_16.lex");
    CardFread(CARD_CURSOR_LEX_SECOND, "game\\cur2D_01.lex");
    CardFread(CARD_CURSOR_LEX_SECOND + 1 * CARD_CURSOR_LEX_SIZE, "game\\cur2D_02.lex");
    CardFread(CARD_CURSOR_LEX_SECOND + 2 * CARD_CURSOR_LEX_SIZE, "game\\cur2D_03.lex");
    CardFread(CARD_CURSOR_LEX_SECOND + 3 * CARD_CURSOR_LEX_SIZE, "game\\cur2D_04.lex");
    CardFread(CARD_CURSOR_LEX_SECOND + 4 * CARD_CURSOR_LEX_SIZE, "game\\cur2D_05.lex");
    CardFread(CARD_CURSOR_LEX_SECOND + 5 * CARD_CURSOR_LEX_SIZE, "game\\cur2D_06.lex");
    CardFread(CARD_BLACK_LEX, "game\\black.lex");
    CardFread(CARD_FRAME_LEX_YELLOW, "game\\Cur_WakuY.lex");
    CardFread(CARD_FRAME_LEX_GREEN, "game\\Cur_WakuG.lex");  /* 0x00a00e8c */
    CardFread(CARD_FRAME_LEX_RED, "game\\Cur_WakuR.lex");  /* 0x00a00e9c */
    CardFread(CARD_FRAME_LEX_WHITE, "game\\Cur_WakuW.lex");  /* 0x00a00eb0 */
    CardFread(CARD_WAKU6_LEX, "game\\waku6.lex");  /* 0x00a00ec4 */
    CardFread(CARD_PLAYER1_LEX, "game\\1p.lex");  /* 0x00a00ed8 */
    CardFread(CARD_PLAYER2_LEX, "game\\2p.lex");  /* 0x00a00eec */
    CardFread(CARD_CURSOR_XTX, "game\\cur2D.xtx");
    CardFread(CARD_FRAME_XTX, "game\\Cur_Waku.xtx");
    CardFread(CARD_PLAYER_NUMBER_XTX, "game\\1p2p.xtx");  /* 0x00a00f1c */
    work->cursorLex[CARD_CURSOR_2D_11] = CARD_CURSOR_LEX_FIRST;
    work->cursorLex[CARD_CURSOR_2D_01] = CARD_CURSOR_LEX_SECOND;
    work->cursorLex[CARD_CURSOR_FRAME_YELLOW] = CARD_FRAME_LEX_YELLOW;
    work->cursorXtx[CARD_CURSOR_2D_11] = CARD_CURSOR_XTX;
    work->cursorXtx[CARD_CURSOR_2D_01] = CARD_CURSOR_XTX;
    work->cursorXtx[CARD_CURSOR_FRAME_YELLOW] = CARD_FRAME_XTX;
    CardFread(CARD_TITLE_MODEL_ADDR, "title\\window1.lex");
    CardFread(CARD_TITLE_MODEL_ADDR + 1 * CARD_TITLE_WINDOW_SIZE, "title\\window2.lex");
    CardFread(CARD_TITLE_MODEL_ADDR + 2 * CARD_TITLE_WINDOW_SIZE, "title\\window3.lex");
    CardFread(CARD_TITLE_MODEL_ADDR + 3 * CARD_TITLE_WINDOW_SIZE, "title\\window4.lex");
    CardFread(CARD_TITLE_MODEL_ADDR + 4 * CARD_TITLE_WINDOW_SIZE, "title\\window5.lex");
    CardFread(CARD_TITLE_MODEL_ADDR + 5 * CARD_TITLE_WINDOW_SIZE, "title\\window6.lex");
    CardFread(CARD_TITLE_MODEL_ADDR + 6 * CARD_TITLE_WINDOW_SIZE, "title\\window7.lex");
    CardFread(CARD_TITLE_MODEL_ADDR + 7 * CARD_TITLE_WINDOW_SIZE, "title\\window8.lex");
    CardFread(CARD_TITLE_TEXTURE_ADDR, "title\\window.bin");
    CardFread(CARD_COMP_LEX, "deckmake\\comp.lex");  /* 0x00a01014 */
    CardFread(CARD_COMP_XTX, "deckmake\\comp.xtx");  /* 0x00a01028 */
    CardFread(CARD_BASE_BIN, "NewCardBase.bin");  /* 0x00a01038 */
    CardFread(CARD_NAME_LEX, "name.lex");  /* 0x00a0104c */
    CardFread(CARD_CARDGRAP_LEX, "cardgrap.lex");  /* 0x00a01060 */
    CardFread(CARD_NEWCARD_B_LEX, "NewCard_B.lex");  /* 0x00a01074 */
    CardFread(CARD_NEWCARD_E_LEX, "NewCard_E.lex");  /* 0x00a01088 */
    CardFread(CARD_NEWCARD_S_LEX, "NewCard_S.lex");  /* 0x00a0109c */
    CardFread(CARD_KAGE_LEX, "card_kage.lex");  /* 0x00a010b0 */
    CardFread(CARD_MENU_ITEM_LEX, "title\\menu1.lex");
    CardFread(CARD_MENU_OPTION_LEX, "title\\menu2.lex");
    CardFread(CARD_MENU_TEXTURE, "title\\menu.bin");
    CardFread(CARD_LINE_LEX, "line1.lex");
    CardFread(CARD_LINE_LEX + 1 * CARD_LINE_LEX_SIZE, "line2.lex");
    CardFread(CARD_LINE_LEX + 2 * CARD_LINE_LEX_SIZE, "line3.lex");
    CardFread(CARD_LINE_LEX + 3 * CARD_LINE_LEX_SIZE, "line4.lex");
    CardFread(CARD_LINE_XTX, "line.xtx");  /* 0x00a01150 */
    CardFread(CARD_XG_URA_UP_LEX, "deckmake\\xg_ura_up.lex");  /* 0x00a01164 */
    CardFread(CARD_XG_URA_UP_XTX, "deckmake\\xg_ura_up.xtx");  /* 0x00a01178 */
    CardFread(CARD_SMALL_LEX(0), "o0.lex");
    CardFread(CARD_SMALL_LEX(1), "o1.lex");
    CardFread(CARD_SMALL_LEX(2), "o2.lex");
    CardFread(CARD_SMALL_LEX(3), "o3.lex");
    CardFread(CARD_SMALL_LEX(4), "o4.lex");
    CardFread(CARD_SMALL_LEX(5), "o5.lex");
    CardFread(CARD_SMALL_LEX(6), "o6.lex");
    CardFread(CARD_SMALL_LEX(7), "o7.lex");
    CardFread(CARD_SMALL_LEX(8), "o8.lex");
    CardFread(CARD_SMALL_LEX(9), "o9.lex");
    CardFread(CARD_SMALL_LEX(10), "s0.lex");
    CardFread(CARD_SMALL_LEX(11), "s1.lex");
    CardFread(CARD_SMALL_LEX(12), "s2.lex");
    CardFread(CARD_SMALL_LEX(13), "s3.lex");
    CardFread(CARD_SMALL_LEX(14), "s4.lex");
    CardFread(CARD_SMALL_LEX(15), "s5.lex");
    CardFread(CARD_SMALL_LEX(16), "s6.lex");
    CardFread(CARD_SMALL_LEX(17), "s7.lex");
    CardFread(CARD_SMALL_LEX(18), "s8.lex");
    CardFread(CARD_SMALL_LEX(19), "s9.lex");
    CardFread(CARD_SMALL_LEX(20), "Ftype_1.lex");
    CardFread(CARD_SMALL_LEX(21), "Ftype_2.lex");
    CardFread(CARD_SMALL_LEX(22), "Ftype_3.lex");
    CardFread(CARD_SMALL_LEX(23), "Ftype_4.lex");
    CardFread(CARD_SMALL_LEX(24), "Ftype_5.lex");
    CardFread(CARD_SMALL_LEX(25), "Ftype_6.lex");
    CardFread(CARD_SMALL_LEX(26), "Ftype_Any1.lex");
    CardFread(CARD_SMALL_LEX(27), "Ftype_No1.lex");
    CardFread(CARD_SMALL_LEX(28), "Ftype_Which.lex");
    CardFread(CARD_SMALL_LEX(29), "Btype_1.lex");
    CardFread(CARD_SMALL_LEX(30), "Btype_2.lex");
    CardFread(CARD_SMALL_LEX(31), "Btype_3.lex");
    CardFread(CARD_SMALL_LEX(32), "Btype_4.lex");
    CardFread(CARD_SMALL_LEX(33), "Btype_5.lex");
    CardFread(CARD_SMALL_LEX(34), "Btype_6.lex");
    CardFread(CARD_SMALL_LEX(35), "Btype_7.lex");
    CardFread(CARD_SMALL_LEX(36), "Btype_8.lex");
    CardFread(CARD_SMALL_LEX(37), "Atype_1.lex");
    CardFread(CARD_SMALL_LEX(38), "Atype_2.lex");
    CardFread(CARD_SMALL_LEX(39), "Atype_3.lex");
    CardFread(CARD_SMALL_LEX(40), "Atype_4.lex");
    CardFread(CARD_SMALL_LEX(41), "L0.lex");
    CardFread(CARD_SMALL_LEX(42), "L1.lex");
    CardFread(CARD_SMALL_LEX(43), "L2.lex");
    CardFread(CARD_SMALL_LEX(44), "L3.lex");
    CardFread(CARD_SMALL_LEX(45), "L4.lex");
    CardFread(CARD_SMALL_LEX(46), "L5.lex");
    CardFread(CARD_SMALL_LEX(47), "L6.lex");
    CardFread(CARD_SMALL_LEX(48), "L7.lex");
    CardFread(CARD_SMALL_LEX(49), "L8.lex");
    CardFread(CARD_SMALL_LEX(50), "L9.lex");
    CardFread(CARD_SMALL_LEX(51), "R0.lex");
    CardFread(CARD_SMALL_LEX(52), "R1.lex");
    CardFread(CARD_SMALL_LEX(53), "R2.lex");
    CardFread(CARD_SMALL_LEX(54), "R3.lex");
    CardFread(CARD_SMALL_LEX(55), "R4.lex");
    CardFread(CARD_SMALL_LEX(56), "R5.lex");
    CardFread(CARD_SMALL_LEX(57), "R6.lex");
    CardFread(CARD_SMALL_LEX(58), "R7.lex");
    CardFread(CARD_SMALL_LEX(59), "R8.lex");
    CardFread(CARD_SMALL_LEX(60), "R9.lex");
    CardFread(CARD_SMALL_LEX(61), "game\\field.lex");
    CardFread(CARD_MARK_C_LEX, "card_m_c.lex");  /* 0x00a01660 */
    CardFread(CARD_MARK_U_LEX, "card_m_u.lex");  /* 0x00a01674 */
    CardFread(CARD_MARK_R_LEX, "card_m_r.lex");  /* 0x00a01688 */
    CardFread(CARD_MARK_P_LEX, "card_m_p.lex");  /* 0x00a0169c */
    CardFread(CARD_INFO0_BIN, "CardInfo0.bin");  /* 0x00a016b0 */
    CardFread(CARD_INFO1_BIN, "CardInfo1.bin");  /* 0x00a016c4 */
    CardFread(CARD_INFO2_BIN, "CardInfo2.bin");  /* 0x00a016d8 */
    CardFread(CARD_RARE_XTX, "card_Rare.xtx");  /* 0x00a016ec */
    CardFread(CARD_HELP1_LEX, "help\\help1.lex");  /* 0x00a01700 */
    CardFread(CARD_HELP2_LEX, "help\\help2.lex");  /* 0x00a01714 */
    CardFread(CARD_HELP3_LEX, "help\\help3.lex");  /* 0x00a01728 */
    CardFread(CARD_HELP_MENU1_LEX, "help\\Hmenu1.lex");  /* 0x00a0173c */
    CardFread(CARD_HELP_MENU2_LEX, "help\\Hmenu2.lex");  /* 0x00a01750 */
    CardFread(CARD_HELP1_XTX, "help\\help1.xtx");  /* 0x00a01760 */
    CardFread(CARD_HELP2_XTX, "help\\help2.xtx");  /* 0x00a01774 */
    CardFread(CARD_BATTLE_WINDOW_LEX, cardBattleWin1LexName);
    CardFread(CARD_BATTLE_WINDOW_LEX + 1 * CARD_BATTLE_WINDOW_SIZE, cardBattleWin2LexName);
    CardFread(CARD_BATTLE_WINDOW_LEX + 2 * CARD_BATTLE_WINDOW_SIZE, cardBattleWin3LexName);
    CardFread(CARD_BATTLE_WINDOW_LEX + 3 * CARD_BATTLE_WINDOW_SIZE, cardBattleWin4LexName);
    CardFread(CARD_BATTLE_WINDOW_LEX + 4 * CARD_BATTLE_WINDOW_SIZE, cardBattleWin5LexName);
    CardFread(CARD_BATTLE_WINDOW_LEX + 5 * CARD_BATTLE_WINDOW_SIZE, cardBattleWinBinName);
    CardFread(CARD_TITLE_LEX, "title\\NewTitle.lex");
    CardFread(CARD_VIEWER_LEX, "deckmake\\viewer.lex");
    CardFread(CARD_DECKLIST_LEX, "deckmake\\decklist.lex");
    CardFread(CARD_DECKMAKE_LEX, "deckmake\\deckmake.lex");
    CardFread(CARD_MAT_FG_LEX, "game\\mat_fg.lex");
    CardFread(CARD_MAT_MOJI_LEX, "game\\mat_moji.lex");
    CardFread(CARD_MAT_INFO_LEX, "game\\mat_info.lex");  /* 0x00a01860 */
    CardFread(CARD_MAT_HPATK_LEX, "game\\mat_hpatk.lex");  /* 0x00a01874 */
    strcpy(filename, BGFileNameList[work->save->background]);
    strcat(filename, cardLexExtension);
    CardFread(CARD_BACKGROUND_LEX, filename);
    CardFread(CARD_TITLE_XTX, "title\\NewTitle.xtx");
    CardFread(CARD_DECK_GRA_BIN, "deckmake\\deck_gra.bin");
    CardFread(CARD_MAT_MOJI_J_LEX, "game\\mat_mojiJ.lex");
    CardFread(CARD_MAT_MOJI_J_XTX, "game\\mat_mojiJ.xtx");
    CardFread(CARD_MAT_FG_XTX, "game\\mat_fg.xtx");
    strcpy(filename, BGFileNameList[work->save->background]);
    strcat(filename, cardXtxExtension);
    CardFread(CARD_BACKGROUND_XTX, filename);
    work->models[CARD_MODEL_TITLE].model = CARD_TITLE_LEX;
    work->models[CARD_MODEL_TITLE].texture = CARD_TITLE_XTX;
    work->models[CARD_MODEL_VIEWER].model = CARD_VIEWER_LEX;
    work->models[CARD_MODEL_VIEWER].texture = CARD_DECK_GRA_BIN;
    work->models[CARD_MODEL_DECKLIST].model = CARD_DECKLIST_LEX;
    work->models[CARD_MODEL_DECKLIST].texture = CARD_DECK_GRA_BIN;
    work->models[CARD_MODEL_DECKMAKE].model = CARD_DECKMAKE_LEX;
    work->models[CARD_MODEL_DECKMAKE].texture = CARD_DECK_GRA_BIN;
    work->models[CARD_MODEL_MAT_FG].model = CARD_MAT_FG_LEX;
    work->models[CARD_MODEL_MAT_FG].texture = CARD_MAT_FG_XTX;
    if (work->save->flags & CARD_SAVE_LARGE_CARDS) {
        work->models[CARD_MODEL_MAT_MOJI].model = CARD_MAT_MOJI_LEX;
        work->models[CARD_MODEL_MAT_MOJI].texture = CARD_MAT_FG_XTX;
        work->models[CARD_MODEL_MAT_MOJI_2].model = CARD_MAT_MOJI_LEX;
        work->models[CARD_MODEL_MAT_MOJI_2].texture = CARD_MAT_FG_XTX;
    } else {
        work->models[CARD_MODEL_MAT_MOJI].model = CARD_MAT_MOJI_J_LEX;
        work->models[CARD_MODEL_MAT_MOJI].texture = CARD_MAT_MOJI_J_XTX;
        work->models[CARD_MODEL_MAT_MOJI_2].model = CARD_MAT_MOJI_J_LEX;
        work->models[CARD_MODEL_MAT_MOJI_2].texture = CARD_MAT_MOJI_J_XTX;
    }
    work->models[CARD_MODEL_MAT_FG_2].model = CARD_MAT_FG_LEX;
    work->models[CARD_MODEL_MAT_FG_2].texture = CARD_MAT_FG_XTX;
    work->models[CARD_MODEL_BACKGROUND].model = CARD_BACKGROUND_LEX;
    work->models[CARD_MODEL_BACKGROUND].texture = CARD_BACKGROUND_XTX;
    for (i = 0; i < CARD_MODEL_COUNT; i++)
        xglMatrixUnit(work->models[i].matrix);
    work->models[CARD_MODEL_TITLE].matrix[3][1] = 1.4728f;
    modelScale[0] = 1.13f;
    modelScale[1] = modelScale[2] = modelScale[3] = 1.0f;
    xglMatrixScale(work->models[CARD_MODEL_VIEWER].matrix, work->models[CARD_MODEL_VIEWER].matrix, modelScale);
    xglMatrixScale(work->models[CARD_MODEL_DECKLIST].matrix, work->models[CARD_MODEL_DECKLIST].matrix, modelScale);
    xglMatrixScale(work->models[CARD_MODEL_DECKMAKE].matrix, work->models[CARD_MODEL_DECKMAKE].matrix, modelScale);
    CardHelpFileLoad();
    InitCpuDeck();
    CardTitleInit(work);
    eyePosition[0] = 0.0f;
    eyePosition[1] = 0.0f;
    eyePosition[2] = 5.8f;
    eyePosition[3] = 1.0f;
    {
        float *cameraEye = &xglStudioSelectGetActiveCamera(0)->position.vector.x;

        /*
         * sceVu0CopyVector shape: addiu/addiu, then lq/sq through $2. Plain C compiles this copy
         * differently: Vector4 assignment gives two ldl/ldr/sdl/sdr pairs, a 16-byte-aligned
         * struct or a long long union gives ld/sd pairs, and a 128-bit integer type gives lq/sq
         * with sp-relative loads, the store moved past the following call and no addiu pair.
         */
        __asm__ __volatile__(
            "lq $2, 0(%1)\n"
            "sq $2, 0(%0)\n"
            :
            : "r" (cameraEye), "r" (eyePosition)
            : "$2", "memory");
    }
    xglStudioGetLight(&light);
    work->fadeOutTimer = 0;
    work->modelFadeInTimer = 0;
    work->fadeOutColor[0] = work->fadeBaseColor[0] = CARD_FADE_LEVEL_FULL;
    work->fadeOutColor[1] = work->fadeBaseColor[1] = CARD_FADE_LEVEL_FULL;
    work->fadeOutColor[2] = work->fadeBaseColor[2] = CARD_FADE_LEVEL_FULL;
    work->fadeOutColor[3] = work->fadeBaseColor[3] = CARD_FADE_LEVEL_FULL;
    for (lightNo = 0; lightNo < CARD_LIGHT_COUNT; lightNo++) {
        if (lightNo == 0) {
            work->modelColor[0][0] = work->modelColorTarget[0][0] = s_asIntensity[0][0];
            work->modelColor[0][1] = work->modelColorTarget[0][1] = s_asIntensity[0][1];
            work->modelColor[0][2] = work->modelColorTarget[0][2] = s_asIntensity[0][2];
            work->modelColor[0][3] = work->modelColorTarget[0][3] = s_asIntensity[0][3];
            xglLightIntensityAmbient(light, s_asIntensity);
        } else {
            xglLightIntensityParallel(light, lightNo - 1, s_asIntensity[lightNo]);
            xglLightAngle(light, lightNo - 1, &s_asDirection[lightNo]);
        }
    }
    return 1;
}

/* Prizes CardMainItemGetSub stores in item13, and the item box ids they add. */
#define PRIZE_CARD_PACK_821 0
#define PRIZE_CARD_PACK_822 1
#define PRIZE_PROMO_CARD 2
#define PRIZE_TUNED_CIRCUIT 3

void CardMainItemGetSub(int *prizes, int round)
{
    prizes[PRIZE_CARD_PACK_821] = prizes[PRIZE_CARD_PACK_822] = prizes[PRIZE_PROMO_CARD] = prizes[PRIZE_TUNED_CIRCUIT] = 0;

    if (round <= 0)
        return;

    {
        int remaining = round;

        do {
            int result = xglSRand();

            if (xglFlagsGet(301, 1) == 0)
                result = 0;
            if (result & 1)
                prizes[PRIZE_CARD_PACK_822]++;
            else
                prizes[PRIZE_CARD_PACK_821]++;
            remaining--;
        } while (remaining != 0);
    }
}

#define EVENT_ITEM_CARD_PACK_821 54      /* li 0x00a01ca0 */
#define EVENT_ITEM_CARD_PACK_822 55      /* li 0x00a01ccc */
#define ACCESSORY_TUNED_CIRCUIT 142      /* li 0x00a01d28 */
#define PROMO_SELECT_SHION 1             /* prizes[PRIZE_PROMO_CARD]: 1 awards Swim Suit Shion, 2 Swim Suit MOMO (li 0x00a01cf0) */


/* Declared here: main/data.h declares dataEvtBoxInc void (measured: the loop counters of CardMainAddItemSub
 * are allocated differently when its result is not an int). */
extern int dataEvtBoxInc(int event);
extern void dataAccBoxInc(int accessory);

/*
 * Hands the player the prizes of a won game.  prizes[] is the list
 * CardMainItemGetSub filled in item13: the number of card packs #821 and #822,
 * the promotional card (1 selects card id 1, any other value card id 8) and
 * the Tuned Circuit accessory.  Afterwards the owned-card table is rebuilt
 * from the save data, which packs two bits per card id, four card ids a byte.
 */
void CardMainAddItemSub(CardGameWork *work, int *prizes)
{
    int i;
    int shift;
    int packed;

    for (i = 0; i < prizes[PRIZE_CARD_PACK_821]; i++)
        dataEvtBoxInc(EVENT_ITEM_CARD_PACK_821);
    for (i = 0; i < prizes[PRIZE_CARD_PACK_822]; i++)
        dataEvtBoxInc(EVENT_ITEM_CARD_PACK_822);

    if (prizes[PRIZE_PROMO_CARD] != 0) {
        if (prizes[PRIZE_PROMO_CARD] == PROMO_SELECT_SHION)
            CardAddList(work, CARD_ID_SWIM_SUIT_SHION);
        else
            CardAddList(work, CARD_ID_SWIM_SUIT_MOMO);
    }

    if (prizes[PRIZE_TUNED_CIRCUIT] != 0)
        dataAccBoxInc(ACCESSORY_TUNED_CIRCUIT);

    for (i = 0; i < CARD_ID_COUNT; i++) {
        packed = work->save->cardCounts[i / CARD_COUNTS_PER_BYTE];
        shift = i % CARD_COUNTS_PER_BYTE * CARD_COUNT_BITS;
        shift = packed >> shift;
        packed = shift & CARD_COUNT_MAX;
        work->ownedCount[i] = packed;
    }
}

extern float atColor[4][4];
extern float auColor[4][4];

/*
 * START menu of the card game (NewCMPGameModeExit, 0x00a01da0).  CardGameProc
 * opens it with flowRequest 6 and passes the controller that pressed START.
 * It moves the cursor with up/down, confirms with Circle, cancels with Cross
 * and closes with START.
 *
 * Returns -1 when the menu is closed, 1 when the "End Phase" entry was chosen
 * and 0 otherwise.
 */
#define EXIT_MENU_ITEM_COUNT 4
#define EXIT_MENU_SUB_COUNT 4

/* Pad bits of CardCommandInput.repeat. */
#define EXIT_PAD_START 0x0800   /* andi 0x00a01e74 */
#define EXIT_PAD_UP 0x1000      /* andi 0x00a01e44 */
#define EXIT_PAD_DOWN 0x4000    /* andi 0x00a01e4c */
#define EXIT_PAD_CIRCLE 0x0020  /* andi 0x00a01f08 */
#define EXIT_PAD_CROSS 0x0040   /* andi 0x00a01f00 */

/* Items of the START menu (switch order of the original: 0, 2, 1, 3). */
#define EXIT_ITEM_HELP 0
#define EXIT_ITEM_OPTIONS 1
#define EXIT_ITEM_END_PHASE 2
#define EXIT_ITEM_GAME_OVER 3

/* CardSaveData.flags bits toggled by the option rows. */
#define CARD_SAVE_AUTO_P1 0x08     /* andi 0x00a02074 */
#define CARD_SAVE_AUTO_P2 0x04     /* andi 0x00a02090 */
#define CARD_SAVE_AUTO_SPIN 0x10   /* andi 0x00a020e0 */

#define EXIT_MENU_FADE_ALPHA 0x80
#define EXIT_MENU_TEXT_FIRST 70   /* CardPlayTMessList: guide text per item, then per option row at +5 */
#define EXIT_MENU_TEXT_OPTION_OFFSET 5   /* the option row texts follow at +5: lw index x4 at 0x00a02938 / 0x00a02950 differ by 20 */

/* Return values: -1 closes the menu (li s8 0x00a01efc, 0x00a01f28), 1 ends the phase (li s8 0x00a01fc4). */
#define EXIT_RESULT_CLOSE (-1)
#define EXIT_RESULT_END_PHASE 1

/* exitMenuSub: -1 while the item list is active (bgez tests at 0x00a01e54, 0x00a01f10, 0x00a01f5c, 0x00a0291c; stored
 * with li 0x00a01f14 and sh 0x00a01f3c), otherwise the option row, switched on at 0x00a02018..0x00a02048. */
#define EXIT_SUB_NONE (-1)
#define EXIT_OPTION_AUTO_P1 0     /* beqz 0x00a0202c, first row, set by 0x00a01fe0 */
#define EXIT_OPTION_AUTO_P2 1     /* beql 0x00a02018 */
#define EXIT_OPTION_HIDE_TURN 2   /* beq 0x00a02040 */
#define EXIT_OPTION_AUTO_SPIN 3   /* beq 0x00a02048 */



int NewCMPGameModeExit(int player, CardGameWork *work, int round)
{
    float matrix[4][4];
    float placement[4][4];
    float position[4];
    float scale[4];
    u16 repeat;
    int result = 0;

    repeat = work->command.input[0].repeat;
    if (player)
        repeat = work->command.input[1].repeat;

    xglFontDebugPrintf(24, 10, "Build:Nov 24 2002 11:02:50");

    position[0] = position[1] = 0.0f;
    position[2] = 1.01f;
    position[3] = 1.0f;
    scale[0] = scale[1] = 0.8f;
    scale[2] = scale[3] = 1.0f;
    xglMatrixUnit(matrix);

    if (repeat & EXIT_PAD_UP) {
        if (work->command.exitMenuSub < 0) {
            if (work->command.exitMenuItem > 0) {
                work->command.exitMenuItem--;
                xglSoundEffectNormalID(SE_CURSOR, 0);
            }
        } else if (work->command.exitMenuSub > 0) {
            work->command.exitMenuSub--;
            xglSoundEffectNormalID(SE_CURSOR, 0);
        }
    } else if (repeat & EXIT_PAD_DOWN) {
        if (work->command.exitMenuSub < 0) {
            if (work->command.exitMenuItem < EXIT_MENU_ITEM_COUNT - 1) {
                work->command.exitMenuItem++;
                xglSoundEffectNormalID(SE_CURSOR, 0);
            }
        } else if (work->command.exitMenuSub < EXIT_MENU_SUB_COUNT - 1) {
            work->command.exitMenuSub++;
            xglSoundEffectNormalID(SE_CURSOR, 0);
        }
    }

    if (repeat & EXIT_PAD_START) {
        xglSoundEffectNormalID(SE_CANCEL, 0);
        result = EXIT_RESULT_CLOSE;
    }

    if (repeat & EXIT_PAD_CROSS) {
        if (work->command.exitMenuSub < 0) {
            work->command.exitMenuItem = 0;
            xglSoundEffectNormalID(SE_CANCEL, 0);
            result = EXIT_RESULT_CLOSE;
        } else {
            work->command.exitMenuSub = EXIT_SUB_NONE;
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
    } else if (repeat & EXIT_PAD_CIRCLE) {
        if (work->command.exitMenuSub < 0) {
            switch (work->command.exitMenuItem) {
            case EXIT_ITEM_HELP:
                work->fadeOutColor[3] = EXIT_MENU_FADE_ALPHA;
                xglSoundEffectNormalID(SE_DECIDE, 0);
                work->helpState = CARD_HELP_STATE_OPEN;
                work->menuPlayer = player;
                break;
            case EXIT_ITEM_END_PHASE:
                work->fadeOutColor[3] = EXIT_MENU_FADE_ALPHA;
                result = EXIT_RESULT_END_PHASE;
                break;
            case EXIT_ITEM_OPTIONS:
                xglSoundEffectNormalID(SE_DECIDE, 0);
                work->command.exitMenuSub = EXIT_OPTION_AUTO_P1;
                break;
            case EXIT_ITEM_GAME_OVER:
                work->fadeOutColor[3] = EXIT_MENU_FADE_ALPHA;
                xglSoundEffectNormalID(SE_DECIDE, 0);
                work->fadeOutStep = CARD_FADE_STEP;
                work->fadeOutTimer = CARD_FADE_TICKS;
                work->fase = CARD_FASE_TITLE;
                break;
            }
        } else {
            switch (work->command.exitMenuSub) {
            case EXIT_OPTION_AUTO_P1:
                work->save->flags = (work->save->flags & ~CARD_SAVE_AUTO_P1) |
                                    ((work->save->flags + CARD_SAVE_AUTO_P1) & CARD_SAVE_AUTO_P1);
                xglSoundEffectNormalID(SE_DECIDE, 0);
                break;
            case EXIT_OPTION_AUTO_P2:
                work->save->flags = (work->save->flags & ~CARD_SAVE_AUTO_P2) |
                                    ((work->save->flags + CARD_SAVE_AUTO_P2) & CARD_SAVE_AUTO_P2);
                xglSoundEffectNormalID(SE_DECIDE, 0);
                break;
            case EXIT_OPTION_HIDE_TURN:
                work->save->flags = (work->save->flags & ~CARD_SAVE_HIDE_TURN) |
                                    ((work->save->flags + CARD_SAVE_HIDE_TURN) & CARD_SAVE_HIDE_TURN);
                xglSoundEffectNormalID(SE_DECIDE, 0);
                break;
            case EXIT_OPTION_AUTO_SPIN:
                work->save->flags = (work->save->flags & ~CARD_SAVE_AUTO_SPIN) |
                                    ((work->save->flags + CARD_SAVE_AUTO_SPIN) & CARD_SAVE_AUTO_SPIN);
                xglSoundEffectNormalID(SE_DECIDE, 0);
                break;
            }
        }
    }

    /* Item plates. */
    xglFontPrint(0, 0, CARD_TEXT_COLOR, "\v\r\0\f\x18\x18\x18");
    if (work->command.exitMenuItem == 0) {
        nmlModelSetLight(&atColor, &asDir);
        xglFontPrint(0, 0, CARD_TEXT_COLOR, cardHelpFontCodes.code.text);
    } else {
        nmlModelSetLight(&auColor, &asDir);
    }
    xglFontPrint(0, 0, 0, "\031\003");
    xglFontPrint(226, 138, CARD_TEXT_COLOR, cardHelpLabel);
    position[1] = 0.58869994f;
    xglMatrixTrans(placement, matrix, position);
    xglMatrixScale(placement, placement, scale);
    nmlModelSetPlace(&placement);
    nmlModelSetTexture(CARD_MENU_TEXTURE);
    nmlModelEntryCard(CARD_MENU_ITEM_LEX);

    xglFontPrint(0, 0, CARD_TEXT_COLOR, "\v\r\0\f\x18\x18\x18");
    if (work->command.exitMenuItem == 2) {
        xglFontPrint(0, 0, CARD_TEXT_COLOR, cardHelpFontCodes.code.text);
        nmlModelSetLight(&atColor, &asDir);
    } else {
        nmlModelSetLight(&auColor, &asDir);
    }
    xglFontPrint(0, 0, 0, "\031\003");
    xglFontPrint(216, 175, CARD_TEXT_COLOR, " Options");
    position[1] = 0.0098f;
    xglMatrixTrans(placement, matrix, position);
    xglMatrixScale(placement, placement, scale);
    nmlModelSetPlace(&placement);
    nmlModelSetTexture(CARD_MENU_TEXTURE);
    nmlModelEntryCard(CARD_MENU_ITEM_LEX);

    xglFontPrint(0, 0, CARD_TEXT_COLOR, "\v\r\0\f\x18\x18\x18");
    if (work->command.exitMenuItem == 1) {
        nmlModelSetLight(&atColor, &asDir);
        xglFontPrint(0, 0, CARD_TEXT_COLOR, cardHelpFontCodes.code.text);
    } else {
        nmlModelSetLight(&auColor, &asDir);
    }
    xglFontPrint(0, 0, 0, "\031\003");
    xglFontPrint(196, 213, CARD_TEXT_COLOR, "   End Phase");
    position[1] = 0.3f;
    xglMatrixTrans(placement, matrix, position);
    xglMatrixScale(placement, placement, scale);
    nmlModelSetPlace(&placement);
    nmlModelSetTexture(CARD_MENU_TEXTURE);
    nmlModelEntryCard(CARD_MENU_ITEM_LEX);

    xglFontPrint(0, 0, CARD_TEXT_COLOR, "\v\r\0\f\x18\x18\x18");
    if (work->command.exitMenuItem == 3) {
        xglFontPrint(0, 0, CARD_TEXT_COLOR, cardHelpFontCodes.code.text);
        nmlModelSetLight(&atColor, &asDir);
    } else {
        nmlModelSetLight(&auColor, &asDir);
    }
    xglFontPrint(0, 0, 0, "\031\003");
    xglFontPrint(206, 251, CARD_TEXT_COLOR, " Game Over");
    position[1] = -0.2792f;
    xglMatrixTrans(placement, matrix, position);
    xglMatrixScale(placement, placement, scale);
    nmlModelSetPlace(&placement);
    nmlModelSetTexture(CARD_MENU_TEXTURE);
    nmlModelEntryCard(CARD_MENU_ITEM_LEX);

    if (work->command.exitMenuSub >= 0) {
        position[0] = 1.4374f;

        xglFontPrint(0, 0, CARD_TEXT_COLOR, cardPlayerLabelFontCodes.text);
        if (work->command.exitMenuSub == EXIT_OPTION_AUTO_P1) {
            xglFontPrint(0, 0, CARD_TEXT_COLOR, cardPlayerAutoLabel.code.text);
            nmlModelSetLight(&asColor, &asDir);
        }
        xglFontPrint(0, 0, 0, "\031\003");
        if (work->save->flags & CARD_SAVE_AUTO_P1)
            xglFontPrint(360, 155, CARD_TEXT_COLOR, "1P: Auto");
        else
            xglFontPrint(360, 155, CARD_TEXT_COLOR, "1P: Manual");
        position[1] = 0.4415f;
        xglMatrixTrans(placement, matrix, position);
        xglMatrixScale(placement, placement, scale);
        nmlModelSetPlace(&placement);
        nmlModelSetTexture(CARD_MENU_TEXTURE);
        nmlModelEntryCard(CARD_MENU_OPTION_LEX);

        xglFontPrint(0, 0, CARD_TEXT_COLOR, cardPlayerLabelFontCodes.text);
        if (work->command.exitMenuSub == EXIT_OPTION_AUTO_P2) {
            xglFontPrint(0, 0, CARD_TEXT_COLOR, cardPlayerAutoLabel.code.text);
            nmlModelSetLight(&asColor, &asDir);
        }
        xglFontPrint(0, 0, 0, "\031\003");
        if (work->save->flags & CARD_SAVE_AUTO_P2)
            xglFontPrint(360, 193, CARD_TEXT_COLOR, "2P: Auto");
        else
            xglFontPrint(360, 193, CARD_TEXT_COLOR, "2P: Manual");
        position[1] = 0.1509f;
        xglMatrixTrans(placement, matrix, position);
        xglMatrixScale(placement, placement, scale);
        nmlModelSetPlace(&placement);
        nmlModelSetTexture(CARD_MENU_TEXTURE);
        nmlModelEntryCard(CARD_MENU_OPTION_LEX);

        xglFontPrint(0, 0, CARD_TEXT_COLOR, cardPlayerLabelFontCodes.text);
        if (work->command.exitMenuSub == EXIT_OPTION_HIDE_TURN) {
            xglFontPrint(0, 0, CARD_TEXT_COLOR, cardPlayerAutoLabel.code.text);
            nmlModelSetLight(&asColor, &asDir);
        }
        xglFontPrint(0, 0, 0, "\031\003");
        if (!(work->save->flags & CARD_SAVE_HIDE_TURN))
            xglFontPrint(341, 230, CARD_TEXT_COLOR, " Turn indicator on");
        else
            xglFontPrint(341, 230, CARD_TEXT_COLOR, " Turn indicator off");
        position[1] = -0.1376f;
        xglMatrixTrans(placement, matrix, position);
        xglMatrixScale(placement, placement, scale);
        nmlModelSetPlace(&placement);
        nmlModelSetTexture(CARD_MENU_TEXTURE);
        nmlModelEntryCard(CARD_MENU_OPTION_LEX);

        xglFontPrint(0, 0, CARD_TEXT_COLOR, cardPlayerLabelFontCodes.text);
        if (work->command.exitMenuSub == EXIT_OPTION_AUTO_SPIN) {
            xglFontPrint(0, 0, CARD_TEXT_COLOR, cardPlayerAutoLabel.code.text);
            nmlModelSetLight(&asColor, &asDir);
        }
        xglFontPrint(0, 0, 0, "\031\003");
        if (!(work->save->flags & CARD_SAVE_AUTO_SPIN))
            xglFontPrint(360, 265, CARD_TEXT_COLOR, "Auto spin off");
        else
            xglFontPrint(360, 265, CARD_TEXT_COLOR, "Auto spin on");
        position[1] = -0.40780002f;
        xglMatrixTrans(placement, matrix, position);
        xglMatrixScale(placement, placement, scale);
        nmlModelSetPlace(&placement);
        nmlModelSetTexture(CARD_MENU_TEXTURE);
        nmlModelEntryCard(CARD_MENU_OPTION_LEX);
    }

    /* Title plate behind the menu and the guide text line. */
    xglMatrixUnit(matrix);
    position[0] = 0.0f;
    position[1] = -1.2f;
    position[2] = 1.01f;
    scale[0] = scale[1] = 0.85f;
    xglMatrixUnit(matrix);
    xglMatrixTrans(matrix, matrix, position);
    xglMatrixScale(matrix, matrix, scale);
    nmlModelSetPlace(&matrix);
    nmlModelSetTexture(CARD_TITLE_TEXTURE_ADDR);
    nmlModelEntryCard(CARD_TITLE_MODEL_ADDR);

    xglFontPrint(0, 0, CARD_TEXT_COLOR, cardMenuFooterFontCode);
    {
        char *guide;

        if (work->command.exitMenuSub < 0)
            guide = CardPlayTMessList[EXIT_MENU_TEXT_FIRST + work->command.exitMenuItem];
        else
            guide = CardPlayTMessList[EXIT_MENU_TEXT_FIRST + EXIT_MENU_TEXT_OPTION_OFFSET + work->command.exitMenuSub];
        CardDispTxtSub(24, 322, guide, 23);
    }
    xglFontPrint(0, 0, CARD_TEXT_COLOR, cardMenuFooterEndCode);

    position[0] = position[1] = 0.0f;
    position[2] = position[3] = 1.0f;
    xglMatrixUnit(matrix);
    xglMatrixTrans(matrix, matrix, position);
    nmlModelSetPlace(&matrix);
    nmlModelEntryCard(CARD_BLACK_LEX);

    return result;
}


/*
 * Game-over display of a card game (CaedGameWinMode, 0x00a029f8).  CardGameProc
 * calls it once per frame while CardCommandControl.flowRequest is 3..5 with the
 * outcome (0, 1, 2 or 3; -1 and anything else select the default sequence) and
 * the round it passes on (CardMainProc's win12).  The function runs a
 * state machine on command.step: step 0 arms a 25 frame wait, step 1 refreshes
 * the field HP and jumps to the banner sequence of the outcome (steps 2..5,
 * 10..12, 20..22, 30..33, 40..43).  Every later step draws its banner models
 * and moves on when the shared timer, reloaded with 17, has counted down.
 * Steps 5, 12, 22 and 33 keep drawing for good; step 43 keeps cycling the timer.
 */
/*
 * Outcomes CardGameProc passes (flowRequest 3..5 select the case through its jump table at 0x00a4f620, index
 * flowRequest - 1; flowRequest 3 is raised at 0x00a317d0 when CardPlayYamaCnt of the enemy side is 0, flowRequest 4
 * at 0x00a317f0 for the player side): jal 0x00a31bfc, 0x00a31c14, 0x00a31c3c, 0x00a31c54, 0x00a31c70.
 */

#define CARD_WIN_STEP_BEGIN 0
#define CARD_WIN_STEP_SELECT 1
#define CARD_WIN_STEP_OUTCOME_0 2
#define CARD_WIN_STEP_OUTCOME_1 10
#define CARD_WIN_STEP_OUTCOME_2 20
#define CARD_WIN_STEP_OUTCOME_3 30
#define CARD_WIN_STEP_OTHER 40

#define CARD_WIN_BEGIN_DELAY 25     /* li 0x00a02a58, stored to the timer with sb at 0x00a02a64 */
#define CARD_WIN_STEP_FRAMES 17     /* 0x00a02abc: frames each step of a sequence lasts */
#define CARD_WIN_FINAL_ROUND 5      /* 0x00a02b00: outcome 0 plays CardPlaySound4 instead of 3 */
#define CARD_WIN_ALPHA 72           /* 0x00a02c2c */

/*
 * The banner models and their texture sit in one resource block at 0x01950000,
 * 0x510 bytes apart; CardGameInit reads the files there (jal 0x00a25868..0x00a25920).
 * Slot 9 is the texture (0x00a02c1c).  The sequences draw the first banner (victory7) and then
 * victory4 (step 4/5), victory2 (11/12) or victory3 (21/22), victory1, victory5, victory6
 * (31..33) or NewLost2, NewLost1 (40..43).  The lost banners alternate between two
 * textures that CardGameInit reads to 0x01960000 and 0x01963680 (jal 0x00a2592c, 0x00a25940).
 */
enum CardWinResource {
    CARD_WIN_VICTORY1,      /* game\victory1.lex, slots 0..6 are victory1..7 */
    CARD_WIN_VICTORY2,
    CARD_WIN_VICTORY3,
    CARD_WIN_VICTORY4,
    CARD_WIN_VICTORY5,
    CARD_WIN_VICTORY6,
    CARD_WIN_VICTORY7,
    CARD_WIN_LOST1,         /* game\NewLost1.lex */
    CARD_WIN_LOST2,         /* game\NewLost2.lex */
    CARD_WIN_VICTORY_BIN    /* game\victory.bin: the texture of the victory banners */
};

#define CARD_WIN_RESOURCE_BASE 0x01950000
#define CARD_WIN_RESOURCE_SIZE 0x510
#define CARD_WIN_RESOURCE(slot) ((slot) * CARD_WIN_RESOURCE_SIZE + CARD_WIN_RESOURCE_BASE)
#define CARD_WIN_TEXTURE ((void *) CARD_WIN_RESOURCE(CARD_WIN_VICTORY_BIN))
#define CARD_WIN_LOST_XTX ((void *) 0x01960000)        /* game\NewLost1.xtx, then game\NewLost2.xtx 0x3680 bytes on */
#define CARD_WIN_LOST_XTX_SIZE 0x3680       /* li 13952 at 0x00a03710 and 0x00a03834, multiplied by the lowest timer bit (andi/mult 0x00a03714/0x00a03718) */
#define CARD_WIN_ALT_TEXTURE(flip) (((flip) & 1) * CARD_WIN_LOST_XTX_SIZE + CARD_WIN_LOST_XTX)

#define CARD_WIN_ZOOM_RATE (4.0f / 9.0f)    /* 0x3EE38E39: scale gained per timer tick */
#define CARD_WIN_SHRINK_RATE (1.0f / 18.0f) /* 0x3D638E39: step 42's second banner */

/*
 * The scale of a pulsing banner is read from the timer before xglMatrixUnit
 * (the multiply sits in that call's delay slot, 0x00a02be4); `grow` and `rest`
 * are block-scoped so that they stay local pseudos and the 1.0 is kept in a
 * callee-saved register across the call.
 */

/* Draw one banner model at the current place matrix. */
#define CARD_WIN_DRAW(model) \
    nmlModelSetPlace(&place); \
    nmlModelSetTexture(CARD_WIN_TEXTURE); \
    nmlModelSetToumei(1); \
    nmlModelSetAlpha(CARD_WIN_ALPHA); \
    nmlModelEntry(model)

#define CARD_WIN_DRAW_ALT(model) \
    nmlModelSetPlace(&place); \
    nmlModelSetTexture(CARD_WIN_ALT_TEXTURE(timer)); \
    nmlModelSetToumei(1); \
    nmlModelSetAlpha(CARD_WIN_ALPHA); \
    nmlModelEntry(model)

void CaedGameWinMode(int outcome, CardGameWork *work, int round)
{
    static u8 timer;
    float place[4][4];
    float scale[4];

    switch (work->command.step) {
    case CARD_WIN_STEP_BEGIN:
        work->command.step++;
        timer = CARD_WIN_BEGIN_DELAY;
        break;
    case CARD_WIN_STEP_SELECT:
        if (timer == 0) {
            if (work->command.slideDirection != 0) {
                work->command.slideDirection = 0;
                work->command.slideTimer = CARD_SLIDE_FRAMES;
            }
            work->mess = 0;
            work->interruptMess = 0;
            work->permanentMess = 0;
            work->player.hand.deckCount = CardPlayYamaCnt(&work->player);
            work->enemy.hand.deckCount = CardPlayYamaCnt(&work->enemy);
            CGPRefreshFieldHP(work);
            timer = CARD_WIN_STEP_FRAMES;
            switch (outcome) {
            case CARD_WIN_OUTCOME_ENEMY_DECK_OUT:
                if (round == CARD_WIN_FINAL_ROUND)
                    CardPlaySound4();
                else
                    CardPlaySound3();
                work->command.step = CARD_WIN_STEP_OUTCOME_0;
                break;
            case CARD_WIN_OUTCOME_ENEMY_DECK_OUT_FASE9:
                CardPlaySound3();
                work->command.step = CARD_WIN_STEP_OUTCOME_1;
                break;
            case CARD_WIN_OUTCOME_PLAYER_DECK_OUT_FASE9:
                CardPlaySound3();
                work->command.step = CARD_WIN_STEP_OUTCOME_2;
                break;
            case CARD_WIN_OUTCOME_FINAL_ROUND:
                work->command.step = CARD_WIN_STEP_OUTCOME_3;
                break;
            default:               /* CARD_WIN_OUTCOME_PLAYER_DECK_OUT and anything else */
                work->command.step = CARD_WIN_STEP_OTHER;
                break;
            }
        } else {
            timer--;
        }
        break;
    case CARD_WIN_STEP_OUTCOME_0:
        if (timer != 0) {
            timer--;
        } else {
            timer = CARD_WIN_STEP_FRAMES;
            work->command.step++;
        }
        break;
    case CARD_WIN_STEP_OUTCOME_0 + 1:
        {
            float grow = timer * CARD_WIN_ZOOM_RATE;
            float rest = 1.0f;
            xglMatrixUnit(place);
            scale[3] = scale[2] = rest;
            scale[0] = scale[1] = grow + rest;
        }
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY7));
        if (timer != 0) {
            timer--;
        } else {
            timer = CARD_WIN_STEP_FRAMES;
            work->command.step++;
        }
        break;
    case CARD_WIN_STEP_OUTCOME_0 + 2:
        xglMatrixUnit(place);
        scale[0] = scale[1] = 1.0f;
        scale[2] = scale[3] = 1.0f;
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY7));
        {
            float grow = timer * CARD_WIN_ZOOM_RATE;
            float rest = 1.0f;
            xglMatrixUnit(place);
            scale[3] = scale[2] = rest;
            scale[0] = scale[1] = grow + rest;
        }
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY4));
        if (timer == 0) {
            timer = CARD_WIN_STEP_FRAMES;
            work->command.step++;
        } else {
            timer--;
        }
        break;
    case CARD_WIN_STEP_OUTCOME_0 + 3:
        xglMatrixUnit(place);
        scale[0] = scale[1] = 1.0f;
        scale[2] = scale[3] = 1.0f;
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY7));
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY4));
        break;
    case CARD_WIN_STEP_OUTCOME_1:
        {
            float grow = timer * CARD_WIN_ZOOM_RATE;
            float rest = 1.0f;
            xglMatrixUnit(place);
            scale[3] = scale[2] = rest;
            scale[0] = scale[1] = grow + rest;
        }
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY7));
        if (timer != 0) {
            timer--;
        } else {
            timer = CARD_WIN_STEP_FRAMES;
            work->command.step++;
        }
        break;
    case CARD_WIN_STEP_OUTCOME_1 + 1:
        xglMatrixUnit(place);
        scale[0] = scale[1] = 0.9f;
        scale[2] = scale[3] = 1.0f;
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY7));
        {
            float grow = timer * CARD_WIN_ZOOM_RATE;
            float rest = 1.0f;
            xglMatrixUnit(place);
            scale[3] = scale[2] = rest;
            scale[0] = scale[1] = grow + rest;
        }
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY2));
        if (timer == 0) {
            timer = CARD_WIN_STEP_FRAMES;
            work->command.step++;
        } else {
            timer--;
        }
        break;
    case CARD_WIN_STEP_OUTCOME_1 + 2:
        xglMatrixUnit(place);
        scale[0] = scale[1] = 0.9f;
        scale[2] = scale[3] = 1.0f;
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY7));
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY2));
        break;
    case CARD_WIN_STEP_OUTCOME_2:
        {
            float grow = timer * CARD_WIN_ZOOM_RATE;
            float rest = 1.0f;
            xglMatrixUnit(place);
            scale[3] = scale[2] = rest;
            scale[0] = scale[1] = grow + rest;
        }
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY7));
        if (timer != 0) {
            timer--;
        } else {
            timer = CARD_WIN_STEP_FRAMES;
            work->command.step++;
        }
        break;
    case CARD_WIN_STEP_OUTCOME_2 + 1:
        xglMatrixUnit(place);
        scale[0] = scale[1] = 0.9f;
        scale[2] = scale[3] = 1.0f;
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY7));
        {
            float grow = timer * CARD_WIN_ZOOM_RATE;
            float rest = 1.0f;
            xglMatrixUnit(place);
            scale[3] = scale[2] = rest;
            scale[0] = scale[1] = grow + rest;
        }
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY3));
        if (timer == 0) {
            timer = CARD_WIN_STEP_FRAMES;
            work->command.step++;
        } else {
            timer--;
        }
        break;
    case CARD_WIN_STEP_OUTCOME_2 + 2:
        xglMatrixUnit(place);
        scale[0] = scale[1] = 0.9f;
        scale[2] = scale[3] = 1.0f;
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY7));
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY3));
        break;
    case CARD_WIN_STEP_OUTCOME_3:
        {
            float grow = timer * CARD_WIN_ZOOM_RATE;
            float rest = 1.0f;
            xglMatrixUnit(place);
            scale[3] = scale[2] = rest;
            scale[0] = scale[1] = grow + rest;
        }
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY1));
        if (timer != 0) {
            timer--;
        } else {
            timer = CARD_WIN_STEP_FRAMES;
            work->command.step++;
        }
        break;
    case CARD_WIN_STEP_OUTCOME_3 + 1:
        xglMatrixUnit(place);
        scale[0] = scale[1] = 0.9f;
        scale[2] = scale[3] = 1.0f;
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY1));
        {
            float grow = timer * CARD_WIN_ZOOM_RATE;
            float rest = 1.0f;
            xglMatrixUnit(place);
            scale[3] = scale[2] = rest;
            scale[0] = scale[1] = grow + rest;
        }
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY5));
        if (timer == 0) {
            timer = CARD_WIN_STEP_FRAMES;
            work->command.step++;
        } else {
            timer--;
        }
        break;
    case CARD_WIN_STEP_OUTCOME_3 + 2:
        xglMatrixUnit(place);
        scale[0] = scale[1] = 0.9f;
        scale[2] = scale[3] = 1.0f;
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY1));
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY5));
        {
            float grow = timer * CARD_WIN_ZOOM_RATE;
            float rest = 1.0f;
            xglMatrixUnit(place);
            scale[3] = scale[2] = rest;
            scale[0] = scale[1] = grow + rest;
        }
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY6));
        if (timer != 0) {
            timer--;
        } else {
            timer = CARD_WIN_STEP_FRAMES;
            work->command.step++;
        }
        break;
    case CARD_WIN_STEP_OUTCOME_3 + 3:
        xglMatrixUnit(place);
        scale[0] = scale[1] = 0.9f;
        scale[2] = scale[3] = 1.0f;
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY1));
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY5));
        CARD_WIN_DRAW(CARD_WIN_RESOURCE(CARD_WIN_VICTORY6));
        break;
    case CARD_WIN_STEP_OTHER:
        {
            float grow = timer * CARD_WIN_ZOOM_RATE;
            float rest = 1.0f;
            xglMatrixUnit(place);
            scale[3] = scale[2] = rest;
            scale[0] = scale[1] = grow + rest;
        }
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW_ALT(CARD_WIN_RESOURCE(CARD_WIN_LOST2));
        if (timer != 0) {
            timer--;
        } else {
            timer = CARD_WIN_STEP_FRAMES;
            work->command.step++;
        }
        break;
    case CARD_WIN_STEP_OTHER + 1:
        xglMatrixUnit(place);
        scale[0] = scale[1] = 1.0f;
        scale[2] = scale[3] = 1.0f;
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW_ALT(CARD_WIN_RESOURCE(CARD_WIN_LOST2));
        if (timer != 0) {
            timer--;
        } else {
            timer = CARD_WIN_STEP_FRAMES;
            work->command.step++;
        }
        break;
    case CARD_WIN_STEP_OTHER + 2:
        {
            float grow = timer * CARD_WIN_ZOOM_RATE;
            float rest = 1.0f;
            xglMatrixUnit(place);
            scale[2] = scale[3] = rest;
            scale[0] = scale[1] = grow + rest;
        }
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW_ALT(CARD_WIN_RESOURCE(CARD_WIN_LOST1));
        xglMatrixUnit(place);
        scale[2] = scale[3] = 1.0f;
        /* The original adds 0.0f after the multiply (add.s 0x00a038f0). */
        scale[0] = scale[1] = timer * CARD_WIN_SHRINK_RATE + 0.0f;
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW_ALT(CARD_WIN_RESOURCE(CARD_WIN_LOST2));
        if (timer != 0) {
            timer--;
        } else {
            timer = CARD_WIN_STEP_FRAMES;
            work->command.step++;
        }
        break;
    case CARD_WIN_STEP_OTHER + 3:
        xglMatrixUnit(place);
        scale[0] = scale[1] = 1.0f;
        scale[2] = scale[3] = 1.0f;
        xglMatrixScale(place, place, scale);
        CARD_WIN_DRAW_ALT(CARD_WIN_RESOURCE(CARD_WIN_LOST1));
        if (timer != 0)
            timer--;
        else
            timer = CARD_WIN_STEP_FRAMES;
        break;
    default:
        break;
    }
}


void CMTSetFontColor(s32 count, s32 player)
{
    if (count == player) {
        xglFontPrint(0, 0, 0, "\031\003");
    } else if (count < player) {
        xglFontPrint(0, 0, 0, "\031\003");
    } else {
        xglFontPrint(0, 0, 0, "\031\003");
    }
}

static inline void CardHideModels(CardGameWork *work) {
    int model;

    for (model = CARD_MODEL_COUNT - 1; model >= 0; model--)
        work->models[model].visible = 0;
}

static inline void CardLoadBattleWindows(void) {
    CardFread(CARD_BATTLE_WINDOW(0), cardBattleWin1LexName);
    CardFread(CARD_BATTLE_WINDOW(1), cardBattleWin2LexName);
    CardFread(CARD_BATTLE_WINDOW(2), cardBattleWin3LexName);
    CardFread(CARD_BATTLE_WINDOW(3), cardBattleWin4LexName);
    CardFread(CARD_BATTLE_WINDOW(4), cardBattleWin5LexName);
    CardFread(CARD_BATTLE_WINDOW_TEXTURE, cardBattleWinBinName);
}

/* CardGameWork.faseStep of the screens that run steps in sequence (CardMainProc). */
enum CardViewStep {
    CARD_VIEW_STEP_LOAD,
    CARD_VIEW_STEP_FADE_IN        /* later steps: CardViewProc */
};

enum CardVsStep {
    CARD_VS_STEP_LOAD,            /* fade out, load the type icons, list the stored deck */
    CARD_VS_STEP_PICK_DECK_1P,
    CARD_VS_STEP_PICK_DECK_2P,    /* cancel returns to 1P */
    CARD_VS_STEP_FADE_TO_BATTLE,
    CARD_VS_STEP_BATTLE,
    CARD_VS_STEP_RESULT           /* wait for a face button */
};

enum CardTournamentStep {
    CARD_TOURNAMENT_STEP_LOAD,
    CARD_TOURNAMENT_STEP_FADE_IN,
    CARD_TOURNAMENT_STEP_PICK_DECK,
    CARD_TOURNAMENT_STEP_DRAW_OPPONENTS,   /* shuffle the pool into lst11, falls into the next step */
    CARD_TOURNAMENT_STEP_OPPONENTS,        /* the five opponent plates; the round about to start */
    CARD_TOURNAMENT_STEP_FADE_TO_BATTLE,
    CARD_TOURNAMENT_STEP_BATTLE,
    CARD_TOURNAMENT_STEP_ROUND_WON,
    CARD_TOURNAMENT_STEP_CHAMPION = 10,    /* all rounds won: roll the prizes */
    CARD_TOURNAMENT_STEP_PRIZE_WON,
    CARD_TOURNAMENT_STEP_ADD_PRIZES = 15,
    CARD_TOURNAMENT_STEP_LOST = 20,        /* a round was lost: roll the consolation packs */
    CARD_TOURNAMENT_STEP_PRIZE_LOST
};

enum CardExhibitionStep {
    CARD_EXHIBITION_STEP_LOAD,
    CARD_EXHIBITION_STEP_FADE_IN,
    CARD_EXHIBITION_STEP_PICK_DECK,
    CARD_EXHIBITION_STEP_DRAW_OPPONENT,    /* picks lst14, falls into the next step */
    CARD_EXHIBITION_STEP_CONFIRM,
    CARD_EXHIBITION_STEP_FADE_TO_BATTLE,
    CARD_EXHIBITION_STEP_BATTLE,
    CARD_EXHIBITION_STEP_RESULT,
    CARD_EXHIBITION_STEP_PRIZE
};

enum CardDeckBuildStep {
    CARD_DECK_BUILD_STEP_INIT,
    CARD_DECK_BUILD_STEP_FADE_OUT,
    CARD_DECK_BUILD_STEP_LOAD,
    CARD_DECK_BUILD_STEP_FADE_IN,
    CARD_DECK_BUILD_STEP_LOAD_PROMPT,      /* "Load deck?" */
    CARD_DECK_BUILD_STEP_PICK_DECK,
    CARD_DECK_BUILD_STEP_BUILD_INIT = 10,
    CARD_DECK_BUILD_STEP_BUILD,
    CARD_DECK_BUILD_STEP_SAVE_INIT,        /* falls into the next step */
    CARD_DECK_BUILD_STEP_SAVE
};

/* Enemy decks the tournament and the exhibition draw from: all of them once flag 0x186 is set, otherwise the first
 * ones, more after flag 0x12D. */
#define CARD_ENEMY_POOL_ALL 40
#define CARD_ENEMY_POOL_ADVANCED 19
#define CARD_ENEMY_POOL_FIRST 10
#define CARD_FLAG_ALL_ENEMY_DECKS 0x186
#define CARD_TOURNAMENT_ROUNDS 5   /* opponent plates (cld1.count), CardBattleWindow 0..4; win12 == 5 wins the cup */

s32 CardMainProc(CardGameWork *work) {
    static CardListWindow cld0;
    static CardListWindow cld1;
    static CardListWindow cldtmp;
    static int lst11[48];
    static int win12;
    static int item13[4];
    static int lst14[1];
    static int win15;
    static int pack16;
    float matrix[4][4];
    float placement[4][4];
    float position[4];
    StudioLight *studioLight;
    int i;
    int win;
    float pulse;

    work->command.input[0].held = PadData[0].half_28;
    work->command.input[0].pressed = PadData[0].half_2a;
    work->command.input[0].directions = PadData[0].directions;
    work->command.input[0].repeat = PadData[0].half_2a;
    work->command.input[1].held = PadData[0].half_28;
    work->command.input[1].pressed = PadData[0].half_2a;
    work->command.input[1].directions = PadData[0].directions;
    work->command.input[1].repeat = PadData[0].half_2a;
    if (work->fase == CARD_FASE_BATTLE_2P) {
        work->command.input[1].held = PadData[1].half_28;
        work->command.input[1].pressed = PadData[1].half_2a;
        work->command.input[1].directions = PadData[1].directions;
        work->command.input[1].repeat = PadData[1].half_2a;
    }

    xglStudioGetLight(&studioLight);
    xglLightIntensityAmbient(studioLight, work->modelColor);

    CCtimer_5 = (CCtimer_5 + 1) & 0x1F;
    pulse = xglSin(CCtimer_5 * 11.25f / 180.0f * 3.1415927f) * 0.5f + 1.0f;
    CCColor[3][0] = CCColor[3][1] = CCColor[3][2] = pulse;
    if (CCtimer_5 & 1)
        KKColor[3][0] = KKColor[3][1] = KKColor[3][2] = 1.5f;
    else
        KKColor[3][0] = KKColor[3][1] = KKColor[3][2] = 0.75f;

    if (work->helpState) {
        xglFontDebugPrintf(0x54, 0, "Help");
        CardHelpProc(work);
    } else {
        switch (work->fase) {
        case CARD_FASE_TITLE:
            CardFadeOut(work);
            if (work->fadeOutTimer == 0) {
                xglSoundLoadRequestSmd("Umnmode", CARD_SCRATCH_BUFFER);
                work->fase = CARD_FASE_MENU;
                CardTitleInit(work);
                work->fadeOutTimer = CARD_FADE_TICKS;
            }
            break;

        case CARD_FASE_MENU: {
            int previousFase;

                xglFontDebugPrintf(0x54, 0, "Menu");
                previousFase = work->fase;
                CardFadeIn(work);
                if (SubNo_2 < 0) {
                    if (PadData[0].half_2a & PAD_UP) {
                        MenuNo_1--;
                        xglSoundEffectNormalID(SE_CURSOR, 0);
                    } else if (PadData[0].half_2a & PAD_DOWN) {
                        MenuNo_1++;
                        xglSoundEffectNormalID(SE_CURSOR, 0);
                    }
                    if (MenuNo_1 < 0)
                        MenuNo_1 = 3;
                    if (MenuNo_1 >= 4)
                        MenuNo_1 = 0;
                    if (PadData[0].half_2a & PAD_CIRCLE) {
                        switch (MenuNo_1) {
                        case 0:
                        case 1:
                        case 2:
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                            SubNo_2 = 0;
                            break;
                        case 3:
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                            return 0;
                        case 4:
                            work->fase = CARD_FASE_TEST_MENU;
                            break;
                        }
                    }
                } else {
                    if (PadData[0].half_2a & PAD_UP) {
                        SubNo_2--;
                        xglSoundEffectNormalID(SE_CURSOR, 0);
                    } else if (PadData[0].half_2a & PAD_DOWN) {
                        SubNo_2++;
                        xglSoundEffectNormalID(SE_CURSOR, 0);
                    }
                    if (MenuNo_1 == 0) {
                        if (SubNo_2 < 0)
                            SubNo_2 = 3;
                        if (SubNo_2 >= 4)
                            SubNo_2 = 0;
                    } else if (MenuNo_1 == 1) {
                        if (SubNo_2 < 0)
                            SubNo_2 = 2;
                        if (SubNo_2 >= 3)
                            SubNo_2 = 0;
                    } else {
                        if (SubNo_2 < 0)
                            SubNo_2 = 1;
                        if (SubNo_2 >= 2)
                            SubNo_2 = 0;
                    }
                    if (PadData[0].half_2a & PAD_CROSS) {
                        xglSoundEffectNormalID(SE_CANCEL, 0);
                        SubNo_2 = -1;
                    } else if (PadData[0].half_2a & PAD_CIRCLE) {
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        if (MenuNo_1 == 0) {
                            switch (SubNo_2) {
                            case 0:
                                work->fase = CARD_FASE_EXHIBITION;
                                break;
                            case 1:
                                work->fase = CARD_FASE_TOURNAMENT;
                                break;
                            case 2:
                                work->fase = CARD_FASE_BATTLE_2P;
                                break;
                            case 3:
                                work->save->background++;
                                if (work->save->background >= 6)
                                    work->save->background = 0;
                                CMIBGInit(work);
                                break;
                            default:
                                /* Toggles the bit by adding it, as the original does. */
                                work->save->flags = (work->save->flags & ~CARD_SAVE_LARGE_CARDS) |
                                                    ((work->save->flags + CARD_SAVE_LARGE_CARDS) & CARD_SAVE_LARGE_CARDS);
                                xglSoundEffectNormalID(SE_DECIDE, 0);
                                if (work->save->flags & CARD_SAVE_LARGE_CARDS) {
                                    work->models[CARD_MODEL_MAT_MOJI].model = CARD_MAT_MOJI_LEX;
                                    work->models[CARD_MODEL_MAT_MOJI].texture = CARD_MAT_FG_XTX;
                                    work->models[CARD_MODEL_MAT_MOJI_2].model = CARD_MAT_MOJI_LEX;
                                    work->models[CARD_MODEL_MAT_MOJI_2].texture = CARD_MAT_FG_XTX;
                                } else {
                                    work->models[CARD_MODEL_MAT_MOJI].model = CARD_MAT_MOJI_J_LEX;
                                    work->models[CARD_MODEL_MAT_MOJI].texture = CARD_MAT_MOJI_J_XTX;
                                    work->models[CARD_MODEL_MAT_MOJI_2].model = CARD_MAT_MOJI_J_LEX;
                                    work->models[CARD_MODEL_MAT_MOJI_2].texture = CARD_MAT_MOJI_J_XTX;
                                }
                                break;
                            }
                        } else if (MenuNo_1 == 1) {
                            switch (SubNo_2) {
                            case 0:
                                work->fase = CARD_FASE_DECK_BUILD;
                                break;
                            case 1:
                                work->faseStep = 0;
                                work->fase = CARD_FASE_OPEN_PACKS;
                                break;
                            default:
                                work->fase = CARD_FASE_VIEW_CARDS;
                                break;
                            }
                        } else {
                            switch (SubNo_2) {
                            case 0:
                                work->fase = CARD_FASE_TUTORIAL;
                                work->fadeOutTimer = CARD_FADE_TICKS;
                                work->fadeOutStep = CARD_FADE_STEP;
                                work->faseStep = 0;
                                break;
                            case 1:
                                work->helpState = CARD_HELP_STATE_OPEN;
                                work->fadeOutTimer = CARD_FADE_TICKS;
                                work->fadeOutStep = CARD_FADE_STEP;
                                work->faseStep = 0;
                                break;
                            }
                        }
                    }
                }

                {
                    char menuColor[] = "\v\r\0\f\x10\x10\x10";
                    char cursorColor[] = "\f(((";
                    CardFontCode5 selectColor = cardHelpFontCodes.code;
                    void *model;
                    void *texture;

                    position[0] = position[1] = position[2] = 0.0f;
                    position[3] = 1.0f;
                    xglMatrixUnit(matrix);

                    xglFontPrint(0, 0, CARD_TEXT_COLOR, menuColor);
                    if (SubNo_2 >= 0)
                        xglFontPrint(0, 0, CARD_TEXT_COLOR, cursorColor);
                    if (MenuNo_1 == 0) {
                        nmlModelSetLight(&asColor, &asDir);
                        xglFontPrint(0, 0, CARD_TEXT_COLOR, selectColor.text);
                    }
                    xglFontPrint(0, 0, 0, "\031\003");
                    xglFontPrint(0xDE, 0x8C, CARD_TEXT_COLOR, " Game");
                    position[1] = 0.6787f;
                    xglMatrixTrans(placement, matrix, position);
                    nmlModelSetPlace(&placement);
                    nmlModelSetTexture(CARD_MENU_TEXTURE);
                    nmlModelEntryCard(CARD_MENU_ITEM_LEX);

                    xglFontPrint(0, 0, CARD_TEXT_COLOR, menuColor);
                    if (SubNo_2 >= 0)
                        xglFontPrint(0, 0, CARD_TEXT_COLOR, cursorColor);
                    if (MenuNo_1 == 1) {
                        nmlModelSetLight(&asColor, &asDir);
                        xglFontPrint(0, 0, CARD_TEXT_COLOR, selectColor.text);
                    }
                    xglFontPrint(0, 0, 0, "\031\003");
                    xglFontPrint(0xDE, 0xB1, CARD_TEXT_COLOR, " Card");
                    position[1] = 0.33f;
                    xglMatrixTrans(placement, matrix, position);
                    nmlModelSetPlace(&placement);
                    nmlModelSetTexture(CARD_MENU_TEXTURE);
                    nmlModelEntryCard(CARD_MENU_ITEM_LEX);

                    xglFontPrint(0, 0, CARD_TEXT_COLOR, menuColor);
                    if (SubNo_2 >= 0)
                        xglFontPrint(0, 0, CARD_TEXT_COLOR, cursorColor);
                    if (MenuNo_1 == 2) {
                        xglFontPrint(0, 0, CARD_TEXT_COLOR, selectColor.text);
                        nmlModelSetLight(&asColor, &asDir);
                    }
                    xglFontPrint(0, 0, 0, "\031\003");
                    xglFontPrint(0xDE, 0xD6, CARD_TEXT_COLOR, cardHelpLabel);
                    position[1] = -0.0202f;
                    xglMatrixTrans(placement, matrix, position);
                    nmlModelSetPlace(&placement);
                    nmlModelSetTexture(CARD_MENU_TEXTURE);
                    nmlModelEntryCard(CARD_MENU_ITEM_LEX);

                    xglFontPrint(0, 0, CARD_TEXT_COLOR, menuColor);
                    if (SubNo_2 >= 0)
                        xglFontPrint(0, 0, CARD_TEXT_COLOR, cursorColor);
                    if (MenuNo_1 == 3) {
                        xglFontPrint(0, 0, CARD_TEXT_COLOR, selectColor.text);
                        nmlModelSetLight(&asColor, &asDir);
                    }
                    xglFontPrint(0, 0, 0, "\031\003");
                    xglFontPrint(0xDE, 0xFB, CARD_TEXT_COLOR, " Exit");
                    position[1] = -0.3692f;
                    xglMatrixTrans(placement, matrix, position);
                    nmlModelSetPlace(&placement);
                    nmlModelSetTexture(CARD_MENU_TEXTURE);
                    nmlModelEntryCard(CARD_MENU_ITEM_LEX);

                    if (SubNo_2 >= 0) {
                        CardFontCode8 itemColor = cardPlayerLabelFontCodes;
                        CardFontCode5 itemSelectColor = cardPlayerAutoLabel.code;

                        position[0] = 1.8374f;
                        if (MenuNo_1 == 0) {
                            xglFontPrint(0, 0, CARD_TEXT_COLOR, itemColor.text);
                            if (SubNo_2 == 0) {
                                xglFontPrint(0, 0, CARD_TEXT_COLOR, itemSelectColor.text);
                                nmlModelSetLight(&asColor, &asDir);
                            }
                            xglFontPrint(0, 0, 0, "\031\003");
                            xglFontPrint(0x168, 0x7A, CARD_TEXT_COLOR, "Exhibition");
                            position[1] = 0.8521f;
                            xglMatrixTrans(placement, matrix, position);
                            nmlModelSetPlace(&placement);
                            nmlModelSetTexture(CARD_MENU_TEXTURE);
                            nmlModelEntryCard(CARD_MENU_OPTION_LEX);

                            xglFontPrint(0, 0, CARD_TEXT_COLOR, itemColor.text);
                            if (SubNo_2 == 1) {
                                xglFontPrint(0, 0, CARD_TEXT_COLOR, itemSelectColor.text);
                                nmlModelSetLight(&asColor, &asDir);
                            }
                            xglFontPrint(0, 0, 0, "\031\003");
                            xglFontPrint(0x168, 0x9E, CARD_TEXT_COLOR, "Tournament");
                            position[1] = 0.5015f;
                            xglMatrixTrans(placement, matrix, position);
                            nmlModelSetPlace(&placement);
                            nmlModelSetTexture(CARD_MENU_TEXTURE);
                            nmlModelEntryCard(CARD_MENU_OPTION_LEX);

                            xglFontPrint(0, 0, CARD_TEXT_COLOR, itemColor.text);
                            if (SubNo_2 == 2) {
                                xglFontPrint(0, 0, CARD_TEXT_COLOR, itemSelectColor.text);
                                nmlModelSetLight(&asColor, &asDir);
                            }
                            xglFontPrint(0, 0, 0, "\031\003");
                            xglFontPrint(0x168, 0xC2, CARD_TEXT_COLOR, "2P Battle");
                            position[1] = 0.1509f;
                            xglMatrixTrans(placement, matrix, position);
                            nmlModelSetPlace(&placement);
                            nmlModelSetTexture(CARD_MENU_TEXTURE);
                            nmlModelEntryCard(CARD_MENU_OPTION_LEX);

                            xglFontPrint(0, 0, CARD_TEXT_COLOR, itemColor.text);
                            if (SubNo_2 == 3) {
                                xglFontPrint(0, 0, CARD_TEXT_COLOR, itemSelectColor.text);
                                nmlModelSetLight(&asColor, &asDir);
                            }
                            xglFontPrint(0, 0, 0, "\031\003");
                            xglFontPrint(0x168, 0xE8, CARD_TEXT_COLOR, "Change BG");
                            position[1] = -0.1976f;
                            xglMatrixTrans(placement, matrix, position);
                            nmlModelSetPlace(&placement);
                            nmlModelSetTexture(CARD_MENU_TEXTURE);
                            nmlModelEntryCard(CARD_MENU_OPTION_LEX);
                        } else if (MenuNo_1 == 1) {
                            xglFontPrint(0, 0, CARD_TEXT_COLOR, itemColor.text);
                            if (SubNo_2 == 0) {
                                xglFontPrint(0, 0, CARD_TEXT_COLOR, itemSelectColor.text);
                                nmlModelSetLight(&asColor, &asDir);
                            }
                            xglFontPrint(0, 0, 0, "\031\003");
                            xglFontPrint(0x168, 0xC2, CARD_TEXT_COLOR, "Create deck");
                            position[1] = 0.1509f;
                            xglMatrixTrans(placement, matrix, position);
                            nmlModelSetPlace(&placement);
                            nmlModelSetTexture(CARD_MENU_TEXTURE);
                            nmlModelEntryCard(CARD_MENU_OPTION_LEX);

                            xglFontPrint(0, 0, CARD_TEXT_COLOR, itemColor.text);
                            if (SubNo_2 == 1) {
                                xglFontPrint(0, 0, CARD_TEXT_COLOR, itemSelectColor.text);
                                nmlModelSetLight(&asColor, &asDir);
                            }
                            xglFontPrint(0, 0, 0, "\031\003");
                            xglFontPrint(0x168, 0xE8, CARD_TEXT_COLOR, "Open card");
                            position[1] = -0.1976f;
                            xglMatrixTrans(placement, matrix, position);
                            nmlModelSetPlace(&placement);
                            nmlModelSetTexture(CARD_MENU_TEXTURE);
                            nmlModelEntryCard(CARD_MENU_OPTION_LEX);

                            xglFontPrint(0, 0, CARD_TEXT_COLOR, itemColor.text);
                            if (SubNo_2 == 2) {
                                xglFontPrint(0, 0, CARD_TEXT_COLOR, itemSelectColor.text);
                                nmlModelSetLight(&asColor, &asDir);
                            }
                            xglFontPrint(0, 0, 0, "\031\003");
                            xglFontPrint(0x168, 0x10D, CARD_TEXT_COLOR, "View cards");
                            position[1] = -0.5478f;
                            xglMatrixTrans(placement, matrix, position);
                            nmlModelSetPlace(&placement);
                            nmlModelSetTexture(CARD_MENU_TEXTURE);
                            nmlModelEntryCard(CARD_MENU_OPTION_LEX);
                        } else {
                            xglFontPrint(0, 0, CARD_TEXT_COLOR, itemColor.text);
                            if (SubNo_2 == 0) {
                                xglFontPrint(0, 0, CARD_TEXT_COLOR, itemSelectColor.text);
                                nmlModelSetLight(&asColor, &asDir);
                            }
                            xglFontPrint(0, 0, 0, "\031\003");
                            xglFontPrint(0x168, 0xE8, CARD_TEXT_COLOR, "Tutorial");
                            position[1] = -0.1976f;
                            xglMatrixTrans(placement, matrix, position);
                            nmlModelSetPlace(&placement);
                            nmlModelSetTexture(CARD_MENU_TEXTURE);
                            nmlModelEntryCard(CARD_MENU_OPTION_LEX);

                            xglFontPrint(0, 0, CARD_TEXT_COLOR, itemColor.text);
                            if (SubNo_2 == 1) {
                                xglFontPrint(0, 0, CARD_TEXT_COLOR, itemSelectColor.text);
                                nmlModelSetLight(&asColor, &asDir);
                            }
                            xglFontPrint(0, 0, 0, "\031\003");
                            xglFontPrint(0x168, 0x10D, CARD_TEXT_COLOR, "Help");
                            position[1] = -0.5478f;
                            xglMatrixTrans(placement, matrix, position);
                            nmlModelSetPlace(&placement);
                            nmlModelSetTexture(CARD_MENU_TEXTURE);
                            nmlModelEntryCard(CARD_MENU_OPTION_LEX);
                        }
                    }

                    position[0] = -1.8833f;
                    model = TitleMdlLst[MenuNo_1 * 2];
                    position[1] = 0.1576f;
                    texture = TitleMdlLst[MenuNo_1 * 2 + 1];
                    if (model != 0) {
                        xglMatrixTrans(placement, matrix, position);
                        nmlModelSetPlace(&placement);
                        nmlModelSetTexture(texture);
                        nmlModelEntryCard(model);
                    }
                }
                xglFontPrint(0, 0, CARD_TEXT_COLOR, cardMenuFooterFontCode);
                xglFontPrint(0, 0, 0, "\031\003");
                {
                    char *guide;

                    if (SubNo_2 < 0)
                        guide = CardPlayTMessList[50 + MenuNo_1];
                    else
                        guide = CardPlayTMessList[55 + MenuNo_1 * 5 + SubNo_2];
                    CardDispTxtSub(0x18, 0x138, guide, 0x17);
                }
                xglFontPrint(0xD8, 0x90, CARD_TEXT_COLOR, cardMenuFooterEndCode);
                if (previousFase != work->fase) {
                    cld0.mode = 2;
                    ModeExit_4 = 0;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    switch (work->fase) {
                    case CARD_FASE_TITLE:
                        break;
                    case CARD_FASE_VIEW_CARDS:
                        work->fadeOutTimer = CARD_FADE_TICKS;
                        work->fadeOutStep = CARD_FADE_STEP;
                        work->faseStep = 0;
                        work->command.step = 0;
                        break;
                    case CARD_FASE_DECK_BUILD:
                        work->fadeOutTimer = CARD_FADE_TICKS;
                        work->fadeOutStep = CARD_FADE_STEP;
                        work->faseStep = 0;
                        work->command.step = 0;
                        break;
                    case CARD_FASE_BATTLE_2P:
                        work->fadeOutTimer = CARD_FADE_TICKS;
                        work->fadeOutStep = CARD_FADE_STEP;
                        work->faseStep = 0;
                        work->command.step = 0;
                        break;
                    case CARD_FASE_TOURNAMENT:
                        work->fadeOutTimer = CARD_FADE_TICKS;
                        work->fadeOutStep = CARD_FADE_STEP;
                        work->faseStep = 0;
                        work->command.step = 0;
                        break;
                    case CARD_FASE_EXHIBITION:
                        work->fadeOutTimer = CARD_FADE_TICKS;
                        work->fadeOutStep = CARD_FADE_STEP;
                        work->faseStep = 0;
                        work->command.step = 0;
                        break;
                    }
                }
                break;
        }

        case CARD_FASE_OPEN_PACKS:
            CardOpenProc(work, &ModeExit_4);
            break;

        case CARD_FASE_VIEW_CARDS:
            switch (work->faseStep) {
            case CARD_VIEW_STEP_LOAD:
                CardFadeOut(work);
                if (work->fadeOutTimer == 0) {
                    work->fadeOutTimer = CARD_FADE_TICKS;
                    work->command.step = 0;
                    work->fadeOutStep = CARD_FADE_STEP;
                    work->faseStep++;
                    CardHideModels(work);
                    work->models[CARD_MODEL_VIEWER].visible = 1;
                    cld0.baseColor = CARD_LIST_BASE_COLOR;
                    cld0.cursorColor = CARD_LIST_CURSOR_COLOR;
                    cld0.x = 316;
                    cld0.y = 60;
                    cld0.textLength = CARD_LIST_TEXT_LENGTH;
                    cld0.rows = CARD_LIST_ROWS;
                    cld0.active = 0;
                    cld0.cursor = 0;
                    cld0.top = 0;
                    cld0.count = 0;
                }
                break;
            case CARD_VIEW_STEP_FADE_IN:
                CardFadeIn(work);
                if (work->fadeOutTimer == 0)
                    work->faseStep++;
                break;
            default:
                CardViewProc(work, &cld0, &ModeExit_4);
                break;
            }
            break;

        case CARD_FASE_BATTLE_2P: {
            int previous;
            int result;
            int last;
            char *title;
            char *enemyTitle;
            int exiting;

                exiting = 0;
                switch (work->faseStep) {
                case CARD_VS_STEP_LOAD:
                    CardFadeOut(work);
                    if (work->fadeOutTimer == 0) {
                        work->fadeOutTimer = CARD_FADE_TICKS;
                        work->fadeOutStep = CARD_FADE_STEP;
                        work->command.step = 0;
                        work->faseStep++;
                        for (i = 0; i < CARD_MODEL_COUNT; i++)
                            work->models[i].visible = 0;
                        work->models[CARD_MODEL_DECKLIST].visible = 1;
                        CardFread(CARD_TYPE_LEX(0), "deckmake\\type1_B.lex");
                        CardFread(CARD_TYPE_LEX(1), "deckmake\\type1_E.lex");
                        CardFread(CARD_TYPE_LEX(2), "deckmake\\type1_S.lex");
                        CardFread(CARD_TYPE_LEX(3), "deckmake\\type2_0.lex");
                        CardFread(CARD_TYPE_LEX(4), "deckmake\\type2_1.lex");
                        CardFread(CARD_TYPE_LEX(5), "deckmake\\type2_2.lex");
                        CardFread(CARD_TYPE_LEX(6), "deckmake\\type2_3.lex");
                        CardFread(CARD_TYPE_LEX(7), "deckmake\\type2_4.lex");
                        CardFread(CARD_TYPE_LEX(8), "deckmake\\type2_5.lex");
                        CardFread(CARD_TYPE_LEX(9), "deckmake\\type2_6.lex");
                        CardFread(CARD_TYPE_XTX(0), "deckmake\\type1_B.xtx");
                        CardFread(CARD_TYPE_XTX(1), "deckmake\\type1_E.xtx");
                        CardFread(CARD_TYPE_XTX(2), "deckmake\\type1_S.xtx");
                        CardFread(CARD_TYPE_XTX(3), "deckmake\\type2_0.xtx");
                        cld0.baseColor = CARD_LIST_BASE_COLOR;
                        cld0.cursorColor = CARD_LIST_CURSOR_COLOR;
                        cld0.cursor = 0;
                        cld1.mode = 0;
                        cld1.cursor = 0;
                        cld0.active = 1;
                        cld0.top = 0;
                        cld0.rows = CARD_LIST_ROWS;
                        cld0.textLength = CARD_LIST_TEXT_LENGTH;
                        cld1.textLength = CARD_LIST_TEXT_LENGTH;
                        cld1.top = 0;
                        cld0.x = CARD_LIST_LEFT_X;
                        cld0.y = 64;
                        cld0.count = 0;
                        cld1.x = CARD_LIST_RIGHT_X;
                        cld1.y = 60;
                        cld1.shownCount = 1;
                        cld1.rows = CARD_LIST_ROWS;
                        previous = -1;
                        last = -1;
                        for (i = 0; i < CARD_DECK_SIZE; i++) {
                            if (previous != work->save->decks[0].cards[i]) {
                                last++;
                                cld1.entries[last].cardId = work->save->decks[0].cards[i];
                                cld1.entries[last].count = 1;
                            } else {
                                cld1.entries[last].count++;
                            }
                            previous = work->save->decks[0].cards[i];
                        }
                        cld1.count = last + 1;
                    }
                    break;
                case CARD_VS_STEP_PICK_DECK_1P:
                    CardFadeIn(work);
                    title = P1DeckLoadStr;
                    if (ModeExit_4) {
                        title = 0;
                        xglFontPrint(0x18, 0x138, CARD_TEXT_COLOR, ModeEndMess0);
                        xglFontPrint(0xE0, 0x168, CARD_TEXT_COLOR, ModeEndMess1);
                        exiting = 1;
                        if (PadData[0].half_2a & PAD_CIRCLE) {
                            work->fadeOutTimer = CARD_FADE_TICKS;
                            work->fadeOutStep = CARD_FADE_STEP;
                            work->fase = CARD_FASE_TITLE;
                            ModeExit_4 = 0;
                        }
                        if (PadData[0].half_2a & (PAD_START | PAD_CROSS)) {
                            xglSoundEffectNormalID(SE_CANCEL, 0);
                            ModeExit_4 = 0;
                        }
                    } else if (work->command.input[0].pressed & PAD_START) {
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        ModeExit_4 = 1;
                    }
                    result = CardLoadDeckProc(exiting, &cld0, &cld1, work, title);
                    if (result != 0) {
                        if (result >= 0) {
                            CardCopyRam2Deck(work->playerDeck, work, result - 1);
                            work->command.step = 0;
                            work->faseStep++;
                        } else {
                            ModeExit_4 = 1;
                        }
                    }
                    break;
                case CARD_VS_STEP_PICK_DECK_2P:
                    enemyTitle = P2DeckLoadStr;
                    if (ModeExit_4) {
                        enemyTitle = 0;
                        xglFontPrint(0x18, 0x138, CARD_TEXT_COLOR, ModeEndMess0);
                        xglFontPrint(0xE0, 0x168, CARD_TEXT_COLOR, ModeEndMess1);
                        exiting = 1;
                        if (PadData[0].half_2a & PAD_CIRCLE) {
                            work->fadeOutTimer = CARD_FADE_TICKS;
                            work->fadeOutStep = CARD_FADE_STEP;
                            work->fase = CARD_FASE_TITLE;
                            ModeExit_4 = 0;
                        }
                        if (PadData[0].half_2a & (PAD_START | PAD_CROSS)) {
                            xglSoundEffectNormalID(SE_CANCEL, 0);
                            ModeExit_4 = 0;
                        }
                    } else if (work->command.input[0].pressed & PAD_START) {
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        ModeExit_4 = 1;
                    }
                    result = CardLoadDeckProc(exiting, &cld0, &cld1, work, enemyTitle);
                    if (result != 0) {
                        if (result < 0) {
                            work->command.step = 0;
                            work->faseStep--;
                        } else {
                            CardCopyRam2Deck(work->enemyDeck, work, result - 1);
                            work->command.step = 0;
                            work->fadeOutTimer = CARD_FADE_TICKS;
                            work->fadeOutStep = CARD_FADE_STEP;
                            work->faseStep++;
                        }
                    }
                    break;
                case CARD_VS_STEP_FADE_TO_BATTLE:
                    CardFadeOut(work);
                    if (work->fadeOutTimer == 0) {
                        work->fadeOutTimer = CARD_FADE_TICKS;
                        work->fadeOutStep = CARD_FADE_STEP;
                        work->faseStep++;
                        work->command.step = 0;
                        CardPlaySound1();
                        CardGameInit(work);
                        CardPlayInitWork(&work->player, work->playerDeck);
                        CardPlayInitWork(&work->enemy, work->enemyDeck);
                    }
                    break;
                case CARD_VS_STEP_BATTLE:
                    CardFadeIn(work);
                    if (CardGameProc(0, &cldtmp, work, 0) != 0) {
                        work->resultWait = CARD_RESULT_WAIT_FRAMES;
                        work->faseStep++;
                    }
                    break;
                case CARD_VS_STEP_RESULT:
                    CardGameProc(0, &cldtmp, work, 0);
                    if (work->resultWait <= 0 &&
                        ((work->command.input[0].pressed & CARD_PAD_ANY_FACE_BUTTON) ||
                         (work->command.input[1].pressed & CARD_PAD_ANY_FACE_BUTTON))) {
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        work->fadeOutTimer = CARD_FADE_TICKS;
                        work->fadeOutStep = CARD_FADE_STEP;
                        work->fase = CARD_FASE_TITLE;
                        ModeExit_4 = 0;
                    }
                    break;
                }
                break;
        }

        case CARD_FASE_TOURNAMENT: {
            int line;
            int pack;
            int outcome;
            int pick;
            int previous;
            int result;
            int count;
            int last;
            char *title;
            int exiting;

                exiting = 0;
                switch (work->faseStep) {
                case CARD_TOURNAMENT_STEP_LOAD:
                    item13[PRIZE_TUNED_CIRCUIT] = 0;
                    item13[PRIZE_PROMO_CARD] = 0;
                    item13[PRIZE_CARD_PACK_822] = 0;
                    item13[PRIZE_CARD_PACK_821] = 0;
                    CardFadeOut(work);
                    if (work->fadeOutTimer == 0) {
                        win12 = 0;
                        work->fadeOutTimer = CARD_FADE_TICKS;
                        work->faseStep++;
                        work->fadeOutStep = CARD_FADE_STEP;
                        for (i = 0; i < CARD_MODEL_COUNT; i++)
                            work->models[i].visible = 0;
                        work->models[CARD_MODEL_DECKLIST].visible = 1;
                        CardLoadBattleWindows();
                        cld0.baseColor = CARD_LIST_BASE_COLOR;
                        cld0.cursorColor = CARD_LIST_CURSOR_COLOR;
                        cld0.cursor = 0;
                        cld1.mode = 0;
                        cld1.cursor = 0;
                        cld0.active = 1;
                        cld0.top = 0;
                        cld0.rows = CARD_LIST_ROWS;
                        cld0.textLength = CARD_LIST_TEXT_LENGTH;
                        cld1.textLength = CARD_LIST_TEXT_LENGTH;
                        cld1.top = 0;
                        cld0.x = CARD_LIST_LEFT_X;
                        cld0.y = 64;
                        cld0.count = 0;
                        cld1.x = CARD_LIST_RIGHT_X;
                        cld1.y = 60;
                        cld1.shownCount = 1;
                        cld1.rows = CARD_LIST_ROWS;
                        previous = -1;
                        last = -1;
                        for (i = 0; i < CARD_DECK_SIZE; i++) {
                            if (previous != work->save->decks[0].cards[i]) {
                                last++;
                                cld1.entries[last].cardId = work->save->decks[0].cards[i];
                                cld1.entries[last].count = 1;
                            } else {
                                cld1.entries[last].count++;
                            }
                            previous = work->save->decks[0].cards[i];
                        }
                        cld1.count = last + 1;
                    }
                    break;
                case CARD_TOURNAMENT_STEP_FADE_IN:
                    CardFadeIn(work);
                    if (work->fadeOutTimer == 0)
                        work->faseStep++;
                    break;
                case CARD_TOURNAMENT_STEP_PICK_DECK:
                    title = P1DeckLoadStr;
                    if (ModeExit_4) {
                        title = 0;
                        xglFontPrint(0x18, 0x138, CARD_TEXT_COLOR, ModeEndMess0);
                        xglFontPrint(0xE0, 0x180, CARD_TEXT_COLOR, ModeEndMess1);
                        exiting = 1;
                        if (PadData[0].half_2a & PAD_CIRCLE) {
                            work->fadeOutTimer = CARD_FADE_TICKS;
                            work->fadeOutStep = CARD_FADE_STEP;
                            work->fase = CARD_FASE_TITLE;
                            ModeExit_4 = 0;
                        }
                        if (PadData[0].half_2a & (PAD_START | PAD_CROSS)) {
                            xglSoundEffectNormalID(SE_CANCEL, 0);
                            ModeExit_4 = 0;
                        }
                    } else if (work->command.input[0].pressed & PAD_START) {
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        ModeExit_4 = 1;
                    }
                    result = CardLoadDeckProc(exiting, &cld0, &cld1, work, title);
                    if (result != 0) {
                        if (result >= 0) {
                            CardCopyRam2Deck(work->playerDeck, work, result - 1);
                            work->command.step = 0;
                            work->faseStep++;
                        } else {
                            ModeExit_4 = 1;
                        }
                    }
                    break;
                case CARD_TOURNAMENT_STEP_DRAW_OPPONENTS:
                    count = CARD_ENEMY_POOL_ALL;
                    if (xglFlagsGet(CARD_FLAG_ALL_ENEMY_DECKS, 1) == 0)
                        count = xglFlagsGet(0x12D, 1) != 0 ? CARD_ENEMY_POOL_ADVANCED : CARD_ENEMY_POOL_FIRST;
                    for (pick = 0; pick < count; pick++)
                        lst11[pick] = -1;
                    for (pick = 0; pick < count;) {
                        int *slot = &lst11[xglSRand() % count];

                        if (*slot < 0) {
                            *slot = pick;
                            pick++;
                        }
                    }
                    cld1.count = CARD_TOURNAMENT_ROUNDS;
                    work->faseStep++;
                    /* fall through */
                case CARD_TOURNAMENT_STEP_OPPONENTS:
                    if (ModeExit_4) {
                        xglFontPrint(0, 0, 0, "\031\003");
                        xglFontPrint(0x18, 0x138, CARD_TEXT_COLOR, "\r\0Exit game?");
                        exiting = 1;
                        switch (win12) {
                        case 1:
                            xglFontPrint(0, 0, 0, "\031\003");
                            xglFontPrint(0x18, 0x150, CARD_TEXT_COLOR, "The prize is 1 Card Pack if you stop here.");
                            break;
                        case 2:
                            xglFontPrint(0, 0, 0, "\031\003");
                            xglFontPrint(0x18, 0x150, CARD_TEXT_COLOR, "The prize is 2 Card Packs if you stop here.");
                            break;
                        case 3:
                            xglFontPrint(0, 0, 0, "\031\003");
                            xglFontPrint(0x18, 0x150, CARD_TEXT_COLOR, "The prize is 3 Card Packs if you stop here.");
                            break;
                        case 4:
                            xglFontPrint(0, 0, 0, "\031\003");
                            xglFontPrint(0x18, 0x150, CARD_TEXT_COLOR, "The prize is 4 Card Packs if you stop here.");
                            break;
                        }
                        xglFontPrint(0xE0, 0x180, CARD_TEXT_COLOR, ModeEndMess1);
                        if (work->command.input[0].pressed & PAD_CIRCLE) {
                            CardMainItemGetSub(item13, win12);
                            work->faseStep = CARD_TOURNAMENT_STEP_ADD_PRIZES;
                        } else if (work->command.input[0].pressed & (PAD_START | PAD_CROSS)) {
                            xglSoundEffectNormalID(SE_CANCEL, 0);
                            ModeExit_4 = 0;
                        }
                    } else {
                        if (work->command.input[0].pressed & PAD_START) {
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                            ModeExit_4 = 1;
                        }
                        if (work->command.input[0].pressed & PAD_CIRCLE) {
                            work->faseStep++;
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                        } else if (work->command.input[0].pressed & PAD_CROSS) {
                            if (win12 != 0) {
                                xglSoundEffectNormalID(SE_BUZZER, 0);
                                ModeExit_4 = 1;
                            } else {
                                work->faseStep--;
                                xglSoundEffectNormalID(SE_CANCEL, 0);
                            }
                        }
                        if (win12 == 0) {
                            xglFontPrint(0, 0, 0, "\031\003");
                            xglFontPrint(0x18, 0x138, CARD_TEXT_COLOR, "\v\x19\x03\r\0Select an opponent.");
                            xglFontPrint(0x18, 0x150, CARD_TEXT_COLOR, "\v\x19\x03\r\0Is this ok?");
                            xglFontPrint(0xE0, 0x168, CARD_TEXT_COLOR, ModeContMess1);
                        } else {
                            switch (win12) {
                            case 1:
                                xglFontPrint(0, 0, 0, "\031\003");
                                xglFontPrint(0x18, 0x138, CARD_TEXT_COLOR, "\v\x19\x03\r\0Entering the second round.");
                                break;
                            case 2:
                                xglFontPrint(0, 0, 0, "\031\003");
                                xglFontPrint(0x18, 0x138, CARD_TEXT_COLOR, "\v\x19\x03\r\0Entering the third round.");
                                break;
                            case 3:
                                xglFontPrint(0, 0, 0, "\031\003");
                                xglFontPrint(0x18, 0x138, CARD_TEXT_COLOR, "\v\x19\x03\r\0Entering the fourth round.");
                                break;
                            case 4:
                                xglFontPrint(0, 0, 0, "\031\003");
                                xglFontPrint(0x18, 0x138, CARD_TEXT_COLOR, "\v\x19\x03\r\0Entering the final round.");
                                break;
                            }
                            xglFontPrint(0xE0, 0x180, CARD_TEXT_COLOR, ModeContMess0);
                        }
                    }
                    CardHideModels(work);
                    work->models[CARD_MODEL_BACKGROUND].visible = 1;
                    work->models[CARD_TITLE_MODEL_SLOT].visible = 1;
                    position[1] = 1.751f;
                    position[3] = 1.0f;
                    position[0] = 0.0f;
                    position[2] = 0.0f;
                    xglMatrixUnit(placement);
                    xglMatrixTrans(matrix, placement, position);
                    nmlModelSetTexture(CARD_BATTLE_WINDOW_TEXTURE);
                    nmlModelSetPlace(&matrix);
                    nmlModelEntryCard(CARD_BATTLE_WINDOW(0));
                    position[1] -= 0.55f;
                    CMTSetFontColor(win12, 0);
                    xglFontPrint(0x6E, 0x25, CARD_TEXT_COLOR, EnemyDeckListStr[lst11[0]]);
                    xglMatrixTrans(matrix, placement, position);
                    nmlModelSetTexture(CARD_BATTLE_WINDOW_TEXTURE);
                    nmlModelSetPlace(&matrix);
                    nmlModelEntryCard(CARD_BATTLE_WINDOW(1));
                    position[1] -= 0.55f;
                    CMTSetFontColor(win12, 1);
                    xglFontPrint(0x6E, 0x5E, CARD_TEXT_COLOR, EnemyDeckListStr[lst11[1]]);
                    xglMatrixTrans(matrix, placement, position);
                    nmlModelSetTexture(CARD_BATTLE_WINDOW_TEXTURE);
                    nmlModelSetPlace(&matrix);
                    nmlModelEntryCard(CARD_BATTLE_WINDOW(2));
                    position[1] -= 0.55f;
                    CMTSetFontColor(win12, 2);
                    xglFontPrint(0x6E, 0x97, CARD_TEXT_COLOR, EnemyDeckListStr[lst11[2]]);
                    xglMatrixTrans(matrix, placement, position);
                    nmlModelSetTexture(CARD_BATTLE_WINDOW_TEXTURE);
                    nmlModelSetPlace(&matrix);
                    nmlModelEntryCard(CARD_BATTLE_WINDOW(3));
                    position[1] -= 0.55f;
                    CMTSetFontColor(win12, 3);
                    xglFontPrint(0x6E, 0xD2, CARD_TEXT_COLOR, EnemyDeckListStr[lst11[3]]);
                    xglMatrixTrans(matrix, placement, position);
                    nmlModelSetTexture(CARD_BATTLE_WINDOW_TEXTURE);
                    nmlModelSetPlace(&matrix);
                    nmlModelEntryCard(CARD_BATTLE_WINDOW(4));
                    position[1] -= 0.55f;
                    CMTSetFontColor(win12, 4);
                    xglFontPrint(0x6E, 0x10D, CARD_TEXT_COLOR, EnemyDeckListStr[lst11[4]]);
                    work->fadeOutTimer = CARD_FADE_TICKS;
                    work->fadeOutStep = CARD_FADE_STEP;
                    break;
                case CARD_TOURNAMENT_STEP_FADE_TO_BATTLE:
                    CardFadeOut(work);
                    if (work->fadeOutTimer == 0) {
                        work->fadeOutStep = CARD_FADE_STEP;
                        work->fadeOutTimer = CARD_FADE_TICKS;
                        if (win12 >= 4)
                            CardPlaySound2();
                        else
                            CardPlaySound1();
                        CardGameInit(work);
                        CardPlayInitWork(&work->player, work->playerDeck);
                        CardCopyRom2Deck(work->enemyDeck, lst11[win12]);
                        work->enemyDeckId = lst11[win12];
                        CardPlayInitWorkFake(&work->enemy, work->enemyDeck, work->definitions);
                        work->flags |= CGP_FLAG_SIDE1_AUTO;
                        work->faseStep++;
                    }
                    break;
                case CARD_TOURNAMENT_STEP_BATTLE:
                    CardFadeIn(work);
                    outcome = CardGameProc(0, &cldtmp, work, win12);
                    if (outcome != 0) {
                        if (outcome == 1) {
                            win12++;
                            work->faseStep++;
                            work->resultWait = CARD_RESULT_WAIT_FRAMES;
                        } else {
                            work->resultWait = CARD_RESULT_WAIT_FRAMES;
                            work->faseStep = CARD_TOURNAMENT_STEP_LOST;
                        }
                    }
                    break;
                case CARD_TOURNAMENT_STEP_ROUND_WON:
                    CardGameProc(0, &cldtmp, work, win12);
                    if (work->resultWait <= 0 && (work->command.input[0].pressed & CARD_PAD_ANY_FACE_BUTTON)) {
                        if (win12 == CARD_TOURNAMENT_ROUNDS) {
                            work->faseStep = CARD_TOURNAMENT_STEP_CHAMPION;
                            work->resultWait = CARD_RESULT_WAIT_FRAMES;
                            xglSoundEffectNormalID(6, 0);
                            work->command.step = 0;
                            work->command.flowRequest = 5;
                        } else {
                            CardHideModels(work);
                            work->models[CARD_MODEL_DECKLIST].visible = 1;
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                            work->faseStep = CARD_TOURNAMENT_STEP_OPPONENTS;
                        }
                    }
                    break;
                case CARD_TOURNAMENT_STEP_CHAMPION:
                    CardGameProc(0, &cldtmp, work, win12);
                    if (work->resultWait == 0 && (work->command.input[0].pressed & CARD_PAD_ANY_FACE_BUTTON)) {
                        item13[PRIZE_TUNED_CIRCUIT] = 0;
                        item13[PRIZE_PROMO_CARD] = 0;
                        item13[PRIZE_CARD_PACK_822] = 0;
                        item13[PRIZE_CARD_PACK_821] = 0;
                        for (win = 0; win < win12; win++) {
                            int roll = xglSRand();

                            if (xglFlagsGet(0x12D, 1) == 0)
                                roll = 0;
                            if (roll & 1)
                                item13[PRIZE_CARD_PACK_822]++;
                            else
                                item13[PRIZE_CARD_PACK_821]++;
                        }
                        if (!(work->save->flags & CARD_SAVE_TUNED_CIRCUIT_GIVEN)) {
                            item13[PRIZE_TUNED_CIRCUIT] = 1;
                            work->save->flags |= CARD_SAVE_TUNED_CIRCUIT_GIVEN;
                        } else {
                            item13[PRIZE_PROMO_CARD] = (xglSRand() & 1) + 1;
                        }
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        work->faseStep++;
                        work->command.step = -1;
                    }
                    break;
                case CARD_TOURNAMENT_STEP_PRIZE_WON: {
                    char packCount[3];

                    CardGameProc(0, &cldtmp, work, win12);
                    work->models[CARD_TITLE_MODEL_SLOT].visible = 1;
                    line = 0;
                    xglFontPrint(0, 0, 0, "\031\003");
                    xglFontPrint(0x14, 0x138, CARD_TEXT_COLOR, "\v\x19\x03\r\0Congratulations. Your prize is: ");
                    if (item13[PRIZE_CARD_PACK_821] != 0) {
                        xglFontPrint(0, 0, 0, "\031\003");
                        packCount[0] = ZenkakuListA[item13[PRIZE_CARD_PACK_821] * 2];
                        packCount[1] = ZenkakuListA[item13[PRIZE_CARD_PACK_821] * 2 + 1];
                        packCount[2] = 0;
                        line = 1;
                        xglFontPrint(0x14, 0x150, CARD_TEXT_COLOR, "\v\x19\x03\r\0Card Pack #821");
                        xglFontPrint(0xC8, 0x150, CARD_TEXT_COLOR, packCount);
                    }
                    if (item13[PRIZE_CARD_PACK_822] != 0) {
                        packCount[0] = ZenkakuListA[item13[PRIZE_CARD_PACK_822] * 2];
                        packCount[1] = ZenkakuListA[item13[PRIZE_CARD_PACK_822] * 2 + 1];
                        packCount[2] = 0;
                        xglFontPrint(0, 0, 0, "\031\003");
                        xglFontPrint(0x14, line * 24 + 0x150, CARD_TEXT_COLOR, "\v\x19\x03\r\0Card Pack #822");
                        xglFontPrint(0xC8, line * 24 + 0x150, CARD_TEXT_COLOR, packCount);
                        line++;
                    }
                    if (item13[PRIZE_PROMO_CARD] != 0) {
                        xglFontPrint(0, 0, 0, "\031\003");
                        xglFontPrint(0x14, line * 24 + 0x150, CARD_TEXT_COLOR, "\v\x19\x03\r\0\xA3\xB1 Promotional Card");
                        line++;
                    }
                    if (item13[PRIZE_TUNED_CIRCUIT] != 0) {
                        xglFontPrint(0, 0, 0, "\031\003");
                        xglFontPrint(0x14, line * 24 + 0x150, CARD_TEXT_COLOR, "\v\x19\x03\r\0Tuned Circuit");
                    }
                    if (work->command.input[0].pressed & CARD_PAD_ANY_FACE_BUTTON) {
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        work->faseStep = CARD_TOURNAMENT_STEP_ADD_PRIZES;
                    }
                    break;
                }
                case CARD_TOURNAMENT_STEP_ADD_PRIZES:
                    CardMainAddItemSub(work, item13);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    work->fadeOutTimer = CARD_FADE_TICKS;
                    work->fadeOutStep = CARD_FADE_STEP;
                    work->fase = CARD_FASE_TITLE;
                    ModeExit_4 = 0;
                    break;
                case CARD_TOURNAMENT_STEP_LOST:
                    CardGameProc(0, &cldtmp, work, win12);
                    if (work->resultWait <= 0) {
                        for (pack = 0; pack < win12; pack++) {
                            int roll = xglSRand();

                            if (xglFlagsGet(0x12D, 1) == 0)
                                roll = 0;
                            if (roll & 1)
                                item13[PRIZE_CARD_PACK_822]++;
                            else
                                item13[PRIZE_CARD_PACK_821]++;
                        }
                        work->faseStep++;
                    }
                    break;
                case CARD_TOURNAMENT_STEP_PRIZE_LOST: {
                    char packCount[3];

                    line = 0;
                    CardGameProc(0, &cldtmp, work, win12);
                    work->models[CARD_TITLE_MODEL_SLOT].visible = 1;
                    xglFontPrint(0, 0, 0, "\031\003");
                    xglFontPrint(0x14, 0x138, CARD_TEXT_COLOR, "\v\x19\x03\r\0Sorry, you lost. Your prize is: ");
                    if (win12 != 0) {
                        if (item13[PRIZE_CARD_PACK_821] != 0) {
                            packCount[0] = ZenkakuListA[item13[PRIZE_CARD_PACK_821] * 2];
                            packCount[1] = ZenkakuListA[item13[PRIZE_CARD_PACK_821] * 2 + 1];
                            line = 1;
                            packCount[2] = 0;
                            xglFontPrint(0, 0, 0, "\031\003");
                            xglFontPrint(0x14, 0x150, CARD_TEXT_COLOR, "\v\x19\x03\r\0Card Pack #821");
                            xglFontPrint(0xC8, 0x150, CARD_TEXT_COLOR, packCount);
                        }
                        if (item13[PRIZE_CARD_PACK_822] != 0) {
                            xglFontPrint(0, 0, 0, "\031\003");
                            packCount[0] = ZenkakuListA[item13[PRIZE_CARD_PACK_822] * 2];
                            packCount[1] = ZenkakuListA[item13[PRIZE_CARD_PACK_822] * 2 + 1];
                            packCount[2] = 0;
                            xglFontPrint(0x14, line * 24 + 0x150, CARD_TEXT_COLOR, "\v\x19\x03\r\0Card Pack #822");
                            xglFontPrint(0xC8, line * 24 + 0x150, CARD_TEXT_COLOR, packCount);
                            line++;
                        }
                    } else {
                        xglFontPrint(0, 0, 0, "\031\003");
                    }
                    xglFontPrint(0x14, line * 24 + 0x150, CARD_TEXT_COLOR, "\v\x19\x03\r\0Nothing.");
                    if (work->command.input[0].pressed & CARD_PAD_ANY_FACE_BUTTON) {
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        work->faseStep = CARD_TOURNAMENT_STEP_ADD_PRIZES;
                    }
                    break;
                }
                }
                break;
        }

        case CARD_FASE_EXHIBITION: {
            int previous;
            int model;
            int result;
            int count;
            int last;
            char *title;
            int exiting;

                exiting = 0;
                switch (work->faseStep) {
                case CARD_EXHIBITION_STEP_LOAD:
                    CardFadeOut(work);
                    if (work->fadeOutTimer == 0) {
                        win15 = 0;
                        work->fadeOutTimer = CARD_FADE_TICKS;
                        work->faseStep++;
                        work->fadeOutStep = CARD_FADE_STEP;
                        for (i = 0; i < CARD_MODEL_COUNT; i++)
                            work->models[i].visible = 0;
                        work->models[CARD_MODEL_DECKLIST].visible = 1;
                        CardLoadBattleWindows();
                        cld0.baseColor = CARD_LIST_BASE_COLOR;
                        cld0.cursorColor = CARD_LIST_CURSOR_COLOR;
                        cld0.cursor = 0;
                        cld1.mode = 0;
                        cld1.cursor = 0;
                        cld0.active = 1;
                        cld0.top = 0;
                        cld0.rows = CARD_LIST_ROWS;
                        cld0.textLength = CARD_LIST_TEXT_LENGTH;
                        cld1.textLength = CARD_LIST_TEXT_LENGTH;
                        cld1.top = 0;
                        cld0.x = CARD_LIST_LEFT_X;
                        cld0.y = 64;
                        cld0.count = 0;
                        cld1.x = CARD_LIST_RIGHT_X;
                        cld1.y = 60;
                        cld1.shownCount = 1;
                        cld1.rows = CARD_LIST_ROWS;
                        previous = -1;
                        last = -1;
                        for (i = 0; i < CARD_DECK_SIZE; i++) {
                            if (previous != work->save->decks[0].cards[i]) {
                                last++;
                                cld1.entries[last].cardId = work->save->decks[0].cards[i];
                                cld1.entries[last].count = 1;
                            } else {
                                cld1.entries[last].count++;
                            }
                            previous = work->save->decks[0].cards[i];
                        }
                        cld1.count = last + 1;
                    }
                    break;
                case CARD_EXHIBITION_STEP_FADE_IN:
                    CardFadeIn(work);
                    if (work->fadeOutTimer == 0)
                        work->faseStep++;
                    break;
                case CARD_EXHIBITION_STEP_PICK_DECK:
                    title = P1DeckLoadStr;
                    if (ModeExit_4) {
                        title = 0;
                        xglFontPrint(0x18, 0x138, CARD_TEXT_COLOR, ModeEndMess0);
                        xglFontPrint(0xE0, 0x168, CARD_TEXT_COLOR, ModeEndMess1);
                        exiting = 1;
                        if (PadData[0].half_2a & PAD_CIRCLE) {
                            work->fadeOutTimer = CARD_FADE_TICKS;
                            work->fadeOutStep = CARD_FADE_STEP;
                            work->fase = CARD_FASE_TITLE;
                            ModeExit_4 = 0;
                        }
                        if (PadData[0].half_2a & (PAD_START | PAD_CROSS)) {
                            xglSoundEffectNormalID(SE_CANCEL, 0);
                            ModeExit_4 = 0;
                        }
                    } else if (PadData[0].half_2a & (PAD_START | PAD_CROSS)) {
                        xglSoundEffectNormalID(SE_CANCEL, 0);
                        ModeExit_4 = 1;
                    }
                    result = CardLoadDeckProc(exiting, &cld0, &cld1, work, title);
                    if (result != 0) {
                        if (result < 0) {
                            ModeExit_4 = 1;
                        } else {
                            CardCopyRam2Deck(work->playerDeck, work, result - 1);
                            work->command.step = 0;
                            work->faseStep++;
                            for (model = CARD_MODEL_COUNT - 1; model >= 0; model--)
                                work->models[model].visible = 0;
                            work->models[CARD_MODEL_BACKGROUND].visible = 1;
                            work->models[CARD_TITLE_MODEL_SLOT].visible = 1;
                        }
                    }
                    break;
                case CARD_EXHIBITION_STEP_DRAW_OPPONENT:
                    count = CARD_ENEMY_POOL_ALL;
                    if (xglFlagsGet(CARD_FLAG_ALL_ENEMY_DECKS, 1) == 0)
                        count = xglFlagsGet(0x12D, 1) != 0 ? CARD_ENEMY_POOL_ADVANCED : CARD_ENEMY_POOL_FIRST;
                    lst14[0] = xglSRand() % count;
                    cld1.count = 1;
                    work->faseStep++;
                    /* fall through */
                case CARD_EXHIBITION_STEP_CONFIRM:
                    if (ModeExit_4) {
                        xglFontPrint(0x18, 0x138, CARD_TEXT_COLOR, ModeEndMess0);
                        xglFontPrint(0xE0, 0x168, CARD_TEXT_COLOR, ModeEndMess1);
                        if (PadData[0].half_2a & PAD_CIRCLE) {
                            work->fadeOutTimer = CARD_FADE_TICKS;
                            work->fadeOutStep = CARD_FADE_STEP;
                            work->fase = CARD_FASE_TITLE;
                            ModeExit_4 = 0;
                        }
                        if (PadData[0].half_2a & (PAD_START | PAD_CROSS)) {
                            xglSoundEffectNormalID(SE_CANCEL, 0);
                            ModeExit_4 = 0;
                        }
                    } else {
                        xglFontPrint(0, 0, 0, "\031\003");
                        xglFontPrint(0x18, 0x138, CARD_TEXT_COLOR, "\v\x19\x03\r\0Select an opponent.");
                        xglFontPrint(0x18, 0x150, CARD_TEXT_COLOR, "\v\x19\x03\r\0Is this ok?");
                        xglFontPrint(0xE0, 0x168, CARD_TEXT_COLOR, ModeContMess1);
                        if (work->command.input[0].pressed & PAD_CIRCLE) {
                            work->faseStep++;
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                        } else if (work->command.input[0].pressed & PAD_START) {
                            ModeExit_4 = 1;
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                        } else if (work->command.input[0].pressed & PAD_CROSS) {
                            work->faseStep--;
                            xglSoundEffectNormalID(SE_CANCEL, 0);
                        }
                    }
                    xglMatrixUnit(placement);
                    position[1] = 1.751f;
                    position[3] = 1.0f;
                    position[0] = 0.0f;
                    position[2] = 0.0f;
                    xglMatrixTrans(placement, placement, position);
                    nmlModelSetTexture(CARD_BATTLE_WINDOW_TEXTURE);
                    nmlModelSetPlace(&placement);
                    nmlModelEntryCard(CARD_BATTLE_WINDOW(0));
                    xglFontPrint(0, 0, 0, "\031\003");
                    xglFontPrint(0x78, 0x25, CARD_TEXT_COLOR, EnemyDeckListStr[lst14[0]]);
                    work->fadeOutTimer = CARD_FADE_TICKS;
                    work->fadeOutStep = CARD_FADE_STEP;
                    break;
                case CARD_EXHIBITION_STEP_FADE_TO_BATTLE:
                    CardFadeOut(work);
                    if (work->fadeOutTimer == 0) {
                        work->fadeOutTimer = CARD_FADE_TICKS;
                        work->fadeOutStep = CARD_FADE_STEP;
                        work->faseStep++;
                        CardPlaySound1();
                        CardGameInit(work);
                        CardPlayInitWork(&work->player, work->playerDeck);
                        CardCopyRom2Deck(work->enemyDeck, lst14[0]);
                        work->enemyDeckId = lst14[win15];
                        CardPlayInitWorkFake(&work->enemy, work->enemyDeck, work->definitions);
                        work->enemyDeckId = lst14[0];
                        work->flags |= CGP_FLAG_SIDE1_AUTO;
                    }
                    break;
                case CARD_EXHIBITION_STEP_BATTLE:
                    CardFadeIn(work);
                    result = CardGameProc(0, &cldtmp, work, 0);
                    if (result != 0) {
                        if (result == 1)
                            win15++;
                        work->resultWait = CARD_RESULT_WAIT_FRAMES;
                        work->faseStep++;
                    }
                    break;
                case CARD_EXHIBITION_STEP_RESULT:
                    CardGameProc(0, &cldtmp, work, 0);
                    if (work->resultWait <= 0 && (work->command.input[0].pressed & CARD_PAD_ANY_FACE_BUTTON)) {
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        if (win15 != 0) {
                            int roll = xglSRand();

                            if (xglFlagsGet(0x12D, 1) == 0)
                                roll = 0;
                            if (roll & 1) {
                                pack16 = 2;
                                dataEvtBoxInc(EVENT_ITEM_CARD_PACK_822);
                            } else {
                                pack16 = 1;
                                dataEvtBoxInc(EVENT_ITEM_CARD_PACK_821);
                            }
                            work->faseStep++;
                        } else {
                            work->fadeOutTimer = CARD_FADE_TICKS;
                            work->fadeOutStep = CARD_FADE_STEP;
                            work->fase = CARD_FASE_TITLE;
                            ModeExit_4 = 0;
                        }
                    }
                    break;
                case CARD_EXHIBITION_STEP_PRIZE:
                    CardGameProc(0, &cldtmp, work, 0);
                    work->models[CARD_TITLE_MODEL_SLOT].visible = 1;
                    xglFontPrint(0, 0, 0, "\031\003");
                    xglFontPrint(0x18, 0x138, CARD_TEXT_COLOR, "\v\x19\x03\r\0Congratulations!");
                    if (pack16 == 1)
                        xglFontPrint(0x18, 0x150, CARD_TEXT_COLOR, "\v\x19\x03\r\0Your prize is 1 Card Pack #821.");
                    else
                        xglFontPrint(0x18, 0x150, CARD_TEXT_COLOR, "\v\x19\x03\r\0Your prize is 1 Card Pack #822.");
                    if (work->command.input[0].pressed & CARD_PAD_ANY_FACE_BUTTON) {
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        work->fadeOutTimer = CARD_FADE_TICKS;
                        work->fadeOutStep = CARD_FADE_STEP;
                        work->fase = CARD_FASE_TITLE;
                        ModeExit_4 = 0;
                    }
                    break;
                }
                break;
        }

        case CARD_FASE_DECK_BUILD: {
            int status;
            int entry;
            int card;
            int previous;
            int last;
            int exiting;

                exiting = 0;
                xglFontPrint(0, 0, 0, "\031\003");
                xglFontDebugPrintf(0x54, 0, "\v\x19\x03\r\0Deck building mode");
                switch (work->faseStep) {
                case CARD_DECK_BUILD_STEP_INIT:
                case CARD_DECK_BUILD_STEP_FADE_OUT:
                case CARD_DECK_BUILD_STEP_LOAD:
                case CARD_DECK_BUILD_STEP_FADE_IN:
                    break;
                default:
                    if (work->command.input[0].pressed & PAD_START) {
                        ModeExit_4 = (ModeExit_4 + 1) & 1;
                        if (ModeExit_4)
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                        else
                            xglSoundEffectNormalID(SE_CANCEL, 0);
                    }
                    break;
                }
                if (ModeExit_4) {
                    xglFontPrint(0, 0, 0, "\031\003");
                    xglFontPrint(0x18, 0x138, CARD_TEXT_COLOR, "\v\x19\x03\r\0Exit deck building mode?");
                    xglFontPrint(0xE0, 0x168, CARD_TEXT_COLOR, ModeEndMess1);
                    exiting = 1;
                    if (PadData[0].half_2a & PAD_CIRCLE) {
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        work->fadeOutTimer = CARD_FADE_TICKS;
                        work->fadeOutStep = CARD_FADE_STEP;
                        work->fase = CARD_FASE_TITLE;
                        ModeExit_4 = 0;
                    }
                    if (PadData[0].half_2a & PAD_CROSS) {
                        xglSoundEffectNormalID(SE_CANCEL, 0);
                        ModeExit_4 = 0;
                    }
                } else if (Message_6 != 0) {
                    exiting = 1;
                    xglFontPrint(0x18, 0x138, 0xFFFF, "\v\r\0\f\x80@@");
                    xglFontPrint(0, 0, 0, "\031\003");
                    xglFontPrint(0x18, 0x138, 0xFFFF, CardErrorList[Message_6]);
                    if (work->command.input[0].pressed & CARD_PAD_ANY_FACE_BUTTON) {
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        MessageCnt_7 = 0;
                        Message_6 = 0;
                    }
                    if (MessageCnt_7 > 0) {
                        MessageCnt_7--;
                    } else {
                        MessageCnt_7 = 0;
                        Message_6 = 0;
                    }
                }
                switch (work->faseStep) {
                case CARD_DECK_BUILD_STEP_INIT:
                    work->command.step = 0;
                    work->faseStep++;
                    for (entry = 0; entry < CARD_DECK_SIZE; entry++) {
                        cld1.entries[entry].cardId = -1;
                        cld1.entries[entry].count = 0;
                    }
                    work->fadeOutTimer = CARD_FADE_TICKS;
                    work->fadeOutStep = CARD_FADE_STEP;
                    break;
                case CARD_DECK_BUILD_STEP_FADE_OUT:
                    CardFadeOut(work);
                    if (work->fadeOutTimer == 0)
                        work->faseStep++;
                    break;
                case CARD_DECK_BUILD_STEP_LOAD:
                    work->fadeOutTimer = CARD_FADE_TICKS;
                    work->fadeOutStep = CARD_FADE_STEP;
                    work->faseStep++;
                    for (i = 0; i < CARD_MODEL_COUNT; i++)
                        work->models[i].visible = 0;
                    work->models[CARD_MODEL_DECKLIST].visible = 1;
                    CardFread(CARD_TYPE_LEX(0), "deckmake\\type1_B.lex");
                    CardFread(CARD_TYPE_LEX(1), "deckmake\\type1_E.lex");
                    CardFread(CARD_TYPE_LEX(2), "deckmake\\type1_S.lex");
                    CardFread(CARD_TYPE_LEX(3), "deckmake\\type2_0.lex");
                    CardFread(CARD_TYPE_LEX(4), "deckmake\\type2_1.lex");
                    CardFread(CARD_TYPE_LEX(5), "deckmake\\type2_2.lex");
                    CardFread(CARD_TYPE_LEX(6), "deckmake\\type2_3.lex");
                    CardFread(CARD_TYPE_LEX(7), "deckmake\\type2_4.lex");
                    CardFread(CARD_TYPE_LEX(8), "deckmake\\type2_5.lex");
                    CardFread(CARD_TYPE_LEX(9), "deckmake\\type2_6.lex");
                    CardFread(CARD_TYPE_XTX(0), "deckmake\\type1_B.xtx");
                    CardFread(CARD_TYPE_XTX(1), "deckmake\\type1_E.xtx");
                    CardFread(CARD_TYPE_XTX(2), "deckmake\\type1_S.xtx");
                    CardFread(CARD_TYPE_XTX(3), "deckmake\\type2_0.xtx");
                    cld0.baseColor = CARD_LIST_BASE_COLOR;
                    cld0.cursorColor = CARD_LIST_CURSOR_COLOR;
                    cld0.cursor = 0;
                    cld1.mode = 0;
                    cld1.cursor = 0;
                    cld0.active = 1;
                    cld0.y = 60;
                    cld0.top = 0;
                    cld0.rows = CARD_LIST_ROWS;
                    cld0.textLength = CARD_LIST_TEXT_LENGTH;
                    cld1.textLength = CARD_LIST_TEXT_LENGTH;
                    cld1.top = 0;
                    cld0.x = CARD_LIST_LEFT_X;
                    cld0.count = 0;
                    cld1.x = CARD_LIST_RIGHT_X;
                    cld1.y = 60;
                    cld1.shownCount = 1;
                    cld1.rows = CARD_LIST_ROWS;
                    last = -1;
                    previous = -1;
                    for (i = 0; i < CARD_DECK_SIZE; i++) {
                        if (previous != work->save->decks[0].cards[i]) {
                            last++;
                            cld1.entries[last].cardId = work->save->decks[0].cards[i];
                            cld1.entries[last].count = 1;
                        } else {
                            cld1.entries[last].count++;
                        }
                        previous = work->save->decks[0].cards[i];
                    }
                    cld1.count = last + 1;
                    break;
                case CARD_DECK_BUILD_STEP_FADE_IN:
                    CardFadeIn(work);
                    if (work->fadeOutTimer == 0)
                        work->faseStep++;
                    break;
                case CARD_DECK_BUILD_STEP_LOAD_PROMPT:
                    if (!exiting) {
                        xglFontPrint(0, 0, 0, "\031\003");
                        xglFontPrint(0x18, 0x138, CARD_TEXT_COLOR, "\v\x19\x03\r\0Load deck?");
                        xglFontPrint(0x18, 0x168, CARD_TEXT_COLOR,
                                     "\r\x02\f\x80@@\xA1\xFB \v\x19\x03\r\0Yes\xA1\xA1\r\x02\f@@\x80\xA1\xDF \v\x19\x03\r\0No");
                        xglFontPrint(0x78, 0x120, CARD_TEXT_COLOR, cardMenuFooterEndCode);
                        if (work->command.input[0].pressed & PAD_CIRCLE) {
                            int prevId;
                            int lastEntry;

                            xglSoundEffectNormalID(SE_DECIDE, 0);
                            prevId = -1;
                            lastEntry = -1;
                            for (card = 0; card < CARD_DECK_SIZE; card++) {
                                if (prevId != work->save->decks[0].cards[card]) {
                                    lastEntry++;
                                    cld1.entries[lastEntry].cardId = work->save->decks[0].cards[card];
                                    cld1.entries[lastEntry].count = 1;
                                } else {
                                    cld1.entries[lastEntry].count++;
                                }
                                prevId = work->save->decks[0].cards[card];
                            }
                            cld1.count = lastEntry + 1;
                            work->faseStep++;
                        } else if (work->command.input[0].pressed & PAD_CROSS) {
                            xglSoundEffectNormalID(SE_CANCEL, 0);
                            cld1.count = 0;
                            work->faseStep = CARD_DECK_BUILD_STEP_BUILD_INIT;
                        }
                    }
                    break;
                case CARD_DECK_BUILD_STEP_PICK_DECK:
                    status = CardLoadDeckProc(exiting, &cld0, &cld1, work, 0);
                    if (status != 0) {
                        if (status < 0) {
                            work->faseStep--;
                        } else {
                            CardCopyRam2CLD(&cld1, work, (status & 0x7F) - 1);
                            work->command.step = 0;
                            work->faseStep = CARD_DECK_BUILD_STEP_BUILD_INIT;
                        }
                    }
                    break;
                case CARD_DECK_BUILD_STEP_BUILD_INIT:
                    CardHideModels(work);
                    work->faseStep++;
                    work->models[CARD_MODEL_DECKMAKE].visible = 1;
                    cld0.baseColor = CARD_LIST_BASE_COLOR;
                    cld0.cursorColor = CARD_LIST_CURSOR_COLOR;
                    cld0.active = 1;
                    cld0.textLength = CARD_LIST_TEXT_LENGTH;
                    cld0.cursor = 0;
                    cld0.top = 0;
                    cld0.rows = CARD_LIST_ROWS;
                    cld0.count = 0;
                    cld1.x = CARD_LIST_RIGHT_X;
                    cld1.y = 60;
                    cld1.textLength = CARD_LIST_TEXT_LENGTH;
                    cld1.cursor = 0;
                    cld1.shownCount = 1;
                    cld1.top = 0;
                    cldtmp.cursor = 0;
                    cldtmp.top = 0;
                    cld0.x = CARD_LIST_LEFT_X;
                    cld0.y = 64;
                    cld0.mode = 2;
                    cld1.rows = CARD_LIST_ROWS;
                    cldtmp.x = CARD_LIST_RIGHT_X;
                    cldtmp.y = 60;
                    cldtmp.textLength = CARD_LIST_TEXT_LENGTH;
                    cldtmp.shownCount = 1;
                    cldtmp.rows = CARD_LIST_ROWS;
                    cldtmp.count = 0;
                    break;
                case CARD_DECK_BUILD_STEP_BUILD:
                    status = CardMakeDeckProc(exiting, &cld0, &cld1, work);
                    if (status != 0) {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                        Message_6 = status;
                        MessageCnt_7 = 45;
                    }
                    if (work->command.input[0].pressed & PAD_SQUARE) {
                        if (CardGetDeckAllCnt(&cld1) == CARD_DECK_SIZE) {
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                            work->faseStep++;
                        } else {
                            xglSoundEffectNormalID(SE_BUZZER, 0);
                            Message_6 = 2;
                            MessageCnt_7 = 45;
                        }
                    }
                    break;
                case CARD_DECK_BUILD_STEP_SAVE_INIT:
                    work->command.step = 0;
                    work->faseStep++;
                    cld0.count = 5;
                    cld0.top = 0;
                    cld0.cursor = 0;
                    CardCopyRam2CLD(&cldtmp, work, 0);
                    CardHideModels(work);
                    work->models[CARD_MODEL_DECKLIST].visible = 1;
                    /* fall through */
                case CARD_DECK_BUILD_STEP_SAVE: {
                    char saveTitle[] = "\v\x19\x03\r\0Save Mode";

                    status = CardSaveDeckProc(exiting, &cld0, &cldtmp, work, saveTitle);
                    if (status != 0) {
                        if (status < 0) {
                            xglSoundEffectNormalID(SE_CANCEL, 0);
                            work->faseStep = CARD_DECK_BUILD_STEP_BUILD_INIT;
                        } else {
                            CardCopyCLD2Ram(&cld1, work, status - 1);
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                            work->fadeOutTimer = CARD_FADE_TICKS;
                            work->fadeOutStep = CARD_FADE_STEP;
                            work->fase = CARD_FASE_TITLE;
                        }
                    }
                    break;
                }
                }
                break;
        }

        case CARD_FASE_TUTORIAL: {
            int running = 1;

            switch (work->faseStep) {
            case 0:
                CardFadeOut(work);
                if (work->fadeOutTimer == 0)
                    work->faseStep++;
                break;
            case 1:
                work->faseStep = 2;
                work->fadeOutTimer = CARD_FADE_TICKS;
                work->fadeOutStep = CARD_FADE_STEP;
                CardHideModels(work);
                CardTutorialInit();
                break;
            case 2:
                CardFadeIn(work);
                if (work->fadeOutTimer == 0)
                    work->faseStep++;
                break;
            case 3:
                running = CardTutorialProc(work);
                break;
            }
            if (running == 0) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                work->faseStep = 0;
                work->fadeOutTimer = CARD_FADE_TICKS;
                work->fadeOutStep = CARD_FADE_STEP;
                work->fase = CARD_FASE_TITLE;
            }
            break;
        }

        case CARD_FASE_TEST_MENU: {
            int soundList[25] = {
                0x20001, 0x20002, 0x20003, 0x20004, 0x20005, 0x20006, 0x20007,
                0x20008, 0x20009, 0x2000A, 0x2000D, 0x2000E, 0x2000F, 0x2000F,
                0x2000F, 0x2000F, 0x2000F, 0x2000F, 0x2000F, 0x2000F, 0x2000F,
                0x2000F, 0x2000F, 0x2000F, 0x2000F,
            };
            char volumeText[16];

            if ((PadData[0].half_2a & PAD_R1) && vol_19 < 127) {
                vol_19 += 10;
                if (vol_19 >= 128)
                    vol_19 = 127;
            }
            if ((PadData[0].half_2a & PAD_L1) && vol_19 > 0) {
                vol_19 -= 10;
                if (vol_19 < 0)
                    vol_19 = 0;
            }
            if (ModeExit_4) {
                xglFontPrint(0xC0, 0x90, 0xFFF8, ModeEndMess2);
                xglFontPrint(0xA8, 0xC0, 0xFFF8, ModeEndMess3);
                if (PadData[0].half_2a & PAD_CIRCLE) {
                    work->fadeOutTimer = CARD_FADE_TICKS;
                    work->fadeOutStep = CARD_FADE_STEP;
                    work->fase = CARD_FASE_TITLE;
                    ModeExit_4 = 0;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
                if (PadData[0].half_2a & PAD_CROSS) {
                    ModeExit_4 = 0;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                }
            }
            xglFontPrint(0x30, 0x60, CARD_TEXT_COLOR, "1P Deck");
            xglFontPrint(0x48, 0x78, CARD_TEXT_COLOR, "T1");
            xglFontPrint(0x90, 0x78, CARD_TEXT_COLOR, "T2");
            xglFontPrint(0xD8, 0x78, CARD_TEXT_COLOR, "T3");
            xglFontPrint(0x120, 0x78, CARD_TEXT_COLOR, "T4");
            xglFontPrint(0x168, 0x78, CARD_TEXT_COLOR, "T5");
            xglFontPrint(0x48, 0x90, CARD_TEXT_COLOR, "T6");
            xglFontPrint(0x90, 0x90, CARD_TEXT_COLOR, "T7");
            xglFontPrint(0xD8, 0x90, CARD_TEXT_COLOR, "T8");
            xglFontPrint(0x120, 0x90, CARD_TEXT_COLOR, "T9");
            xglFontPrint(0x168, 0x90, CARD_TEXT_COLOR, "T0");
            xglFontPrint(0x30, 0xC0, CARD_TEXT_COLOR, "2P deck");
            xglFontPrint(0x48, 0xD8, CARD_TEXT_COLOR, "T1");
            xglFontPrint(0x90, 0xD8, CARD_TEXT_COLOR, "T2");
            xglFontPrint(0xD8, 0xD8, CARD_TEXT_COLOR, "T3");
            xglFontPrint(0x120, 0xD8, CARD_TEXT_COLOR, "T4");
            xglFontPrint(0x168, 0xD8, CARD_TEXT_COLOR, "T5");
            xglFontPrint(0x48, 0xF0, CARD_TEXT_COLOR, "T6");
            xglFontPrint(0x90, 0xF0, CARD_TEXT_COLOR, "T7");
            xglFontPrint(0xD8, 0xF0, CARD_TEXT_COLOR, "T8");
            xglFontPrint(0x120, 0xF0, CARD_TEXT_COLOR, "T9");
            xglFontPrint(0x168, 0xF0, CARD_TEXT_COLOR, "T0");
            xglFontPrint(0x30, 0x120, CARD_TEXT_COLOR, "Game play");
            xglFontPrint(0x18, (s16) (SubMenu_3 * 0x60 + 0x60), CARD_TEXT_COLOR, ">");
            if (p0num_17 < 5)
                xglFontPrint((s16) (p0num_17 * 0x48 + 0x30), 0x78, CARD_TEXT_COLOR, ">");
            else
                xglFontPrint((s16) (p0num_17 * 0x48 - 0x138), 0x90, CARD_TEXT_COLOR, ">");
            if (p1num_18 < 5)
                xglFontPrint((s16) (p1num_18 * 0x48 + 0x30), 0xD8, CARD_TEXT_COLOR, ">");
            else
                xglFontPrint((s16) (p1num_18 * 0x48 - 0x138), 0xF0, CARD_TEXT_COLOR, ">");
            if (PadData[0].half_2a & PAD_UP) {
                SubMenu_3--;
                xglSoundEffectNormalID(SE_CURSOR, 0);
            }
            if (PadData[0].half_2a & PAD_DOWN) {
                SubMenu_3++;
                xglSoundEffectNormalID(SE_CURSOR, 0);
            }
            if (SubMenu_3 < 0)
                SubMenu_3 = 0;
            if (SubMenu_3 >= 3)
                SubMenu_3 = 2;
            sprintf(volumeText, "%d", vol_19);
            xglFontDebugPrintf(0x18, 0x24, volumeText);
            switch (SubMenu_3) {
            case 0:
                if (PadData[0].half_2a & PAD_RIGHT) {
                    p0num_17++;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                }
                if (PadData[0].half_2a & PAD_LEFT) {
                    p0num_17--;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                }
                if (PadData[0].half_2a & PAD_CIRCLE)
                    xglSoundEffectParamDirect(soundList[p0num_17], vol_19, 0x40);
                if (p0num_17 < 0)
                    p0num_17 = 0;
                if (p0num_17 >= 10)
                    p0num_17 = 9;
                break;
            case 1:
                if (PadData[0].half_2a & PAD_RIGHT) {
                    p1num_18++;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                }
                if (PadData[0].half_2a & PAD_LEFT) {
                    p1num_18--;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                }
                if (PadData[0].half_2a & PAD_CIRCLE)
                    xglSoundEffectNormalID(soundList[p1num_18 + 7], 0);
                if (p1num_18 < 0)
                    p1num_18 = 0;
                if (p1num_18 >= 10)
                    p1num_18 = 9;
                break;
            case 2:
                if (PadData[0].half_2a & PAD_CIRCLE) {
                    ModeExit_4 = 0;
                    work->fase = CARD_FASE_TEST_BATTLE;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    CardGameInit(work);
                    switch (p0num_17) {
                    case 0:
                        CardSampleInitDeck0(work->playerDeck);
                        break;
                    case 1:
                        CardSampleInitDeck1(work->playerDeck);
                        break;
                    case 2:
                        CardSampleInitDeck2(work->playerDeck);
                        break;
                    case 3:
                        CardSampleInitDeck3(work->playerDeck);
                        break;
                    case 4:
                        CardSampleInitDeck4(work->playerDeck);
                        break;
                    case 5:
                        CardSampleInitDeck5(work->playerDeck);
                        break;
                    case 6:
                        CardSampleInitDeck6(work->playerDeck);
                        break;
                    case 7:
                        CardSampleInitDeck7(work->playerDeck);
                        break;
                    case 8:
                        CardSampleInitDeck8(work->playerDeck);
                        break;
                    case 9:
                        CardSampleInitDeck9(work->playerDeck);
                        break;
                    }
                    switch (p1num_18) {
                    case 0:
                        CardSampleInitDeck0(work->enemyDeck);
                        break;
                    case 1:
                        CardSampleInitDeck1(work->enemyDeck);
                        break;
                    case 2:
                        CardSampleInitDeck2(work->enemyDeck);
                        break;
                    case 3:
                        CardSampleInitDeck3(work->enemyDeck);
                        break;
                    case 4:
                        CardSampleInitDeck4(work->enemyDeck);
                        break;
                    case 5:
                        CardSampleInitDeck5(work->enemyDeck);
                        break;
                    case 6:
                        CardSampleInitDeck6(work->enemyDeck);
                        break;
                    case 7:
                        CardSampleInitDeck7(work->enemyDeck);
                        break;
                    case 8:
                        CardSampleInitDeck8(work->enemyDeck);
                        break;
                    case 9:
                        CardSampleInitDeck9(work->enemyDeck);
                        break;
                    }
                    CardPlayNoInitWork(&work->player, work->playerDeck);
                    CardPlayNoInitWork(&work->enemy, work->enemyDeck);
                }
                break;
            }
            break;
        }

        case CARD_FASE_TEST_BATTLE:
            if (CardGameProc(ModeExit_4, &cldtmp, work, 0) != 0 &&
                (work->command.input[0].pressed & CARD_PAD_ANY_FACE_BUTTON)) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                work->fadeOutTimer = CARD_FADE_TICKS;
                work->fadeOutStep = CARD_FADE_STEP;
                work->fase = CARD_FASE_TITLE;
                ModeExit_4 = 0;
            }
            break;
        }
    }

    if (work->helpState) {
        float helpLight[4][4];

        memset(helpLight, 0, sizeof(helpLight));
        helpLight[0][3] = 1.0f;
        helpLight[1][3] = 1.0f;
        helpLight[2][3] = 1.0f;
        helpLight[3][0] = 0.3f;
        helpLight[3][1] = 0.3f;
        helpLight[3][2] = 0.3f;
        helpLight[3][3] = 1.0f;
        nmlModelSetLight(&helpLight, &asDir);
        nmlModelSetTexture(work->models[CARD_MODEL_BACKGROUND].texture);
        xglMatrixUnit(placement);
        nmlModelSetPlace(&placement);
        nmlModelEntryCard(work->models[CARD_MODEL_BACKGROUND].model);
    } else {
        for (i = 0; i < CARD_MODEL_COUNT; i++) {
            CardModelEntry *model = &work->models[i];

            if (model->visible) {
                if (i == CARD_MODEL_MAT_FG)
                    nmlModelSetLight(&Player1Color, &asDir);
                if (i == CARD_MODEL_MAT_FG_2)
                    nmlModelSetLight(&Player2Color, &asDir);
                if (i == CARD_MODEL_MAT_MOJI || i == CARD_MODEL_MAT_MOJI_2) {
                    float cardLight[4][4];

                    memset(cardLight, 0, sizeof(cardLight));
                    cardLight[0][3] = 1.0f;
                    cardLight[1][3] = 1.0f;
                    cardLight[2][3] = 1.0f;
                    cardLight[3][0] = 0.5f;
                    cardLight[3][1] = 0.5f;
                    cardLight[3][2] = 0.5f;
                    cardLight[3][3] = 1.0f;
                    nmlModelSetLight(&cardLight, &asDir);
                } else if (i == CARD_MODEL_BACKGROUND) {
                    float dimLight[4][4];

                    memset(dimLight, 0, sizeof(dimLight));
                    dimLight[0][3] = 1.0f;
                    dimLight[1][3] = 1.0f;
                    dimLight[2][3] = 1.0f;
                    dimLight[3][0] = 0.3f;
                    dimLight[3][1] = 0.3f;
                    dimLight[3][2] = 0.3f;
                    dimLight[3][3] = 1.0f;
                    nmlModelSetLight(&dimLight, &asDir);
                }
                nmlModelSetTexture(model->texture);
                nmlModelSetPlace(&work->models[i].matrix);
                nmlModelEntryCard(model->model);
            }
        }
    }

    if (work->helpState) {
        u16 pressed;

        if (work->menuPlayer == 0)
            pressed = work->command.input[0].pressed;
        else
            pressed = work->command.input[1].pressed;
        if (pressed & (PAD_START | PAD_SELECT)) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            work->helpState = 0;
        }
    } else if (ModeExit_4 == 0 && work->fase != CARD_FASE_TUTORIAL) {
        if (work->command.input[0].pressed & PAD_SELECT) {
            xglSoundEffectNormalID(SE_DECIDE, 0);
            work->helpState = 1;
            work->menuPlayer = 0;
            CardHelpPageInit(work);
        } else if (work->command.input[1].pressed & PAD_SELECT) {
            xglSoundEffectNormalID(SE_DECIDE, 0);
            work->helpState = 1;
            work->menuPlayer = 1;
            CardHelpPageInit(work);
        }
    }

    xglRenderGlobalFadeSet(0.0f, 0.0f, 0.0f, (0x80 - work->fadeOutColor[3]) * 0.0078125f);

    if (work->command.slideTimer != 0) {
        float step = 0.1f;

        work->command.slideTimer--;
        if (work->command.slideDirection)
            step = -0.1f;
        work->command.slideOffset += step;
    } else {
        work->command.slideOffset = 0.0f;
        if (work->command.slideDirection)
            work->command.slideOffset = -1.0f;
    }

    position[2] = 5.8f;
    position[3] = 1.0f;
    position[0] = 0.0f;
    position[1] = 0.0f;
    {
        float *eye = &xglStudioSelectGetActiveCamera(0)->position.vector.x;

        /* sceVu0CopyVector: the original copies the quadword with lq/sq. */
        __asm__ __volatile__(
            "lq $2, 0(%1)\n"
            "sq $2, 0(%0)\n"
            :
            : "r" (eye), "r" (position)
            : "$2", "memory");
    }
    return 1;
}

/* Error messages are separate strings in the OV10 card-data image. */
static char CardErrTxt01[32];
static char CardErrTxt02[48];
static char CardErrTxt03[48];
static char CardErrTxt04[80];
static char CardErrTxt05[96];


static char *CardErrorList[6] = {
    CardErrTxt01, CardErrTxt01, CardErrTxt02,
    CardErrTxt03, CardErrTxt04, CardErrTxt05
};

char P1DeckLoadStr[32] = "\x0b\x19\x03\x0d\0Load deck on 1P side";
char P2DeckLoadStr[32] = "\x0b\x19\x03\x0d\0Load deck on 2P side";
char ModeEndMess0[16] = "\x0b\x19\x03\x0d\0Finished?";
char ModeEndMess1[48] =
    "\x0d\x04\x0c\x80@@\xa1\xfb \x0b\x19\x03\x0d\0Exit\xa1\xa1"
    "\x0d\x04\x0c@@\x80\xa1\xdf \x0b\x19\x03\x0d\0Continue";
char ModeEndMess2[40] = "\x0b\x19\x03\x0d\0Do you wish to exit the game?";
char ModeEndMess3[48] =
    "\x0d\x04\x0c\x80@@\xa1\xfb \x0b\x19\x03\x0d\0Exit\xa1\xa1"
    "\x0d\x04\x0c@@\x80\xa1\xdf \x0b\x19\x03\x0d\0Continue";
char ModeContMess0[48] =
    "\x0d\x04\x0c\x80@@\xa1\xfb \x0b\x19\x03\x0d\0Continue\xa1\xa1"
    "\x0d\x04\x0c@@\x80\xa1\xdf \x0b\x19\x03\x0d\0Exit";
char ModeContMess1[48] =
    "\x0d\x04\x0c\x80@@\xa1\xfb \x0b\x19\x03\x0d\0Accept\xa1\xa1"
    "\x0d\x04\x0c@@\x80\xa1\xdf \x0b\x19\x03\x0d\0Change";

/* CardPlayTMessList follows the menu text objects in the original .data run.
 * Repeated entries intentionally point at the same string label. */
static char CardPlayTMessTxt00[16];
static char CardPlayTMessTxt01[24];
static char CardPlayTMessTxt02[24];
static char CardPlayTMessTxt03[24];
static char CardPlayTMessTxt04[24];
static char CardPlayTMessTxt05[24];
static char CardPlayTMessTxt06[24];
static char CardPlayTMessTxt07[24];
static char CardPlayTMessTxt08[24];
static char CardPlayTMessTxt09[24];
static char CardPlayTMessTxt10[24];
static char CardPlayTMessTxt11[24];
static char CardPlayTMessTxt12[24];
static char CardPlayTMessTxt13[24];
static char CardPlayTMessTxt14[24];
static char CardPlayPMessTxt01[24];
static char CardPlayPMessTxt02[24];
static char CardPlayPMessTxt03[24];
static char CardPlayDMessTxt01[40];
static char CardPlayDMessTxt02[56];
static char CardPlayDMessTxt03[56];
static char CardPlayDMessTxt04[56];
static char CardPlayDMessTxt05[80];
static char CardPlayDMessTxt11[48];
static char CardPlayDMessTxt12[40];
static char CardPlayDMessTxt13[32];
static char CardPlayDMessTxt14[40];
static char CardMenuMessTxt00[184];
static char CardMenuMessTxt01[48];
static char CardMenuMessTxt02[128];
static char CardMenuMessTxt03[16];
static char CardMenuMessTxt10[32];
static char CardMenuMessTxt11[64];
static char CardMenuMessTxt12[40];
static char CardMenuMessTxt20[48];
static char CardMenuMessTxt21[32];
static char CardMenuMessTxt22[24];
static char CardMenuMessTxt23[24];
static char CardMenuMessTxt24[24];
static char CardMenuMessTxt30[32];
static char CardMenuMessTxt31[104];
static char CardExitMessTxt00[104];
static char CardExitMessTxt01[88];
static char CardExitMessTxt02[64];
static char CardExitMessTxt03[16];
static char CardExitMessTxt10[96];
static char CardExitMessTxt11[96];
static char CardExitMessTxt12[136];
static char CardExitMessTxt13[104];

char *CardPlayTMessList[79] = {
    CardPlayTMessTxt00, CardPlayTMessTxt01, CardPlayTMessTxt02,
    CardPlayTMessTxt04, CardPlayTMessTxt03, CardPlayTMessTxt05,
    CardPlayTMessTxt06, CardPlayTMessTxt07, CardPlayTMessTxt08,
    CardPlayTMessTxt09, CardPlayTMessTxt11, CardPlayTMessTxt10,
    CardPlayTMessTxt12, CardPlayTMessTxt13, CardPlayTMessTxt14,
    CardPlayTMessTxt00, CardPlayTMessTxt00, CardPlayTMessTxt00,
    CardPlayTMessTxt00, CardPlayTMessTxt00, CardPlayPMessTxt01,
    CardPlayPMessTxt02, CardPlayPMessTxt03, CardPlayTMessTxt00,
    CardPlayTMessTxt00, CardPlayTMessTxt00, CardPlayTMessTxt00,
    CardPlayTMessTxt00, CardPlayTMessTxt00, CardPlayTMessTxt00,
    CardPlayDMessTxt01, CardPlayDMessTxt02, CardPlayDMessTxt03,
    CardPlayDMessTxt04, CardPlayDMessTxt05, CardPlayTMessTxt00,
    CardPlayTMessTxt00, CardPlayTMessTxt00, CardPlayTMessTxt00,
    CardPlayTMessTxt00, CardPlayDMessTxt11, CardPlayDMessTxt12,
    CardPlayDMessTxt13, CardPlayDMessTxt14, CardPlayTMessTxt00,
    CardPlayDMessTxt11, CardPlayDMessTxt12, CardPlayDMessTxt13,
    CardPlayTMessTxt00, CardPlayTMessTxt00, CardMenuMessTxt00,
    CardMenuMessTxt01, CardMenuMessTxt02, CardMenuMessTxt03,
    CardMenuMessTxt00, CardMenuMessTxt10, CardMenuMessTxt11,
    CardMenuMessTxt12, CardMenuMessTxt23, CardMenuMessTxt24,
    CardMenuMessTxt20, CardMenuMessTxt21, CardMenuMessTxt22,
    CardMenuMessTxt00, CardMenuMessTxt00, CardMenuMessTxt30,
    CardMenuMessTxt31, CardPlayTMessTxt00, CardPlayTMessTxt00,
    CardMenuMessTxt00, CardExitMessTxt00, CardExitMessTxt01,
    CardExitMessTxt02, CardExitMessTxt03, CardPlayTMessTxt00,
    CardExitMessTxt10, CardExitMessTxt11, CardExitMessTxt12,
    CardExitMessTxt13
};

float asDir[4][4] = {
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { -0.017453294f, -0.017453294f, -0.017453294f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f }
};
float asColor[4][4] = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 1.5f, 1.5f, 1.5f, 1.0f }
};
float atColor[4][4] = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 2.0f, 2.0f, 2.0f, 1.0f }
};
float auColor[4][4] = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 1.2f, 1.2f, 1.2f, 1.0f }
};
float CCColor[4][4] = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f }
};
float KKColor[4][4] = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f }
};

char BGFileNameList[6][16] = {
    "game\\BG1", "game\\BG2", "game\\BG3",
    "game\\BG4", "game\\BG5", "game\\BG6"
};
void *TitleMdlLst[10] = {
    CARD_TITLE_LEX_BASE, CARD_TITLE_XTX_GAME,
    CARD_TITLE_LEX_BASE + CARD_TITLE_LEX_SIZE, CARD_TITLE_XTX_CARD,
    CARD_TITLE_LEX_BASE + 2 * CARD_TITLE_LEX_SIZE, CARD_TITLE_XTX_HELP,
    0, 0, 0, 0
};

static s16 MenuNo_1 = 0;
static s16 SubNo_2 = -1;
static s16 SubMenu_3 = 0;
static s16 ModeExit_4 = 0;
static u8 CCtimer_5 = 0;
static s16 Message_6 = 0;
static s16 MessageCnt_7 = 0;
static s16 p0num_17 = 0;
static s16 p1num_18 = 0;
static s16 vol_19 = 0;

float CLP1Color[4][4] = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.5f, 0.5f, 1.5f, 1.0f }
};
float CLP2Color[4][4] = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.5f, 0.5f, 0.5f, 1.0f }
};

float HandPos1P[4][3] = {
    { -2.516f, -1.839f, 0.002f },
    { -2.1973f, -1.839f, 0.002f },
    { -1.8786f, -1.839f, 0.002f },
    { -1.26f, -1.839f, 0.002f }
};
float DisposePos1P[4][3] = {
    { -2.514f, -0.2222f, 0.002f },
    { -2.514f, -0.611f, 0.002f },
    { -2.514f, -1.0f, 0.002f },
    { -2.514f, -1.389f, 0.002f }
};
float OperationPos1P[4][3] = {
    { 2.514f, -0.2222f, 0.002f },
    { 2.514f, -0.611f, 0.002f },
    { 2.514f, -1.0f, 0.002f },
    { 2.514f, -1.389f, 0.002f }
};
float HandPos2P[4][3] = {
    { 2.516f, 1.839f, 0.002f },
    { 2.1973f, 1.839f, 0.002f },
    { 1.8786f, 1.839f, 0.002f },
    { 1.26f, 1.839f, 0.002f }
};

/* The font stores EUC-JP bytes. These controls delimit menu labels, and the
 * fixed-width filename records include the original trailing zero bytes. */
const union CardSoundPathStorage cardBattle1SwdPath = { "data\\yamamoto\\snd\\smd\\BATTLE1.SWD" };
const union CardSoundPathStorage cardBattle1SmdPath = { "data\\yamamoto\\snd\\smd\\BATTLE1.SMD" };
const union CardSoundPathStorage cardJingle1SmdPath = { "data\\yamamoto\\snd\\smd\\JINGLE1.SMD" };
const union CardSoundPathStorage cardBattle2SwdPath = { "data\\yamamoto\\snd\\smd\\BATTLE2.SWD" };
const union CardSoundPathStorage cardBattle2SmdPath = { "data\\yamamoto\\snd\\smd\\BATTLE2.SMD" };
const union CardSoundPathStorage cardJingle2SmdPath = { "data\\yamamoto\\snd\\smd\\JINGLE2.SMD" };
char cardGraphicFileName[16] = "CARDGRAP.BIN";
char cardLexExtension[8] = ".lex";
char cardXtxExtension[8] = ".xtx";
char cardBattleWin1LexName[24] = "deckmake\\battlewin1.lex";
char cardBattleWin2LexName[24] = "deckmake\\battlewin2.lex";
char cardBattleWin3LexName[24] = "deckmake\\battlewin3.lex";
char cardBattleWin4LexName[24] = "deckmake\\battlewin4.lex";
char cardBattleWin5LexName[24] = "deckmake\\battlewin5.lex";
char cardBattleWinBinName[24] = "deckmake\\battlewin.bin";

const CardFontCodeRecord cardHelpFontCodes = {
    { "\x0c\x10\x10\x10" }, { 0, 0, 0 }
};
char cardHelpLabel[8] = " Help";
const CardFontCode8 cardPlayerLabelFontCodes = { "\x0b\x0d\0\x0c```" };
const CardFontCodeRecord cardPlayerAutoLabel = {
    { "\x0c\x80" }, { 0, 0, 0 }
};
char cardMenuFooterFontCode[8] = "\x0b\x0d";
char cardMenuFooterEndCode[16] = "\x0b";



static char CardPlayTMessTxt00[16] = "Text error";

static char CardMenuMessTxt00[184] = "Play the card game. The three types of games are Exhibition, 2-Player Battle, and Tournament. You can access the help menu by pressing the SELECT Button anytime during the game.";

static char CardMenuMessTxt01[48] = "Create decks, unseal cards, and view cards.";

static char CardMenuMessTxt02[128] = "This is the tutorial and manual for the card game. Press the SELECT Button to look up any questions you have during the game.";

static char CardMenuMessTxt03[16] = "Quit Xeno Card.";

static char CardMenuMessTxt10[32] = "Play a single-player card game.";

static char CardMenuMessTxt11[64] = "Play a single elimination tournament against five players.";

static char CardMenuMessTxt12[40] = "Play a card game with two players.";

static char CardMenuMessTxt20[48] = "Select the 40 cards you will use in the game.";

static char CardMenuMessTxt21[32] = "Open card box or card pack.";

static char CardMenuMessTxt22[24] = "View your cards.";

static char CardMenuMessTxt23[24] = "Change the background.";

static char CardMenuMessTxt24[24] = "Change the game mat.";

static char CardMenuMessTxt30[32] = "This is the card game tutorial.";

static char CardMenuMessTxt31[104] = "This is the card game manual. Look up any questions during the game by pressing the SELECT Button.";

static char CardExitMessTxt00[104] = "Enters the help mode. You can view an explanation of the current phase by\npressing the SELECT Button.";

static char CardExitMessTxt02[64] = "Ends the current phase. You can also press the \242\242 Button.";

static char CardExitMessTxt03[16] = "Exit game.";

static char CardExitMessTxt01[88] = "Phase progression\nManual: Shows you every step.\nAuto: Skip phases when you cannot move.";

static char CardExitMessTxt10[96] = "Phase progression for P1\nManual: Shows you every step.\nAuto: Skip phases when you cannot move.";

static char CardExitMessTxt11[96] = "Phase progression for P2\nManual: Shows you every step.\nAuto: Skip phases when you cannot move.";

static char CardExitMessTxt12[136] = "Display options for turn indicator\nShow: Use this option if you are not used to the game.\nHide: This will hide the turn indicator.";

static char CardExitMessTxt13[104] = "The game mat automatically turns to face the player currently in control during 2-Player Battle.";



static char CardErrTxt01[32] = "\031\003You do not have enough cards.";

static char CardErrTxt02[48] = "\031\003You do not have enough cards to make a deck.";

static char CardErrTxt03[48] = "\031\003You can't add any more cards to the deck.";

static char CardErrTxt04[80] = "\031\003This card can't be placed in the deck because the card has not been acquired.";

static char CardErrTxt05[96] = "\031\003There are 3 of the same cards already in the deck. You can't add any more to the deck.";

static char CardPlayTMessTxt01[24] = "\013\031\003\r\000\243\261P Draw Phase";

static char CardPlayTMessTxt02[24] = "\013\031\003\r\000\243\261P Move Phase";

static char CardPlayTMessTxt03[24] = "\013\031\003\r\000\243\261P Set Phase";

static char CardPlayTMessTxt04[24] = "\013\031\003\r\000\243\261P Event Phase";

static char CardPlayTMessTxt05[24] = "\013\031\003\r\000\243\261P Block Phase";

static char CardPlayTMessTxt06[24] = "\013\031\003\r\000\243\261P Battle Phase";

static char CardPlayTMessTxt07[24] = "\013\031\003\r\000\243\261P Adjust Phase";

static char CardPlayTMessTxt08[24] = "\013\031\003\r\000\243\262P Draw Phase";

static char CardPlayTMessTxt09[24] = "\013\031\003\r\000\243\262P Move Phase";

static char CardPlayTMessTxt10[24] = "\013\031\003\r\000\243\262P Set Phase";

static char CardPlayTMessTxt11[24] = "\013\031\003\r\000\243\262P Event Phase";

static char CardPlayTMessTxt12[24] = "\013\031\003\r\000\243\262P Block Phase";

static char CardPlayTMessTxt13[24] = "\013\031\003\r\000\243\262P Battle Phase";

static char CardPlayTMessTxt14[24] = "\013\031\003\r\000\243\262P Adjust Phase";

static char CardPlayPMessTxt01[24] = "\013\031\003\r\000Player 1 wins";

static char CardPlayPMessTxt02[24] = "\013\031\003\r\000Player 2 wins";

static char CardPlayPMessTxt03[24] = "\013\031\003\r\000Drawing card.";

static char CardPlayDMessTxt01[40] = "\013\031\003\r\000You can't place any more cards.";

static char CardPlayDMessTxt02[56] = "\013\031\003\r\000The requirements of the battlefield are not met.";

static char CardPlayDMessTxt03[56] = "\\13\031\003\r\000The requirements of the field are not met.";

static char CardPlayDMessTxt04[56] = "\013\031\003\r\000The requirement of the discarded card are not met.";

static char CardPlayDMessTxt05[80] = "\013\031\003\r\000You can't place this card because the same card is already on the field.";

static char CardPlayDMessTxt11[48] = "\013\031\003\r\000You can't play this card in this phase.";

static char CardPlayDMessTxt12[40] = "\013\031\003\r\000You can't play in this phase.";

static char CardPlayDMessTxt13[32] = "\013\031\003\r\000You have no cards to play.";

static char CardPlayDMessTxt14[40] = "\031\003\r\000\014\200@@Too many cards in your hand.";

/* ---- end of TU (data definitions), appended last so no existing .data object offset moves ---- */
/* Version tag CardDataLoad compares the card file against; the original object is 13 bytes at 0x00a42040. */
static u8 IDdata[13] = "CardData1.10";

static char CardPlayPMessTxt01[24];

char *CardPlayPMessList[1] = { CardPlayPMessTxt01 };

static float s_asIntensity[4][4] = { { 0.80000001192092896f, 0.80000001192092896f, 0.80000001192092896f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } };

static Vector4 s_asDirection[4] = { { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f } };

u8 CardInitialDataPack[40] = {
    0x00, 0x10, 0x10, 0x11, 0x11, 0x12, 0x12, 0x13, 0x13, 0x16, 0x17, 0x17, 0x18, 0x19, 0x19, 0x24,
    0x24, 0x24, 0x21, 0x21, 0x21, 0x2B, 0x30, 0x31, 0x31, 0x31, 0x32, 0x32, 0x32, 0x53, 0x55, 0x56,
    0x54, 0x54, 0x54, 0x5B, 0x5B, 0x5B, 0x6F, 0x6F
};

u8 SampleDeckA[40] = {
    0x00, 0x00, 0x00, 0x03, 0x04, 0x04, 0x07, 0x07, 0x0F, 0x0F, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15,
    0x16, 0x16, 0x16, 0x1E, 0x1F, 0x1F, 0x20, 0x23, 0x23, 0x24, 0x24, 0x24, 0x25, 0x2B, 0x2D, 0x2E,
    0x33, 0x54, 0x54, 0x54, 0x56, 0x56, 0x64, 0x69
};

u8 SampleDeckB[40] = {
    0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0x15, 0x15, 0x15, 0x16, 0x16, 0x16, 0x42, 0x42, 0x42, 0x43,
    0x43, 0x43, 0x48, 0x48, 0x48, 0x49, 0x49, 0x49, 0x4B, 0x5D, 0x5D, 0x5D, 0x5F, 0x5F, 0x61, 0x61,
    0x6A, 0x6A, 0x6A, 0x74, 0x74, 0x74, 0x86, 0x88
};

float HalfColor[4][4] = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.3f, 0.3f, 0.3f, 1.0f }
};

float MiddleColor[4][4] = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.6f, 0.6f, 0.6f, 1.0f }
};

float CLColor[4][4] = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 1.5f, 1.5f, 1.5f, 1.0f }
};

float CNColor[4][4] = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 1.2f, 1.2f, 1.2f, 1.0f }
};

float CMColor[4][4] = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 1.5f, 1.5f, 1.5f, 1.0f }
};

float BattlePos1P[4][3] = { { -0.54000002145767212f, -0.3970000147819519f, 0.0020000000949949026f }, { 0.54000002145767212f, -0.3970000147819519f, 0.0020000000949949026f }, { -0.54000002145767212f, -1.1799999475479126f, 0.0020000000949949026f }, { 0.54000002145767212f, -1.1799999475479126f, 0.0020000000949949026f } };

float JunkPos1P[4][3] = { { 2.2969999313354492f, -1.809999942779541f, 0.0020000000949949026f }, { 2.2969999313354492f, -1.809999942779541f, 0.0020000000949949026f }, { 2.2969999313354492f, -1.809999942779541f, 0.0020000000949949026f }, { 2.2969999313354492f, -1.809999942779541f, 0.0020000000949949026f } };

float DisposePos2P[4][3] = {
    { 2.514f, 0.2222f, 0.002f },
    { 2.514f, 0.611f, 0.002f },
    { 2.514f, 1.0f, 0.002f },
    { 2.514f, 1.389f, 0.002f }
};

float OperationPos2P[4][3] = {
    { -2.514f, 0.2222f, 0.002f },
    { -2.514f, 0.611f, 0.002f },
    { -2.514f, 1.0f, 0.002f },
    { -2.514f, 1.389f, 0.002f }
};

float BattlePos2P[4][3] = {
    { 0.54f, 0.397f, 0.002f },
    { -0.54f, 0.397f, 0.002f },
    { 0.54f, 1.18f, 0.002f },
    { -0.54f, 1.18f, 0.002f }
};

float JunkPos2P[4][3] = { { -2.2969999313354492f, 1.809999942779541f, 0.0020000000949949026f }, { -1.878600001335144f, 1.8389999866485596f, 0.0020000000949949026f }, { -2.1972999572753906f, 1.8389999866485596f, 0.0020000000949949026f }, { -2.5160000324249268f, 1.8389999866485596f, 0.0020000000949949026f } };
