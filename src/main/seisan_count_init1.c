#include "common.h"
#include "shared.h"
#include "sdv.h"
#include "main/xgl_task.h"
#include "main/xgl_font.h"
#include "main/xgl_studio.h"
#include "main/xgl_packet.h"
#include "seisan_count_init1.h"

typedef struct SeisanLabelBytes {
    unsigned char bytes[5];
} SeisanLabelBytes;

typedef union SeisanNumberCoordinates {
    struct { short x, y; } axes;
    unsigned int packed;
} SeisanNumberCoordinates;

/* Original tu171 digit literals: each is an eight-byte character item.
 * tu172's original readers copy the first aligned halfword into a label.
 * The casts in msg_number express that existing reader view; these declarations
 * do not recover or allocate storage belonging to tu171. */
extern const char SeisanDigit0Text[8];

extern const char SeisanDigit1Text[8];
extern const char SeisanDigit2Text[8];
extern const char SeisanDigit3Text[8];
extern const char SeisanDigit4Text[8];
extern const char SeisanDigit5Text[8];
extern const char SeisanDigit6Text[8];
extern const char SeisanDigit7Text[8];
extern const char SeisanDigit8Text[8];
extern const char SeisanDigit9Text[8];
extern const char SeisanPartyText[8];
extern const char SeisanClearTimeText[16];
extern const char SeisanMoneyText[8];
extern const char SeisanNextHintText[8];
extern const char SeisanEndHintText[8];
extern const char SeisanItemAcquiredText[16];
extern const char SeisanNoItemsText[16];
extern const char SeisanLevelUpText[16];
extern const char SeisanNewAttackText[16];
extern const char SeisanNewEtherText[16];
extern const char seisanStartPointsText[7];
extern const char seisanEndPointsText[7];
extern const char seisanTotalPointsText[7];

/* The original eight-byte prompt-pointer table remains in data scaffolding. */
extern const char *msg00_1_0036DAC8[];

static void tskSeisanStatus(TskObject *task, void *data);

const char combatantTitleText[10] = "Combatant";

const SeisanRibbonData seisanRibbonTable = {
    { 0x00000000, 0x0000FFFF, 0x80000000, 0x01C00000,
      0x0000FFFF, 0x80000000, 0x00000200, 0x0000FFFF,
      0x80000000, 0x01C00200, 0x0000FFFF, 0x80000000,
      0x00000000, 0x00000000, 0x00000000 }
};

static const unsigned short *msg_number[10] = {
    (const unsigned short *)SeisanDigit0Text,
    (const unsigned short *)SeisanDigit1Text,
    (const unsigned short *)SeisanDigit2Text,
    (const unsigned short *)SeisanDigit3Text,
    (const unsigned short *)SeisanDigit4Text,
    (const unsigned short *)SeisanDigit5Text,
    (const unsigned short *)SeisanDigit6Text,
    (const unsigned short *)SeisanDigit7Text,
    (const unsigned short *)SeisanDigit8Text,
    (const unsigned short *)SeisanDigit9Text
};

static const char *msg00_0_0036DAB8[3] = {
    SeisanPartyText, SeisanClearTimeText, SeisanMoneyText
};

static const char *msg00_1_0036DAC8[] = {
    SeisanNextHintText, SeisanEndHintText
};

static const char *msg00_2_0036DAD0[] = {
    SeisanItemAcquiredText, SeisanNoItemsText
};

static const char *msg00_3_0036DAD8[4] = {
    seisanTotalPointsText, seisanEndPointsText, seisanStartPointsText, SeisanLevelUpText
};

static const char *msg01_4_0036DAE8[] = {
    SeisanNewAttackText, SeisanNewEtherText
};

static SeisanCountData SeisanCN;

static inline int SeisanLearnedCount(int partySlot)
{
    const SeisanHissatuEntry *results = SeisanResult->entries;
    return results[partySlot].count;
}

static inline int SeisanLearnedId(const SeisanHissatuEntry *results,
                                  int partySlot, int learnedIndex)
{
    return results[partySlot].learnedIds[learnedIndex];
}

static inline int SeisanRewardCharacter(int partySlot)
{
    const SeisanHissatuEntry *results = SeisanResult->entries;
    return results[partySlot].characterNo;
}

static inline SeisanHissatuEntry *SeisanRewardRow(int partySlot,
                                                int (*results)[15])
{
    return (void *)&results[partySlot][0];
}

static inline SeisanControlState *SeisanControlGet(void)
{
    return (void *)SeisanWork;
}

static inline void SeisanItemBoxClear(SeisanItemEntry *item)
{
    int itemSlot;
    for (itemSlot = 0; itemSlot < 8; itemSlot++) {
        item->itemId = 0;
        item->count = 0;
        item++;
    }
}

void SeisanCountInit1(void)
{
    int chrNo;

    memset(&SeisanCN, 0, sizeof(SeisanCN));
    for (chrNo = 1; chrNo < 13; chrNo++)
    {
        SeisanCN.nextExp[chrNo - 1] = func_A19210(chrNo)->nextExp;
    }
}

static inline void SeisanRollbackStats(SeisanUnitRecord *unit,
                                      SeisanCharRecord *character,
                                      const SeisanHissatuEntry *row)
{
    unit->hp -= row->hp;
    unit->ep -= row->ep;
    character->techPoints -= row->techPoints;
    character->skillPoints -= row->skillPoints;
    character->specialPoints -= row->specialPoints;
    unit->level -= row->level;
}

void SeisanCountInit2(void)
{
    int i;
    SeisanHissatuEntry *result = SeisanResult->entries;

    SeisanCN.state = 0;
    for (i = 0; i < 3; i++) {
        int characterNo = result[i].characterNo;
        if ((unsigned int)(characterNo - 1) < 12) {
            SeisanUnitRecord *unit = func_A191C0(characterNo);
            SeisanCharRecord *character = func_A19210(characterNo);

            SeisanCN.targets[i].level = unit->level;
            SeisanCN.targets[i].hp = unit->hp;
            SeisanCN.targets[i].ep = unit->ep;
            {
                SeisanCN.targets[i].experience = character->experience;
                SeisanCN.targets[i].nextExp = character->nextExp;
                SeisanCN.targets[i].techPoints = character->techPoints;
                SeisanCN.targets[i].skillPoints = character->skillPoints;
                SeisanCN.targets[i].specialPoints = character->specialPoints;
                SeisanRollbackStats(unit, character, &SeisanResult->entries[i]);
                {
                    SeisanResultData *freshResults = SeisanResult;
                    result = freshResults->entries;
                    character->experience -= (freshResults->entries + i)->experience;
                }
                character->nextExp = SeisanCN.nextExp[characterNo - 1];
            }
        }
    }
    SeisanCN.countedMoney = 0;
}

int SeisanNumberCount(int current, int target, int step, int *moving)
{
  int overshot;
  int next;
  next = current;
  if (next != target)
  {
    if (next < target)
    {
      next += step;
      overshot = target < next;
    }
    else
    {
      next -= step;
      overshot = next < target;
    }
    if (overshot)
    {
      next = target;
    }
    else
    {
      *moving = 1;
    }
  }
  return next;
}

void SeisanCountMain(int mode)
{
    const BattleExperienceAndParameters *levelData =
        (const BattleExperienceAndParameters *)func_A19338(0);
    int moving = 0;
    int i;
    SeisanHissatuEntry *entries;
    SeisanUnitRecord *unit;
    SeisanCharRecord *character;
    int characterNo;

    switch (SeisanCN.state) {
    case 0:
    case 3:
        xglSoundEffectStopID(14, 0);
        return;
    case 1:
        for (i = 0; i < 3; i++) {
            entries = SeisanResult->entries;
            characterNo = entries[i].characterNo;
            if ((unsigned int)(characterNo - 1) < 12) {
                unit = func_A191C0(characterNo);
                character = func_A19210(characterNo);

                unit->hp = SeisanNumberCount(unit->hp, SeisanCN.targets[i].hp, 1, &moving);
                unit->ep = SeisanNumberCount(unit->ep, SeisanCN.targets[i].ep, 1, &moving);
                character->techPoints = SeisanNumberCount(character->techPoints, SeisanCN.targets[i].techPoints, 1, &moving);
                character->skillPoints = SeisanNumberCount(character->skillPoints, SeisanCN.targets[i].skillPoints, 1, &moving);
                character->specialPoints = SeisanNumberCount(character->specialPoints, SeisanCN.targets[i].specialPoints, 1, &moving);
                character->experience = SeisanNumberCount(character->experience, SeisanCN.targets[i].experience, 2, &moving);
                if (unit->level != SeisanCN.targets[i].level) {
                    character->nextExp -= 2;
                    if (character->nextExp <= 0) {
                        unit->level = SeisanNumberCount(unit->level, SeisanCN.targets[i].level, 1, &moving);
                        /* At level 99 the original reads the first parameter pair,
                         * immediately after the 99 real experience entries. */
                        character->nextExp = ((const BattleDataS32 *)
                            &((const unsigned char *)levelData)[4 * unit->level])->value +
                            character->nextExp;
                        xglSoundEffectNormalID(4, 0);
                    }
                } else {
                    character->nextExp = SeisanNumberCount(character->nextExp, SeisanCN.targets[i].nextExp, 2, &moving);
                }
            }
        }
        if (SeisanMg->money != SeisanCN.countedMoney) {
            SeisanCN.countedMoney += 5;
            if (SeisanCN.countedMoney >= SeisanMg->money)
                SeisanCN.countedMoney = SeisanMg->money;
            else
                moving = 1;
        }
        xglSoundEffectNormalID(14, 0);
        if (!moving)
            SeisanCN.state = 3;
        break;
    case 2:
        for (i = 0; i < 3; i++) {
            entries = SeisanResult->entries;
            characterNo = entries[i].characterNo;
            if ((unsigned int)(characterNo - 1) < 12 && entries[i].level != 0) {
                xglSoundEffectNormalID(4, 0);
                break;
            }
        }
        for (i = 0; i < 3; i++) {
            entries = SeisanResult->entries;
            characterNo = entries[i].characterNo;
            if ((unsigned int)(characterNo - 1) < 12) {
                unsigned short restoredHP;
                unsigned short restoredEP;
                unit = func_A191C0(characterNo);
                character = func_A19210(characterNo);

                restoredHP = SeisanCN.targets[i].hp;
                restoredEP = SeisanCN.targets[i].ep;
                character->techPoints = SeisanCN.targets[i].techPoints;
                unit->hp = restoredHP;
                character->skillPoints = SeisanCN.targets[i].skillPoints;
                unit->ep = restoredEP;
                character->specialPoints = SeisanCN.targets[i].specialPoints;
                character->experience = SeisanCN.targets[i].experience;
                character->nextExp = SeisanCN.targets[i].nextExp;
                unit->level = SeisanCN.targets[i].level;
            }
        }
        SeisanCN.countedMoney = SeisanMg->money;
        xglSoundEffectNormalID(14, 0);
        SeisanCN.state = 3;
        break;
    }
}

void TskObjectSet2(TskObject *task, TskObjectWorker worker, void *data)
{
    task->data = data;
    task->worker = worker;
    task->state = 0;
}

void tskTskMain2(TskObject *task)
{
    TskObjectWorker worker = task->worker;

    if (SeisanWork[1] == 0xff) {
        xglTaskWaitRemove(&task->base);
        return;
    }

    if (task->state != 0) {
        if (task->state != 2)
            return;
    } else {
        worker(task, task->data);
        task->state = 2;
    }

    worker(task, task->data);
}

static void SeisanTimeEx(TskObject *task, SeisanTimePanel *panel)
{
    XglClock elapsed;
    short targetY;
    int i;

    switch (task->state) {
    case 0:
        panel->depth = 0xfffff0;
        WindowDXSet(&panel->window);
        panel->window.x = 16;
        panel->window.y = -64;
        panel->window.depth = panel->depth;
        panel->window.width = 480;
        panel->window.height = 32;
        panel->window.title = msg00_0_0036DAB8[0];
        panel->window.state = 1;
        WindowDXMain(&panel->window);
        panel->window.state = 3;
        for (i = 0; i < 2; i++) {
            eTagFontSet(&panel->tags[i], msg00_0_0036DAB8[i + 1]);
            panel->tags[i].depth = panel->depth + 2;
        }
        for (i = 0; i < 3; i++) {
            eNumberSet(&panel->numbers[i], 0);
            panel->numbers[i].depth = panel->depth + 2;
        }
        xglClockUInt2DayTime(&elapsed, func_A22908() / 30);
        PartyTimeLimitCheck(&elapsed);
        PartyTimeDispChange(&elapsed);
        panel->numbers[0].option = 2;
        /* The number receives the first four display-clock bytes as one word. */
        __builtin_memcpy(&panel->numbers[0].value, &elapsed, sizeof(int));
        {
            int money = dataMoneyBoxChk();
            panel->numbers[1].digits = 9;
            panel->numbers[1].option = 0;
            panel->numbers[1].value = money;
        }
        panel->numbers[2].value = SeisanMg->money;
        panel->numbers[2].digits = panel->numbers[1].digits;
        panel->numbers[2].option = 7;
        for (i = 0; i < 2; i++) {
            eSpriteSet(&panel->sprites[i], i + 0x707);
            panel->sprites[i].depth = panel->depth + 2;
        }
        break;
    case 2: {
        SeisanControlState *control = (SeisanControlState *)SeisanWork;
        targetY = 32;
        if (control->state == SEISAN_STATE_CLOSING)
            targetY = -64;
        MoveSlide(&panel->window.y, &targetY, 3.0f);
        WindowDXMain(&panel->window);
        panel->numbers[0].x = panel->window.x + 91;
        panel->numbers[0].y = panel->window.y + 8;
        eNumberMain(&panel->numbers[0]);
        panel->tags[0].x = panel->numbers[0].x - 8;
        panel->tags[0].y = panel->window.y + 8;
        eTagFontMain(&panel->tags[0]);
        panel->numbers[1].x = panel->window.x + 238;
        panel->numbers[1].y = panel->window.y + 8;
        panel->numbers[1].value = dataMoneyBoxChk() - SeisanMg->money + SeisanCN.countedMoney;
        eNumberMain(&panel->numbers[1]);
        panel->tags[1].x = panel->numbers[1].x - 8;
        panel->tags[1].y = panel->window.y + 8;
        eTagFontMain(&panel->tags[1]);
        panel->sprites[0].x = panel->window.x + 340;
        panel->sprites[0].y = panel->numbers[2].y = panel->window.y + 8;
        eSpriteMain(&panel->sprites[0]);
        panel->numbers[2].x = panel->window.x + 350;
        panel->numbers[2].value = SeisanCN.countedMoney;
        panel->numbers[2].y = panel->window.y + 8;
        eNumberMain(&panel->numbers[2]);
        panel->sprites[1].x = panel->window.x + 461;
        panel->sprites[1].y = panel->numbers[2].y = panel->window.y + 8;
        eSpriteMain(&panel->sprites[1]);
        break;
    }
    }
}

static void tskSeisanButton(TskObject *task, SeisanButtonWork *button)
{
    short targetY;

    switch (task->state) {
    case 0: {
        button->color = 0xfffff0;
        eSpriteSet(&button->sprite, 0x200);
        button->sprite.x = -0x70;
        button->sprite.y = 0x1a4;
        button->sprite.color = button->color;
        eMessageSet(&button->message, msg00_1_0036DAC8[0]);
        button->message.mode = 0x20;
        button->message.x = button->sprite.x + 0x14;
        button->message.y = button->sprite.y;
        button->message.color = button->color;
        break;
    }
    case 2: {
        unsigned char *work = SeisanWork;
        unsigned char flags = work[SEISAN_WORK_FLAGS];
        targetY = 0x180;
        if (flags & 4)
            button->message.text = msg00_1_0036DAC8[1];
        if (work[SEISAN_WORK_STATE] == SEISAN_STATE_CLOSING)
            targetY = 0x220;
        MoveSlide(&button->sprite.x, &targetY, 3.0f);
        eSpriteMain(&button->sprite);
        button->message.x = button->sprite.x + 0x14;
        button->message.y = button->sprite.y;
        eMessageMain(&button->message);
        break;
    }
    }
}

static void tskSeisanItem(TskObject *task, void *data)
{
    SeisanItemPanel *panel = data;
    unsigned short ordinalText[3];
    unsigned short quantityText[3];
    short targetX;
    int i;

    switch (task->state) {
    case 0:
        panel->active = 0;
        panel->itemCount = 0;
        panel->depth = 0xfffff0;
        for (i = 0; i < SeisanMg->itemCount; i++) {
            if (SeisanItemBox[i].count != 0)
                panel->itemCount++;
        }
        panel->windows[0].y = 128;
        panel->windows[0].height = 32;
        panel->windows[1].y = 160;
        panel->windows[1].height = panel->itemCount * 24 + 6;
        for (i = 0; i < 2; i++) {
            WindowDXSet(&panel->windows[i]);
            panel->windows[i].x = 832;
            panel->windows[i].width = 288;
            panel->windows[i].depth = panel->depth;
            panel->windows[i].state = 1;
            WindowDXMain(&panel->windows[i]);
            panel->windows[i].state = 3;
        }
        if (panel->itemCount)
            eMessageSet(&panel->title, msg00_2_0036DAD0[0]);
        else
            eMessageSet(&panel->title, msg00_2_0036DAD0[1]);
        panel->title.mode = 32;
        panel->title.color = panel->depth + 2;
        for (i = 0; i < panel->itemCount; i++) {
            eMessageSet(&panel->items[i], MenuTextGet(SeisanItemBox[i].itemId)->name);
            panel->items[i].mode = 32;
            panel->items[i].color = panel->depth + 2;
        }
        break;
    case 2:
        targetX = 112;
        switch (SeisanWork[SEISAN_WORK_STATE]) {
        case 0x20:
        case 0x30:
        case 0x50:
        case 0x58:
        case 0x60:
            targetX = 832;
            break;
        case 0x80:
            panel->active = 1;
            break;
        case SEISAN_STATE_CLOSING:
            targetX = -320;
            break;
        }
        if (panel->active) {
            MoveSlide(&panel->windows[0].x, &targetX, 3.0f);
            WindowDXMain(&panel->windows[0]);
            panel->title.x = panel->windows[0].x + 64;
            panel->title.y = panel->windows[0].y + 3;
            if (panel->itemCount == 0)
                panel->title.x += 20;
            eMessageMain(&panel->title);
            if (panel->itemCount) {
                panel->windows[1].x = panel->windows[0].x;
                WindowDXMain(&panel->windows[1]);
                for (i = 0; i < panel->itemCount; i++) {
                    const unsigned short *ordinalSource;
                    const unsigned short *quantitySource;
                    unsigned short ordinalGlyph;
                    unsigned short quantityGlyph;
                    int printX;
                    int printY;
                    panel->items[i].x = panel->windows[1].x + 43;
                    panel->items[i].y = panel->windows[1].y + i * 24 + 3;
                    eMessageMain(&panel->items[i]);
                    /* Two-byte glyph pairs followed by the original terminating byte. */
                    ordinalSource = msg_number[i + 1];
                    printY = panel->windows[1].y + i * 24 + 2;
                    printX = panel->windows[1].x + 3;
                    ordinalGlyph = ordinalSource[0];
                    __builtin_memcpy(ordinalText, seisanOrdinalPrefixText, 5);
                    ordinalText[0] = ordinalGlyph;
                    xglFontPrint(printX, printY, 0xffffff, (const char *)ordinalText);
                    quantitySource = msg_number[SeisanItemBox[i].count];
                    printY = panel->windows[1].y + i * 24 + 2;
                    printX = panel->windows[1].x + 221;
                    *(SeisanLabelBytes *)quantityText = *(const SeisanLabelBytes *)seisanQuantityMarkText;
                    quantityGlyph = quantitySource[0];
                    quantityText[1] = quantityGlyph;
                    xglFontPrint(printX, printY, 0xffffff, (const char *)quantityText);
                }
            }
        }
        break;
    }
}

static void tskSeisanStatus(TskObject *task, void *data)
{
    SeisanStatusPanel *panel = data;
    short targetX;
    int i;
    int columnX;
    int baselineY;
    SeisanHissatuEntry *entries;

    switch (task->state) {
    case 0:
        panel->enabled = 0;
        panel->displayFlags = 1;
        panel->depth = 0xfffff0;
        {
            SeisanHissatuEntry *initialEntries = SeisanResult->entries;
            if ((unsigned int)(initialEntries[panel->partySlot].characterNo - 1) >= 12) {
                panel->state = 3;
                break;
            }
        }
        panel->enabled = 1;
        WindowDXSet(&panel->window);
        panel->window.x = 1024;
        panel->window.ribbonInitialized = 1;
        panel->window.depth = panel->depth;
        panel->window.width = 480;
        panel->window.height = 99;
        panel->window.y = panel->partySlot * 116 + 88;
        panel->window.title = combatantTitleText;
        panel->window.callback = MenuStatusDisp;
        panel->window.callbackArgument = &panel->profile;
        panel->profile.mode = -1;
        panel->profile.x = 0;
        panel->profile.y = 0;
        {
            SeisanHissatuEntry *profileEntries = SeisanResult->entries;
            panel->profile.characterNo = profileEntries[panel->partySlot].characterNo;
        }
        panel->window.state = 1;
        panel->profile.flags = 0;
        WindowDXMain(&panel->window);
        panel->window.state = 3;
        for (i = 0; i < 10; i++) {
            eNumberSet(&panel->numbers[i], 0);
            panel->numbers[i].depth = 0xffffff;
        }
        for (i = 0; i < 4; i++) {
            eTagFontSet(&panel->tags[i], msg00_3_0036DAD8[i]);
            panel->tags[i].depth = 0xffffff;
            if (i == 3)
                continue;
            panel->tags[i].red = 112;
        }
        eMessageSet(&panel->learnedTitle, 0);
        panel->learnedTitle.color = 0xffffff;
        panel->learnedTitle.mode = 32;
        for (i = 0; i < 6; i++) {
            eMessageSet(&panel->learned[i], 0);
            panel->learned[i].color = 0xffffff;
            panel->learned[i].mode = 32;
        }
        for (i = 0; i < 14; i++) {
            panel->sprites[i].uv = (i & 1) + 0x707;
            endSpriteSet(&panel->sprites[i], 255);
            panel->sprites[i].depth = 0xffffff;
        }
        panel->learnedVisible = 0;
        panel->levelFlashTimer = 0;
        {
            SeisanHissatuEntry *levelEntries = SeisanResult->entries;
            panel->previousLevel = func_A191C0(levelEntries[panel->partySlot].characterNo)->level;
        }
        break;
    case 2: {
        SeisanUnitRecord *unit;
        SeisanCharRecord *character;
        SeisanTimeWindow *window;

        if (!panel->enabled)
            break;
        targetX = 16;
        {
            SeisanHissatuEntry *unitEntries = SeisanResult->entries;
            unit = func_A191C0(unitEntries[panel->partySlot].characterNo);
        }
        {
            unsigned int partySlot = panel->partySlot;
            SeisanHissatuEntry *characterEntries = SeisanResult->entries;
            character = func_A19210(characterEntries[partySlot].characterNo);
        }
        if (panel->levelFlashTimer)
            panel->levelFlashTimer--;
        switch (SeisanWork[SEISAN_WORK_STATE]) {
        case 0x20:
            if (panel->previousLevel != unit->level) {
                panel->previousLevel = unit->level;
                panel->levelFlashTimer = 32;
            }
            break;
        case 0x50:
        case 0x58: {
            if (SeisanLearnedCount(panel->partySlot) != 0) {
                const char *learnedTitle = msg01_4_0036DAE8[0];
                panel->profile.mode = -2;
                panel->displayFlags = 16;
                panel->learnedTitle.text = learnedTitle;
                for (i = 0; i < SeisanLearnedCount(panel->partySlot); i++) {
                    panel->learned[i].text = func_A2C7D8(
                        SeisanLearnedId(SeisanResult->entries, panel->partySlot, i))->name;
                }
                for (; i < 6; i++)
                    panel->learned[i].text = 0;
                panel->learnedVisible = 1;
            } else {
                targetX = -512;
            }
            break;
        }
        case 0x60:
            panel->learnedTitle.text = msg01_4_0036DAE8[1];
            panel->displayFlags = 16;
            panel->profile.mode = 0;
            break;
        case 0x80:
        case SEISAN_STATE_CLOSING:
            targetX = -512;
            break;
        }
        window = &panel->window;
        MoveSlide(&window->x, &targetX, 3.0f);
        WindowDXMain(window);
        if (panel->displayFlags & 1) {
            {
                columnX = panel->window.x + 228;
                baselineY = panel->window.y + 31;

                panel->sprites[0].x = columnX - 11;
                panel->sprites[0].y = baselineY - 1;
                endPrintExtFunc(panel->depth, 2, &panel->sprites[0]);
                panel->numbers[0].x = columnX;
                ((SeisanNumberCoordinates *)(void *)&panel->numbers[0])->axes.y = baselineY + 1;
                entries = SeisanResult->entries;
                panel->numbers[0].value = entries[panel->partySlot].hp -
                    (SeisanCN.targets[panel->partySlot].hp - unit->hp);
                if (panel->numbers[0].value != 0) {
                    panel->numbers[0].digits = 4;
                    panel->numbers[0].option = 8;
                } else {
                    panel->numbers[0].digits = 5;
                    panel->numbers[0].option = 9;
                }
                eNumberMain(&panel->numbers[0]);
                panel->sprites[1].x = columnX + 54;
                panel->sprites[1].y = baselineY - 1;
                endPrintExtFunc(panel->depth, 2, &panel->sprites[1]);
            }
            {
                columnX = panel->window.x + 228;
                baselineY = panel->window.y + 47;

                panel->sprites[2].x = columnX - 11;
                panel->sprites[2].y = baselineY - 1;
                endPrintExtFunc(panel->depth, 2, &panel->sprites[2]);
                panel->numbers[1].x = columnX;
                panel->numbers[1].y = baselineY + 1;
                entries = SeisanResult->entries;
                panel->numbers[1].value = entries[panel->partySlot].ep -
                    (SeisanCN.targets[panel->partySlot].ep - unit->ep);
                if (panel->numbers[1].value != 0) {
                    panel->numbers[1].digits = 2;
                    panel->numbers[1].option = 8;
                } else {
                    panel->numbers[1].digits = 3;
                    panel->numbers[1].option = 9;
                }
                eNumberMain(&panel->numbers[1]);
                panel->sprites[3].x = columnX + 34;
                panel->sprites[3].y = baselineY - 1;
                endPrintExtFunc(panel->depth, 2, &panel->sprites[3]);
            }
            panel->tags[0].x = panel->window.x + 344;
            panel->tags[0].y = panel->window.y + 30;
            eTagFontMain(&panel->tags[0]);
            panel->numbers[2].x = panel->window.x + 352;
            panel->numbers[2].value = character->techPoints;
            panel->numbers[2].y = panel->window.y + 32;
            panel->numbers[2].option = 1;
            panel->numbers[2].digits = 4;
            eNumberMain(&panel->numbers[2]);
            panel->tags[1].x = panel->window.x + 344;
            panel->tags[1].y = panel->window.y + 46;
            eTagFontMain(&panel->tags[1]);
            panel->numbers[3].x = panel->window.x + 352;
            panel->numbers[3].value = character->skillPoints;
            panel->numbers[3].y = panel->window.y + 48;
            panel->numbers[3].option = 1;
            panel->numbers[3].digits = 4;
            eNumberMain(&panel->numbers[3]);
            panel->tags[2].x = panel->window.x + 344;
            panel->tags[2].y = panel->window.y + 62;
            eTagFontMain(&panel->tags[2]);
            panel->numbers[4].x = panel->window.x + 352;
            panel->numbers[4].y = panel->window.y + 64;
            panel->numbers[4].value = character->specialPoints;
            panel->numbers[4].option = 1;
            panel->numbers[4].digits = 4;
            eNumberMain(&panel->numbers[4]);
            {
                columnX = panel->window.x + 409;
                baselineY = panel->window.y + 31;

                panel->sprites[4].x = columnX - 11;
                panel->sprites[4].y = baselineY - 1;
                endPrintExtFunc(panel->depth, 2, &panel->sprites[4]);
                panel->numbers[5].x = columnX;
                panel->numbers[5].y = baselineY + 1;
                entries = SeisanResult->entries;
                panel->numbers[5].value = entries[panel->partySlot].techPoints -
                    (SeisanCN.targets[panel->partySlot].techPoints - character->techPoints);
                if (panel->numbers[5].value != 0) {
                    panel->numbers[5].digits = 4;
                    panel->numbers[5].option = 8;
                } else {
                    panel->numbers[5].digits = 5;
                    panel->numbers[5].option = 9;
                }
                eNumberMain(&panel->numbers[5]);
                panel->sprites[5].x = columnX + 54;
                panel->sprites[5].y = baselineY - 1;
                endPrintExtFunc(panel->depth, 2, &panel->sprites[5]);
            }
            {
                columnX = panel->window.x + 409;
                baselineY = panel->window.y + 47;

                panel->sprites[6].x = columnX - 11;
                panel->sprites[6].y = baselineY - 1;
                endPrintExtFunc(panel->depth, 2, &panel->sprites[6]);
                panel->numbers[6].x = columnX;
                panel->numbers[6].y = baselineY + 1;
                entries = SeisanResult->entries;
                panel->numbers[6].value = entries[panel->partySlot].skillPoints -
                    (SeisanCN.targets[panel->partySlot].skillPoints - character->skillPoints);
                if (panel->numbers[6].value != 0) {
                    panel->numbers[6].digits = 4;
                    panel->numbers[6].option = 8;
                } else {
                    panel->numbers[6].digits = 5;
                    panel->numbers[6].option = 9;
                }
                eNumberMain(&panel->numbers[6]);
                panel->sprites[7].x = columnX + 54;
                panel->sprites[7].y = baselineY - 1;
                endPrintExtFunc(panel->depth, 2, &panel->sprites[7]);
            }
            {
                columnX = panel->window.x + 409;
                baselineY = panel->window.y + 63;

                panel->sprites[8].x = columnX - 11;
                panel->sprites[8].y = baselineY - 1;
                endPrintExtFunc(panel->depth, 2, &panel->sprites[8]);
                panel->numbers[7].x = columnX;
                panel->numbers[7].y = baselineY + 1;
                entries = SeisanResult->entries;
                panel->numbers[7].value = entries[panel->partySlot].specialPoints -
                    (SeisanCN.targets[panel->partySlot].specialPoints - character->specialPoints);
                if (panel->numbers[7].value != 0) {
                    panel->numbers[7].digits = 4;
                    panel->numbers[7].option = 8;
                } else {
                    panel->numbers[7].digits = 5;
                    panel->numbers[7].option = 9;
                }
                eNumberMain(&panel->numbers[7]);
                panel->sprites[9].x = columnX + 54;
                panel->sprites[9].y = baselineY - 1;
                endPrintExtFunc(panel->depth, 2, &panel->sprites[9]);
            }
            {
                columnX = panel->window.x + 228;
                baselineY = panel->window.y + 64;

                panel->sprites[10].x = columnX - 11;
                panel->sprites[10].y = baselineY - 1;
                endPrintExtFunc(panel->depth, 2, &panel->sprites[10]);
                panel->numbers[8].x = columnX;
                panel->numbers[8].y = baselineY + 1;
                if (unit->level == 99) {
                    panel->numbers[8].value = -1;
                    panel->numbers[8].option = 1;
                    panel->numbers[8].digits = 6;
                } else {
                    entries = SeisanResult->entries;
                    panel->numbers[8].value = (SeisanCN.targets[panel->partySlot].experience -
                        character->experience) - entries[panel->partySlot].experience;
                    panel->numbers[8].option = 8;
                    panel->numbers[8].digits = 5;
                }
                eNumberMain(&panel->numbers[8]);
                panel->sprites[11].x = columnX + 64;
                panel->sprites[11].y = baselineY - 1;
                endPrintExtFunc(panel->depth, 2, &panel->sprites[11]);
            }
            columnX = panel->window.x + 228;
            baselineY = panel->window.y + 80;
            panel->sprites[12].x = columnX - 11;
            panel->sprites[12].y = baselineY - 1;
            endPrintExtFunc(panel->depth, 2, &panel->sprites[12]);
            panel->numbers[9].x = columnX;
            panel->numbers[9].y = baselineY + 1;
            entries = SeisanResult->entries;
            panel->numbers[9].value = entries[panel->partySlot].experience -
                (SeisanCN.targets[panel->partySlot].experience - character->experience);
            panel->numbers[9].digits = 5;
            panel->numbers[9].option = 8;
            eNumberMain(&panel->numbers[9]);
            panel->sprites[13].x = columnX + 64;
            panel->sprites[13].y = baselineY - 1;
            endPrintExtFunc(panel->depth, 2, &panel->sprites[13]);
            if (panel->levelFlashTimer) {
                panel->tags[3].x = panel->window.x + 306;
                panel->tags[3].y = panel->window.y + 7;
                eTagFontMain(&panel->tags[3]);
            }
        }
        if (SeisanWork[SEISAN_WORK_STATE] == 0x20) {
            panel->separator[0].y = panel->window.y + 28;
            panel->separator[0].x = panel->separator[1].x = panel->window.x + 304;
            panel->separator[1].y = panel->window.y + panel->window.height - 3;
        } else {
            panel->separator[0].y = panel->window.y + 3;
            panel->separator[0].x = panel->separator[1].x = panel->window.x + 238;
            panel->separator[1].y = panel->window.y + panel->window.height - 3;
        }
        panel->separator[0].depth = panel->separator[1].depth = 0xfffffe;
        panel->separator[1].alpha = panel->separator[0].alpha = 128;
        panel->separator[0].red = panel->separator[0].green = panel->separator[0].blue = 192;
        panel->separator[1].red = panel->separator[1].green = panel->separator[1].blue = 192;
        panel->separator[2].depth = 0;
        endPrintExtFunc(panel->depth, 0, panel->separator);
        if (panel->learnedVisible) {
            int topY;
            int rightX;
            int bottomY;
            for (i = 0; i < 4; i++) {
                panel->highlight[i].blue = 0;
                panel->highlight[i].green = 0;
                panel->highlight[i].red = 0;
                panel->highlight[i].alpha = i < 2 ? 64 : 0;
                panel->highlight[i].depth = panel->depth + 2;
            }
            panel->highlight[0].x = panel->highlight[1].x = window->x + 238;
            panel->highlight[i].depth = 0;
            topY = (unsigned short)panel->window.y + 3;
            rightX = (unsigned short)window->x + (unsigned short)panel->window.width - 3;
            bottomY = (unsigned short)panel->window.y + (unsigned short)panel->window.height - 3;
            panel->highlight[2].x = panel->highlight[3].x = rightX;
            panel->highlight[1].y = panel->highlight[3].y = bottomY;
            panel->highlight[0].y = panel->highlight[2].y = topY;
            endPrintExtFunc(panel->depth, 3, panel->highlight);
        }
        if (panel->displayFlags & 16) {
            unsigned short firstLabels[3];
            unsigned short lastLabels[3];

            panel->learnedTitle.x = panel->window.x + 88;
            panel->learnedTitle.y = panel->window.y + 32;
            eMessageMain(&panel->learnedTitle);
            if (SeisanWork[SEISAN_WORK_STATE] == 0x50) {
                for (i = 0; i < 4; i++) {
                    panel->learned[i].x = panel->window.x + 288;
                    panel->learned[i].y = panel->window.y + i * 24 + 3;
                    eMessageMain(&panel->learned[i]);
                    if (panel->learned[i].text) {
                        const unsigned short *ordinalSource = msg_number[i + 1];
                        int printX = panel->learned[i].x - 40;
                        int printY = panel->learned[i].y - 2;
                        *(SeisanLabelBytes *)firstLabels = *(const SeisanLabelBytes *)seisanOrdinalPrefixText;
                        firstLabels[0] = ordinalSource[0];
                        xglFontPrint(printX, printY, 0xffffff, (const char *)firstLabels);
                    }
                }
            } else {
                for (i = 0; i < 2; i++) {
                    panel->learned[i + 4].x = panel->window.x + 288;
                    panel->learned[i + 4].y = panel->window.y + i * 24 + 3;
                    eMessageMain(&panel->learned[i + 4]);
                    if (panel->learned[i + 4].text) {
                        const unsigned short *ordinalSource = msg_number[i + 5];
                        int printX = panel->learned[i + 4].x - 40;
                        int printY = panel->learned[i + 4].y - 2;
                        *(SeisanLabelBytes *)lastLabels = *(const SeisanLabelBytes *)seisanOrdinalPrefixText;
                        lastLabels[0] = ordinalSource[0];
                        xglFontPrint(printX, printY, 0xffffff, (const char *)lastLabels);
                    }
                }
            }
        }
        break;
    }
    }
}

static inline void SeisanDrawFade(SeisanFadeData *fade)
{
    endPrintExtFunc(0x02ffffff, 3, fade->vertices);
}

void SeisanFadeMain(void)
{
    SeisanFadeData *fade = SeisanFade;
    int i;

    switch (fade->state) {
    case 0:
        fade->mode = 0;
        for (i = 0; i < 4; i++) {
            fade->vertices[i].alpha = 0;
            fade->vertices[i].blue = 0;
            fade->vertices[i].green = 0;
            fade->vertices[i].red = 0;
            fade->vertices[i].depth = 0xffffff;
        }
        fade->vertices[0].x = 0;
        fade->vertices[0].y = 0;
        fade->vertices[1].x = 0;
        fade->vertices[1].y = 448;
        fade->vertices[2].x = 512;
        fade->vertices[2].y = 0;
        fade->vertices[3].x = 512;
        fade->vertices[3].y = 448;
        fade->state = 2;
        fade->terminatorDepth = 0;
    case 2:
        if (SeisanWork[SEISAN_WORK_STATE] == SEISAN_STATE_CLOSING) {
            int alpha = (16 - SeisanWork[5]) * 8;
            fade->vertices[3].alpha = alpha;
            fade->vertices[2].alpha = alpha;
            fade->vertices[1].alpha = alpha;
            fade->vertices[0].alpha = alpha;
        }
        SeisanDrawFade(fade);
        break;
    }
}

int subSeisanHissatuCheck00(unsigned char *seisan_work)
{
    int i;

    for (i = 0; i < SeisanWork[6]; i++)
    {
        if (SeisanResult->entries[i].count != 0)
        {
            return 0x50;
        }
    }
    return 0;
}

int subSeisanHissatuCheck01(void)
{
    int i;

    for (i = 0; i < SeisanWork[6]; i++)
    {
        if (SeisanResult->entries[i].count >= 5)
        {
            return 0x58;
        }
    }
    return 0;
}

int subSeisanEtherCheck(void)
{
    return 0;
}

int subSeisanItemCheck(void)
{
    return 0x80;
}

void subSeisanCheck(void)
{
    switch (SeisanWork[SEISAN_WORK_STATE]) {
    case 0x20:
        SeisanWork[SEISAN_WORK_NEXT_STATE] =
            subSeisanHissatuCheck00(SeisanWork);
        if (SeisanWork[SEISAN_WORK_NEXT_STATE] != 0)
            break;
        /* fallthrough */
    case 0x50:
        SeisanWork[SEISAN_WORK_NEXT_STATE] = subSeisanHissatuCheck01();
        if (SeisanWork[SEISAN_WORK_NEXT_STATE] != 0)
            break;
        /* fallthrough */
    case 0x58:
        SeisanWork[SEISAN_WORK_NEXT_STATE] = subSeisanEtherCheck();
        if (SeisanWork[SEISAN_WORK_NEXT_STATE] != 0)
            break;
        /* fallthrough */
    case 0x60:
        SeisanWork[SEISAN_WORK_NEXT_STATE] = subSeisanItemCheck();
        if (SeisanWork[SEISAN_WORK_NEXT_STATE] != 0)
            break;
        /* fallthrough */
    case 0x80:
        SeisanWork[SEISAN_WORK_NEXT_STATE] = SEISAN_STATE_CLOSING;
        SeisanWork[SEISAN_WORK_FLAGS] |= 4;
        break;
    }
}

void subSeisanMain(void)
{
    if (SeisanWork[1] != SeisanWork[3]) {
        SeisanWork[1] = SeisanWork[3];
        SeisanWork[2] |= 1;
    } else {
        SeisanWork[2] &= 0xfe;
    }
    if (SeisanWork[5] != 0)
        SeisanWork[5]--;

    switch (SeisanWork[1]) {
    case 0x20:
        if ((unsigned char)(SeisanWork[2] & 1)) {
            SeisanWork[5] = 0x20;
            subSeisanCheck();
        }
        if (SeisanWork[5] == 0 && SeisanCN.state == 0)
            SeisanCN.state = 1;
        if ((PadData.half_2a & 0x20) && SeisanWork[5] == 0) {
            if (SeisanCN.state == 3)
                SeisanWork[3] = SeisanWork[4];
            else
                SeisanCN.state = 2;
        }
        SeisanCountMain(0);
        break;
    case 0x30:
        break;
    case 0x50:
        if ((unsigned char)(SeisanWork[2] & 1)) {
            SeisanWork[5] = 16;
            subSeisanCheck();
        }
        if ((PadData.half_2a & 0x20) && SeisanWork[5] == 0)
            SeisanWork[3] = SeisanWork[4];
        break;
    case 0x58:
        if ((unsigned char)(SeisanWork[2] & 1)) {
            SeisanWork[5] = 16;
            subSeisanCheck();
        }
        if ((PadData.half_2a & 0x20) && SeisanWork[5] == 0)
            SeisanWork[3] = SeisanWork[4];
        break;
    case 0x60:
        if ((unsigned char)(SeisanWork[2] & 1)) {
            SeisanWork[5] = 16;
            subSeisanCheck();
        }
        if ((PadData.half_2a & 0x20) && SeisanWork[5] == 0)
            SeisanWork[3] = SeisanWork[4];
        break;
    case 0x80:
        if ((unsigned char)(SeisanWork[2] & 1)) {
            SeisanWork[5] = 16;
            subSeisanCheck();
        }
        if ((PadData.half_2a & 0x20) && SeisanWork[5] == 0)
            SeisanWork[3] = SeisanWork[4];
        break;
    case 0xf0:
        if ((unsigned char)(SeisanWork[2] & 1))
            SeisanWork[5] = 16;
        if (SeisanWork[5] == 0) {
            unsigned char *work;
            MenuBgTaskBreak();
            work = SeisanWork;
            work[3] = 0xff;
            work[1] = 0xff;
        } else {
            SeisanWork[5]--;
        }
        break;
    }
    SeisanWork[2] &= 0xfd;
}

int SeisanMain(void)
{
    subSeisanMain();
    return SeisanWork[SEISAN_WORK_STATE] != SEISAN_STATE_FINISHED;
}

void SeisanDisp(void)
{
    SeisanRibbonData ribbon;

    ribbon = seisanRibbonTable;
    endPrintDirectRibbon(&ribbon);
    endPrintExtFunc(0, 100, 0);
    if (MenuLoadSync())
    {
        xglTaskExecute(SeisanBgTask);
    }
    else
    {
        MenuBgTaskMain();
        xglTaskExecute(SeisanBgTask);
        xglTaskExecute(SeisanTask);
        SeisanFadeMain();
    }
}

void SeisanInit(void)
{
    SeisanRewardSnapshot before[16];
    SeisanItemCategoryTable itemGroups;
    int itemCodes[8];
    SeisanBackgroundColor background;
    unsigned char *work;
    TskObject *object;
    SeisanStatusPanel *status;
    int oldMoney;
    int rewardTotal;
    int withinCap;
    int partySlot;
    int i;
    int entrySlot;

    work = MenuWorkEndGet();
    work = (void *)(((unsigned int)work + 127) & ~127U);
    endPrintInit();
    work = (void *)MenuBgTaskInit(work, 3);
    xglStudioGetCamera2(3)->active = 1;
    SeisanTask = (void *)work;
    work = xglTaskInitial(work, 16, 0);
    object = (TskObject *)xglTaskEntryNext(SeisanTask,
                    (int (*)(XglTaskPrefix *))tskTskMain2,
                    SeisanTask ? SeisanTask->active_tail : 0);
    TskObjectSet2(object, (TskObjectWorker)SeisanTimeEx, work);
    work += sizeof(SeisanTimePanel);
    object = (TskObject *)xglTaskEntryNext(SeisanTask,
                    (int (*)(XglTaskPrefix *))tskTskMain2,
                    SeisanTask ? SeisanTask->active_tail : 0);
    TskObjectSet2(object, (TskObjectWorker)tskSeisanButton, work);
    work += sizeof(SeisanButtonWork);
    object = (TskObject *)xglTaskEntryNext(SeisanTask,
                    (int (*)(XglTaskPrefix *))tskTskMain2,
                    SeisanTask ? SeisanTask->active_tail : 0);
    TskObjectSet2(object, (TskObjectWorker)tskSeisanItem, work);
    work += sizeof(SeisanItemPanel);
    for (partySlot = 0; partySlot < 3; partySlot++) {
        status = (void *)work;
        object = (TskObject *)xglTaskEntryNext(SeisanTask,
                        (int (*)(XglTaskPrefix *))tskTskMain2,
                        SeisanTask ? SeisanTask->active_tail : 0);
        TskObjectSet2(object, (TskObjectWorker)tskSeisanStatus, status);
        status->partySlot = partySlot;
        work += sizeof(*status);
    }
    SeisanFade = (void *)work;
    work += sizeof(*SeisanFade);
    SeisanFade->state = 0;
    SeisanWork = work;
    work += 64;
    {
        SeisanQuadword *const clearBase = (void *)SeisanWork;
        SeisanQuadword clearValue = 0;
        rewardTotal = 3;
        do {
            clearBase[3 - rewardTotal] = clearValue;
        } while (--rewardTotal >= 0);
    }
    SeisanControlGet()->requestedState = 32;
    SeisanItemBox = (void *)work;
    {
        SeisanItemEntry *item = (void *)work;
        work += 8 * sizeof(*item);
        SeisanItemBoxClear(item);
    }
    SeisanResult = (void *)work;
    work += sizeof(*SeisanResult);
    memset(SeisanResult, 0, sizeof(*SeisanResult));
    SeisanCountInit1();
    {
        int snapshotCharacter;
        for (snapshotCharacter = 1; snapshotCharacter < 13; snapshotCharacter++) {
            SeisanCharRecord *character = func_A19210(snapshotCharacter);
            before[snapshotCharacter - 1].techPoints = character->techPoints;
            before[snapshotCharacter - 1].skillPoints = character->skillPoints;
            before[snapshotCharacter - 1].specialPoints = character->specialPoints;
            before[snapshotCharacter - 1].experience = character->experience;
        }
    }
    oldMoney = dataMoneyBoxChk();
    func_A17C08(SeisanResult);
    SeisanMg = func_00A0F110();
    for (i = 0; i < 3; i++) {
        int characterNo = SeisanRewardCharacter(i);
        withinCap = (unsigned int)(characterNo - 1) < 12;
        if (withinCap) {
            func_A19210(characterNo);
            {
            int rewardCharacter = SeisanRewardCharacter(i);
            SeisanHissatuEntry *reward = SeisanRewardRow(i, (void *)SeisanResult);
            {
                rewardTotal = before[rewardCharacter - 1].techPoints + reward->techPoints;
                if (rewardTotal >= 10000)
                    reward->techPoints = reward->techPoints - rewardTotal + 9999;
            }
            {
                int total = before[SeisanRewardCharacter(i) - 1].skillPoints + reward->skillPoints;
                withinCap = total < 10000;
                if (!withinCap)
                    reward->skillPoints = reward->skillPoints - total + 9999;
            }
            {
                int total = before[SeisanRewardCharacter(i) - 1].specialPoints + reward->specialPoints;
                withinCap = total < 10000;
                if (!withinCap)
                    reward->specialPoints = reward->specialPoints - total + 9999;
            }
            {
                int total = before[SeisanRewardCharacter(i) - 1].experience + reward->experience;
                withinCap = total > 9999999;
                if (withinCap)
                    reward->experience = reward->experience - total + 9999999;
            }
            }
        }
    }
    rewardTotal = oldMoney + SeisanMg->money;
    if (rewardTotal > 999999999)
        SeisanMg->money = SeisanMg->money - rewardTotal + 999999999;
    SeisanCountInit2();
    itemGroups = seisanItemCategoryTable[0];
    {
    int itemIndex;
    for (itemIndex = 0; itemIndex < SeisanMg->itemCount; itemIndex++) {
        int type = SeisanMg->itemTypes[itemIndex];
        if (type != 10)
            itemCodes[itemIndex] = (unsigned short)SeisanMg->itemIds[itemIndex] + (itemGroups.entries[type] << 16);
    }
    for (itemIndex = 0; itemIndex < SeisanMg->itemCount; itemIndex++) {
        entrySlot = 0;
        if (SeisanMg->itemCount > 0) {
            do {
                if (itemCodes[itemIndex] == SeisanItemBox[entrySlot].itemId) {
                    SeisanItemBox[entrySlot].itemId = itemCodes[itemIndex];
                    SeisanItemBox[entrySlot].count++;
                    break;
                }
                entrySlot++;
            } while (entrySlot < SeisanMg->itemCount);
        }
        if (SeisanMg->itemCount == entrySlot) {
            entrySlot = 0;
            if (SeisanMg->itemCount > 0) {
                SeisanItemEntry *empty = SeisanItemBox;
                do {
                    if (empty->count == 0) {
                        int itemCode = itemCodes[itemIndex];
                        empty->count = 1;
                        empty->itemId = itemCode;
                        break;
                    }
                    entrySlot++;
                    empty++;
                } while (entrySlot < SeisanMg->itemCount);
            }
        }
    }
    }
    {
    int partyIndex;
    for (partyIndex = 0; partyIndex < 3; partyIndex++) {
        if (SeisanRewardCharacter(partyIndex) != 0)
            SeisanWork[6]++;
    }
    }
    endPrintDirectFrameCopy(xglPacketGetCurrent(), sRender.height << 5, sRender.flipBase << 5);
    memset(&background, 0, sizeof(background));
    endBackTexDraw(&background);
    MenuLoadInit();
    MenuFontLoad(1, 1);
    WindowTexLoad(0, 0x10001);
    GameResourcePreLoad(GameLoopState[6]);
    MenuWorkEndCheck(work);
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
/* JP: ・
 * EN: . */
const char seisanOrdinalPrefixText[5] = "\241\241\241\246";

/* JP: ×
 * EN: X */
const char seisanQuantityMarkText[5] = "\241\337\241\241";

/* Text: \x01S.Pts */
const char seisanStartPointsText[7] = "\001S.Pts";

/* Text: \x01E.Pts */
const char seisanEndPointsText[7] = "\001E.Pts";

/* Text: \x01T.Pts */
const char seisanTotalPointsText[7] = "\001T.Pts";

const SeisanItemCategoryTable seisanItemCategoryTable[1] = { { .entries = { 1, 3, 4, 5 } } };

unsigned char *SeisanWork = 0;

XglTaskScheduler *SeisanBgTask = 0;

XglTaskScheduler *SeisanTask = 0;

SeisanResultData *SeisanResult = 0;

/* Remaining original small data, in address order. */
SeisanMoneyData *SeisanMg = 0;

SeisanItemEntry *SeisanItemBox = 0;

SeisanFadeData *SeisanFade = 0;

const char MenuShopCardsText[6] = "Cards";

const char MenuShopItemsText[6] = "Items";

const char MenuShopSellText[5] = "Sell";

const char MenuShopBuyText[4] = "Buy";

const char MenuShopStoreText[5] = "Shop";

const char MenuShopQuitText[6] = "Quit.";

const char MenuShopSelectText[8] = "Select ";

const char MenuShopIsText[5] = " is ";

const char MenuShopGoldUnitText[3] = "G.";

/* Text:  sold.\x1F */
const char MenuShopSoldText[8] = " sold.\037";

const char MenuShopPeriodText[3] = ".\n";
