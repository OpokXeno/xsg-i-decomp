#include "common.h"

typedef struct MenuTecData {
    unsigned char unmodeled_00[8];
    unsigned char levelPointIncrement[8];
    unsigned short levelPointBase[8];
    unsigned short speedPointCost[8];
    unsigned short waitPointCost[8];
} MenuTecData;

extern MenuTecData *MenuTecDataBuf;

void *MenuTecDataGet(int chrNo);

struct MenuTecSaveRecord *MenuTecSaveDataGet(int chrNo);

int MenuTecNextSpeedPointGet(int chrNo, int point);

/* dataPlChaGet (ov01/data_unit_org_get.c VA 0x00a19210, still INCLUDE_ASM
 * there and not yet linkable by that name): returns the running character's
 * control record, whose word at +0xC this function decrements. Kept under
 * the scaffold's undefined_funcs_auto.txt placeholder name until that TU
 * recovers it. */

int *func_A19210(int chrNo);

typedef struct MenuWorkState {
    unsigned char unmodeled_00[3];
    unsigned char state;
    unsigned char unmodeled_04[0x1C];
    unsigned char resetFlags;
    unsigned char unmodeled_21[0xF];
    signed char listCursor;
    signed char mode;
    signed char rowCursor;
    signed char selectCursor;
    unsigned char unmodeled_34[0x0C];
    signed char characterNo;
    unsigned char unmodeled_41[2];
    signed char tecBit;
    signed char point;
    unsigned char unmodeled_45[0x3b];
} MenuWorkState;

extern MenuWorkState MenuWork;

int MenuTecTLevLimitCheck(int chrNo, int point);

int MenuTecNextTLevPointGet(int chrNo, int point);

/* dataUnitOrgGet (ov01/data_unit_org_get.c VA 0x00a191c0, still INCLUDE_ASM
 * there and not yet linkable by that name): returns the running character's
 * parameter record (the full record is src/main/menu_para_pt_rate_get.c's
 * CharParaData); MenuTecEquipCheck only reads the six equipped technique ids
 * at +0x82. Kept under the scaffold's undefined_funcs_auto.txt placeholder
 * name until that TU recovers it. */

typedef struct MenuTecEquipRecord {
    unsigned char unmodeled_00[0x82];
    short techniqueId[6]; /* +0x82 */
} MenuTecEquipRecord;

MenuTecEquipRecord *func_A191C0(int chrNo);

/* dataTecGet (ov01/data_unit_org_get.c VA 0x00a1a378, still INCLUDE_ASM there
 * and not yet linkable by that name): returns a pointer to the technique's
 * metadata record, or a NULL pointer plus a diagnostic print for a
 * non-positive index. Kept under the scaffold's undefined_funcs_auto.txt
 * placeholder name until that TU recovers it. */

unsigned char *func_A1A378(int tecBit);

/* Returns whether either of the technique's two low target-metadata bits is
 * set (elf_names annotation). */

int BitToTecNo(int bit);

#include "main/party.h"

typedef struct MenuTecSaveRecord {
    unsigned char level[8];
    unsigned char speed[8];
    unsigned char wait[8];
    unsigned char levelLimit[8];
} MenuTecSaveRecord;

typedef struct MenuTecPartySave {
    unsigned char unmodeled_00[0x3c];
    MenuTecSaveRecord technique[8];
} MenuTecPartySave;

typedef struct MenuTecCharTechnique {
    short levelAdjustment;
    short waitAdjustment;
    unsigned char unmodeled_04[8];
} MenuTecCharTechnique;

typedef struct MenuTecCharacter {
    unsigned char unmodeled_00[0x0c];
    int points;
    unsigned char unmodeled_10[0x3c];
    MenuTecCharTechnique technique[8];
} MenuTecCharacter;

int MenuTecNextWaitPointGet(int chrNo, int point);

/* dataPlChaGet (ov01/data_unit_org_get.c VA 0x00a19210, still INCLUDE_ASM
 * there and not yet linkable by that name): returns the running character's
 * control record, whose word at +0xC this function decrements. Kept under
 * the scaffold's undefined_funcs_auto.txt placeholder name until that TU
 * recovers it. */

#include "shared.h"

typedef struct MenuTecPasWork MenuTecPasWork;

typedef struct MenuTecMenuWork MenuTecMenuWork;

typedef struct MenuTecL1R1Work MenuTecL1R1Work;

typedef struct MenuTecListWork MenuTecListWork;

typedef struct MenuTecSetWinWork MenuTecSetWinWork;

extern MenuTecPasWork *MenuTecPas;

extern MenuTecMenuWork *MenuTecMenu;

extern MenuTecL1R1Work *MenuTecL1R1;

extern MenuTecListWork *MenuTecList;

extern MenuTecSetWinWork *MenuTecSetWin;

typedef struct MenuTecListRow {
    const char *name;
    int amount;
    unsigned char flag;
} MenuTecListRow;

struct MenuTecListRow *MenuListGet(int list);

struct MenuTecListRow *MenuListMake(int list, int mode);

int MenuSortCheck(int list);

void MoveSlide(short *current, short *target, float rate);

void WindowDXSet(void *window);

void WindowDXMain(void *window);

void eSpriteSet(void *sprite, short spriteId);

void eSpriteMain(void *sprite);

void eNumberSet(void *number, int mode);

void eNumberMain(void *number);

void eTagFontSet(void *tag, const char *tagWord);

void eTagFontMain(void *tag);

extern PadPrefix PadData;

#define PAD_L1 0x0004

#define PAD_R1 0x0008

typedef struct MenuTecLabel {
    unsigned char unmodeled_00[4];
    short x;                        /* +0x04 */
    short y;                        /* +0x06 */
    unsigned int drawOrder;         /* +0x08 */
    signed char color[4];           /* +0x0C */
    unsigned char unmodeled_10[0x1C - 0x10];
    const char *text;               /* +0x1C */
} MenuTecLabel;

typedef struct MenuTecNumber {
    short x;                        /* +0x00 */
    short y;                        /* +0x02 */
    unsigned int drawOrder;         /* +0x04 */
    unsigned char unmodeled_08[6];
    unsigned char format;           /* +0x0E */
    unsigned char digits;           /* +0x0F */
    unsigned char unmodeled_10[4];
    int value;                      /* +0x14 */
    unsigned char unmodeled_18[0x90 - 0x18];
} MenuTecNumber;

typedef struct MenuTecPasWindow {
    short x;                            /* +0x00 */
    short y;                            /* +0x02 */
    int color;                          /* +0x04 */
    short width;                        /* +0x08 */
    short height;                       /* +0x0A */
    unsigned char unmodeled_0c[0x10 - 0x0C];
    signed char state;                  /* +0x10 */
    unsigned char unmodeled_11[0x14 - 0x11];
    void (*callback)(void);             /* +0x14 */
    void *callbackArg;                  /* +0x18 */
    unsigned char unmodeled_1c[0x194 - 0x1C];
} MenuTecPasWindow;

typedef struct MenuTecPasMessage {
    unsigned char unmodeled_00;
    unsigned char mode;                 /* +0x01 */
    unsigned char unmodeled_02[2];
    short x;                            /* +0x04 */
    short y;                            /* +0x06 */
    int color;                          /* +0x08 */
    unsigned char unmodeled_0c[0x44 - 0x0C];
} MenuTecPasMessage;

typedef struct MenuTecPasBox {
    short x;                            /* +0x00 */
    short y;                            /* +0x02 */
    int color;                          /* +0x04 */
    short width;                        /* +0x08 */
    short height;                       /* +0x0A */
} MenuTecPasBox;

struct MenuTecPasWork {
    unsigned char state;                /* +0x000 */
    unsigned char unmodeled_01[3];
    int color;                          /* +0x004 */
    MenuTecPasWindow window;            /* +0x008 */
    MenuTecPasMessage message[3];       /* +0x19C */
    unsigned char unmodeled_268[0x334 - 0x268];
    MenuTecPasBox box;                  /* +0x334 */
    unsigned char callbackWork[0x10];   /* +0x340 */
};

extern const char *msg_0_0036DC68[];

int MenuPasLengthGet(const char *text);

void MenuPasWindow(void);

void eMessageSet(void *message, const char *text);

void eMessageMain(void *message);

void endPrintExtFunc(int kind, int id, void *data);

typedef struct MenuTecWindowDX {
    short x;                            /* +0x00 */
    short y;                            /* +0x02 */
    int color;                          /* +0x04 */
    short width;                        /* +0x08 */
    short height;                       /* +0x0A */
    const char *title;                  /* +0x0C */
    unsigned char state;                /* +0x10 */
    unsigned char unmodeled_11[0x14 - 0x11];
    void *select;                       /* +0x14 */
    void *selectArg;                    /* +0x18 */
    unsigned char unmodeled_1c[0x194 - 0x1C];
} MenuTecWindowDX;

typedef struct MenuTecSetWinRow {
    MenuTecLabel label[2];          /* +0x00: the row name, then its status text */
    MenuTecNumber number[2];        /* +0x40: the current level, then the next cost */
} MenuTecSetWinRow;

typedef struct MenuTecCursor {
    signed char mode;               /* +0x00 */
    unsigned char unmodeled_01[3];
    short x;                        /* +0x04 */
    short y;                        /* +0x06 */
    unsigned int drawOrder;         /* +0x08 */
} MenuTecCursor;

struct MenuTecSetWinWork {
    unsigned char state;                /* +0x000 */
    unsigned char flags;
    unsigned char unmodeled_02[2];
    int color;                          /* +0x004 */
    MenuTecWindowDX window;             /* +0x008 */
    MenuTecSetWinRow row[3];            /* +0x19C */
    MenuTecLabel footer;                /* +0x5BC */
    unsigned char unmodeled_5dc[0x61C - 0x5DC];
    MenuTecCursor cursor;               /* +0x61C */
};

typedef struct MenuTecList {
    unsigned char unmodeled_00[0x02];
    unsigned short hasPrompt;           /* +0x02 */
    int cursor;                         /* +0x04 */
    const char *items;                  /* +0x08 */
    const char *prompt;                 /* +0x0C */
    unsigned char unmodeled_10[0x5F8 - 0x10];
} MenuTecSelectList;

struct MenuTecMenuWork {
    unsigned char state;                /* +0x000 */
    unsigned char unmodeled_01[3];
    int color;                          /* +0x004 */
    MenuTecWindowDX window[2];          /* +0x008 */
    MenuTecSelectList list[2];          /* +0x330 */
};

extern const char *msg00_1_0036DC78[];

extern const char *msg01_2_0036DC80[];

void MenuSelectWindow(MenuTecWindowDX *window, MenuTecSelectList *list);

typedef struct MenuTecL1R1Sprite {
    unsigned char unmodeled_00[0x04];
    short x;    /* +0x04 */
    short y;    /* +0x06 */
    int work;   /* +0x08 */
    unsigned char unmodeled_0c[0x28 - 0x0c];
} MenuTecL1R1Sprite;

struct MenuTecL1R1Work {
    unsigned char state;         /* +0x00 */
    unsigned char unmodeled_01;
    signed char slideOffset[2];  /* +0x02: added into sprite[i].x while non-zero */
    int spriteWork;              /* +0x04: shared initial value for both sprites' work */
    MenuTecL1R1Sprite sprite[2]; /* +0x08: L1, then R1 */
};

typedef struct MenuTecSPWindow {
    unsigned char state;                /* +0x00 */
    unsigned char rowFlags;             /* +0x01 */
    unsigned char unmodeled_02[2];
    short position;                     /* +0x04 */
    short contentHeight;                /* +0x06 */
    int color;                          /* +0x08 */
    short width;                        /* +0x0C */
    short height;                       /* +0x0E */
    const char *caption;                /* +0x10 */
    unsigned char columns;              /* +0x14 */
    unsigned char rows;                 /* +0x15 */
    unsigned char unmodeled_16[0x1C - 0x16];
    MenuTecListRow *items;              /* +0x1C */
    unsigned char unmodeled_20[0x26 - 0x20];
    unsigned char selectionSet;         /* +0x26 */
    unsigned char unmodeled_27;
} MenuTecSPWindow;

struct MenuTecListWork {
    unsigned char state;                /* +0x00 */
    unsigned char unmodeled_01;
    unsigned char valid;                /* +0x02 */
    unsigned char side;                 /* +0x03 */
    int color;                          /* +0x04 */
    MenuTecSPWindow window;             /* +0x08 */
};

extern const char *msg00_7[];

extern const char *msg01_8_0036DCB8[];

extern const char *msg02_9[];

extern const char *msg03_10[];

void eCursolSet(void *cursor, signed char width);

void eCursolMain(void *cursor);

void eCursolModeChange(void *cursor, int mode);

int MenuTecSpeedLimitCheck(int chrNo, int point);

int MenuTecWaitLimitCheck(int chrNo, int point);

int MenuTecCharTLevUpCheck(void);

int MenuTecCharSpeedUpCheck(void);

int MenuTecCharWaitUpCheck(void);

typedef struct MenuTecL1R1SpriteIds {
    short id[2];
} MenuTecL1R1SpriteIds;

typedef struct MenuTecL1R1SlideStep {
    signed char side[2];
} MenuTecL1R1SlideStep;

int func_A197E8(int chrNo, int tecNo);

void *MenuSortAddrGet(int list);

extern const unsigned char D_004C92E8[28];

extern const unsigned char MenuTecTrgTbl00[];

extern const unsigned char MenuTecTrgTbl01[];

MenuTecSaveRecord *MenuTecSaveDataGet(int chrNo)
{
    int characterIndex = chrNo & 0xffff;
    MenuTecPartySave *party = (MenuTecPartySave *)PartyDataGet();
    MenuTecSaveRecord *records = party->technique;

    if ((unsigned int)(characterIndex - 1) >= 8U)
        return 0;
    return &records[characterIndex - 1];
}

void *MenuTecDataGet(int characterNo)
{
    unsigned int characterIndex = characterNo & 0xFFFF;
    MenuTecData *records = MenuTecDataBuf;

    if (characterIndex - 1 >= 8U)
        return 0;

    return &records[characterIndex - 1];
}

int BitToTecNo(int bit)
{
    int techniqueBit = bit & 0xffff;

    if (techniqueBit >= 45 && techniqueBit <= 52)
        return techniqueBit - 45;
    if (techniqueBit >= 53 && techniqueBit <= 59)
        return techniqueBit - 53;
    if (techniqueBit >= 60 && techniqueBit <= 67)
        return techniqueBit - 60;
    if (techniqueBit >= 68 && techniqueBit <= 75)
        return techniqueBit - 68;
    if (techniqueBit >= 76 && techniqueBit <= 82)
        return techniqueBit - 76;
    if (techniqueBit >= 83 && techniqueBit <= 90)
        return techniqueBit - 83;
    if (techniqueBit >= 91 && techniqueBit <= 98)
        return techniqueBit - 91;
    return -1;
}

int MenuTecNextTLevPointGet(int chrNo, int point)
{
    MenuTecData *data = MenuTecDataGet(chrNo);
    MenuTecSaveRecord *saveData = MenuTecSaveDataGet(chrNo);

    if (point < 0)
        return 0;
    return data->levelPointBase[point]
         + data->levelPointIncrement[point] * (saveData->level[point] + 1);
}

int MenuTecTLevLimitCheck(int chrNo, int point)
{
    MenuTecSaveRecord *saveData = MenuTecSaveDataGet(chrNo);

    if (point < 0)
        return 0;
    return saveData->level[point] < saveData->levelLimit[point];
}

void MenuTecTLevUp(int chrNo, int point)
{
    int maskedChrNo = chrNo & 0xffff;
    MenuTecSaveRecord *saveData = MenuTecSaveDataGet(maskedChrNo);
    MenuTecCharacter *character = (MenuTecCharacter *)func_A19210(maskedChrNo);

    if (point >= 0) {
        int points = MenuTecNextTLevPointGet(chrNo, point);

        saveData->level[point]++;
        character->technique[point].levelAdjustment = saveData->level[point] * 3 - 3;
        character->points -= points;
    }
}

int MenuTecNextSpeedPointGet(int chrNo, int point)
{
    MenuTecData *data = MenuTecDataGet(chrNo);

    if (point < 0)
        return 0;

    return data->speedPointCost[point];
}

int MenuTecSpeedLimitCheck(int chrNo, int point)
{
    unsigned char *saveData = (unsigned char *)MenuTecSaveDataGet(chrNo);

    if (point < 0)
        return 0;

    return saveData[point + 8] != 1;
}

void MenuTecSpeedUp(int chrNo, int point)
{
    int maskedChrNo = chrNo & 0xFFFF;
    MenuTecSaveRecord *saveData = MenuTecSaveDataGet(maskedChrNo);
    int *unit = func_A19210(maskedChrNo);

    if (point >= 0) {
        int points = MenuTecNextSpeedPointGet(chrNo, point);

        saveData->speed[point] = 1;
        unit[3] -= points;
    }
}

int MenuTecNextWaitPointGet(int chrNo, int point)
{
    MenuTecData *data = MenuTecDataGet(chrNo);

    if (point < 0)
        return 0;

    return data->waitPointCost[point];
}

int MenuTecWaitLimitCheck(int chrNo, int point)
{
    /* The original adds the byte offset to the record address as integers
     * (addu point, record); the pointer form swaps the addu operands. */
    unsigned char *record = (unsigned char *)(point + (int)MenuTecSaveDataGet(chrNo));

    if (point < 0) {
        return 0;
    }
    return record[0x10] != 0;
}

void MenuTecWaitUp(int chrNo, int point)
{
    int maskedChrNo = chrNo & 0xffff;
    MenuTecSaveRecord *saveData = MenuTecSaveDataGet(chrNo);
    MenuTecCharacter *character = (MenuTecCharacter *)func_A19210(maskedChrNo);

    if (point >= 0) {
        int points = MenuTecNextWaitPointGet(chrNo, point);

        saveData->wait[point]--;
        character->technique[point].waitAdjustment = saveData->wait[point];
        character->points -= points;
    }
}

int MenuTecCharTLevUpCheck(void)
{
    int *characterData;
    int nextPoint;

    if (MenuTecTLevLimitCheck(MenuWork.characterNo, MenuWork.point) != 0) {
        characterData = func_A19210(MenuWork.characterNo);
        nextPoint = MenuTecNextTLevPointGet(MenuWork.characterNo, MenuWork.point);
        if (characterData[3] >= nextPoint)
            return 1;
    }

    return 0;
}

int MenuTecCharSpeedUpCheck(void)
{
    int *characterData;
    int nextPoint;

    if (MenuTecSpeedLimitCheck(MenuWork.characterNo, MenuWork.point) != 0) {
        characterData = func_A19210(MenuWork.characterNo);
        nextPoint = MenuTecNextSpeedPointGet(MenuWork.characterNo, MenuWork.point);
        if (characterData[3] >= nextPoint)
            return 1;
    }
    return 0;
}

int MenuTecCharWaitUpCheck(void)
{
    int *characterData;
    int nextPoint;

    if (MenuTecWaitLimitCheck(MenuWork.characterNo, MenuWork.point) != 0) {
        characterData = func_A19210(MenuWork.characterNo);
        nextPoint = MenuTecNextWaitPointGet(MenuWork.characterNo, MenuWork.point);
        if (characterData[3] >= nextPoint)
            return 1;
    }
    return 0;
}

void MenuTecPasMain(void)
{
    MenuTecPasWork *pas = MenuTecPas;
    int tabLength[3];
    short tabTarget[3];
    short slideTarget;
    int i;

    for (i = 0; i < 3; i++)
        tabLength[i] = MenuPasLengthGet(msg_0_0036DC68[i]);

    switch (pas->state) {
    case 0:
        pas->color = 0x00FFFFF0;
        WindowDXSet(&pas->window);
        pas->window.x = -288;
        pas->window.y = 8;
        pas->window.color = pas->color;
        pas->window.width = 272;
        pas->window.height = 30;
        pas->window.callback = MenuPasWindow;
        pas->window.callbackArg = pas->callbackWork;
        pas->window.state = 1;
        WindowDXMain(&pas->window);
        pas->window.state = 3;
        for (i = 0; i < 3; i++) {
            eMessageSet(&pas->message[i], msg_0_0036DC68[i]);
            pas->message[i].mode = 0x20;
            pas->message[i].x = 288;
            pas->message[i].y = 11;
            pas->message[i].color = pas->color + 2;
        }
        pas->state = 2;
        /* fallthrough */
    case 2:
        break;
    default:
        return;
    }

    slideTarget = -16;
    for (i = 0; i < 3; i++)
        tabTarget[i] = 288;

    switch (MenuWork.state) {
    case 0x10:
    case 0x20:
        tabTarget[0] = 16;
        break;
    case 0x30:
    case 0x32:
    case 0x50:
    case 0x52:
    case 0x54:
    case 0x56:
        tabTarget[0] = 16;
        tabTarget[MenuWork.listCursor + 1] = tabLength[0] + 16;
        break;
    default:
        slideTarget = -288;
        break;
    }

    MoveSlide(&pas->window.x, &slideTarget, 3.0f);
    WindowDXMain(&pas->window);
    if (pas->window.state == 3) {
        pas->box.x = pas->window.x + 3;
        pas->box.y = pas->window.y + 3;
        pas->box.color = pas->color;
        pas->box.width = pas->window.width - 6;
        pas->box.height = pas->window.height - 6;
        endPrintExtFunc(pas->color, 0x65, &pas->box);
        for (i = 0; i < 3; i++) {
            MoveSlide(&pas->message[i].x, &tabTarget[i], 5.0f);
            if (pas->message[i].x < 256)
                eMessageMain(&pas->message[i]);
        }
        endPrintExtFunc(pas->color, 0x66, 0);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecInfoMain);

void MenuTecMenuMain(void)
{
    MenuTecMenuWork *work = MenuTecMenu;
    short target[2];
    int i;

    switch (work->state) {
    case 0: {
        work->color = 0x00FFFFF0;
        WindowDXSet(&work->window[0]);
        work->window[0].x = 528;
        work->window[0].y = 192;
        work->window[0].color = work->color;
        work->window[0].width = 193;
        work->window[0].height = 78;
        work->window[0].title = "Menu";
        work->window[0].select = (void *)MenuSelectWindow;
        work->window[0].selectArg = &work->list[0];
        work->list[0].cursor = 0;
        work->list[0].hasPrompt = 0;
        work->list[0].items = msg00_1_0036DC78[0];
        work->window[0].state = 1;
        WindowDXMain(&work->window[0]);
        work->window[0].state = 3;
        WindowDXSet(&work->window[1]);
        work->window[1].x = 528;
        work->window[1].y = 240;
        work->window[1].color = work->color;
        work->window[1].width = 169;
        work->window[1].height = 102;
        work->window[1].title = "Select";
        work->window[1].select = (void *)MenuSelectWindow;
        work->window[1].selectArg = &work->list[1];
        work->list[1].cursor = 0;
        work->list[1].hasPrompt = 1;
        work->list[1].items = msg01_2_0036DC80[0];
        work->list[1].prompt = msg01_2_0036DC80[1];
        work->window[1].state = 1;
        WindowDXMain(&work->window[1]);
        work->window[1].state = 3;
        work->state = 2;
    }
        /* fallthrough */
    case 2:
        target[0] = target[1] = 528;
        switch (MenuWork.state) {
        case 0x20:
            if (MenuWork.resetFlags & 1) {
                work->window[0].x = 528;
                work->list[0].cursor = 0;
            }
            target[0] = 288;
            work->list[0].cursor = MenuWork.listCursor;
            break;
        case 0x54:
            if (MenuWork.resetFlags & 1) {
                work->window[1].x = 528;
                work->list[1].cursor = 0;
            }
            target[1] = 288;
            work->list[1].cursor = MenuWork.selectCursor;
            break;
        }
        for (i = 0; i < 2; i++) {
            MoveSlide(&work->window[i].x, &target[i], 3.0f);
            WindowDXMain(&work->window[i]);
        }
        break;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecExMain);

void MenuTecL1R1Main(void)
{
    MenuTecL1R1Work *self = MenuTecL1R1;
    signed char *slideOffset;
    int i;

    switch (self->state) {
    case 0: {
        MenuTecL1R1SpriteIds spriteIds = { { 0x112, 0x110 } };

        self->spriteWork = 0x00FFFFFF;
        for (i = 0; i < 2; i++) {
            eSpriteSet(&self->sprite[i], spriteIds.id[i]);
            self->sprite[i].work = self->spriteWork;
            self->sprite[i].y = 140;
        }
        self->sprite[0].x = -45;
        self->sprite[1].x = 528;
        slideOffset = self->slideOffset;
        self->slideOffset[1] = 0;
        self->slideOffset[0] = 0;
        i = 2;
        self->state = i;
        break;
    }
    case 2:
        slideOffset = self->slideOffset;
        break;
    default:
        return;
    }

    {
        short target[2];

        target[0] = -45;
        target[1] = 528;
        if (MenuWork.state == 0x20) {
            target[0] = 8;
            target[1] = 475;
            if (PadData.half_2a & PAD_L1) {
                self->slideOffset[0] = -6;
            } else if (PadData.half_2a & PAD_R1) {
                self->slideOffset[1] = 6;
            }
        }

        {
            MenuTecL1R1SlideStep step = { { 1, -1 } };

            for (i = 0; i < 2; i++) {
                if (self->slideOffset[i] != 0) {
                    self->slideOffset[i] = self->slideOffset[i] + step.side[i];
                }
            }
        }

        for (i = 0; i < 2; i++) {
            MoveSlide(&self->sprite[i].x, &target[i], 3.0f);
            self->sprite[i].x = self->sprite[i].x + slideOffset[i];
            eSpriteMain(&self->sprite[i]);
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecStatusMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecListMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", subMenuTecListChange_6);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecListMain2);

void MenuTecSetWinMain(void)
{
    MenuTecSetWinWork *work = MenuTecSetWin;
    int i;
    short slide;
    int j;

    /* Fills the three rows from the technique under the cursor. */
    void subListChange(void)
    {
        work->row[0].number[0].value = MenuTecSaveDataGet(MenuWork.characterNo)->level[MenuWork.point];
        work->row[1].number[0].format = 10;
        if (MenuTecSpeedLimitCheck(MenuWork.characterNo, MenuWork.point) != 0) {
            work->row[1].number[0].value = (int)msg02_9[0];
        } else {
            work->row[1].number[0].value = (int)msg02_9[1];
        }
        work->row[2].number[0].format = 10;
        work->row[2].number[0].value = (int)msg03_10[MenuTecSaveDataGet(MenuWork.characterNo)->wait[MenuWork.point]];
        if (MenuTecTLevLimitCheck(MenuWork.characterNo, MenuWork.point) != 0) {
            work->row[0].number[1].format = 0;
            work->row[0].number[1].value = MenuTecNextTLevPointGet(MenuWork.characterNo, MenuWork.point);
        } else {
            work->row[0].number[1].format = 9;
        }
        if (MenuTecSpeedLimitCheck(MenuWork.characterNo, MenuWork.point) != 0) {
            work->row[1].number[1].format = 0;
            work->row[1].number[1].value = MenuTecNextSpeedPointGet(MenuWork.characterNo, MenuWork.point);
        } else {
            work->row[1].number[1].format = 9;
        }
        if (MenuTecWaitLimitCheck(MenuWork.characterNo, MenuWork.point) != 0) {
            work->row[2].number[1].format = 0;
            work->row[2].number[1].value = MenuTecNextWaitPointGet(MenuWork.characterNo, MenuWork.point);
        } else {
            work->row[2].number[1].format = 9;
        }
        if (MenuTecTLevLimitCheck(MenuWork.characterNo, MenuWork.point) != 0 && MenuTecCharTLevUpCheck() != 0) {
            work->row[0].label[1].text = msg01_8_0036DCB8[0];
        } else {
            work->row[0].label[1].text = 0;
        }
        if (MenuTecSpeedLimitCheck(MenuWork.characterNo, MenuWork.point) != 0 && MenuTecCharSpeedUpCheck() != 0) {
            work->row[1].label[1].text = msg01_8_0036DCB8[0];
        } else {
            work->row[1].label[1].text = 0;
        }
        if (MenuTecWaitLimitCheck(MenuWork.characterNo, MenuWork.point) != 0 && MenuTecCharWaitUpCheck() != 0) {
            work->row[2].label[1].text = msg01_8_0036DCB8[0];
        } else {
            work->row[2].label[1].text = 0;
        }
        for (i = 0; i < 3; i++) {
            work->row[i].label[1].color[0] = 96;
            work->row[i].label[1].color[2] = 96;
            work->row[i].label[1].color[1] = -128;
        }
    }

    switch (work->state) {
    case 0:
        work->color = 0x00FFFFF8;
        WindowDXSet(&work->window);
        work->window.x = 528;
        work->window.y = 144;
        work->window.color = work->color;
        work->window.width = 240;
        work->window.height = 64;
        work->window.title = "Set\003,Lv\003\022T.Pts";
        work->window.state = 1;
        WindowDXMain(&work->window);
        work->window.state = 3;
        for (i = 0; i < 3; i++) {
            eTagFontSet(&work->row[i].label[0], msg00_7[i]);
            eTagFontSet(&work->row[i].label[1], 0);
            eNumberSet(&work->row[i].number[0], 0);
            eNumberSet(&work->row[i].number[1], 0);
            work->row[i].number[0].digits = 2;
            work->row[i].number[1].digits = 4;
            for (j = 0; j < 2; j++) {
                work->row[i].label[j].drawOrder = work->color + 2;
                work->row[i].number[j].drawOrder = work->color + 2;
                work->row[i].number[j].format = 0;
                work->row[i].number[j].value = 0;
            }
        }
        eTagFontSet(&work->footer, 0);
        eCursolSet(&work->cursor, 0);
        work->cursor.mode = 0;
        work->cursor.drawOrder = work->color + 2;
        work->flags = 0;
        work->state = 2;
        /* fallthrough */
    case 2:
        slide = 528;
        switch (MenuWork.state) {
        case 0x32:
            if (MenuWork.resetFlags & 1) {
                work->cursor.mode = 0;
                work->window.y = 290;
                work->window.width = 216;
            }
            subListChange();
            slide = 288;
            break;
        case 0x50:
        case 0x56:
            if (MenuWork.resetFlags & 1) {
                work->cursor.mode = 0;
                work->window.y = 144;
                work->window.width = 240;
            }
            subListChange();
            slide = 264;
            break;
        case 0x52:
        case 0x54:
            if (MenuWork.resetFlags & 1) {
                eCursolModeChange(&work->cursor, 32);
                work->window.y = 144;
            }
            subListChange();
            slide = 264;
            break;
        default:
            work->cursor.mode = 0;
            break;
        }
        MoveSlide(&work->window.x, &slide, 3.0f);
        WindowDXMain(&work->window);
        for (i = 0; i < 3; i++) {
            work->row[i].label[0].x = work->window.x + 67;
            work->row[i].label[0].y = work->window.y + i * 18 + 6;
            eTagFontMain(&work->row[i].label[0]);
            work->row[i].number[0].x = work->window.x + 75;
            work->row[i].number[0].y = work->window.y + i * 18 + 6;
            eNumberMain(&work->row[i].number[0]);
            work->row[i].number[1].x = work->window.x + 105;
            work->row[i].number[1].y = work->window.y + i * 18 + 6;
            eNumberMain(&work->row[i].number[1]);
            work->row[i].label[1].x = work->window.x + 157;
            work->row[i].label[1].y = work->window.y + i * 18 + 6;
            eTagFontMain(&work->row[i].label[1]);
        }
        work->cursor.x = work->window.x + 3;
        work->cursor.y = work->window.y + MenuWork.rowCursor * 18 + 6;
        eCursolMain(&work->cursor);
        break;
    }
}

void MenuTecSortSet00(void)
{
    int *sortRow = MenuSortAddrGet(0);
    struct {
        short first;
        short last;
    } range[7];
    int tecNo;

    __builtin_memcpy(range, D_004C92E8, sizeof(range));
    for (tecNo = range[MenuWork.characterNo - 1].first; tecNo <= range[MenuWork.characterNo - 1].last; tecNo++) {
        if (func_A197E8(MenuWork.characterNo, tecNo) != 0)
            *sortRow++ = 0xA0000 + (tecNo & 0xFFFF);
    }
    *sortRow = 0;
}

int MenuTecEquipCheck(int chrNo, int tecNo)
{
    int maskedTecNo = tecNo & 0xFFFF;
    MenuTecEquipRecord *equip = func_A191C0(chrNo & 0xFFFF);
    int count = 0;
    int i;

    for (i = 0; i < 6; i++) {
        if (equip->techniqueId[i] == maskedTecNo)
            count++;
    }

    return count;
}

int MenuTecTrgCheck(int tecBit)
{
    return (func_A1A378(tecBit & 0xFFFF)[0] & 3) != 0;
}

int MenuTecTypeCheck(int chrNo, int bit)
{
    int tecNo;

    chrNo = chrNo & 0xFFFF;
    tecNo = BitToTecNo(bit);
    return ((unsigned char *)MenuTecSaveDataGet(chrNo) + tecNo)[8] == 1;
}

void MenuTecListMake00(void)
{
    short *techniqueIds;
    int sortCount;
    struct MenuTecListRow *rows;

    rows = MenuListGet(0);
    techniqueIds = MenuSortAddrGet(0);
    sortCount = MenuSortCheck(0);
    MenuListMake(0, 0);
    if (sortCount > 0) {
        int remaining = sortCount;

        do {
            short techniqueId = *techniqueIds;
            int equippedCount;

            techniqueIds += 2;
            remaining--;
            equippedCount = MenuTecEquipCheck(MenuWork.characterNo, techniqueId);
            rows->flag = 0;
            rows->amount = equippedCount;
            rows++;
        } while (remaining != 0);
    }
}

void MenuTecListMake00_2(void)
{
    MenuTecListRow *rows = MenuListGet(0);
    int *sortRow = MenuSortAddrGet(0);
    int count = MenuSortCheck(0);
    int lowMode = MenuWork.mode < 2;
    int i;

    for (i = 0; i < count; i++) {
        int learned = MenuTecTypeCheck(MenuWork.characterNo, sortRow[i]);

        if (lowMode == learned || learned == 1) {
            const unsigned char *table = MenuWork.characterNo == 7 ? MenuTecTrgTbl01 : MenuTecTrgTbl00;
            unsigned char wanted = table[MenuWork.mode];

            if (wanted == MenuTecTrgCheck(sortRow[i]))
                rows[i].flag = 0;
            else
                rows[i].flag = 1;
        } else {
            rows[i].flag = 1;
        }
    }
}

void MenuTecListMake01(void)
{
    MenuTecListRow *rows = MenuListGet(0);
    int *sortRow = MenuSortAddrGet(0);
    int count = MenuSortCheck(0);
    signed char savedBit = MenuWork.tecBit;
    signed char savedPoint = MenuWork.point;
    int i;

    MenuListMake(0, 0);
    for (i = 0; i < count; i++) {
        rows[i].amount = MenuTecEquipCheck(MenuWork.characterNo, (short)sortRow[i]);
        MenuWork.tecBit = sortRow[i];
        MenuWork.point = BitToTecNo(MenuWork.tecBit);
        if (MenuTecCharTLevUpCheck() != 0 || MenuTecCharWaitUpCheck() != 0)
            rows[i].flag = 0;
        else if (MenuTecCharSpeedUpCheck() != 0)
            rows[i].flag = 0;
        else
            rows[i].flag = 1;
    }
    MenuWork.tecBit = savedBit;
    MenuWork.point = savedPoint;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTec);

/* Defined after MenuTec: the original .sdata holds it at 0x004DB518, after
 * every string literal of the TU's functions (MenuTec's ends at 0x004DB517). */
MenuTecData *MenuTecDataBuf = 0;