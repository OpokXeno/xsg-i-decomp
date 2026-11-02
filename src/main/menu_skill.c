#include "common.h"
#include "shared.h"
#include "main/party.h"
#include "menu_skill.h"
#include "main/menu_skill.h"
#include "main/menu_system.h"

const char skillExtractCommandText[9] = "/Extract";

static const char * msg_0_0036DD58[3] = { MenuSkillTitleText, skillExtractCommandText, MenuSkillSetSuffixText };

static short MenuSkillParty[8];

unsigned int SkillSetLvGet(int skill_id)
{
    int skill_index = (unsigned short)skill_id;
    MenuSkillData *skill_data = SkillDataBuf;

    if (skill_index <= 0)
        return 0;
    return skill_data->level_cap_by_skill_id[skill_index - 1];
}

int SkillGetPtGet(int skill_id)
{
    int skill_index = (unsigned short)skill_id;
    MenuSkillData *skill_data = SkillDataBuf;

    if (skill_index <= 0)
        return 0;
    return skill_data->point_cost_by_skill_id[skill_index];
}

unsigned short SkillNextLvGet(int level)
{
    return SkillDataBuf->next_level_points[level];
}

unsigned int SkillCharSkillLvGet(int character_id)
{
    PartyDataPrefix *party = PartyDataGet();
    PartySkillLevelArray *skill_levels = &party->skill_levels;
    int character_index = (unsigned short)character_id;

    if (character_index == 0)
        return 0;
    if (character_index >= 9)
        return 0;
    return skill_levels->level_by_character[character_id - 1];
}

int SkillCharPointPlus(int character_id, int point_delta)
{
    int character_index = (unsigned short)character_id;
    PlayerCharacter *character;
    PartyDataPrefix *party;
    unsigned char *level_by_character;
    PartySkillPoints *points_to_next_level;
    int level;
    int character_points;
    int remaining_points;
    PartySkillPoints next_level_points;

    if (character_index == 0 || character_index >= 9)
        return 0;

    character = dataPlChaGet(character_index);
    party = PartyDataGet();
    character_points = character->skill_points - point_delta;
    if (character_points < 0)
        character_points = 0;
    character->skill_points = character_points;

    level_by_character = &party->skill_levels.level_by_character[character_id - 1];
    if (*level_by_character >= 5)
        return 1;

    points_to_next_level = (PartySkillPoints *)((unsigned int)(character_id - 1) * 8 + (unsigned int)party->next_level_points);
    remaining_points = *points_to_next_level;
    remaining_points -= point_delta;
    if (remaining_points <= 0) {
        xglSoundEffectNormalID(1, 0);
        level = *level_by_character + 1;
        *level_by_character = level;
        if ((unsigned char)level < 5) {
            next_level_points = SkillNextLvGet(*level_by_character);
            *points_to_next_level = next_level_points;
            if (remaining_points < 0)
                *points_to_next_level = next_level_points - -remaining_points;
        } else {
            *points_to_next_level = 0;
        }
    } else {
        *points_to_next_level = remaining_points;
    }
    return 0;
}

void MenuSkillPasMain(void)
{
    SkillPasWork *work = MenuSkillPas;
    int length[3];
    short target[3];
    short windowTarget;
    int i;

    for (i = 0; i < 3; i++)
        length[i] = MenuPasLengthGet(msg_0_0036DD58[i]);

    switch (work->state) {
    case 0:
        work->color = 0x00fffff0;
        WindowDXSet(&work->window);
        work->window.x = -288;
        work->window.y = 8;
        work->window.style = work->color;
        work->window.width = 272;
        work->window.height = 30;
        work->window.close_callback = MenuPasWindow;
        work->window.close_callback_arg = work->callbackWork;
        work->window.state = 1;
        WindowDXMain(&work->window);
        work->window.state = 3;
        for (i = 0; i < 3; i++) {
            eMessageSet(&work->message[i], msg_0_0036DD58[i]);
            work->message[i].mode = 32;
            work->message[i].x = 288;
            work->message[i].y = 11;
            work->message[i].draw_parameter = work->color + 2;
        }
        work->state = 2;
        /* fallthrough */
    case 2:
        break;
    default:
        return;
    }

    windowTarget = -16;
    for (i = 0; i < 3; i++)
        target[i] = 288;
    switch (MenuWork.state) {
    case SKILL_CHARACTER:
    case SKILL_COMMAND:
        target[0] = 16;
        break;
    case SKILL_LEARN:
    case SKILL_CONFIRM:
    case SKILL_EXTRACT:
        target[0] = 16;
        target[1] = length[0] + 16;
        break;
    case SKILL_EQUIP:
        target[0] = 16;
        target[2] = length[0] + 16;
        break;
    default:
        windowTarget = -288;
        break;
    }
    MoveSlide(&work->window.x, &windowTarget, 3.0f);
    WindowDXMain(&work->window);
    if (work->window.state == 3) {
        work->box.x = work->window.x + 3;
        work->box.y = work->window.y + 3;
        work->box.color = work->color;
        work->box.width = work->window.width - 6;
        work->box.height = work->window.height - 6;
        endPrintExtFunc(work->color, 101, &work->box);
        for (i = 0; i < 3; i++) {
            MoveSlide(&work->message[i].x, &target[i], 5.0f);
            if (work->message[i].x < 256)
                eMessageMain(&work->message[i]);
        }
        endPrintExtFunc(work->color, 102, 0);
    }
}

void MenuSkillInfoMain(void)
{
    MenuSkillInfoWork **workSlot = &MenuSkillInfo;
    char *characterHelp[] = {
        "Select a character.",
        " cannot be selected."
    };
    char *commandHelp[] = {
        "Extract skills from accessories.",
        "Allocate skills.",
        "Cancel."
    };
    char *skillHelp[] = {
        "Skill: ",
        "\nEffect: ",
        /* Text:  has been extracted. \x1F */
        " has been extracted. \x1F"
    };
    char *learnHelp[] = {
        "Skill has already been extracted.",
        "Skill cannot be extracted from the selected item."
    };
    MenuSkillInfoWork *work = *workSlot;
    short target;

    switch (work->state) {
    case 0:
        work->color = 0x00fffff0;
        WindowDXSet(&work->window);
        work->window.x = -16;
        work->window.style = work->color;
        work->window.y = 480;
        work->window.width = 544;
        work->window.height = 54;
        work->window.close_callback = MenuInfoWindow;
        work->window.close_callback_arg = &work->info;
        work->info.x = 0;
        work->info.y = 0;
        work->info.text = 0;
        work->window.state = 1;
        WindowDXMain(&work->window);
        work->window.state = 3;
        work->selected = -1;
        work->opened = 1;
        work->reasonShown = 0;
        memset(work->message, 0, sizeof(work->message));
        work->state = 2;
        /* fallthrough */
    case 2:
        break;
    default:
        return;
    }
    target = 386;
    if (MenuWork.flags & MENU_FLAG_ENTER)
        work->reasonShown = 0;
    if (work->reasonShown == 0) {
        if (PadData.pressed & PAD_CIRCLE)
            work->reasonShown = 1;
    } else if (PadData.repeat != 0) {
        work->reasonShown = 0;
    }
    switch (MenuWork.state) {
    case SKILL_CHARACTER:
        eMessageCpy(work->message, characterHelp[0]);
        if (work->reasonShown != 0 && MenuWork.char_denied != 0) {
            eMessageCpy(work->message, MenuCharNameGet(MenuWork.characterNo));
            eMessageCat(characterHelp[1]);
        }
        break;
    case SKILL_COMMAND:
        eMessageCpy(work->message, commandHelp[MenuWork.commandCursor]);
        break;
    case SKILL_LEARN:
    case SKILL_CONFIRM:
        if (MenuWork.flags & MENU_FLAG_ENTER)
            work->selected = -1;
        if (MenuWork.list_select < 0) {
            eMessageCpy(work->message, skillEmptyText);
        } else if ((short)MenuSortGet(0, MenuWork.list_select) != 0) {
            SkillRecord *skill = func_A1A548((short)MenuSortGet(0, MenuWork.list_select));

            if (work->reasonShown == 0) {
                eMessageCpy(work->message, skillHelp[0]);
                if (skill->prerequisite != 0)
                    eMessageCat(func_A2C648(skill->prerequisite)->name);
                eMessageCat(skillHelp[1]);
                if (skill->prerequisite != 0)
                    eMessageCat(func_A2C648(skill->prerequisite)->auxiliary);
            } else if (MenuWork.list_accessory != 0) {
                if (func_A1A548(MenuWork.list_accessory)->prerequisite == 0)
                    eMessageCpy(work->message, learnHelp[1]);
                else if (func_A19698(MenuWork.characterNo, skill->prerequisite) != 0)
                    eMessageCpy(work->message, learnHelp[0]);
            }
        }
        break;
    case SKILL_EXTRACT:
        if (MenuWork.flags & MENU_FLAG_ENTER) {
            SkillRecord *skill = func_A1A548((short)MenuSortGet(0, MenuWork.list_select));

            eMessageCpy(work->message, skillHelp[0]);
            if (skill->prerequisite != 0)
                eMessageCat(func_A2C648(skill->prerequisite)->name);
            eMessageCat(skillHelp[2]);
        }
        break;
    case SKILL_EQUIP:
        if (MenuWork.list_select >= 0)
            eMessageCpy(work->message, MenuTextGet(MenuSortGet(0, MenuWork.list_select))->auxiliary);
        else
            eMessageCpy(work->message, skillEmptyText);
        break;
    default:
        target = 512;
        break;
    }
    work->info.text = work->message;
    MoveSlide(&work->window.y, &target, 3.0f);
    WindowDXMain(&work->window);
}

void MenuSkillStatusMain(void)
{
    MenuSkillStatusWork *status = MenuSkillStatus;
    short targetX[8];
    short targetY[8];
    short targetWidth[8];
    short targetHeight[8];
    short targetOffset[8];
    int i;

    switch (status->state) {
    case 0:
        status->color = 0x00FFF000;
        for (i = 0; i < MenuWork.partyCount; i++) {
            short chrNo = MenuSkillParty[i];

            WindowDXSet(&status->window[i]);
            status->window[i].x = (i & 1) * 816 - 272;
            status->window[i].y = (i / 2) * 104 + 64;
            status->window[i].style = status->color;
            status->window[i].width = 240;
            status->window[i].height = 82;
            status->window[i].ribbon_initialized = 1;
            if (PartyAttackerCheck(chrNo) != 0) {
                status->window[i].tag_id = MenuTagTextGet(1);
            } else {
                status->window[i].tag_id = MenuTagTextGet(2);
            }
            status->window[i].close_callback = MenuStatusDisp;
            status->window[i].close_callback_arg = &status->panel[i];
            status->panel[i].offsetX = 0;
            status->panel[i].chrNo = chrNo;
            status->panel[i].enabled = 0;
            status->panel[i].offsetY = 0;
            status->window[i].state = 1;
            WindowDXMain(&status->window[i]);
            status->window[i].state = 3;
        }
        for (i = 0; i < 1; i++) {
            eCursolSet(&status->cursor[i], 0);
            status->cursor[i].state = 0;
            status->cursor[i].work = 0x00FFFFFF;
        }
        status->cursorOn = 0;
        status->flags = 0;
        status->subState = 0;
        status->moveVert = 0;
        status->lastCursor = MenuWork.partyCursor;
        status->state = 2;
        /* fall through */
    case 2:
        for (i = 0; i < MenuWork.partyCount; i++) {
            targetX[i] = (i & 1) * 816 - 272;
            targetY[i] = (i / 2) * 104 + 64;
            targetWidth[i] = 240;
            targetHeight[i] = 82;
            targetOffset[i] = 0;
        }
        switch (MenuWork.state) {
        case SKILL_CHARACTER:
            if (MenuWork.flags & MENU_FLAG_ENTER) {
                status->cursor[0].state = 32;
                status->cursorOn = 1;
                status->moveVert = 1;
                if ((status->flags & 1) != 0) {
                    if (i != MenuWork.partyCursor) {
                        status->window[i].x = (i & 1) * 816 - 272;
                        status->window[i].y = 24;
                    }
                    status->flags &= ~1;
                }
            }
            for (i = 0; i < MenuWork.partyCount; i++) {
                targetX[i] = (i & 1) * 240 + 16;
            }
            break;
        case SKILL_COMMAND:
            if (MenuWork.flags & MENU_FLAG_ENTER) {
                status->cursor[0].state = 0;
                status->lastCursor = MenuWork.partyCursor;
            }
            /* fall through */
        case SKILL_LEARN:
        case SKILL_CONFIRM:
        case SKILL_EXTRACT:
        case SKILL_EQUIP:
            if (status->lastCursor != MenuWork.partyCursor) {
                status->window[MenuWork.partyCursor].x = 264;
                status->window[MenuWork.partyCursor].y = 32;
                status->lastCursor = MenuWork.partyCursor;
            }
            if ((status->flags & 1) != 0) {
                for (i = 0; i < MenuWork.partyCount; i++) {
                    if (i == MenuWork.partyCursor) {
                        targetX[i] = 264;
                        targetY[i] = 32;
                    } else {
                        targetX[i] = (i & 1) * 816 - 272;
                        status->window[i].x = targetX[i];
                    }
                }
            } else {
                for (i = 0; i < MenuWork.partyCount; i++) {
                    if (i == MenuWork.partyCursor) {
                        targetX[i] = 264;
                        targetY[i] = 32;
                    }
                }
                if (targetY[MenuWork.partyCursor] == status->window[MenuWork.partyCursor].y) {
                    status->flags |= 1;
                }
            }
            status->cursorOn = 0;
            status->moveVert = 0;
            break;
        }
        for (i = 0; i < MenuWork.partyCount; i++) {
            if (status->moveVert == 0) {
                MoveSlide(&status->window[i].width, &targetWidth[i], 3.0f);
                MoveSlide(&status->window[i].height, &targetHeight[i], 3.0f);
                MoveSlide(&status->panel[i].offsetX, &targetOffset[i], 3.0f);
                MoveSlide(&status->window[i].x, &targetX[i], 3.0f);
                if (status->window[i].x == targetX[i]) {
                    MoveSlide(&status->window[i].y, &targetY[i], 3.0f);
                }
            } else {
                MoveSlide(&status->window[i].y, &targetY[i], 3.0f);
                if (status->window[i].y == targetY[i]) {
                    MoveSlide(&status->window[i].width, &targetWidth[i], 3.0f);
                    MoveSlide(&status->panel[i].offsetX, &targetOffset[i], 3.0f);
                    MoveSlide(&status->window[i].x, &targetX[i], 3.0f);
                }
            }
            if (i == MenuWork.partyCursor) {
                status->window[i].style = 0x7F00;
            } else {
                status->window[i].style = 0x7E00;
            }
            WindowDXMain(&status->window[i]);
        }
        status->cursor[0].x = status->window[MenuWork.partyCursor].x + 3;
        status->cursor[0].y = status->window[MenuWork.partyCursor].y
                                     + status->window[MenuWork.partyCursor].height / 2 - 4;
        status->cursor[0].work = 0x00FFFFFF;
        eCursolMain(&status->cursor[0]);
        break;
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char skillExtractSetCancelText[19] = "Extract\nSet\nCancel";

const char skillExtractQuestionText[16] = " Extract skill\?";

void MenuSkillMenuMain(void)
{
    MenuSkillMenuWork *self = MenuSkillMenu;
    short target[2];
    int i;

    switch (self->state) {
    case 0:
        self->color = 0x00FFFFF0;
        for (i = 0; i < 2; i++) {
            WindowDXSet(&self->window[i]);
            self->window[i].x = 528;
            self->window[i].width = 129;
            self->window[i].height = 78;
            self->window[i].style = self->color;
            self->window[i].tag_id = skillMenuText;
            self->window[i].close_callback = MenuSelectWindow;
            self->window[i].close_callback_arg = &self->list[i];
            self->list[i].cursor = 0;
            if (i != 1) {
                self->list[i].hasPrompt = 0;
                self->list[i].items = msg00_1_0036DD68[0];
            } else {
                self->window[i].x = -182;
                self->window[i].width = 169;
                self->window[i].height = 102;
                self->list[i].hasPrompt = 1;
                self->list[i].items = msg01_2_0036DD70[0];
                self->list[i].prompt = msg01_2_0036DD70[1];
            }
            self->window[i].state = 1;
            WindowDXMain(&self->window[i]);
            self->window[i].state = 3;
        }
        self->window[0].y = 176;
        self->window[1].y = 160;
        self->state = 2;
        self->subState = 0;
        /* fall through */
    case 2:
        target[0] = 528;
        target[1] = -182;
        switch (MenuWork.state) {
        case SKILL_COMMAND:
            target[0] = 272;
            self->list[0].cursor = MenuWork.commandCursor;
            break;
        case SKILL_CONFIRM:
            target[1] = 36;
            self->list[1].cursor = MenuWork.itemCursor;
            break;
        }
        for (i = 0; i < 2; i++) {
            MoveSlide(&self->window[i].x, &target[i], 3.0f);
            WindowDXMain(&self->window[i]);
        }
        break;
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* Text: {color #808080}\x1E\x00: Set Search */
const char skillSetSearchText[19] = "\014\200\200\200\036\000: Set Search";

void MenuSkillExMain(void)
{
    MenuSkillExWork *work = MenuSkillEx;
    short ribbonTarget[2];
    short messageTarget;
    int i;
    SkillUnitOrg *unit;
    char *messageColor;

    switch (work->state) {
    case 0:
        work->color = 0x00fffff0;
        for (i = 0; i < 2; i++) {
            eRibbonSet(&work->ribbon[i], 3);
            work->ribbon[i].x = -272;
            work->ribbon[i].y = 48 + i * 32;
            work->ribbon[i].work = work->color;
            work->ribbon[i].width = 256;
            work->ribbon[i].height = 24;
            eTagFontSet(&work->tag[i], msg00_3_0036DD78[i]);
            work->tag[i].work = work->color + 2;
            eNumberSet(&work->number[i], 0);
            work->number[i].work = work->color + 2;
        }
        eMessageSet(&work->message, msg00_3_0036DD78[2]);
        work->message.mode = 32;
        work->message.x = 528;
        work->message.y = 120;
        work->message.draw_parameter = work->color + 2;
        work->state = 2;
        /* fallthrough */
    case 2:
        break;
    default:
        return;
    }
    ribbonTarget[1] = -272;
    ribbonTarget[0] = -272;
    messageTarget = 528;
    switch (MenuWork.state) {
    case SKILL_EQUIP:
        messageTarget = 288;
        /* fallthrough */
    case SKILL_COMMAND:
    case SKILL_LEARN:
    case SKILL_CONFIRM:
    case SKILL_EXTRACT:
        ribbonTarget[1] = 0;
        ribbonTarget[0] = 0;
        break;
    }
    for (i = 0; i < 2; i++) {
        MoveSlide(&work->ribbon[i].x, &ribbonTarget[i], 3.0f);
        eRibbonMain(&work->ribbon[i]);
        work->tag[i].x = work->ribbon[i].x + work->ribbon[i].width - 68;
        work->tag[i].y = work->ribbon[i].y + 4;
        eTagFontMain(&work->tag[i]);
        work->number[i].x = work->ribbon[i].x + work->ribbon[i].width - 60;
        work->number[i].y = work->ribbon[i].y + 4;
        work->number[i].format = 0;
        if (i == 0) {
            work->number[i].digits = 1;
            work->number[i].value = SkillCharSkillLvGet(MenuWork.characterNo);
        } else {
            PlayerCharacter *character;

            work->number[i].digits = 4;
            character = dataPlChaGet(MenuWork.characterNo);
            work->number[i].value = character->skill_points;
        }
        eNumberMain(&work->number[i]);
    }
    /* The original patches the colour escape bytes of this string, which lives in read-only data. */
    messageColor = (char *)msg00_3_0036DD78[2] + 1;
    unit = func_A191C0(MenuWork.characterNo);
    if (MenuWork.list_select < 0 || unit->setSkill[0] + unit->setSkill[1] + unit->setSkill[2] == 0) {
        messageColor[2] = 64;
        messageColor[1] = 64;
        messageColor[0] = 64;
    } else {
        messageColor[2] = -128;
        messageColor[1] = -128;
        messageColor[0] = -128;
    }
    MoveSlide(&work->message.x, &messageTarget, 3.0f);
    eMessageMain(&work->message);
}

void MenuSkillL1R1Main(void)
{
    MenuSkillL1R1Work *self = MenuSkillL1R1;
    /* separate load keeps the sprite slide offsets in their own register */
    signed char *slide_offset_reference;
    int side_index;

    switch (self->state) {
    case 0: {
        SkillL1R1SpriteIdPair spriteIds = skillL1R1SpriteTable[0];

        self->spriteWork = 0x00FFFFFF;
        for (side_index = 0; side_index < 2; side_index++) {
            eSpriteSet(&self->sprite[side_index], spriteIds.id[side_index]);
            self->sprite[side_index].work = self->spriteWork;
            self->sprite[side_index].y = 214;
        }
        self->sprite[0].x = -45;
        self->sprite[1].x = 528;
        slide_offset_reference = self->slideOffset;
        self->slideOffset[1] = 0;
        self->slideOffset[0] = 0;
        /* separate load keeps the active screen state in the loop index register */
        side_index = 2;
        self->state = side_index;
        break;
    }
    case 2:
        slide_offset_reference = self->slideOffset;
        break;
    default:
        return;
    }

    {
        short target[2];
        int menuState = MenuWork.state;

        target[0] = -45;
        target[1] = 528;
        if (menuState == SKILL_COMMAND) {
            target[0] = 8;
            target[1] = 475;
            if (PadData.pressed & PAD_L1) {
                self->slideOffset[0] = -6;
            } else if (PadData.pressed & PAD_R1) {
                self->slideOffset[1] = 6;
            }
        }

        {
            SkillL1R1SlideStep step = skillL1R1SlideStep[0];

            for (side_index = 0; side_index < 2; side_index++) {
                if (self->slideOffset[side_index] != 0) {
                    self->slideOffset[side_index] = self->slideOffset[side_index] + step.side[side_index];
                }
            }
        }

        for (side_index = 0; side_index < 2; side_index++) {
            MoveSlide(&self->sprite[side_index].x, &target[side_index], 3.0f);
            self->sprite[side_index].x = self->sprite[side_index].x + slide_offset_reference[side_index];
            eSpriteMain(&self->sprite[side_index]);
        }
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* JP: 1・ /  2・ /  3・
 * EN: 1. / 2. / 3. */
const char skillNumberedListText[15] = " 1\241\246\n 2\241\246\n 3\241\246";

const char skillSetListText[11] = "Set Skills";

void MenuSkillSetListMain(void)
{
    MenuSkillSetListWork *self = MenuSkillSetList;
    SkillUnitOrg *unit;
    short targetX;
    int i;

    switch (self->state) {
    case 0:
        self->color = 0x00FFFFF0;
        WindowDXSet(&self->window);
        self->window.x = -222;
        self->window.style = self->color;
        self->window.y = 280;
        self->window.width = 209;
        self->window.height = 78;
        self->window.tag_id = skillSetListText;
        self->window.state = 1;
        WindowDXMain(&self->window);
        self->window.state = 3;
        for (i = 0; i < 4; i++) {
            eMessageSet(&self->message[i], 0);
            self->message[i].mode = 32;
            self->message[i].draw_parameter = self->color + 2;
        }
        self->message[3].text = msg00_4_0036DD88[0];
        self->state = 2;
        /* fall through */
    case 2:
        targetX = -222;
        if (MenuWork.state == SKILL_COMMAND || MenuWork.state == SKILL_EQUIP)
            targetX = 16;
        MoveSlide(&self->window.x, &targetX, 3.0f);
        WindowDXMain(&self->window);
        unit = func_A191C0(MenuWork.characterNo);
        for (i = 0; i < 3; i++) {
            if (unit->setSkill[i] != 0) {
                self->message[i].x = self->window.x + 43;
                self->message[i].y = self->window.y + i * 24 + 3;
                self->message[i].text = MenuTextGet(0x80000 + (unsigned short)unit->setSkill[i])->name;
                eMessageMain(&self->message[i]);
            }
        }
        self->message[3].x = self->window.x + 3;
        self->message[3].y = self->window.y + 3;
        eMessageMain(&self->message[3]);
        break;
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const SkillCategoryIconIds skillCategoryIconTable = { .id = { 265, 266, 772, 773, 774 } };

void MenuSkillCategoryMain(void)
{
    MenuSkillCategoryWork *work = MenuSkillCategory;
    /* separate load keeps the category edge offsets in their own register */
    signed char *edge_delta_reference;
    int icon_index;

    switch (work->state) {
    case 0: {
        SkillCategoryIconIds ids = skillCategoryIconTable;

        work->iconResetWork = 0x00FFFFFE;
        for (icon_index = 0; icon_index < 5; icon_index++) {
            eSpriteSet(&work->icons[icon_index].sprite, ids.id[icon_index]);
            work->icons[icon_index].sprite.work = work->iconResetWork;
        }
        work->pulseCounter = 8;
        work->icons[0].flashStep = 1;
        work->phase = 0;
        work->tickCounter = 0;
        work->icons[0].flashLevel = 0;
        work->pendingReset = 0;
        edge_delta_reference = work->edgeDelta;
        work->edgeDelta[1] = 0;
        edge_delta_reference[0] = 0;
        /* separate load keeps the active screen state in the loop index register */
        icon_index = 2;
        work->state = icon_index;
    }
    break;
    case 2:
        edge_delta_reference = work->edgeDelta;
        break;
    default:
        return;
    }

    {
        for (icon_index = 0; icon_index < 2; icon_index++) {
            SkillCategoryIcon *icon = &work->icons[icon_index];

            icon->sprite.color[2] = 0x40;
            icon->sprite.color[1] = 0x40;
            icon->sprite.color[0] = 0x40;
        }

        switch (MenuWork.state) {
        case SKILL_LEARN:
            for (icon_index = 0; icon_index < 2; icon_index++) {
                SkillCategoryIcon *icon = &work->icons[icon_index];

                icon->sprite.color[2] = -0x80;
                icon->sprite.color[1] = -0x80;
                icon->sprite.color[0] = -0x80;
            }
            if (PadData.repeat & PAD_LEFT) {
                work->edgeDelta[0] = -6;
            } else if (PadData.repeat & PAD_RIGHT) {
                work->edgeDelta[1] = 6;
            }
            /* fall through */
        case SKILL_CONFIRM:
        case SKILL_EXTRACT:
            if (work->pulseCounter != 0) {
                work->pulseCounter--;
            }
            work->icons[0].flashStep = 1;
            break;
        default:
            if (work->pulseCounter != 8) {
                work->pulseCounter++;
            }
            work->icons[0].flashStep = 0;
            break;
        }

        {
            SkillCategoryEdgeStep step = skillCategoryEdgeStep[0];

            for (icon_index = 0; icon_index < 2; icon_index++) {
                if (work->edgeDelta[icon_index] != 0) {
                    work->edgeDelta[icon_index] = work->edgeDelta[icon_index] + step.side[icon_index];
                }
            }
        }

        if (work->tickCounter & 1) {
            work->icons[0].flashLevel += work->icons[0].flashStep;
            if (work->icons[0].flashLevel >= 11) {
                work->icons[0].flashStep = -work->icons[0].flashStep;
            }
        }
        work->tickCounter++;

        for (icon_index = 0; icon_index < 2; icon_index++) {
            float cosVal;
            int negTwicePulse;
            unsigned char edge;

            work->icons[icon_index].sprite.x = 288 + icon_index * 110;
            cosVal = xglCos((float)icon_index * 3.1415927f);
            negTwicePulse = -(work->pulseCounter * 2);
            work->icons[icon_index].sprite.x = (short)((float)work->icons[icon_index].sprite.x + cosVal * (float)negTwicePulse);
            edge = edge_delta_reference[icon_index];
            work->icons[icon_index].sprite.y = 120;
            work->icons[icon_index].sprite.x += (signed char)edge;
            work->icons[icon_index].sprite.alpha = (unsigned char)(128.0f - (float)work->pulseCounter * 0.125f * 128.0f);
        }

        for (icon_index = 2; icon_index < 5; icon_index++) {
            float cosVal;
            int negTwicePulse;
            short x;

            work->icons[icon_index].sprite.x = 324 + (icon_index - 2) * 24;
            cosVal = xglCos((float)(icon_index - 2) * 1.0471976f);
            negTwicePulse = -(work->pulseCounter * 2);
            work->icons[icon_index].sprite.y = 119;
            x = (short)((float)work->icons[icon_index].sprite.x + cosVal * (float)negTwicePulse);
            work->icons[icon_index].sprite.x = x;
            work->icons[icon_index].sprite.alpha = (unsigned char)(128.0f - (float)work->pulseCounter * 0.125f * 128.0f);
            {
                SkillCategoryIcon *icon = &work->icons[icon_index];

                if (icon_index - 2 == MenuWork.mode) {
                    icon->sprite.color[2] = -0x80;
                    icon->sprite.color[1] = -0x80;
                    icon->sprite.color[0] = -0x80;
                } else {
                    icon->sprite.color[2] = 0x40;
                    icon->sprite.color[1] = 0x40;
                    icon->sprite.color[0] = 0x40;
                }
            }
        }

        for (icon_index = 0; icon_index < 5; icon_index++) {
            eSpriteMain(&work->icons[icon_index].sprite);
        }
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* Text: List{x 656}S.Lv{x 519}S.Pts */
const char skillListHeaderText[18] = "List\003\220S.Lv\003\007S.Pts";

static int MenuSkillListChange00(void)
{
    int *sortRow = MenuSortAddrGet(0);
    MenuListEntry *rows = MenuListGet(0);
    WindowSPCursor *window = &MenuSkillList->window;
    PlayerCharacter *character = dataPlChaGet(MenuWork.characterNo);
    int count;
    int i;

    MenuSortSet(0, 16, MenuWork.mode + 256);
    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        short skillId = sortRow[i];
        SkillRecord *record = func_A1A548(skillId);
        int cost = SkillGetPtGet(skillId);
        int characterLevel = SkillCharSkillLvGet(MenuWork.characterNo);

        rows[i].amount = cost;
        rows[i].status = SkillSetLvGet(skillId);
        rows[i].flag = 0;
        if (record->prerequisite == 0)
            rows[i].flag = 1;
        else if (func_A19698(MenuWork.characterNo, record->prerequisite) != 0)
            rows[i].flag = 1;
        if (characterLevel < rows[i].status)
            rows[i].flag = 1;
        if (character->skill_points < cost)
            rows[i].flag = 1;
    }
    window->title = skillListHeaderText;
    window->draw_flags = 7;
    window->columns = 1;
    window->rows = 8;
    window->width = 276;
    window->height = 198;
    window->items = MenuListGet(0);
    WindowSPItemChange(window);
    WindowSPSetSelect(window, &MenuKeepSelect[MenuWork.mode * 5]);
    window->mode = 4;
    return WindowSPSelect(window, 0);
}

static int MenuSkillListChange01(void)
{
    int *sortRow = MenuSortAddrGet(0);
    MenuListEntry *rows = MenuListGet(0);
    WindowSPCursor *window = &MenuSkillList->window;
    int count;
    int i;

    MenuSortSet(0, 64, MenuWork.characterNo);
    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        rows[i].amount = SkillSetLvGet(sortRow[i]);
        if (MenuSkillEquipCheck(MenuWork.characterNo, sortRow[i]) != 0)
            rows[i].status = 1;
        else
            rows[i].status = 0;
    }
    window->title = skillSetText;
    window->draw_flags = 3;
    window->columns = 1;
    window->rows = 8;
    window->width = 256;
    window->height = 198;
    window->items = MenuListGet(0);
    WindowSPItemChange(window);
    WindowSPSetSelect(window, &MenuKeepSelect[20]);
    return WindowSPSelect(window, 0);
}

void MenuSkillListMain(void)
{
    MenuSkillListWork *work = MenuSkillList;
    short target;
    int equippedCount;
    int i;

    switch (work->state) {
    case 0:
    {
        work->window.x = 528;
        work->window.y = 64;
        work->color = 0x00fffff8;
        work->window.style = 0x00fffff8;
        work->window.draw_flags = 7;
        work->window.title = skillEmptyText;
        work->window.rows = 11;
        work->window.height = 270;
        work->window.state = 0;
        work->window.items = 0;
        WindowSPSet(&work->window);
        work->window.state = 17;
        for (equippedCount = 0; equippedCount < 3; equippedCount++) {
            eSpriteSet(&work->equipped[equippedCount], 1546);
            work->equipped[equippedCount].work = 0x00ffffff;
        }
        for (equippedCount = 0; equippedCount < 8; equippedCount++) {
            eNumberSet(&work->levelNumber[equippedCount], 0);
            work->levelNumber[equippedCount].work = 0x00fffffa;
        }
        work->resetPending = 0;
        work->state = 2;
        work->visible = 0;
        work->displayMode = 0;
        /* fallthrough */
    }
    case 2:
        work->visible = 0;
        work->displayMode = 0;
        switch (MenuWork.state) {
        case SKILL_LEARN:
            work->state = 10;
            break;
        case SKILL_EQUIP:
            work->state = 20;
            break;
        }
        break;
    case 10:
    case 20:
        work->window.x = 528;
        work->window.y = 160;
        work->visible = 1;
        if (work->state == 10) {
            MenuSkillListChange00();
            work->displayMode = 1;
            work->state = 11;
        } else {
            MenuSkillListChange01();
            work->displayMode = 2;
            work->state = 21;
        }
        work->window.state = 17;
        /* fallthrough */
    case 11:
    case 13:
    case 21:
        target = 528;
        switch (work->state) {
        case 11:
            target = 220;
            break;
        case 21:
            target = 240;
            break;
        }
        MoveSlide(&work->window.x, &target, 3.0f);
        if (work->window.x == target) {
            switch (work->state) {
            case 11:
            case 21:
                work->state = 12;
                break;
            case 13:
                work->state = 2;
                break;
            }
        }
        break;
    case 12:
        if (MenuWork.state == SKILL_COMMAND)
            work->state = 13;
        break;
    }
    if (work->visible == 0)
        return;
    WindowSPMain(&work->window);
    if (work->displayMode != 0) {
        equippedCount = 0;
        if (work->window.item_count > 0) {
            for (i = 0; i < 8; i++) {
                int row = work->window.current_row + i;
                if (row < work->window.item_count) {
                    if (work->displayMode == 1) {
                        work->levelNumber[i].x = work->window.x + work->window.width - 85;
                        work->levelNumber[i].y = work->window.y + i * 24 + 8;
                        work->levelNumber[i].digits = 1;
                        work->levelNumber[i].format = 0;
                        work->levelNumber[i].value = work->window.items[row].status;
                        if (work->window.items[row].flag == 0) {
                            work->levelNumber[i].color_b = -128;
                            work->levelNumber[i].color_g = -128;
                            work->levelNumber[i].color_r = -128;
                        } else {
                            work->levelNumber[i].color_b = 64;
                            work->levelNumber[i].color_g = 64;
                            work->levelNumber[i].color_r = 64;
                        }
                        eNumberMain(&work->levelNumber[i]);
                    } else if (work->window.items[row].status != 0) {
                        work->equipped[equippedCount].x = work->window.x + work->window.width - 39;
                        work->equipped[equippedCount].y = work->window.y + i * 24 + 3;
                        eSpriteMain(&work->equipped[equippedCount]);
                        equippedCount++;
                    }
                }
            }
        }
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char skillEmptyText[1] = "";

const char skillYesNoText[7] = "Yes\nNo";

const char skillMenuText[5] = "Menu";

/* Text: \x01S.Pts */
char skillPointsText[7] = "\001S.Pts";

/* Text: \x01S.Lv */
char skillLevelText[6] = "\001S.Lv";

const SkillL1R1SpriteIdPair skillL1R1SpriteTable[1] = { { .id = { 274, 272 } } };

const SkillL1R1SlideStep skillL1R1SlideStep[1] = { { .side = { 1, -1 } } };

const SkillCategoryEdgeStep skillCategoryEdgeStep[1] = { { .side = { 1, -1 } } };

const char skillSetText[4] = "Set";

const char skillDataPath[30] = "data\\endou\\skill\\skilldat.bin";

void MenuSkill(void)
{
    if (MenuWork.state == 0) {
        void *heap;
        PartyAttackPosition *slot;
        int i;
        int id;

        heap = MainMenuWorkEnd;
        SkillDataBuf = (MenuSkillData *)(((int)heap + 0x7FF) & ~0x7FF);
        xglCdReadFile(skillDataPath, SkillDataBuf, 0, 1);
        heap = (unsigned char *)SkillDataBuf + SKILL_DATA_SIZE;
        MenuSkillPas = heap;
        heap = (unsigned char *)MenuSkillPas + SKILL_PAS_SIZE;
        MenuSkillPas->state = 0;
        MenuSkillInfo = heap;
        heap = (unsigned char *)MenuSkillInfo + SKILL_INFO_SIZE;
        MenuSkillInfo->state = 0;
        MenuSkillStatus = heap;
        heap = (unsigned char *)MenuSkillStatus + SKILL_STATUS_SIZE;
        MenuSkillStatus->state = 0;
        MenuSkillMenu = heap;
        heap = (unsigned char *)MenuSkillMenu + SKILL_MENU_SIZE;
        MenuSkillMenu->state = 0;
        MenuSkillEx = heap;
        heap = (unsigned char *)MenuSkillEx + SKILL_EX_SIZE;
        MenuSkillEx->state = 0;
        MenuSkillL1R1 = heap;
        heap = (unsigned char *)MenuSkillL1R1 + SKILL_L1R1_SIZE;
        MenuSkillL1R1->state = 0;
        MenuSkillSetList = heap;
        heap = (unsigned char *)MenuSkillSetList + SKILL_SET_LIST_SIZE;
        MenuSkillSetList->state = 0;
        MenuSkillCategory = heap;
        heap = (unsigned char *)MenuSkillCategory + SKILL_CATEGORY_SIZE;
        MenuSkillCategory->state = 0;
        MenuSkillList = heap;
        MenuSkillList->state = 0;
        MenuWork.next_state = SKILL_CHARACTER;
        MenuWork.wait = 12;
        MenuWork.flags = 0;
        MenuWork.partyCursor = 0;
        MenuWork.partyCount = 0;
        MenuKeepSelectReset();
        slot = PartyDataGet()->attack_slots;
        for (i = 0; i < 3; i++, slot++) {
            id = slot->party_id;
            if (id != 0) {
                MenuSkillParty[MenuWork.partyCount] = MenuMaryIdChange(id);
                MenuWork.partyCount++;
            }
        }
        for (id = 1; id < 8; id++) {
            if (PartyFriendLockCheck(id, 1) != 0 && PartyAttackerCheck(id) == 0) {
                MenuSkillParty[MenuWork.partyCount] = MenuMaryIdChange(id);
                MenuWork.partyCount++;
            }
        }
    }

    if (MenuWork.state != MenuWork.next_state) {
        MenuWork.state = MenuWork.next_state;
        MenuWork.flags |= MENU_FLAG_ENTER;
    } else {
        MenuWork.flags &= ~MENU_FLAG_ENTER;
    }
    if (MenuWork.wait != 0) {
        MenuWork.wait--;
    }

    switch (MenuWork.state) {
    /* Pick the character. */
    case SKILL_CHARACTER:
        if (MenuWork.flags & MENU_FLAG_ENTER) {
            MenuWork.commandCursor = 0;
            MenuWork.characterNo = MenuSkillParty[MenuWork.partyCursor];
        }
        if (MenuWork.wait != 0) {
            break;
        }
        if (PadData.repeat & (PAD_UP | PAD_RIGHT | PAD_DOWN | PAD_LEFT)) {
            if (MenuWork.partyCount == 1) {
                break;
            }
            switch (PadData.repeat) {
            case PAD_UP:
                MenuWork.partyCursor -= 2;
                break;
            case PAD_DOWN:
                MenuWork.partyCursor += 2;
                break;
            case PAD_LEFT:
                MenuWork.partyCursor -= 1;
                break;
            case PAD_RIGHT:
                MenuWork.partyCursor += 1;
                break;
            }
            if (MenuWork.partyCursor < 0) {
                MenuWork.partyCursor += MenuWork.partyCount;
            }
            if (MenuWork.partyCursor >= MenuWork.partyCount) {
                MenuWork.partyCursor -= MenuWork.partyCount;
            }
            xglSoundEffectNormalID(SE_CURSOR, 0);
            MenuWork.characterNo = MenuSkillParty[MenuWork.partyCursor];
        } else if (PadData.pressed == PAD_CIRCLE) {
            if (MenuMainCharCheck(MenuWork.characterNo) != 0) {
                MenuWork.next_state = SKILL_COMMAND;
                MenuWork.wait = 24;
                xglSoundEffectNormalID(SE_DECIDE, 0);
                MenuWork.char_denied = 0;
            } else {
                xglSoundEffectNormalID(SE_BUZZER, 0);
                MenuWork.char_denied = 1;
            }
        } else if (PadData.pressed == PAD_CROSS) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            MenuWork.next_state = SKILL_LEAVE;
        }
        break;

    /* The command menu: learn, equip or back; L1/R1 page the character. */
    case SKILL_COMMAND:
        if (MenuWork.wait != 0) {
            break;
        }
        if (PadData.pressed == PAD_L1) {
            int i;

            for (i = 0; i < MenuWork.partyCount; i++) {
                if (MenuWork.partyCursor == 0) {
                    MenuWork.partyCursor = MenuWork.partyCount - 1;
                } else {
                    MenuWork.partyCursor--;
                }
                MenuWork.characterNo = MenuSkillParty[MenuWork.partyCursor];
                if (MenuMainCharCheck(MenuWork.characterNo) != 0) {
                    break;
                }
            }
            xglSoundEffectNormalID(SE_CURSOR, 0);
            MenuKeepSelectReset();
        }
        if (PadData.pressed == PAD_R1) {
            int i;

            for (i = 0; i < MenuWork.partyCount; i++) {
                if (MenuWork.partyCursor == MenuWork.partyCount - 1) {
                    MenuWork.partyCursor = 0;
                } else {
                    MenuWork.partyCursor++;
                }
                MenuWork.characterNo = MenuSkillParty[MenuWork.partyCursor];
                if (MenuMainCharCheck(MenuWork.characterNo) != 0) {
                    break;
                }
            }
            xglSoundEffectNormalID(SE_CURSOR, 0);
            MenuKeepSelectReset();
        }
        MenuWork.commandCursor = MenuSelectMove(MenuWork.commandCursor, 3, 0);
        if (PadData.pressed == PAD_CIRCLE) {
            unsigned char command_state[3] = { SKILL_LEARN, SKILL_EQUIP, SKILL_CHARACTER };

            MenuWork.list_select = -1;
            MenuWork.next_state = command_state[MenuWork.commandCursor];
            if (MenuWork.next_state == SKILL_CHARACTER) {
                MenuWork.wait = 12;
                MenuKeepSelectReset();
                xglSoundEffectNormalID(SE_CANCEL, 0);
            } else {
                MenuWork.wait = 8;
                xglSoundEffectNormalID(SE_DECIDE, 0);
            }
        }
        if (PadData.pressed == PAD_CROSS) {
            MenuWork.next_state = SKILL_CHARACTER;
            MenuWork.wait = 12;
            MenuKeepSelectReset();
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        break;

    /* The accessory list: Circle extracts the skill of the accessory. */
    case SKILL_LEARN:
        if (MenuWork.flags & MENU_FLAG_ENTER) {
            MenuWork.list_accessory = 0;
            MenuSkillListChange00();
        }
        if (MenuWork.wait == 0) {
            MenuWork.list_select = WindowSPSelect(&MenuSkillList->window, PadData.repeat);
            if (MenuWork.list_select >= 0) {
                WindowSPKeepSelect(&MenuSkillList->window, &MenuKeepSelect[MenuWork.mode * 5]);
                MenuWork.list_accessory = MenuSortGet(0, MenuWork.list_select);
            }
            if (PadData.repeat == PAD_LEFT) {
                if (MenuWork.mode == 0) {
                    MenuWork.mode = 2;
                } else {
                    MenuWork.mode--;
                }
                WindowSPKeepSelectCheck(&MenuKeepSelect[MenuWork.mode * 5]);
                MenuWork.list_select = MenuSkillListChange00();
                xglSoundEffectNormalID(SE_CURSOR, 0);
            }
            if (PadData.repeat == PAD_RIGHT) {
                if (MenuWork.mode == 2) {
                    MenuWork.mode = 0;
                } else {
                    MenuWork.mode++;
                }
                WindowSPKeepSelectCheck(&MenuKeepSelect[MenuWork.mode * 5]);
                MenuWork.list_select = MenuSkillListChange00();
                xglSoundEffectNormalID(SE_CURSOR, 0);
            }
            if (PadData.pressed == PAD_CIRCLE && MenuWork.list_select >= 0) {
                short accessory = MenuSortGet(0, MenuWork.list_select);
                SkillRecord *data = func_A1A548(accessory);
                PlayerCharacter *character = dataPlChaGet(MenuWork.characterNo);
                int cost = SkillGetPtGet(accessory);

                if (data->prerequisite != 0
                    && SkillCharSkillLvGet(MenuWork.characterNo)
                           >= SkillSetLvGet(accessory)
                    && func_A19698(MenuWork.characterNo, data->prerequisite) == 0
                    && character->skill_points >= cost) {
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    MenuWork.next_state = SKILL_CONFIRM;
                    MenuWork.wait = 8;
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
                MenuWork.list_select = MenuSkillListChange00();
            }
            if (PadData.pressed == PAD_CROSS) {
                MenuWork.next_state = SKILL_COMMAND;
                MenuWork.wait = 8;
                if (MenuCursorKeepCheck() == 0) {
                    MenuWork.commandCursor = 0;
                }
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        } else {
            MenuWork.list_select = WindowSPSelect(&MenuSkillList->window, 0);
            MenuWork.list_accessory = MenuSortGet(0, MenuWork.list_select);
        }
        break;

    /* Yes/No before the extraction. */
    case SKILL_CONFIRM:
        if (MenuWork.flags & MENU_FLAG_ENTER) {
            MenuWork.itemCursor = 0;
        }
        if (MenuWork.wait != 0) {
            break;
        }
        MenuWork.itemCursor = MenuSelectMove(MenuWork.itemCursor, 2, 0);
        if (PadData.pressed == PAD_CIRCLE) {
            if (MenuWork.itemCursor == 0) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                MenuWork.next_state = SKILL_EXTRACT;
            } else {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                MenuWork.next_state = SKILL_LEARN;
            }
        }
        if (PadData.pressed == PAD_CROSS) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            MenuWork.next_state = SKILL_LEARN;
        }
        break;

    /* The extraction: the skill is learned and its points are spent. */
    case SKILL_EXTRACT:
        if (MenuWork.flags & MENU_FLAG_ENTER) {
            short accessory = MenuSortGet(0, MenuWork.list_select);
            SkillRecord *data = func_A1A548(accessory);
            int cost = SkillGetPtGet(accessory);

            func_A19600(MenuWork.characterNo, data->prerequisite);
            SkillCharPointPlus(MenuWork.characterNo, cost);
            xglSoundEffectNormalID(SE_LEARN, 0);
        }
        if (MenuWork.wait == 0 && PadData.pressed == PAD_CIRCLE) {
            MenuWork.next_state = SKILL_LEARN;
        }
        break;

    /* The equip list: Circle sets or removes the skill, Square jumps to the
     * next marked row. */
    case SKILL_EQUIP:
        if (MenuWork.flags & MENU_FLAG_ENTER) {
            MenuSkillListChange01();
        }
        if (MenuWork.wait != 0) {
            break;
        }
        MenuWork.list_select = WindowSPSelect(&MenuSkillList->window, PadData.repeat);
        if (MenuWork.list_select >= 0) {
            WindowSPKeepSelect(&MenuSkillList->window, &MenuKeepSelect[KEEP_EQUIP * 5]);
        }
        if (PadData.repeat == PAD_SQUARE && MenuWork.list_select >= 0) {
            int start = MenuWork.list_select + 1;
            int count = MenuSortCheck(0);
            MenuListEntry *rows = MenuListGet(0);
            int row = 0;
            int i;

            for (i = 0; i < count; i++) {
                row = (start + i) % count;
                if (rows[row].status != 0) {
                    break;
                }
            }
            WindowSPSelectJump(&MenuSkillList->window, row);
            xglSoundEffectNormalID(SE_DECIDE, 0);
        }
        if (PadData.pressed == PAD_CIRCLE && MenuWork.list_select >= 0) {
            short accessory = MenuSortGet(0, MenuWork.list_select);
            int slot = MenuSkillEquipCheck(MenuWork.characterNo, accessory);

            if (slot != 0) {
                MenuSkillEquip(MenuWork.characterNo, 0, slot - 1);
                xglSoundEffectNormalID(SE_DECIDE, 0);
            } else {
                MenuSkillEquip(MenuWork.characterNo, accessory, -1);
                xglSoundEffectNormalID(SE_DECIDE, 0);
            }
            MenuWork.list_select = MenuSkillListChange01();
        }
        if (PadData.pressed == PAD_CROSS) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            MenuWork.next_state = SKILL_COMMAND;
            MenuWork.wait = 8;
            WindowSPKeepSelectCheck(&MenuKeepSelect[KEEP_EQUIP * 5]);
            if (MenuCursorKeepCheck() == 0) {
                MenuWork.commandCursor = 0;
            }
        }
        break;

    /* Leaving the menu. */
    case SKILL_LEAVE:
        if (MenuWork.flags & MENU_FLAG_ENTER) {
            MenuWork.wait = 16;
        }
        if (MenuWork.wait == 0) {
            MenuWork.next_state = SKILL_EXIT;
        }
        break;

    case SKILL_EXIT:
        ChangeTopLevel(0);
        break;
    }

    MenuWork.flags |= MENU_FLAG_DRAW;
    MenuSkillStatusMain();
    MenuSkillPasMain();
    MenuSkillInfoMain();
    MenuSkillMenuMain();
    MenuSkillExMain();
    MenuSkillL1R1Main();
    MenuSkillSetListMain();
    MenuSkillCategoryMain();
    MenuSkillListMain();
    /* Text: {reset}skill */
    xglFontDebugPrintf(0, 64, "\x0bskill");
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* Defined after MenuSkill: the original .sdata holds it at 0x004DB658, after
 * MenuSkill's "\x0bskill" literal at 0x004DB650. */
MenuSkillData *SkillDataBuf = 0;

static const char * msg00_1_0036DD68[] = { skillExtractSetCancelText };

static const char * msg01_2_0036DD70[] = { skillYesNoText, skillExtractQuestionText };

static const char * msg00_3_0036DD78[3] = { skillLevelText, skillPointsText, skillSetSearchText };

static const char * msg00_4_0036DD88[] = { skillNumberedListText };

SkillPasWork * MenuSkillPas = 0;

MenuSkillInfoWork * MenuSkillInfo = 0;

MenuSkillStatusWork * MenuSkillStatus = 0;

MenuSkillMenuWork * MenuSkillMenu = 0;

MenuSkillExWork * MenuSkillEx = 0;

MenuSkillL1R1Work * MenuSkillL1R1 = 0;

MenuSkillSetListWork * MenuSkillSetList = 0;

MenuSkillCategoryWork * MenuSkillCategory = 0;

MenuSkillListWork * MenuSkillList = 0;
