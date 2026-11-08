/*
 * OV10 original TU 7: 0x00a21348..0x00a21768 (3 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov10/cgp.h"
#include "ov10/card_play_sound1.h"
#include "ov10/card_disp.h"
#include "main/xgl_cd.h"
#include "main/xgl_sound.h"

/* Tutorial viewer memory: layout files and textures are loaded back to back from here. */
#define CARD_TUTORIAL_BUFFER ((char *) 0x01a00000)
#define CARD_TUTORIAL_ALIGN 16            /* every file starts on a 16-byte boundary */
#define CARD_TUTORIAL_PATH_SIZE 0x80
#define CARD_TUTORIAL_SET_COUNT 2         /* layout/texture pairs loaded after the first file */
#define CARD_TUTORIAL_TABLE_SIZE 64       /* tutoxtx_tbl entries; 47 are filled */

/* Tutorial page count and the pad actions CardTutorialProc handles: pages run 0..46.
 * Next advances a page and ends the viewer past the last one, Prev goes back, and
 * Close ends the viewer. */
#define CARD_TUTORIAL_PAGE_COUNT 47
#define CARD_TUTORIAL_PAD_NEXT 0x0008
#define CARD_TUTORIAL_PAD_PREV 0x0004
#define CARD_TUTORIAL_PAD_CLOSE 0x0800

/* Two-entry, NULL-terminated list of tutorial base names; cardTutorialNameListInit holds {"Tutorial", NULL}. */
typedef struct {
    const char *name[CARD_TUTORIAL_SET_COUNT];
} TutorialNameList;

/* Translation vector {1.2f, -1.82f, 0.0f, 1.0f}, read by xglMatrixTrans as four floats. The original copies the 16 bytes from
 * cardTutorialTranslationInit with two doubleword moves. A plain float[4] or struct of floats has only 4-byte alignment and
 * compiles to unaligned ldl/ldr/sdl/sdr pairs; the u64 member gives the aggregate the 8-byte alignment that yields the
 * doubleword copy. */
typedef union CardTutorialTranslation {
    float v[4];
    u64 words[2];
} CardTutorialTranslation;


static int tutopage;
static u32 tulex_tbl[CARD_TUTORIAL_SET_COUNT];
static void *tuxtx_tbl[CARD_TUTORIAL_SET_COUNT];
static u32 lex;

/* Declared void here, though main/nml_model_set.h returns int: with the int return the call
 * sites change, and CardTutorialProc keeps a different register for the lex table address. */
extern void nmlModelSetRenderLevel(int level);
/* Declared here, defined at the end of the file. */
extern const TutorialNameList cardTutorialNameListInit;
extern const char cardTutorialFirstLexName[];
extern const char cardTutorialLexExtension[];
extern const CardTutorialTranslation cardTutorialTranslationInit;
extern const char cardTutorialEndLog[];

/* "data\carddata\tuto\" */
char datapath[0x14] = "data\\carddata\\tuto\\";
/* Declared here: the page names Tu_000..Tu_046, defined at the end of the file, which tutoxtx_tbl lists. */
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
static const char *tutoxtx_tbl[CARD_TUTORIAL_TABLE_SIZE] = {
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

/* Reads the texture file of the tutorial page at tutorialIndex into tuxtx. Returns the xglCdReadFile result. */
int CardTutorialFileLoad(int tutorialIndex)
{
    char path[CARD_TUTORIAL_PATH_SIZE];

    memset(path, 0, sizeof(path));
    strcpy(path, datapath);
    strcat(path, tutoxtx_tbl[tutorialIndex]);
    strcat(path, cardTutorialXtxExtension);
    return xglCdReadFile(path, tuxtx, 0, 1);
}

/*
 * Loads the first layout file, then a layout and a texture file for each set name, each
 * file starting on a 16-byte boundary in the tutorial buffer. Ends by loading page 0.
 */
void CardTutorialInit(void)
{
    char path[CARD_TUTORIAL_PATH_SIZE];
    char *buffer;
    int count;
    int i;
    TutorialNameList names;

    buffer = CARD_TUTORIAL_BUFFER;
    tutopage = 0;
    lex = (u32)buffer;
    names = cardTutorialNameListInit;

    memset(path, 0, sizeof(path));
    strcpy(path, datapath);
    strcat(path, cardTutorialFirstLexName);
    count = xglCdReadFile(path, CARD_TUTORIAL_BUFFER, 0, 1);
    if (count > 0) {
        buffer += count;
        buffer = (char *)(((unsigned int)buffer + CARD_TUTORIAL_ALIGN - 1) & ~(CARD_TUTORIAL_ALIGN - 1u));
    }

    for (i = 0; i < CARD_TUTORIAL_SET_COUNT && names.name[i] != 0; i++) {
        memset(path, 0, sizeof(path));
        strcpy(path, datapath);
        strcat(path, names.name[i]);
        strcat(path, cardTutorialLexExtension);
        tulex_tbl[i] = (u32)buffer;
        count = xglCdReadFile(path, buffer, 0, 1);
        if (count > 0) {
            buffer += count;
            buffer = (char *)(((unsigned int)buffer + CARD_TUTORIAL_ALIGN - 1) & ~(CARD_TUTORIAL_ALIGN - 1u));
        }

        memset(path, 0, sizeof(path));
        strcpy(path, datapath);
        strcat(path, names.name[i]);
        strcat(path, cardTutorialXtxExtension);
        tuxtx_tbl[i] = buffer;
        count = xglCdReadFile(path, buffer, 0, 1);
        if (count > 0) {
            buffer += count;
            buffer = (char *)(((unsigned int)buffer + CARD_TUTORIAL_ALIGN - 1) & ~(CARD_TUTORIAL_ALIGN - 1u));
        }
    }

    tuxtx = buffer;
    CardTutorialFileLoad(0);
}

/*
 * Per-frame tutorial viewer: Next and Prev change the page, Close ends the viewer, and the
 * page model is drawn while it stays open. Returns 0 when the viewer ends, 1 otherwise.
 */
int CardTutorialProc(CardGameWork *work)
{
    Matrix4 source;
    Matrix4 placed;
    CardTutorialTranslation translation = cardTutorialTranslationInit;
    unsigned short pressed;

    xglMatrixUnit(source);
    pressed = work->command.input[0].pressed;
    if (pressed & CARD_TUTORIAL_PAD_NEXT) {
        xglSoundEffectNormalID(SE_DECIDE, 0);
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
            xglSoundEffectNormalID(SE_CANCEL, 0);
            CardTutorialFileLoad(tutopage);
            pressed = work->command.input[0].pressed;
        }
    }
    if (pressed & CARD_TUTORIAL_PAD_CLOSE) {
        return 0;
    }
    xglMatrixTrans(placed, source, translation.v);
    nmlModelSetTexture(tuxtx_tbl[0]);
    nmlModelSetPlace(&placed);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntry(tulex_tbl[0]);
    xglMatrixUnit(source);
    source[3][2] = -1.0f;
    nmlModelSetPlace(&source);
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
