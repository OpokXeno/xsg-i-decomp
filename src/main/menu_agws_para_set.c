#include "common.h"

#include "shared.h"

#include "menu_agws_para_set.h"

/* These labels are the original menu strings referenced by the message table. */

extern const char D_004C6C10[];

extern const char D_004C6C20[];

extern const char D_004C6C00[];

extern const char D_004C6BF0[];

extern const char D_004C6BE0[];

extern const char D_004C6BD0[];

extern const char D_004C6BC0[];

extern const char D_004DAEF8[];

extern const char D_004DAEF0[];

extern const char D_004DAEE8[];

extern const char D_004DAEE0[];

extern const char D_004DAED8[];

extern const char D_004DAED0[];

extern const char D_004DAEC8[];

extern const char D_004DAEC0[];

extern const char D_004DAEB8[];

extern const char D_004DAEB0[];

const char D_004DAFC8[8] = "\001FHP";

const char D_004DAFC0[8] = "\001DPOW";

const char D_004DAFB8[8] = "\001PARM";

const char D_004DAFB0[8] = "\001EARM";

const char D_004DAFA8[8] = "\001AGL";

const char D_004DAFA0[8] = "\001WAGL";

const char D_004DAF98[8] = "\001Weight";

static const char *msg00_0_0036D940[17] = {
    D_004C6C20, D_004DAEF8, D_004C6C10, D_004DAEF0, D_004DAEE8,
    D_004DAEE0, D_004DAED8, D_004C6C00, D_004C6BF0, D_004DAED0,
    D_004DAEC8, D_004DAEC0, D_004C6BE0, D_004DAEB8, D_004DAEB0,
    D_004C6BD0, D_004C6BC0,
};

static const char *msg_10[7] = {
    D_004DAFC8, D_004DAFC0, D_004DAFB8,
    D_004DAFB0, D_004DAFA8, D_004DAFA0,
    D_004DAF98,
};

AgwsParaDisplay MenuAgwsPara = {{0}};

AgwsParaDisplay MenuAgwsPara2 = {{0}};

const unsigned char D_004C6F38[24] = "Menu         Party";

const AgwsCameraPreset D_004C7190 = {{
    { 0.5f, 2.8f, 4.5f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
}};

const AgwsCameraTargets D_004C71B0 = {{
    { 0.5f, 2.8f, 4.5f, 1.0f },
    { -1.5f, 2.0f, 8.0f, 1.0f },
}};

const AgwsModelPositions D_004C71D0 = {{
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.04719758f, 0.0f, 0.0f },
    { 0.0f, -1.04719758f, 0.0f, 0.0f },
    { 0.7853982f, 0.7853982f, 0.0f, 0.0f },
    { 0.7853982f, -0.7853982f, 0.0f, 0.0f },
    { 0.3926991f, 2.0943952f, 0.0f, 0.0f },
    { 0.3926991f, -2.0943952f, 0.0f, 0.0f },
}};

const AgwsModelVector D_004C7240 = {{ 1.0f, 1.0f, 1.0f, 1.0f }};

AgwsSwitchSlideStep D_004DAFE8[4] = {
    { { 1, -1 } }, { { 0, 0 } }, { { 0, 0 } }, { { 0, 0 } },
};

AgwsPasWork *AgwsPas = 0;

AgwsListWork *AgwsList = 0;

AgwsStatusWork *AgwsStatus = 0;

AgwsFaceWork *AgwsFace = 0;

AgwsSwitchWork *AgwsSwitch = 0;

extern const char D_004DAF48[];

void WindowSPSet(AgwsListSPWindow *window);

void WindowSPMain(AgwsListSPWindow *window);

int PartyAttackerCheck(int chrNo);

char *MenuTagTextGet(int index);

int MenuAgwsPilotCheck(int chrNo);

static short WaglGet(AgwsCharPara *para)
{
    short total;
    short weaponId;
    int slot;

    total = 0;
    for (slot = 0; slot < 3; slot++) {
        weaponId = para->weapon[slot];
        if (weaponId != 0) {
            total += func_A1A3D8(MenuRWeaponCheck2(weaponId))->wagl;
        }
    }
    return total;
}

static void ParaSet(AgwsCharPara *para, AgwsParaDisplay *dest, int *attack, int *defense)
{
    dest->value[AGWS_PARA_MAX_HP] = para->maxHp;
    dest->value[AGWS_PARA_ATTACK] = para->attack + attack[0] + attack[1] + attack[2];
    dest->value[AGWS_PARA_PHY_DEFENSE] = para->phyDefense + defense[0];
    dest->value[AGWS_PARA_MAG_DEFENSE] = para->magDefense + defense[1];
    dest->value[AGWS_PARA_STAT8] = (signed char) para->stat8;
    dest->value[AGWS_PARA_WAGL] = WaglGet(para);
    dest->value[AGWS_PARA_STAT9] = para->stat9;
}

void MenuAgwsParaSet(int chrNo, int state)
{
    int attack[4];
    int defense[4];
    AgwsUnitOrg *org;
    short keptHp;
    AgwsCharPara *para;

    if (chrNo != 0) {
        org = func_A191C0(chrNo);
        keptHp = org->hp;
        para = func_00A11108(chrNo, attack, defense);
        ParaSet(para, &MenuAgwsPara2, attack, defense);
        if (state == 0) {
            ParaSet(para, &MenuAgwsPara, attack, defense);
        }
        if (state == 10) {
            ParaSet(para, &MenuAgwsPara, attack, defense);
            org->hp = keptHp;
        }
    }
}

void AgwsPasMain(void)
{
    AgwsPasWork *pas = AgwsPas;
    int tabLength[17];
    short tabTarget[17];
    short slideTarget;
    int i;

    for (i = 0; i < 17; i++)
        tabLength[i] = MenuPasLengthGet(msg00_0_0036D940[i]);

    switch (pas->state) {
    case AGWS_PAS_START:
        pas->color = AGWS_PAS_COLOR;
        WindowDXSet(&pas->window);
        pas->window.x = AGWS_PAS_X_OUT;
        pas->window.y = 8;
        pas->window.color = pas->color;
        pas->window.width = AGWS_PAS_WIDTH;
        pas->window.height = 0x20;
        pas->window.callback = MenuPasWindow;
        pas->window.callbackArg = pas->callbackWork;
        pas->window.state = 1;
        WindowDXMain(&pas->window);
        pas->window.state = 3;
        for (i = 0; i < 17; i++) {
            eMessageSet(&pas->message[i], msg00_0_0036D940[i]);
            pas->message[i].mode = 0x20;
            pas->message[i].x = AGWS_PAS_TAB_PARKED;
            pas->message[i].y = 0xB;
            pas->message[i].color = pas->color + 2;
        }
        pas->mask = 1;
        pas->selectNo = -1;
        pas->state = AGWS_PAS_SLIDE;
        /* fallthrough */
    case AGWS_PAS_SLIDE:
        break;
    default:
        return;
    }

    slideTarget = AGWS_PAS_X_IN;
    for (i = 0; i < 17; i++)
        tabTarget[i] = AGWS_PAS_TAB_PARKED;

    {
        AgwsMenuWork *menu = &MenuWork;

        switch (menu->state) {
        case 0x10:
        case 0x20:
        case 0x22:
            tabTarget[0] = 0x10;
            break;
        case 0x44:
            tabTarget[0] = -(tabLength[0] + AGWS_PAS_TAB_GAP);
            tabTarget[menu->reserveCursor + 1] = 0x10;
            tabTarget[menu->selectedIndex + AGWS_PAS_TAB_SLOT] =
                tabLength[menu->reserveCursor + 1] + 0x10;
            break;
        case 0x46:
            tabTarget[0] = -(tabLength[0] +
                             tabLength[menu->reserveCursor + 1] +
                             AGWS_PAS_TAB_GAP);
            tabTarget[menu->reserveCursor + 1] =
                -(tabLength[menu->reserveCursor + 1] + AGWS_PAS_TAB_GAP);
            tabTarget[menu->selectedIndex + AGWS_PAS_TAB_SLOT] = 0x10;
            tabTarget[AGWS_PAS_TAB_AMMO] =
                tabLength[menu->selectedIndex + AGWS_PAS_TAB_SLOT] + 0x10;
            break;
        case 0x62:
            tabTarget[0] = -(tabLength[0] + AGWS_PAS_TAB_GAP);
            tabTarget[menu->reserveCursor + 1] = 0x10;
            tabTarget[AGWS_PAS_TAB_ACCESSORY] =
                tabLength[menu->reserveCursor + 1] + 0x10;
            break;
        case 0x40:
        case 0x42:
        case 0xC2:
            tabTarget[0] = 0x10;
            tabTarget[menu->reserveCursor + 1] = tabLength[0] + 0x10;
            break;
        default:
            slideTarget = AGWS_PAS_X_OUT;
            break;
        }
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
        for (i = 0; i < 17; i++) {
            MoveSlide(&pas->message[i].x, &tabTarget[i], 3.0f);
            if (pas->message[i].x < AGWS_PAS_TAB_LIMIT)
                eMessageMain(&pas->message[i]);
        }
        endPrintExtFunc(pas->color, 0x66, 0);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_agws_para_set", AgwsInfoMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_agws_para_set", AgwsSelectMain);

void MenuAgwsListMake_Wpn(void)
{
    int *sortRow = MenuSortAddrGet(0);
    AgwsListRow *entries = MenuListGet(0);
    AgwsListSPWindow *window = &AgwsList->window;
    int count;
    int i;

    MenuSortSet(0, 4, MenuWork.weaponSlot[MenuWork.selectedIndex] | 0x2000);
    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        if (MenuWeaponEquipPosCheck(MenuWork.chrNo, (short) sortRow[i],
                                    MenuWork.weaponSlot[MenuWork.selectedIndex],
                                    MenuWork.weaponHand) != 0) {
            entries[i].flag = 0;
        } else {
            entries[i].flag = 1;
        }
    }
    window->width = 229;
    window->rows = 7;
    window->height = 174;
    window->columns = 1;
    window->rowCount = 7;
    window->flags = 0;
    window->items = MenuListGet(0);
    WindowSPItemChange(window);
    WindowSPSetSelect(window, &MenuKeepSelect[MenuWork.weaponSlot[MenuWork.selectedIndex] * 5 - 5]);
}

void MenuAgwsListMake_Gun(void)
{
    int *sortRow = MenuSortAddrGet(0);
    AgwsListRow *entries = MenuListGet(0);
    AgwsListSPWindow *window = &AgwsList->window;
    int count;
    int i;

    MenuSortSet(0, 8, -2);
    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        if (MenuBulletCheck(MenuWork.weaponId, (short) sortRow[i]) != 0) {
            entries[i].flag = 0;
        } else {
            entries[i].flag = 1;
        }
    }
    window->width = 229;
    window->rows = 7;
    window->height = 174;
    window->columns = 1;
    window->rowCount = 7;
    window->flags = 0;
    window->items = MenuListGet(0);
    WindowSPItemChange(window);
    WindowSPSetSelect(window, &MenuKeepSelect[30]);
}

void MenuAgwsListMake_Acc(void)
{
    int *sortRow = MenuSortAddrGet(0);
    AgwsListRow *entries = MenuListGet(0);
    AgwsListSPWindow *window = &AgwsList->window;
    int count;
    int i;

    MenuSortSet(0, 16, -2);
    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        if (MenuAccessoryEquipCheck(MenuWork.chrNo, (short) sortRow[i], MenuWork.equipSlot) != 0) {
            entries[i].flag = 0;
        } else {
            entries[i].flag = 1;
        }
    }
    window->width = 229;
    window->height = 150;
    window->columns = 1;
    window->rows = 6;
    window->rowCount = 7;
    window->flags = 0;
    window->items = MenuListGet(0);
    WindowSPItemChange(window);
    WindowSPSetSelect(window, &MenuKeepSelect[35]);
}

void MenuAgwsListMake_Pilot(void)
{
    AgwsListRow *entries = MenuListGet(0);
    int *sortRow = MenuSortAddrGet(0);
    AgwsListSPWindow *window = &AgwsList->window;
    AgwsUnitOrg *org;
    int count;
    int i;

    MenuSortSet(0, 32, 1);
    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        org = func_A191C0((short) sortRow[i]);
        if (org->agwsId != MenuWork.chrNo && org->agwsId != 0) {
            entries[i].flag = 1;
        } else {
            entries[i].flag = 0;
        }
        if (org->agwsId == MenuWork.chrNo) {
            entries[i].equipped = 1;
        } else {
            entries[i].equipped = 0;
        }
    }
    window->width = 208;
    window->height = 144;
    window->columns = 1;
    window->rows = 5;
    window->rowCount = 3;
    window->flags = (int) D_004C6F38;
    window->items = MenuListGet(0);
    WindowSPItemChange(window);
    WindowSPSetSelect(window, &MenuKeepSelect[40]);
}

void AgwsListMain(void)
{
    AgwsListWork *list = AgwsList;
    short targetX;
    int i;
    int count;
    AgwsListRow *rows;

    switch (list->state) {
    case 0:
        list->color = 0x00FFF000;
        list->window.x = 528;
        list->window.color = list->color;
        list->window.flags = (int)D_004DAF48;
        list->window.y = 64;
        list->window.rowCount = 7;
        list->window.rows = 11;
        list->window.height = 270;
        list->window.state = 0;
        list->window.items = 0;
        WindowSPSet(&list->window);
        list->visible = 1;
        list->state = 2;
        list->selectedRow = 0;
        /* fall through */
    case 2:
        list->visible = 0;
        switch (MenuWork.state) {
        case 0x44: list->state = 10; break;
        case 0x46: list->state = 20; break;
        case 0x62: list->state = 30; break;
        case 0xC2: list->state = 40; break;
        }
        break;
    case 10:
    case 20:
    case 30:
    case 40:
        if (list->state == 40) {
            list->window.x = 528;
            list->window.y = 128;
            MenuAgwsListMake_Pilot();
            list->visible = 2;
            list->state = 41;
        } else {
            list->window.x = -244;
            if (list->state == 30) {
                list->window.y = 126;
            } else {
                list->window.y = 104;
            }
            list->visible = 1;
            if (list->state == 20) {
                list->state = 21;
            } else {
                list->state = 11;
            }
            switch (list->state) {
            case 10: MenuAgwsListMake_Wpn(); break;
            case 20: MenuAgwsListMake_Gun(); break;
            case 30: MenuAgwsListMake_Acc(); break;
            }
        }
        list->window.state = 17;
        /* fall through */
    case 11:
    case 13:
    case 21:
    case 41:
    case 43:
        targetX = 16;
        switch (list->state) {
        case 41: targetX = 288; break;
        case 13: targetX = -244; break;
        case 43: targetX = 528; break;
        }
        MoveSlide(&list->window.x, &targetX, 3.0f);
        if (list->window.x == targetX) {
            switch (list->state) {
            case 21: list->state = 22; break;
            case 11:
            case 41: list->state = 12; break;
            case 13:
            case 43: list->state = 2; break;
            }
        }
        break;
    case 12:
    case 22:
        switch (MenuWork.state) {
        case 0x42: list->state = 13; break;
        case 0x22: list->state = 43; break;
        case 0x40: list->state = 13; break;
        case 0x46:
            if (list->state != 12) break;
            list->state = 20;
            break;
        }
        break;
    }
    if (list->visible != 0) {
        WindowSPMain(&list->window);
        if (list->visible == 2 && MenuWork.listSelect >= 0) {
            count = MenuSortCheck(0);
            rows = MenuListGet(0);
            for (i = 0; i < count; i++) {
                short chrNo = MenuSortGet(0, i);
                if (PartyAttackerCheck(chrNo)) {
                    eTagFontSet(&list->tag[i], MenuTagTextGet(1));
                } else {
                    eTagFontSet(&list->tag[i], MenuTagTextGet(2));
                }
                if ((signed char)rows[i].flag == 1) {
                    list->tag[i].green = 64;
                    list->tag[i].blue = 64;
                    list->tag[i].red = 64;
                } else {
                    list->tag[i].green = 128;
                    list->tag[i].blue = 128;
                    list->tag[i].red = 128;
                }
                list->tag[i].x = list->window.x + 107;
                list->tag[i].depth = list->window.color + 2;
                list->tag[i].y = list->window.y + i * 24 + 7;
                eTagFontMain(&list->tag[i]);
                if (chrNo == MenuAgwsPilotCheck(MenuWork.chrNo)) {
                    eSpriteSet(&list->cursor, 0x60A);
                    list->cursor.x = list->window.x + list->window.width - 24;
                    list->cursor.y = list->window.y + i * 24 + 3;
                    list->cursor.depth = list->window.color + 2;
                    eSpriteMain(&list->cursor);
                }
            }
        }
    }
}

void AgwsStatusMain(void)
{
    AgwsStatusWork *status = AgwsStatus;
    int i;

    switch (status->state) {
    case AGWS_STATUS_START:
        status->color = AGWS_STATUS_COLOR;
        WindowDXSet(&status->window);
        status->window.x = AGWS_STATUS_X_OUT;
        status->window.y = 304;
        status->window.color = status->color;
        status->window.width = 480;
        status->window.height = 64;
        status->window.state = 1;
        WindowDXMain(&status->window);
        status->window.state = 3;
        for (i = 0; i < 7; i++) {
            eTagFontSet(&status->label[i], msg_10[i]);
            status->label[i].color = status->color + 2;
            status->label[i].rgba[0] = 112;
        }
        {
            unsigned char digit[7] = { 4, 3, 3, 3, 2, 2, 4 };

            for (i = 0; i < 7; i++) {
                eNumberSet(&status->number[i], 0);
                status->number[i].color = status->color + 2;
                status->number[i].format = 5;
                status->number[i].digits = digit[i];
            }
        }
        status->selectNo = -1;
        status->state = AGWS_STATUS_SLIDE;
        status->mask = 0;
        /* fallthrough */
    case AGWS_STATUS_SLIDE:
    {
        short slideTarget = AGWS_STATUS_X_OUT;

        switch (MenuWork.state) {
        case 0x40:
            slideTarget = AGWS_STATUS_X_IN;
            if (MenuWork.reserveCursor == 1) {
                status->mask = 1;
            } else {
                status->mask = 0;
            }
            break;
        case 0x22:
            slideTarget = AGWS_STATUS_X_IN;
            break;
        case 0x42:
        case 0x44:
        case 0x46:
        case 0x62:
        case 0xC2:
            status->mask = 0;
            slideTarget = AGWS_STATUS_X_IN;
            break;
        }
        MoveSlide(&status->window.x, &slideTarget, 3.0f);
        WindowDXMain(&status->window);
        if (status->window.state == 3) {
            for (i = 0; i < 6; i++) {
                int rows = 3;

                status->label[i].x = status->window.x + (i / rows) * 160 + 48;
                status->label[i].y = status->window.y + (i % rows) * 20 + 3;
                eTagFontMain(&status->label[i]);
            }
            for (i = 0; i < 6; i++) {
                status->number[i].value =
                    MenuAgwsPara.value[i] + (MenuAgwsPara2.value[i] << 16);
            }
            for (i = 0; i < 6; i++) {
                unsigned short shown[2];

                shown[0] = status->number[i].value;
                shown[1] = status->number[i].value >> 16;
                if (shown[0] < shown[1]) {
                    status->number[i].arrow = 2;
                    status->number[i].arrowHidden = 0;
                } else if (shown[1] < shown[0]) {
                    status->number[i].arrow = 1;
                    status->number[i].arrowHidden = 0;
                } else {
                    status->number[i].arrow = 0;
                    status->number[i].arrowHidden = 1;
                }
                {
                    int rows = 3;

                    status->number[i].x =
                        status->window.x + (i / rows) * 160 + 100;
                    status->number[i].y =
                        status->window.y + (i % rows) * 20 + 3;
                }
                eNumberMain(&status->number[i]);
            }
        }
        break;
    }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_agws_para_set", AgwsWeapon2Main);

INCLUDE_ASM("asm/main/nonmatchings/menu_agws_para_set", AgwsNameMain);

void AgwsFaceMain(void)
{
    AgwsFaceWork *self = AgwsFace;
    short target;

    switch (self->state) {
    case 0:
        self->color = 0x00FFFFF0;
        WindowDXSet(&self->window);
        self->window.x = -103;
        self->window.color = self->color;
        self->window.y = 128;
        self->window.width = 71;
        self->window.height = 100;
        self->window.state = 1;
        WindowDXMain(&self->window);
        self->window.state = 3;
        eSpriteSet(&self->face, 0);
        self->visible = 0;
        self->state = 2;
        break;
    case 2:
        break;
    default:
        return;
    }

    target = -103;
    if (MenuWork.state == 0xC2) {
        if (MenuWork.flags & 1) {
            self->visible = 0;
        } else {
            self->visible = 1;
        }
        target = 208;
    }
    if (self->visible != 0) {
        MoveSlide(&self->window.x, &target, 3.0f);
        if (self->window.x == -103) {
            self->visible = 0;
        }
        WindowDXMain(&self->window);
        if (MenuWork.listSelect >= 0) {
            eSpriteSet(&self->face, MenuFaceEpidGet(MenuSortGet(0, MenuWork.listSelect), 0));
            self->face.x = self->window.x + 3;
            self->face.y = self->window.y + 3;
            self->face.work = self->window.color + 2;
            eSpriteMain(&self->face);
        }
    }
}

void AgwsSwitchMain(void)
{
    AgwsSwitchWork *self = AgwsSwitch;
    int i;

    switch (self->state) {
    case 0:
        self->spriteWork = 0x00FFFFFF;
        eSpriteSet(&self->sprite[0], 0x112);
        eSpriteSet(&self->sprite[1], 0x110);
        self->sprite[0].x = -45;
        self->sprite[1].x = 557;
        for (i = 0; i < 2; i++) {
            self->sprite[i].work = self->spriteWork;
            self->sprite[i].y = 224;
        }
        self->slideOffset[1] = 0;
        self->state = 2;
        self->slideOffset[0] = 0;
        break;
    case 2:
        break;
    default:
        return;
    }

    {
        short target[2];

        target[0] = -45;
        target[1] = 557;
        if (MenuWork.state == 34) {
            target[0] = 16;
            target[1] = 467;
            if (PadData.half_2a & PAD_L1) {
                self->slideOffset[0] = -6;
            } else if (PadData.half_2a & PAD_R1) {
                self->slideOffset[1] = 6;
            }
        }

        {
            AgwsSwitchSlideStep step = D_004DAFE8[0];

            for (i = 0; i < 2; i++) {
                if (self->slideOffset[i] != 0) {
                    self->slideOffset[i] = self->slideOffset[i] + step.side[i];
                }
            }
        }

        for (i = 0; i < 2; i++) {
            MoveSlide(&self->sprite[i].x, &target[i], 3.0f);
            self->sprite[i].x = self->sprite[i].x + self->slideOffset[i];
            eSpriteMain(&self->sprite[i]);
        }
    }
}

void MenuAgwsCameraSet(void)
{
    AgwsCameraPreset preset = D_004C7190;
    StudioCamera *camera;

    camera = xglStudioGetCamera2(0);
    xglCameraInit(camera);
    camera->position = preset.vectors[0];
    camera->state = 1;
    camera->rotation = preset.vectors[1];
}

void MenuAgwsCameraMove(void)
{
    Vector4 *position = &xglStudioGetCamera2(0)->position;
    AgwsCameraTargets targets = D_004C71B0;
    int target;

    switch (MenuWork.state) {
    case 0x10:
    case 0x22:
    case 0xC2:
        target = 0;
        break;
    default:
        target = 1;
        break;
    }
    position->x += (targets.value[target][0] - position->x) * 0.1f;
    position->y += (targets.value[target][1] - position->y) * 0.1f;
    position->z += (targets.value[target][2] - position->z) * 0.1f;
}

void MenuAgwsModelMain(AgwsModelUnit *self)
{
    AgwsModelActor *actor = self->actor;
    AgwsModelTransform *transform;
    int weaponIndex;

    if (actor == 0) {
        if (MenuWork.modelBreak != 0 || MenuWork.state == 0x10) {
            MenuModelUnitBreak(self);
            MenuWork.modelBreak = 0;
        }
    }
    if (self->active == 0) {
        return;
    }
    transform = &actor->transform;
    if (MenuWork.state == 0x22) {
        MenuModelControl2(self, 1);
    } else {
        AgwsModelPositions positions = D_004C71D0;
        AgwsModelVector rotation = D_004C7240;
        Vector4 *position = &transform->position;
        Vector4 *rotationOf = &transform->rotation;
        Vector4 *from = &positions.position[0];

        switch (MenuWork.state) {
        case 0x42:
        case 0x44:
        case 0x46:
            from = &positions.position[MenuWork.weaponSlot[MenuWork.selectedIndex]];
            break;
        }
        position->x += (from->x - position->x) * 0.2f;
        position->y += (from->y - position->y) * 0.2f;
        position->z += (from->z - position->z) * 0.2f;
        rotationOf->x += (rotation.vector.x - rotationOf->x) * 0.2f;
        rotationOf->y += (rotation.vector.y - rotationOf->y) * 0.2f;
        rotationOf->z += (rotation.vector.z - rotationOf->z) * 0.2f;
    }
    if (MenuWork.modelBreak != 0 || MenuWork.state == 0x10) {
        self->state = 30;
        MenuWork.modelBreak = 0;
    }
    switch (self->state) {
    case 0:
        MenuModelUnitOpen(self, 2);
        if (MenuEquipStealMaskCheck() == 0) {
            for (weaponIndex = 0; weaponIndex < 3; weaponIndex++) {
                MenuModelWeaponCreate(self, 0, weaponIndex);
            }
        }
        self->state = 10;
        break;
    case 10:
        if (MenuWork.weaponChange != 0) {
            if (MenuWork.weaponHand >= 0) {
                if (MenuWork.weaponHand < 3) {
                    int hand = MenuWork.weaponHand;
                    short keptWeaponId = self->org->weaponId[hand];
                    signed char keptHand = self->org->hand[hand];

                    if (MenuWork.weaponChange == 1) {
                        self->org->weaponId[hand] = MenuWork.weaponId;
                        self->org->hand[hand] = MenuWork.weaponHandKind;
                        MenuModelWeaponCreate(self, 0, MenuWork.weaponHand);
                        self->org->weaponId[hand] = keptWeaponId;
                        self->org->hand[hand] = keptHand;
                    } else {
                        MenuModelWeaponCreate(self, 0, MenuWork.weaponHand);
                    }
                    MenuWork.weaponChange = 0;
                }
            }
        }
        self->state = 20;
        /* fallthrough */
    case 20:
        if (MenuWork.weaponChange != 0) {
            self->state = 10;
        }
        break;
    case 30:
        MenuModelUnitOpen(self, 1);
        self->state = 31;
        /* fallthrough */
    case 31:
        if (transform->fade == 0.0f) {
            self->taskPhase = -1;
        }
        break;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_agws_para_set", MenuAgws);
