/*
 * OV02 original TU 10: 0x00a097d8..0x00a0baf8 (10 functions)
 */
#include "common.h"
#include "shared.h"

extern void *UmnGunoDataBaseTop;

int UmnGunoDataBaseGet(int id)
{
    int base = (int) UmnGunoDataBaseTop;

    if ((u32) (id - 0x22) < 0x1D) {
        return base + id * 0x68 - 0xDD0;
    }
    return base;
}

INCLUDE_ASM("asm/nonmatchings/ov02/umn_guno_data_base_get", tskUmnDataBasePas);

typedef struct UmnWorkData {
    unsigned char unmodeled_00;         /* +0x00 */
    unsigned char mode;                 /* +0x01 */
    unsigned char unmodeled_02;         /* +0x02 */
    unsigned char stateOrCategory;      /* +0x03: screen step, or model category */
    unsigned char unmodeled_04[0x46 - 0x04];
    signed char cursor;                 /* +0x46 */
    unsigned char unmodeled_47[0x52 - 0x47];
    signed char entry;                  /* +0x52 */
    unsigned char unmodeled_53[0x5D - 0x53];
    signed char dataBaseResult;         /* +0x5D */
    signed char analysed;               /* +0x5E */
    signed char modelFlag;              /* +0x5F: modelReady or modelResetFlag by caller */
    unsigned char unmodeled_60[0x80 - 0x60];
} UmnWorkData;
extern UmnWorkData UmnWork;
typedef struct UmnDataBaseTask {
    unsigned char unmodeled_00[0x10];   /* +0x00 */
    int state;                          /* +0x10 */
} UmnDataBaseTask;
typedef struct UmnDataBaseInfoText {
    short x;                            /* +0x00 */
    short y;                            /* +0x02 */
    unsigned char unmodeled_04[0x0C - 0x04];
    char *text;                         /* +0x0C */
    unsigned char unmodeled_10[0x1B0 - 0x10];
} UmnDataBaseInfoText;
typedef struct UmnDataBaseInfoWindow UmnDataBaseInfoWindow;
struct UmnDataBaseInfoWindow {
    short x;                            /* +0x00 */
    short y;                            /* +0x02 */
    int color;                          /* +0x04 */
    short width;                        /* +0x08 */
    short height;                       /* +0x0A */
    unsigned char unmodeled_0c[0x10 - 0x0C];
    unsigned char state;                /* +0x10 */
    unsigned char unmodeled_11[0x14 - 0x11];
    void (*callback)(UmnDataBaseInfoWindow *window, UmnDataBaseInfoText *text);
    UmnDataBaseInfoText *callbackArg;   /* +0x18 */
    unsigned char unmodeled_1c[0x194 - 0x1C];
};
typedef struct UmnDataBaseInfoWork {
    unsigned char state;                /* +0x00 */
    unsigned char opened;               /* +0x01 */
    unsigned char unmodeled_02[0x08 - 0x02];
    int color;                          /* +0x08 */
    UmnDataBaseInfoWindow window;       /* +0x0C */
    UmnDataBaseInfoText info;           /* +0x1A0 */
} UmnDataBaseInfoWork;
static char *msg00_1_00A10700[3];
void MenuInfoWindow(UmnDataBaseInfoWindow *window, UmnDataBaseInfoText *text);
void WindowDXSet(UmnDataBaseInfoWindow *window);
void WindowDXMain(UmnDataBaseInfoWindow *window);
void MoveSlide(short *current, short *target, float rate);

/*
 * The help line at the bottom of the U.M.N. database screen: the frame the
 * task starts on builds the window, and from then on every frame points the
 * line at the help text of the entry the cursor is on and slides the window
 * to the height the terminal's current step asks for.
 */
void tskUmnDataBaseInfo(UmnDataBaseTask *task, UmnDataBaseInfoWork *work)
{
    int state;
    short slideTarget;

    if (UmnWork.mode != 2) {
        task->state = -1;
        return;
    }

    switch (task->state) {
    case 0:
        work->color = 0x00FF0000;
        work->opened = 0;
        work->state = 0;
        WindowDXSet(&work->window);
        work->window.x = -16;
        work->window.state = 0;
        work->window.y = 480;
        work->window.width = 544;
        work->window.height = 54;
        work->window.callback = MenuInfoWindow;
        work->window.callbackArg = &work->info;
        work->info.x = 0;
        work->window.color = work->color;
        work->window.state = 1;
        work->info.y = 0;
        work->info.text = 0;
        WindowDXMain(&work->window);
        work->window.state = 3;
        break;
    case 2:
        state = UmnWork.stateOrCategory;
        slideTarget = 386;
        /* Only the terminal's two entry-list steps keep the line this high. */
        if (state >= 0x12) {
            slideTarget = 480;
        } else if (state < 0x10) {
            slideTarget = 480;
        }
        work->info.text = msg00_1_00A10700[UmnWork.cursor];
        MoveSlide(&work->window.y, &slideTarget, 3.0f);
        WindowDXMain(&work->window);
        break;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov02/umn_guno_data_base_get", tskUmnDataBaseName);

/* PadPrefix canonically covers [0x00, 0x2C). The original
 * tskUmnDataBaseAnalisis also performs an `ld` from PadData + 0x28, so this
 * TU keeps an explicit four-byte-overlapping wide view ending at +0x30. */
typedef union UmnPadInputPrefix {
    PadPrefix canonical;
    struct {
        unsigned char unmodeled_00[0x28];
        union {
            struct {
                unsigned short half_28;
                unsigned short half_2a;
                unsigned int unmodeled_2c;
            } halves;
            unsigned long buttonRegister;
        } buttons;
    } wide;
} UmnPadInputPrefix;
extern PadPrefix PadData;
#define UMN_PAD_INPUT ((UmnPadInputPrefix *)&PadData)
#define PAD_CIRCLE 0x0020
#define PAD_MOVE   0x002C0000
typedef struct UmnGunoDataBase {
    unsigned char unmodeled_00[0x20];
    short x;                                /* +0x20 */
    short y;                                /* +0x22 */
    short page;                             /* +0x24 */
    unsigned char unmodeled_26[0x28 - 0x26];
    int title;                              /* +0x28 */
    int text;                                /* +0x2C */
    int picture;                            /* +0x30 */
    int sound;                              /* +0x34 */
    long model;                             /* +0x38 */
    char name[0x52 - 0x40];                 /* +0x40 */
    char reading[0x68 - 0x52];              /* +0x52 */
} UmnGunoDataBase;
typedef struct UmnDataBaseWinParam {
    int width;                              /* +0x00 */
    int height;                             /* +0x04 */
    int color;                              /* +0x08 */
    UmnGunoDataBase *entry;                 /* +0x0C */
    short x;                                /* +0x10 */
    short y;                                /* +0x12 */
    short page;                             /* +0x14 */
    unsigned char unmodeled_16[0x18 - 0x16];
    int title;                              /* +0x18 */
    int text;                               /* +0x1C */
    int picture;                            /* +0x20 */
    int sound;                              /* +0x24 */
    long model;                             /* +0x28 */
    unsigned char unmodeled_30[0x34 - 0x30];
    char *name;                             /* +0x34 */
    char *reading;                          /* +0x38 */
} UmnDataBaseWinParam;
typedef struct UmnDataBaseAnalisisWork {
    unsigned char step;                     /* +0x00 */
    unsigned char opened;                   /* +0x01 */
} UmnDataBaseAnalisisWork;
typedef struct UmnDataBaseAnalisisTask {
    unsigned char unmodeled_00[0x10];       /* +0x00 */
    int state;                              /* +0x10 */
    unsigned char unmodeled_14[0x1C - 0x14];
    UmnDataBaseAnalisisWork analisis;       /* +0x1C */
} UmnDataBaseAnalisisTask;
extern int BW3BattleOrDataBase;
void eBattleWinOpen3(UmnDataBaseWinParam *param);
void eBattleWinMain3(void);
void eBattleWinClose3(void);
void xglSoundEffectNormalID(int sound_id, int variant);

/*
 * The analysis window of the U.M.N. database: the player opens it with the
 * circle button on an analysed record and closes it again, and the window is
 * driven every frame for as long as it is open.
 */
void tskUmnDataBaseAnalisis(UmnDataBaseAnalisisTask *task)
{
    UmnDataBaseAnalisisWork *work = &task->analisis;
    UmnDataBaseWinParam param;
    UmnGunoDataBase *entry;

    if (UmnWork.mode != 2) {
        task->state = -1;
        return;
    }

    switch (work->step) {
    case 0:
        work->opened = 0;
        work->step = 1;
        /* fallthrough */
    case 1:
        if (UMN_PAD_INPUT->canonical.half_2a & PAD_CIRCLE) {
            if (UmnWork.stateOrCategory == 0x31) {
                if (UmnWork.analysed != 0) {
                    work->step = 0xA;
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            }
        }
        break;
    case 0xA:
        entry = (UmnGunoDataBase *) UmnGunoDataBaseGet(UmnWork.entry);
        param.width = 0x70;
        param.height = 0x90;
        param.color = 0xFFFFF0;
        param.entry = entry;
        param.x = entry->x;
        param.y = entry->y;
        param.page = entry->page;
        param.title = entry->title;
        param.text = entry->text;
        param.picture = entry->picture;
        param.sound = entry->sound;
        param.model = entry->model;
        param.name = entry->name;
        param.reading = entry->reading;
        BW3BattleOrDataBase = 1;
        eBattleWinOpen3(&param);
        work->opened = 1;
        work->step = 0xC;
        /* fallthrough */
    case 0xC:
        if (UmnWork.stateOrCategory == 0x31 && UmnWork.modelFlag == 0
            && !(UMN_PAD_INPUT->wide.buttons.buttonRegister & PAD_MOVE)) {
            break;
        }
        work->step = 0x14;
        if (UMN_PAD_INPUT->canonical.half_2a & PAD_CIRCLE) {
            xglSoundEffectNormalID(2, 0);
        }
        break;
    case 0x14:
        eBattleWinClose3();
        work->step = 1;
        break;
    }

    if (work->opened != 0) {
        eBattleWinMain3();
    }
}

INCLUDE_ASM("asm/nonmatchings/ov02/umn_guno_data_base_get", tskUmnDataBaseMenu);

#define PAD_SQUARE 0x0080
#define PAD_LEFT   0x0004
#define PAD_RIGHT  0x0008
typedef struct UmnDataBaseExWinMessage {
    unsigned char unmodeled_00[0x04];
    short x;                                /* +0x04 */
    short y;                                /* +0x06 */
    int color;                              /* +0x08 */
    unsigned char unmodeled_0c[0x10 - 0x0C];
    signed char blue;                       /* +0x10 */
    signed char green;                      /* +0x11 */
    signed char red;                        /* +0x12 */
    unsigned char unmodeled_13[0x44 - 0x13];
} UmnDataBaseExWinMessage;
typedef struct UmnDataBaseExWinLine {
    short x;                                /* +0x00 */
    short y;                                /* +0x02 */
    UmnDataBaseExWinMessage message;        /* +0x04 */
} UmnDataBaseExWinLine;
typedef struct UmnDataBaseExWinSprite {
    unsigned char unmodeled_00[0x04];
    short x;                                /* +0x04 */
    short y;                                /* +0x06 */
    int color;                              /* +0x08 */
    unsigned char unmodeled_0c[0x28 - 0x0C];
} UmnDataBaseExWinSprite;
typedef struct UmnDataBaseExWinWork {
    unsigned char step;                     /* +0x000 */
    unsigned char opened;                   /* +0x001 */
    unsigned char unmodeled_002;            /* +0x002 */
    unsigned char textShown;                /* +0x003 */
    int color;                              /* +0x004 */
    UmnDataBaseExWinLine lines[5];          /* +0x008 */
    int drift[2];                           /* +0x170 */
    UmnDataBaseExWinSprite sprites[2];      /* +0x178 */
} UmnDataBaseExWinWork;
typedef struct UmnDataBaseArrowSprites {
    short id[2];
} UmnDataBaseArrowSprites;
typedef struct UmnDataBaseArrowSteps {
    int step[2];
} UmnDataBaseArrowSteps;
static const UmnDataBaseArrowSprites D_00A13138 = {{0x0112, 0x0110}};
static const UmnDataBaseArrowSteps D_00A13140 = {{1, -1}};
static char *text_3_00A10720[5];
void eMessageSet(void *message, const char *text);
void eMessageModeChange(UmnDataBaseExWinMessage *message, int mode);
void eMessageMain(void *message);
void eSpriteSet(UmnDataBaseExWinSprite *sprite, short spriteId);
void eSpriteMain(UmnDataBaseExWinSprite *sprite);

/*
 * The extra window of the U.M.N. database: five lines of help text with an
 * arrow sprite at each end. The lines slide in while the terminal shows a
 * record, the square button hides and shows the text and each arrow runs off
 * its own side once the player pages that way.
 */
void tskUmnDataBaseExWin(UmnDataBaseTask *task, UmnDataBaseExWinWork *work)
{
    UmnDataBaseArrowSprites arrowSprite;
    short lineTarget[5];
    short arrowTarget[2];
    UmnDataBaseArrowSteps arrowStep;
    int buttons;
    int pressed;
    int i;

    if (UmnWork.mode != 2) {
        task->state = -1;
        return;
    }

    switch (task->state) {
    case 0:
        work->color = 0xFFFFF0;
        for (i = 0; i < 5; i++) {
            work->lines[i].y = i * 0x18 + 0x120;
            work->lines[i].x = -0xC4;
            eMessageSet(&work->lines[i].message, text_3_00A10720[i]);
            eMessageModeChange(&work->lines[i].message, 0x20);
        }
        arrowSprite = D_00A13138;
        for (i = 0; i < 2; i++) {
            eSpriteSet(&work->sprites[i], arrowSprite.id[i]);
            work->sprites[i].x = i * 0x23D - 0x2D;
            work->sprites[i].color = work->color;
            work->drift[i] = 0;
            work->sprites[i].y = 0xD6;
        }
        work->opened = 0;
        work->textShown = 1;
        work->step = 0;
        break;
    case 2:
    {
        for (i = 0; i < 5; i++) {
            lineTarget[i] = -0xC4;
        }
        arrowTarget[0] = -0x2D;
        arrowTarget[1] = 0x210;
        if (UmnWork.stateOrCategory == 0x31) {
            for (i = 0; i < 5; i++) {
                lineTarget[i] = 0x10;
            }
            buttons = UMN_PAD_INPUT->canonical.half_2a;
            if (buttons & PAD_SQUARE) {
                work->textShown ^= 1;
                buttons = UMN_PAD_INPUT->canonical.half_2a;
            }
            pressed = buttons & 0xFFFF;
            arrowTarget[0] = 8;
            arrowTarget[1] = 0x1DB;
            if (pressed == PAD_LEFT) {
                work->drift[0] = -6;
            }
            if (pressed == PAD_RIGHT) {
                work->drift[1] = 6;
            }
        } else {
            work->textShown = 1;
        }
        for (i = 0; i < 5; i++) {
            MoveSlide(&work->lines[i].x, &lineTarget[i], 3.0f);
            work->lines[i].message.x = work->lines[i].x;
            work->lines[i].message.y = work->lines[i].y;
            work->lines[i].message.color = work->color;
            if (i == 4) {
                if (UmnWork.analysed != 0) {
                    work->lines[i].message.red = -0x80;
                    work->lines[i].message.green = -0x80;
                    work->lines[i].message.blue = -0x80;
                } else {
                    work->lines[i].message.red = 0x40;
                    work->lines[i].message.green = 0x40;
                    work->lines[i].message.blue = 0x40;
                }
            }
            if (work->textShown != 0) {
                eMessageMain(&work->lines[i].message);
            }
        }
        for (i = 0; i < 2; i++) {
            if (work->drift[i] != 0) {
                arrowStep = D_00A13140;
                work->drift[i] += arrowStep.step[i];
            }
            MoveSlide(&work->sprites[i].x, &arrowTarget[i], 3.0f);
            work->sprites[i].x += work->drift[i];
            eSpriteMain(&work->sprites[i]);
        }
    }
        break;
    }
}

#define PAD_MARU 0x0020
#define PAD_BATU 0x0040
typedef struct UmnDataBaseDispParam {
    unsigned char unmodeled_00[0x08];       /* +0x00 */
    short x;                                /* +0x08 */
    short y;                                /* +0x0A */
    short width;                            /* +0x0C */
    short height;                           /* +0x0E */
    unsigned long color;                    /* +0x10 */
    unsigned char unmodeled_18[0x40 - 0x18];
    void *window;                           /* +0x40 */
    void *list;                             /* +0x44 */
    void *selectCursor;                     /* +0x48 */
    void *indexCursor;                      /* +0x4C */
    short state;                            /* +0x50 */
    unsigned char unmodeled_52[0x58 - 0x52];
    short indexNo;                          /* +0x58 */
    short selectNo;                         /* +0x5A */
    unsigned char unmodeled_5c[0x60 - 0x5C];
} UmnDataBaseDispParam;
typedef struct UmnKeyWordRibbon {
    short x;                                /* +0x00 */
    short y;                                /* +0x02 */
    unsigned int color;                     /* +0x04 */
    short width;                            /* +0x08 */
    short height;                           /* +0x0A */
    unsigned char unmodeled_0c[0x70 - 0x0C];
} UmnKeyWordRibbon;
typedef struct UmnKeyWordTagFont {
    unsigned char unmodeled_00[0x04];       /* +0x00 */
    short x;                                /* +0x04 */
    short y;                                /* +0x06 */
    unsigned int color;                     /* +0x08 */
    unsigned char unmodeled_0c[0x20 - 0x0C];
} UmnKeyWordTagFont;
typedef struct UmnKeyWordNumber {
    short x;                                /* +0x00 */
    short y;                                /* +0x02 */
    unsigned int color;                     /* +0x04 */
    unsigned char unmodeled_08[0x0E - 0x08];
    unsigned char type;                     /* +0x0E */
    unsigned char digits;                   /* +0x0F */
    unsigned char unmodeled_10[0x14 - 0x10];
    int value;                              /* +0x14 */
    unsigned char unmodeled_18[0x90 - 0x18];
} UmnKeyWordNumber;
typedef struct UmnKeyWordMessage {
    unsigned char unmodeled_00;             /* +0x00 */
    unsigned char mode;                     /* +0x01 */
    unsigned char unmodeled_02[0x04 - 0x02];
    short x;                                /* +0x04 */
    short y;                                /* +0x06 */
    unsigned int color;                     /* +0x08 */
    unsigned char unmodeled_0c[0x48 - 0x0C];
} UmnKeyWordMessage;
typedef struct UmnDataBaseKeyWordWork {
    unsigned char step;                     /* +0x000 */
    unsigned char active;                   /* +0x001 */
    unsigned char visible;                  /* +0x002 */
    unsigned char messageShown;             /* +0x003 */
    unsigned int color;                     /* +0x004 */
    unsigned char window[0x194];            /* +0x008 */
    unsigned char list[0x5F8];              /* +0x19C */
    unsigned char selectCursor[0x24];       /* +0x794 */
    unsigned char indexCursor[0x28];        /* +0x7B8 */
    UmnDataBaseDispParam disp;              /* +0x7E0 */
    short x;                                /* +0x840 */
    short y;                                /* +0x842 */
    int indexNo;                            /* +0x844 */
    int selectNo;                           /* +0x848 */
    UmnKeyWordTagFont tag;                  /* +0x84C */
    UmnKeyWordRibbon ribbon;                /* +0x86C */
    UmnKeyWordNumber number;                /* +0x8DC */
    UmnKeyWordMessage message;              /* +0x96C */
} UmnDataBaseKeyWordWork;
#define UMN_KEYWORD_HIDDEN_X (-272)
static const char D_00A13148[5] = "\x01Num";
static char msg00_4[16];
int tyaUmlDispParamReset(UmnDataBaseDispParam *disp, int mode);
int tyaUmlDatabaseMain(UmnDataBaseDispParam *disp);
void eRibbonSet(UmnKeyWordRibbon *ribbon, int mode);
void eRibbonMain(UmnKeyWordRibbon *ribbon);
void eTagFontSet(UmnKeyWordTagFont *tag, const char *text);
void eTagFontMain(UmnKeyWordTagFont *tag);
void eNumberSet(UmnKeyWordNumber *number, int mode);
void eNumberMain(UmnKeyWordNumber *number);

/*
 * The keyword screen of the U.M.N. database: the frame the task starts on
 * builds the database page and the counter row, and from then on the screen
 * waits for the terminal to reach the keyword step, drives the page while it
 * is there and slides the counter row in and out again.
 */
void tskUmnDataBaseKeyWord(UmnDataBaseTask *task, UmnDataBaseKeyWordWork *work)
{
    short slideTarget;

    if (UmnWork.mode != 2) {
        task->state = -1;
        return;
    }

    switch (task->state) {
    case 0:
        work->color = 0x00F00000;
        tyaUmlDispParamReset(&work->disp, 0);
        work->disp.window = work->window;
        work->disp.list = work->list;
        work->disp.selectCursor = work->selectCursor;
        work->disp.indexCursor = work->indexCursor;
        work->disp.x = 1808;
        work->disp.width = 480;
        work->disp.y = 1920;
        work->disp.color = work->color;
        work->disp.height = 314;
        work->x = UMN_KEYWORD_HIDDEN_X;
        work->y = 48;
        eRibbonSet(&work->ribbon, 3);
        work->ribbon.width = 256;
        work->ribbon.height = 24;
        eTagFontSet(&work->tag, D_00A13148);
        eNumberSet(&work->number, 0);
        eMessageSet(&work->message, msg00_4);
        work->message.mode = 32;
        work->step = 0;
        work->active = 0;
        break;
    case 2:
        switch (work->step) {
        case 0:
            work->active = 0;
            work->disp.state = 0;
            /* fallthrough */
        case 1:
            if (UmnWork.stateOrCategory == 0x51) {
                work->step = 10;
            }
            break;
        case 10:
            work->active = 1;
            work->step = 11;
            work->visible = 1;
            work->messageShown = 1;
            /* fallthrough */
        case 11:
            if (UmnWork.stateOrCategory == 0x11 && UmnWork.dataBaseResult == 1) {
                work->step = 0;
            }
            break;
        }

        if (work->active != 0) {
            if (work->disp.selectNo >= 0) {
                if (UMN_PAD_INPUT->canonical.half_2a & PAD_MARU) {
                    work->messageShown = 0;
                }
                if (UMN_PAD_INPUT->canonical.half_2a & PAD_BATU) {
                    work->messageShown = 1;
                }
            }
            UmnWork.dataBaseResult = tyaUmlDatabaseMain(&work->disp);
            work->indexNo = work->disp.selectNo;
            work->selectNo = work->disp.indexNo;
            if (work->messageShown != 0) {
                work->message.x = 48;
                work->message.y = 416;
                work->message.color = work->color + 15;
                eMessageMain(&work->message);
            }
        }

        if (work->visible != 0) {
            slideTarget = UMN_KEYWORD_HIDDEN_X;
            /* Only the terminal's keyword steps keep the row on screen. */
            if (UmnWork.stateOrCategory >> 4 == 5) {
                slideTarget = 0;
            }
            MoveSlide(&work->x, &slideTarget, 3.0f);
            if (work->x == UMN_KEYWORD_HIDDEN_X) {
                work->visible = 0;
            }
            work->y = 48;
            work->ribbon.x = work->x;
            work->ribbon.y = work->y;
            work->ribbon.color = work->color;
            eRibbonMain(&work->ribbon);
            work->tag.x = work->x + 166;
            work->tag.y = work->y + 4;
            work->tag.color = work->ribbon.color + 1;
            eTagFontMain(&work->tag);
            work->number.x = work->tag.x + 41;
            work->number.y = work->tag.y;
            work->number.color = work->tag.color;
            work->number.value = (work->selectNo << 16) + (unsigned short) work->indexNo;
            work->number.type = 3;
            work->number.digits = 3;
            eNumberMain(&work->number);
        }
        break;
    }
}

typedef struct UmnDataBaseModelXform {
    float position[3];              /* +0x00 */
    unsigned char unmodeled_0c[0x20 - 0x0C];
    float scale[3];                  /* +0x20 */
    unsigned char unmodeled_2c[0x40 - 0x2C];
    float baseRotation[3];           /* +0x40 */
    unsigned char unmodeled_4c[0x60 - 0x4C];
    float visible;                    /* +0x60 */
} UmnDataBaseModelXform;
typedef struct UmnDataBaseModelData {
    unsigned char unmodeled_00[0xC0];
    UmnDataBaseModelXform xform;     /* +0xC0 */
} UmnDataBaseModelData;
typedef struct UmnDataBaseModelWork {
    unsigned char unmodeled_00[0x10];
    signed char mode;                /* +0x10 */
    unsigned char active;            /* +0x11 */
    unsigned char unmodeled_12;
    unsigned char state;             /* +0x13 */
    unsigned char unmodeled_14[0x20 - 0x14];
    UmnDataBaseModelData *model;     /* +0x20 */
} UmnDataBaseModelWork;
extern void MenuModelControl(UmnDataBaseModelWork *work);
extern void MenuModelUnitBreak(void);
extern void MenuModelUnitOpen(UmnDataBaseModelWork *work, int mode);

void UmnDataBaseModel(UmnDataBaseModelWork *work)
{
    UmnDataBaseModelData *model = work->model;

    if (model == 0) {
        if (UmnWork.modelFlag != 0) {
            MenuModelUnitBreak();
            UmnWork.modelFlag = 0;
        }
    }

    if (work->active != 0) {
        UmnDataBaseModelXform *xform = &model->xform;

        if (UmnWork.stateOrCategory != 0x31) {
            if (work->state != 0x63) {
                work->state = 0x1E;
            }
        }
        if (UmnWork.modelFlag != 0) {
            work->state = 0x1E;
            UmnWork.modelFlag = 0;
        }
        MenuModelControl(work);
        switch (work->state) {
        case 0: {
            float *scale = xform->scale;

            xform->position[0] = -xform->baseRotation[0];
            xform->position[1] = -xform->baseRotation[1];
            xform->position[2] = -xform->baseRotation[2];
            scale[2] = 0.5f;
            scale[1] = 0.5f;
            scale[0] = 0.5f;
            work->state = 0xA;
        }
            /* fallthrough */
        case 0xA:
            MenuModelUnitOpen(work, 2);
            work->state = 0x14;
            break;
        case 0x14:
            break;
        case 0x1E:
            MenuModelUnitOpen(work, 1);
            work->state = 0x1F;
            /* fallthrough */
        case 0x1F:
            if (xform->visible == 0.0f) {
                work->state = 0x63;
        case 0x63:
                work->mode = -1;
            }
            break;
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/ov02/umn_guno_data_base_get", UmnDataBase);

extern char D_00A12EE0[];
extern char D_00A12EB0[];
extern char D_00A12EA8[];
extern char D_00A13128[];
extern char D_00A13118[];
extern char D_00A13108[];
extern char D_00A130F0[];
extern char D_00A130D8[];

static char *msg00_1_00A10700[3] = {
    D_00A12EE0, D_00A12EB0, D_00A12EA8,
};
static char *text_3_00A10720[5] = {
    D_00A13128, D_00A13118, D_00A13108, D_00A130F0, D_00A130D8,
};
static char msg00_4[16] = "\x1E\0Index Search:";



char D_00A12EA8[8] = "Cancel.";

char D_00A12EB0[48] = "View keywords that you have come across.";

char D_00A12EE0[56] = "View data of all the Gnosis you have encountered.";



char D_00A130D8[24] = "\036\001\241\247Analysis info";

char D_00A130F0[24] = "\036\000\241\247Turn menu off";

char D_00A13108[16] = "\0364\241\247Rotate";

char D_00A13118[16] = "\0365\241\247Zoom in";

char D_00A13128[16] = "\0366\241\247Zoom out";
