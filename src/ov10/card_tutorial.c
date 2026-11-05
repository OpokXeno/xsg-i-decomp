/*
 * OV10 original TU 7: 0x00a21348..0x00a21768 (3 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov10/cgp.h"

extern char *strcat(char *destination, const char *source);
extern char *strcpy(char *destination, const char *source);
extern int xglCdReadFile(const char *name, void *buffer, int mode, int flags);
extern int printf(const char *format, ...);
extern void xglSoundEffectNormalID(int sound_id, int variant);
extern void xglMatrixTrans(Matrix4 destination, Matrix4 source, const float translation[4]);
extern void nmlModelSetRenderLevel(int level);
extern void nmlModelSetTexture(const void *texture);
extern void nmlModelSetPlace(Matrix4 matrix);
extern void nmlModelEntry(u32 model);
static int tutopage;       /* 0x00A487C8 */
static u32 tulex_tbl[2];    /* 0x00A57DB0, 2 entries */
static void *tuxtx_tbl[2];  /* 0x00A57DB8, 2 entries */
static u32 lex;            /* 0x00A57DA8 */

/* "data\carddata\tuto\" */
char datapath[0x14] = "data\\carddata\\tuto\\";
extern const char cardTutorialNameTu046[];
extern const char cardTutorialNameTu045[];
extern const char cardTutorialNameTu044[];
extern const char cardTutorialNameTu043[];
extern const char cardTutorialNameTu042[];
extern const char cardTutorialNameTu041[];
extern const char cardTutorialNameTu040[];
extern const char cardTutorialNameTu039[];
extern const char cardTutorialNameTu038[];
extern const char cardTutorialNameTu037[];
extern const char cardTutorialNameTu036[];
extern const char cardTutorialNameTu035[];
extern const char cardTutorialNameTu034[];
extern const char cardTutorialNameTu033[];
extern const char cardTutorialNameTu032[];
extern const char cardTutorialNameTu031[];
extern const char cardTutorialNameTu030[];
extern const char cardTutorialNameTu029[];
extern const char cardTutorialNameTu028[];
extern const char cardTutorialNameTu027[];
extern const char cardTutorialNameTu026[];
extern const char cardTutorialNameTu025[];
extern const char cardTutorialNameTu024[];
extern const char cardTutorialNameTu023[];
extern const char cardTutorialNameTu022[];
extern const char cardTutorialNameTu021[];
extern const char cardTutorialNameTu020[];
extern const char cardTutorialNameTu019[];
extern const char cardTutorialNameTu018[];
extern const char cardTutorialNameTu017[];
extern const char cardTutorialNameTu016[];
extern const char cardTutorialNameTu015[];
extern const char cardTutorialNameTu014[];
extern const char cardTutorialNameTu013[];
extern const char cardTutorialNameTu012[];
extern const char cardTutorialNameTu011[];
extern const char cardTutorialNameTu010[];
extern const char cardTutorialNameTu009[];
extern const char cardTutorialNameTu008[];
extern const char cardTutorialNameTu007[];
extern const char cardTutorialNameTu006[];
extern const char cardTutorialNameTu005[];
extern const char cardTutorialNameTu004[];
extern const char cardTutorialNameTu003[];
extern const char cardTutorialNameTu002[];
extern const char cardTutorialNameTu001[];
extern const char cardTutorialNameTu000[];
static const char *tutoxtx_tbl[64] = {
    cardTutorialNameTu000, cardTutorialNameTu001, cardTutorialNameTu002, cardTutorialNameTu003,
    cardTutorialNameTu004, cardTutorialNameTu005, cardTutorialNameTu006, cardTutorialNameTu007,
    cardTutorialNameTu008, cardTutorialNameTu009, cardTutorialNameTu010, cardTutorialNameTu011,
    cardTutorialNameTu012, cardTutorialNameTu013, cardTutorialNameTu014, cardTutorialNameTu015,
    cardTutorialNameTu016, cardTutorialNameTu017, cardTutorialNameTu018, cardTutorialNameTu019,
    cardTutorialNameTu020, cardTutorialNameTu021, cardTutorialNameTu022, cardTutorialNameTu023,
    cardTutorialNameTu024, cardTutorialNameTu025, cardTutorialNameTu026, cardTutorialNameTu027,
    cardTutorialNameTu028, cardTutorialNameTu029, cardTutorialNameTu030, cardTutorialNameTu031,
    cardTutorialNameTu032, cardTutorialNameTu033, cardTutorialNameTu034, cardTutorialNameTu035,
    cardTutorialNameTu036, cardTutorialNameTu037, cardTutorialNameTu038, cardTutorialNameTu039,
    cardTutorialNameTu040, cardTutorialNameTu041, cardTutorialNameTu042, cardTutorialNameTu043,
    cardTutorialNameTu044, cardTutorialNameTu045, cardTutorialNameTu046,
};
const char cardTutorialXtxExtension[8] = ".xtx";
static void *tuxtx;

int CardTutorialFileLoad(int tutorialIndex)
{
    char path[0x80];

    memset(path, 0, sizeof(path));
    strcpy(path, datapath);
    strcat(path, tutoxtx_tbl[tutorialIndex]);
    strcat(path, cardTutorialXtxExtension);
    return xglCdReadFile(path, tuxtx, 0, 1);
}

/* Two-entry, NULL-terminated list of tutorial base names; the original pair is at 0x00A4E4B0 ({"Tutorial", NULL}). */
typedef struct {
    const char *name[2];
} TutorialNameList;

extern const TutorialNameList cardTutorialNameListInit;
extern const char cardTutorialFirstLexName[];   /* "Tu_000.lex" at 0x00A4E4B8 */
extern const char cardTutorialLexExtension[];   /* ".lex" at 0x00A4E4C8 */

void CardTutorialInit(void)
{
    char path[0x80];
    char *buffer;
    int count;
    int i;
    TutorialNameList names;

    buffer = (char *)0x01a00000;
    tutopage = 0;
    lex = (u32)buffer;
    names = cardTutorialNameListInit;

    memset(path, 0, sizeof(path));
    strcpy(path, datapath);
    strcat(path, cardTutorialFirstLexName);
    count = xglCdReadFile(path, (void *)0x01a00000, 0, 1);
    if (count > 0) {
        buffer += count;
        buffer = (char *)(((unsigned int)buffer + 15) & ~15u);
    }

    for (i = 0; i < 2 && names.name[i] != 0; i++) {
        memset(path, 0, sizeof(path));
        strcpy(path, datapath);
        strcat(path, names.name[i]);
        strcat(path, cardTutorialLexExtension);
        tulex_tbl[i] = (u32)buffer;
        count = xglCdReadFile(path, buffer, 0, 1);
        if (count > 0) {
            buffer += count;
            buffer = (char *)(((unsigned int)buffer + 15) & ~15u);
        }

        memset(path, 0, sizeof(path));
        strcpy(path, datapath);
        strcat(path, names.name[i]);
        strcat(path, cardTutorialXtxExtension);
        tuxtx_tbl[i] = buffer;
        count = xglCdReadFile(path, buffer, 0, 1);
        if (count > 0) {
            buffer += count;
            buffer = (char *)(((unsigned int)buffer + 15) & ~15u);
        }
    }

    tuxtx = buffer;
    CardTutorialFileLoad(0);
}

/* Tutorial page count and the pad actions CardTutorialProc handles. Evidence (OV10, CardTutorialProc 0x00a215e0):
 * `slti a0,a1,47` at 0x00a2163c ends the pages after tutopage++ (valid pages are 0..46); `andi v0,a1,0x8` at 0x00a21618
 * advances (sound 1, tutopage++, load the page); `andi v0,a1,0x4` at 0x00a21690 goes back (tutopage--, sound 2, load the
 * page); `andi v1,a1,0x800` at 0x00a216d0 closes the viewer (returns 0). */
#define CARD_TUTORIAL_PAGE_COUNT 47

#define CARD_TUTORIAL_PAD_NEXT 0x0008

#define CARD_TUTORIAL_PAD_PREV 0x0004

#define CARD_TUTORIAL_PAD_CLOSE 0x0800

/* Translation vector {1.2f, -1.82f, 0.0f, 1.0f}, read by xglMatrixTrans as four floats. The original copies the 16 bytes from
 * cardTutorialTranslationInit with two doubleword moves (`ld`/`ld`/`sd`/`sd` at 0x00a21600..0x00a21610). A plain float[4] or struct of floats has
 * only 4-byte alignment and compiles to unaligned ldl/ldr/sdl/sdr pairs; the u64 member gives the aggregate the 8-byte alignment
 * that yields the doubleword copy. */
typedef union CardTutorialTranslation {
    float v[4];
    u64 words[2];
} CardTutorialTranslation;
extern const CardTutorialTranslation cardTutorialTranslationInit;
extern const char cardTutorialEndLog[];

int CardTutorialProc(CardGameWork *work)
{
    Matrix4 source;
    Matrix4 placed;
    CardTutorialTranslation translation = cardTutorialTranslationInit;
    unsigned short pressed;

    xglMatrixUnit(source);
    pressed = work->command.input[0].pressed;
    if (pressed & CARD_TUTORIAL_PAD_NEXT) {
        xglSoundEffectNormalID(1, 0);
        tutopage++;
        if (tutopage >= CARD_TUTORIAL_PAGE_COUNT) {
            printf(cardTutorialEndLog);
            return 0;
        }
        if (tutoxtx_tbl[tutopage] == 0) {
            return 0;
        }
        if (CardTutorialFileLoad(tutopage) < 0) {
            return 0;
        }
        pressed = work->command.input[0].pressed;
    }
    if (pressed & CARD_TUTORIAL_PAD_PREV) {
        tutopage--;
        if (tutopage < 0) {
            tutopage = 0;
        } else {
            xglSoundEffectNormalID(2, 0);
            CardTutorialFileLoad(tutopage);
            pressed = work->command.input[0].pressed;
        }
    }
    if (pressed & CARD_TUTORIAL_PAD_CLOSE) {
        return 0;
    }
    xglMatrixTrans(placed, source, translation.v);
    nmlModelSetTexture(tuxtx_tbl[0]);
    nmlModelSetPlace(placed);
    nmlModelSetRenderLevel(2);
    nmlModelEntry(tulex_tbl[0]);
    xglMatrixUnit(source);
    source[3][2] = -1.0f;
    nmlModelSetPlace(source);
    nmlModelSetTexture(tuxtx);
    nmlModelEntry(lex);
    return 1;
}



const char cardTutorialNameTu046[8] = "Tu_046";

const char cardTutorialNameTu045[8] = "Tu_045";

const char cardTutorialNameTu044[8] = "Tu_044";

const char cardTutorialNameTu043[8] = "Tu_043";

const char cardTutorialNameTu042[8] = "Tu_042";

const char cardTutorialNameTu041[8] = "Tu_041";

const char cardTutorialNameTu040[8] = "Tu_040";

const char cardTutorialNameTu039[8] = "Tu_039";

const char cardTutorialNameTu038[8] = "Tu_038";

const char cardTutorialNameTu037[8] = "Tu_037";

const char cardTutorialNameTu036[8] = "Tu_036";

const char cardTutorialNameTu035[8] = "Tu_035";

const char cardTutorialNameTu034[8] = "Tu_034";

const char cardTutorialNameTu033[8] = "Tu_033";

const char cardTutorialNameTu032[8] = "Tu_032";

const char cardTutorialNameTu031[8] = "Tu_031";

const char cardTutorialNameTu030[8] = "Tu_030";

const char cardTutorialNameTu029[8] = "Tu_029";

const char cardTutorialNameTu028[8] = "Tu_028";

const char cardTutorialNameTu027[8] = "Tu_027";

const char cardTutorialNameTu026[8] = "Tu_026";

const char cardTutorialNameTu025[8] = "Tu_025";

const char cardTutorialNameTu024[8] = "Tu_024";

const char cardTutorialNameTu023[8] = "Tu_023";

const char cardTutorialNameTu022[8] = "Tu_022";

const char cardTutorialNameTu021[8] = "Tu_021";

const char cardTutorialNameTu020[8] = "Tu_020";

const char cardTutorialNameTu019[8] = "Tu_019";

const char cardTutorialNameTu018[8] = "Tu_018";

const char cardTutorialNameTu017[8] = "Tu_017";

const char cardTutorialNameTu016[8] = "Tu_016";

const char cardTutorialNameTu015[8] = "Tu_015";

const char cardTutorialNameTu014[8] = "Tu_014";

const char cardTutorialNameTu013[8] = "Tu_013";

const char cardTutorialNameTu012[8] = "Tu_012";

const char cardTutorialNameTu011[8] = "Tu_011";

const char cardTutorialNameTu010[8] = "Tu_010";

const char cardTutorialNameTu009[8] = "Tu_009";

const char cardTutorialNameTu008[8] = "Tu_008";

const char cardTutorialNameTu007[8] = "Tu_007";

const char cardTutorialNameTu006[8] = "Tu_006";

const char cardTutorialNameTu005[8] = "Tu_005";

const char cardTutorialNameTu004[8] = "Tu_004";

const char cardTutorialNameTu003[8] = "Tu_003";

const char cardTutorialNameTu002[8] = "Tu_002";

const char cardTutorialNameTu001[8] = "Tu_001";

const char cardTutorialNameTu000[8] = "Tu_000";

extern const char cardTutorialDirectoryName[];

static int tutopage = 0;

const char cardTutorialDirectoryName[16] = "Tutorial";

const TutorialNameList cardTutorialNameListInit = { .name = { cardTutorialDirectoryName, 0 } };

const char cardTutorialFirstLexName[16] = "Tu_000.lex";

const char cardTutorialLexExtension[8] = ".lex";

const CardTutorialTranslation cardTutorialTranslationInit = { .v = { 1.2000000476837158f, -1.8200000524520874f, 0.0f, 1.0f } };

const char cardTutorialEndLog[16] = "tutoend\n";

static u32 lex;

static u32 tulex_tbl[2];

static void * tuxtx_tbl[2];
