#include "common.h"

static unsigned short MenuTopCommandLock;

#include "menu_top_command_check.h"

static inline int MenuTopStateGet(void)
{
    return MenuWork.state;
}

static inline MenuStatusValuePair MenuStatusEncodeValue(short current, short maximum)
{
    MenuStatusValuePair value;

    value.halfwords[0] = current;
    value.halfwords[1] = maximum;
    return value;
}

int MenuTopCommandCheck(int commandId)
{
    unsigned int commandBit = 1 << (commandId - 1);
    return (commandBit & ~MenuTopCommandLock) != 0;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_top_command_check", TopStatusWinMain);

void TopInfoMain(void)
{
    MenuTopInfo *info = TopInfo;
    short slideTarget;

    switch (info->state) {
    case 0:
        WindowDXSet(&info->window);
        info->window.x = -0x10;
        info->window.y = 0x1E0;
        info->window.width = 0x220;
        info->window.height = 0x36;
        info->window.color = 0xF000F0;
        info->window.draw = MenuInfoWindow;
        info->window.panel = &info->panel;
        info->panel.x = 0;
        info->panel.y = 0;
        info->window.state = 1;
        WindowDXMain(&info->window);
        info->window.state = 3;
        info->slide = -1;
        info->enabled = 1;
        info->state = 2;
        info->panel.flag = 0;
    case 2:
        slideTarget = 0x182;
        switch (MenuWork.state) {
        case 0x20:
            info->slide = -1;
        case 0x21:
            info->panel.flag = 0;
            info->panel.y = 0;
            break;
        default:
            slideTarget = 0x1E0;
            break;
        }
        if (MenuWork.topCursor < 9) {
            info->panel.text = text[MenuWork.topCursor];
        } else {
            info->panel.text = D_004DA890;
        }
        MoveSlide(&info->window.y, &slideTarget, 5.0f);
        WindowDXMain(&info->window);
        break;
    }
}

void TopMenuWinMain(void)
{
    MenuTopMenuWin *menu = TopMenuWin;
    int menuState;
    short slideTarget;
    int i;

    switch (menu->state) {
    case 0:
        menu->color = 0xF000F0;
        for (i = 0; i < 9; i++) {
            if (MenuTopCommandCheck(MenuTopCommand[i]) == 0) {
                command_0[i].locked = 1;
            } else {
                command_0[i].locked = 0;
            }
        }
        WindowDXSet(&menu->window);
        menu->window.x = 0x220;
        menu->window.y = 0x1C;
        menu->window.color = menu->color;
        menu->window.width = 0xC2;
        menu->window.height = 0xDE;
        menu->window.draw = MenuSelectWindow;
        menu->window.panel = &menu->panel;
        menu->window.title = D_004DA8B8;
        menu->panel.commands = command_0;
        menu->panel.cursor = 0;
        menu->panel.rows = 2;
        menu->window.state = 1;
        WindowDXMain(&menu->window);
        menu->window.state = 3;
        menu->enabled = 1;
        menu->state = 2;
    case 2:
        slideTarget = 0x13C;
        menuState = MenuWork.state;
        if (menuState < 0x22) {
            if (menuState >= 0x20) {
                menu->panel.cursor = MenuWork.topCursor;
            } else {
                slideTarget = 0x220;
            }
        } else {
            slideTarget = 0x220;
        }
        MoveSlide(&menu->window.x, &slideTarget, 5.0f);
        WindowDXMain(&menu->window);
        break;
    }
}

void TopFaceExWinMain(void)
{
    MenuTopFaceEx *face = TopFaceExWin;
    short slideTarget[3];
    int count;
    int i;
    int j;

    switch (face->state) {
    case 0:
        face->first = 0;
        face->color = 0xFFFFF0;
        count = 0;
        for (j = 1; j < 8; j++) {
            if (PartyFriendLockCheck(j, 1) != 0 && PartyAttackerCheck(j) == 0) {
                face->ids[count] = j;
                count++;
            }
        }
        face->count = count;
        if (count == 0) {
            face->first = 1;
        }
        for (j = face->first; j < 3; j++) {
            eRibbonSet(&face->ribbon[j].ribbon, 3);
            face->ribbon[j].ribbon.x = 0x280;
            face->ribbon[j].ribbon.y = ((j - face->first) << 5) + 0x100;
            face->ribbon[j].ribbon.color = face->color;
            face->ribbon[j].ribbon.width = 0xF8;
            face->ribbon[j].ribbon.height = 0x18;
        }
        for (j = 0; j < face->count; j++) {
            eSpriteSet(&face->sprite[j], MenuFaceEpidGet(face->ids[j], 1));
            face->sprite[j].color = face->color + 2;
        }
        for (i = 0; i < 3; i++) {
            eTagFontSet(&face->tag[i], msg_1_0036C360[i]);
            face->tag[i].color = face->color + 2;
        }
        for (i = 0; i < 2; i++) {
            eNumberSet(&face->number[i], 0);
            face->number[i].color = face->color + 2;
        }
        face->enabled = 1;
        face->state = 2;
    case 2:
        for (i = 0; i < 3; i++) {
            slideTarget[i] = 0x138;
        }
        if (MenuTopStateGet() >= 0x22 || MenuTopStateGet() < 0x20) {
            for (i = 0; i < 3; i++) {
                slideTarget[i] = 0x280;
            }
        }
        for (i = face->first; i < 3; i++) {
            MoveSlide(&face->ribbon[i].ribbon.x, &slideTarget[i], 5.0f);
            eRibbonMain(&face->ribbon[i].ribbon);
        }
        for (i = face->first; i < 3; i++) {
            face->tag[i].x = face->ribbon[i].ribbon.x + 0x42;
            face->tag[i].y = face->ribbon[i].ribbon.y + 4;
            eTagFontMain(&face->tag[i]);
        }
        for (j = 0; j < face->count; j++) {
            face->sprite[j].x = face->ribbon[0].ribbon.x + j * 20 + 0x4A;
            face->sprite[j].y = face->ribbon[0].ribbon.y;
            eSpriteMain(&face->sprite[j]);
        }
        face->time = *PartyTimeUpDate();
        PartyTimeDispChange(&face->time);
        face->number[0].x = face->ribbon[1].ribbon.x + 0x4A;
        face->number[0].digits = 2;
        face->number[0].y = face->ribbon[1].ribbon.y + 4;
        face->number[0].value = face->time.packed;
        eNumberMain(&face->number[0]);
        face->number[1].x = face->ribbon[2].ribbon.x + 0x4A;
        face->number[1].y = face->ribbon[2].ribbon.y + 4;
        face->number[1].width = 9;
        face->number[1].digits = 0xE;
        face->number[1].value = dataMoneyBoxChk();
        eNumberMain(&face->number[1]);
        break;
    }
}

void TopMenu(void)
{
    int modelIndex;
    int soundEffect;
    MenuTopWork *initialWork = &MenuWork;

    switch (initialWork->state) {
    case 0:
    {
        unsigned char *workEnd = MainMenuWorkEnd;
        unsigned char *window;

        TopStatusWin = (void *)workEnd;
        window = workEnd;
        workEnd += 0x1d84;
        window[0] = 0;
        TopInfo = (void *)workEnd;
        window = workEnd;
        workEnd += 0x350;
        window[0] = 0;
        TopMenuWin = (void *)workEnd;
        window = workEnd;
        workEnd += 0x794;
        window[4] = 0;
        TopFaceExWin = (void *)workEnd;
        workEnd[0] = 0;
        if ((MenuScenarioNo - 0x90u) < 9u) {
            MenuTopCommandLock = 0x332;
        } else {
            MenuTopCommandLock = 0;
        }
        if (((MenuTopPartyData *)PartyDataGet())->takeAgwsMask == 0) {
            MenuTopCommandLock |= 8;
        }
        if (PartyFriendLockCheck(3, 1) == 0) {
            MenuTopCommandLock |= 0x20;
        }
        if (MenuScenarioNo == 1) {
            MenuTopCommandLock |= 0x20;
        }
        initialWork->state = 0x20;
        break;
    }
    case 0x20:
        MenuWork.state = 0x21;
        MenuWork.countdown = 0x10;
        if (MenuCursorKeepCheck() == 0) {
            MenuWork.topCursor = 0;
        }
        /* fall through */
    case 0x21:
        if (MenuWork.countdown == 0) {
            MenuWork.topCursor = MenuSelectMove(MenuWork.topCursor, 9, 0);
            if ((PadData.half_2a & 0x20) != 0) {
                unsigned int commandBit =
                    1u << ((unsigned int)MenuTopCommand[MenuWork.topCursor] - 1u);
                if ((commandBit & ~(unsigned int)MenuTopCommandLock) != 0) {
                    soundEffect = 1;
                    MenuWork.state = 0xf0;
                } else {
                    soundEffect = 5;
                }
                xglSoundEffectNormalID(soundEffect, 0);
            } else if ((PadData.half_2a & 0x40) != 0) {
                xglSoundEffectNormalID(2, 0);
                MenuWork.state = 0xf0;
                MenuWork.topCursor = 9;
            }
        } else {
            MenuWork.countdown--;
        }
        break;
    case 0xf0:
        MenuWork.state = 0xf1;
        if (MenuWork.topCursor == 6) {
            MenuModelMemorySet(12);
            MenuModelMenuMotionLoad();
            for (modelIndex = 0; modelIndex < 3; modelIndex++) {
                if (((MenuTopPartyData *)PartyDataGet())
                        ->slot[modelIndex].id < 0x11) {
                    MenuModelCreate(&MenuModelOut[modelIndex],
                        ((MenuTopPartyData *)PartyDataGet())
                            ->slot[modelIndex].id);
                }
            }
        }
        MenuWork.countdown = 12;
        /* fall through */
    case 0xf1:
        if (MenuWork.countdown == 0) {
            MenuWork.state = 0xff;
        } else {
            MenuWork.countdown--;
        }
        break;
    case 0xff:
        ChangeTopLevel(MenuTopCommand[MenuWork.topCursor]);
        break;
    default:
        break;
    }

    TopStatusWinMain();
    TopInfoMain();
    TopMenuWinMain();
    TopFaceExWinMain();
    if (debug_flag != 0) {
        xglFontDebugPrintf(0, 0x38, D_004DA8D0);
        xglFontDebugHex(0, 0x40, MenuWork.topCursor, 2);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_top_command_check", tskMenuTai);

INCLUDE_ASM("asm/main/nonmatchings/menu_top_command_check", tskMenuTaiPointa);

INCLUDE_ASM("asm/main/nonmatchings/menu_top_command_check", MenuBackModelSet);

void MenuStatusDisp(MenuStatusPanelWindow *window, MenuStatusPanel *work)
{
    int chrNo;
    MenuStatusExp *exp;
    int showStats;
    MenuStatusPara *para;
    int attack[4];
    int defense[4];
    int i;
    int left;
    int top;
    int nameY;
    int ribbonRow;
    int pass;

    showStats = 1;
    if (work->chrNo < 0) {
        chrNo = work->guestChrNo;
        if (work->chrNo == -2) {
            showStats = 0;
        }
    } else {
        chrNo = work->chrNo;
    }
    para = func_00A11108(chrNo, attack, defense);
    exp = func_A19210(chrNo);

    switch (window->state) {
    case 1:
        work->maxHp = para->maxHp;
        work->maxEp = para->maxEp;
        work->epMark = 0;
        work->hpMark = 0;
        eMessageSet(&work->name, MenuCharNameGet(chrNo));
        work->name.mode = 32;
        work->name.color = 0xFFFFFF;
        {
            MenuStatusTagInit init[8] = {
                { 4, 1, para->hp },
                { 4, 1, para->maxHp },
                { 3, 1, para->ep },
                { 3, 1, para->maxEp },
                { 2, 0, para->level },
                { 6, 1, exp->nextExp },
                { 1, 0, para->boost },
                { 6, 1, exp->exp },
            };
            int tag;

            for (tag = 0; tag < 8; tag++) {
                work->tag[tag].r = work->tag[tag].g = work->tag[tag].b = work->tag[tag].a = 128;
                work->tag[tag].value = init[tag].value;
                work->tag[tag].type = init[tag].type;
                work->tag[tag].mode = init[tag].mode;
                work->tag[tag].color = window->color + 2;
            }
        }
        work->shadeRgba[3][2] = 128;
        work->shadeRgba[2][2] = 128;
        work->shadeRgba[1][2] = 128;
        work->shadeRgba[0][2] = 128;
        work->shadeRgba[3][1] = 128;
        work->shadeRgba[2][1] = 128;
        work->shadeRgba[1][1] = 128;
        work->shadeRgba[0][1] = 128;
        work->shadeRgba[3][0] = 128;
        work->shadeRgba[2][0] = 128;
        work->shadeRgba[1][0] = 128;
        work->shadeRgba[0][0] = 128;
        work->shadeRgba[0][3] = 128;
        work->shadeRgba[1][3] = 128;
        work->shadeRgba[3][3] = 16;
        work->shadeRgba[2][3] = 0;
        work->shadeCorner[0].color = work->shadeCorner[1].color =
            work->shadeCorner[2].color = work->shadeCorner[3].color = window->color + 1;
        work->shadeTexture = work->chrNo + 0xC00;
        eRibbonSet(&work->ribbon, 0);
        for (i = 0; i < 6; i++) {
            switch (i) {
            case 0:
            case 1:
            case 2:
            case 3:
                work->ribbon.vertex[i].r = 32;
                work->ribbon.vertex[i].g = 52;
                work->ribbon.vertex[i].b = 52;
                break;
            case 4:
            case 5:
                work->ribbon.vertex[i].b = 0;
                work->ribbon.vertex[i].g = 0;
                work->ribbon.vertex[i].r = 0;
                break;
            }
            if (i >= 2 && i < 4) {
                work->ribbon.vertex[i].a = 32;
            } else {
                work->ribbon.vertex[i].a = 0;
            }
        }
        for (i = 0; i < 4; i++) {
            work->nameShade.vertex[i].b = 16;
            work->nameShade.vertex[i].g = 16;
            work->nameShade.vertex[i].r = 16;
            work->nameShade.vertex[i].color = window->color + 1;
        }
        work->nameShade.vertex[3].a = 0;
        work->nameShade.vertex[1].a = 128;
        work->nameShade.vertex[0].a = 128;
        work->nameShade.vertex[2].a = 0;
        work->nameShade.flags = 0;
        for (i = 0; i < 4; i++) {
            work->headerBand.vertex[i].b = 1;
            work->headerBand.vertex[i].g = 1;
            work->headerBand.vertex[i].r = 1;
            work->headerBand.vertex[i].color = window->color + 2;
        }
        work->headerBand.vertex[3].a = 0;
        work->headerBand.vertex[1].a = 128;
        work->headerBand.vertex[0].a = 128;
        work->headerBand.vertex[2].a = 0;
        work->headerBand.flags = 0;
        for (i = 0; i < 4; i++) {
            work->statsBand.vertex[i].b = 1;
            work->statsBand.vertex[i].g = 1;
            work->statsBand.vertex[i].r = 1;
            work->statsBand.vertex[i].color = window->color + 2;
        }
        work->statsBand.vertex[3].a = 0;
        work->statsBand.vertex[1].a = 128;
        work->statsBand.vertex[0].a = 128;
        work->statsBand.vertex[2].a = 0;
        work->statsBand.flags = 0;
        for (i = 0; i < 4; i++) {
            work->headerFill.vertex[i].b = 1;
            work->headerFill.vertex[i].g = 1;
            work->headerFill.vertex[i].r = 1;
            work->headerFill.vertex[i].a = 32;
            work->headerFill.vertex[i].color = window->color + 3;
        }
        work->headerFill.flags = 0;
        for (i = 0; i < 4; i++) {
            work->statsFill.vertex[i].b = 1;
            work->statsFill.vertex[i].g = 1;
            work->statsFill.vertex[i].r = 1;
            work->statsFill.vertex[i].a = 32;
            work->statsFill.vertex[i].color = window->color + 3;
        }
        work->statsFill.flags = 0;
        for (i = 0; i < 2; i++) {
            work->headerLine.vertex[i].b = 224;
            work->headerLine.vertex[i].g = 224;
            work->headerLine.vertex[i].r = 224;
            work->headerLine.vertex[i].a = 128;
            work->headerLine.vertex[i].color = window->color + 5;
        }
        work->headerLine.flags = 0;
        for (i = 0; i < 2; i++) {
            work->statsLine.vertex[i].b = 240;
            work->statsLine.vertex[i].g = 240;
            work->statsLine.vertex[i].r = 240;
            work->statsLine.vertex[i].a = 128;
            work->statsLine.vertex[i].color = window->color + 5;
        }
        work->statsLine.flags = 0;
        for (i = 0; i < 2; i++) {
            work->divider.vertex[i].b = 240;
            work->divider.vertex[i].g = 240;
            work->divider.vertex[i].r = 240;
            work->divider.vertex[i].a = 128;
            work->divider.vertex[i].color = window->color + 5;
        }
        work->divider.flags = 0;
        break;

    case 3:
    {
        short shown[2];

        left = window->x + work->offsetX;
        top = window->y + work->offsetY;
        work->box.x = window->x + 3;
        work->box.y = window->y + 3;
        work->box.color = window->color;
        work->box.width = window->width - 6;
        work->box.height = window->height - 6;
        endPrintExtFunc(work->box.color, 5, &work->box);

        ribbonRow = 1;
        if (work->hpMark != 2) {
            ribbonRow = (work->epMark == 2) ? 2 : 0;
        }
        if (ribbonRow != 0) {
            work->ribbon.x = left - (5 - ribbonRow) * 10 + 171;
            work->ribbon.y = top + ribbonRow * 16 + 15;
            work->ribbon.width = (5 - ribbonRow) * 20 + 23;
            work->ribbon.height = 13;
            work->ribbon.color = window->color;
            for (pass = 0; pass < 6; pass++) {
                eRibbonMain(&work->ribbon);
            }
        }

        work->name.x = left + 82;
        work->name.y = nameY = top + 3;
        work->name.color = window->color + 15;
        work->name.text = MenuCharNameGet(chrNo);
        eMessageDraw(&work->name);

        if (showStats) {
            if (work->chrNo >= 0) {
                work->label[0].x = left + 138;
            } else {
                work->label[0].x = left + 114;
            }
            work->label[0].color = window->color + 15;
            work->label[0].y = top + 29;
            work->label[0].text = "\001Hp";
            work->label[0].r = 112;
            work->label[0].g = 128;
            work->label[0].b = 128;
            work->label[0].a = 128;
            endPrintExtFunc(work->label[0].color, 7, &work->label[0]);
            eNumberSet(&work->number[0], 0);
            if (work->hpMark == 0) {
                shown[0] = para->hp;
                shown[1] = para->maxHp;
                work->number[0].format = 13;
            } else {
                shown[0] = para->maxHp;
                shown[1] = work->maxHp;
                work->number[0].format = 6;
            }
            if (work->chrNo >= 0) {
                work->number[0].x = left + 186;
            } else {
                work->number[0].x = left + 161;
            }
            work->number[0].y = top + 32;
            work->number[0].color = window->color + 15;
            work->number[0].value = *(MenuStatusValuePair *)shown;
            work->number[0].digits = 4;
            work->number[0].flag[0] = 0;
            if (work->hpMark != 0) {
                if (para->maxHp < work->maxHp) {
                    work->number[0].flag[1] = 2;
                } else if (work->maxHp < para->maxHp) {
                    work->number[0].flag[1] = 1;
                } else {
                    work->number[0].flag[1] = 0;
                }
            } else {
                work->number[0].flag[1] = 0;
            }
            work->number[0].flag[2] = 1;
            eNumberMain(&work->number[0]);
        }
        if (showStats) {
            if (work->chrNo >= 0) {
                work->label[1].x = left + 138;
            } else {
                work->label[1].x = left + 114;
            }
            work->label[1].color = window->color + 15;
            work->label[1].y = top + 45;
            work->label[1].text = "\001Ep";
            work->label[1].r = 112;
            work->label[1].g = 128;
            work->label[1].b = 128;
            work->label[1].a = 128;
            endPrintExtFunc(work->label[1].color, 7, &work->label[1]);
            eNumberSet(&work->number[1], 0);
            if (work->epMark == 0) {
                shown[0] = para->ep;
                shown[1] = para->maxEp;
                work->number[1].format = 4;
            } else {
                shown[0] = para->maxEp;
                shown[1] = work->maxEp;
                work->number[1].format = 6;
            }
            if (work->chrNo >= 0) {
                work->number[1].x = left + 186;
            } else {
                work->number[1].x = left + 161;
            }
            work->number[1].y = top + 48;
            work->number[1].color = window->color + 15;
            work->number[1].value = *(MenuStatusValuePair *)shown;
            work->number[1].digits = 2;
            work->number[1].flag[0] = 0;
            if (work->epMark != 0) {
                if (para->maxEp < work->maxEp) {
                    work->number[1].flag[1] = 2;
                } else if (work->maxEp < para->maxEp) {
                    work->number[1].flag[1] = 1;
                } else {
                    work->number[1].flag[1] = 0;
                }
            } else {
                work->number[1].flag[1] = 0;
            }
            work->number[1].flag[2] = 1;
            eNumberMain(&work->number[1]);
        }

        work->label[2].color = window->color + 15;
        work->label[2].x = left + 205;
        work->label[2].y = top + 7;
        work->label[2].text = "\001Lv";
        work->label[2].r = work->label[2].g = work->label[2].b = work->label[2].a = 128;
        endPrintExtFunc(work->label[2].color, 7, &work->label[2]);
        eNumberSet(&work->number[2], 0);
        work->number[2].x = work->label[2].x + 8;
        work->number[2].color = window->color + 15;
        work->number[2].y = top + 7;
        work->number[2].value.scalar = para->level;
        work->number[2].digits = 2;
        work->number[2].format = 0;
        eNumberMain(&work->number[2]);

        if (showStats) {
            /* Guest panels keep the party column here; the original still
             * tests chrNo (the lb survives) with both arms equal. */
            if (work->chrNo < 0) {
                work->label[3].x = left + 138;
            } else {
                work->label[3].x = left + 138;
            }
            work->label[3].color = window->color + 15;
            work->label[3].y = top + 63;
            work->label[3].text = "\001Nextlv";
            work->label[3].r = 112;
            work->label[3].g = 128;
            work->label[3].b = 128;
            work->label[3].a = 128;
            endPrintExtFunc(work->label[3].color, 7, &work->label[3]);
            eNumberSet(&work->number[3], 0);
            if (work->chrNo < 0) {
                work->number[3].x = left + 146;
            } else {
                work->number[3].x = left + 146;
            }
            work->number[3].y = top + 66;
            work->number[3].color = window->color + 15;
            if (para->level == 99) {
                work->number[3].value.scalar = -1;
            } else {
                work->number[3].value.scalar = exp->nextExp;
            }
            work->number[3].digits = 6;
            work->number[3].format = 1;
            eNumberMain(&work->number[3]);
        }
        if (showStats) {
            if (work->chrNo >= 0 && PartyLeaderCheck(chrNo)) {
                work->leader.x = left + 83;
                work->leader.y = top + 35;
                work->leader.color = window->color + 15;
                work->leader.spriteId = 811;
                endSpriteSet(&work->leader, 255);
                endPrintExtFunc(work->leader.color, 2, &work->leader);
            }
        }
        if (showStats) {
            work->label[5].y = top + 79;
            work->label[5].text = "\001Exp";
            work->label[5].color = window->color + 15;
            work->label[5].r = 112;
            work->label[5].g = 128;
            work->label[5].b = 128;
            work->label[5].a = 128;
            work->label[5].x = left + 138;
            endPrintExtFunc(work->label[5].color, 7, &work->label[5]);
            eNumberSet(&work->number[5], 0);
            work->number[5].x = left + 146;
            work->number[5].y = top + 82;
            work->number[5].color = window->color + 15;
            work->number[5].value.scalar = exp->exp;
            work->number[5].digits = 7;
            work->number[5].format = 1;
            eNumberMain(&work->number[5]);
        }

        work->nameShade.vertex[0].x = work->nameShade.vertex[1].x = left + 3;
        work->nameShade.vertex[0].y = work->nameShade.vertex[2].y = nameY;
        work->nameShade.vertex[2].x = work->nameShade.vertex[3].x = (float)left + 148.5f - 3.0f;
        work->nameShade.vertex[0].color = work->nameShade.vertex[1].color =
            work->nameShade.vertex[2].color = work->nameShade.vertex[3].color = window->color + 1;
        work->nameShade.vertex[1].y = work->nameShade.vertex[3].y = window->height + top - 3;
        endPrintExtFunc(work->nameShade.vertex[0].color, 3, &work->nameShade);

        work->headerBand.vertex[0].x = work->headerBand.vertex[1].x = left + 80;
        work->headerBand.vertex[2].x = work->headerBand.vertex[3].x = left + (window->width - work->offsetX) - 3;
        if (work->chrNo == -2) {
            work->headerBand.vertex[2].x = work->headerBand.vertex[3].x = left + 238;
        }
        work->headerBand.vertex[0].y = work->headerBand.vertex[2].y = window->y + 3;
        work->headerBand.vertex[0].color = work->headerBand.vertex[1].color =
            work->headerBand.vertex[2].color = work->headerBand.vertex[3].color = window->color + 2;
        work->headerBand.vertex[1].y = work->headerBand.vertex[3].y = work->headerBand.vertex[0].y + 23;
        endPrintExtFunc(work->headerBand.vertex[0].color, 3, &work->headerBand);

        work->headerFill.vertex[0].x = work->headerFill.vertex[1].x = left + 3;
        work->headerFill.vertex[2].x = work->headerFill.vertex[3].x = left + (window->width - work->offsetX) - 3;
        if (work->chrNo == -2) {
            work->headerFill.vertex[2].x = work->headerFill.vertex[3].x = left + 238;
        }
        work->headerFill.vertex[0].y = work->headerFill.vertex[2].y = window->y + 3;
        work->headerFill.vertex[0].color = work->headerFill.vertex[1].color =
            work->headerFill.vertex[2].color = work->headerFill.vertex[3].color = window->color + 4;
        work->headerFill.vertex[1].y = work->headerFill.vertex[3].y = work->headerFill.vertex[0].y + 23;
        endPrintExtFunc(work->headerFill.vertex[0].color, 3, &work->headerFill);

        work->statsBand.vertex[0].x = work->statsBand.vertex[1].x = left + 80;
        work->statsBand.vertex[2].x = work->statsBand.vertex[3].x = left + (window->width - work->offsetX) - 3;
        if (work->chrNo >= 0) {
            work->statsBand.vertex[0].y = work->statsBand.vertex[2].y = window->y + 62;
        } else {
            work->statsBand.vertex[2].x = work->statsBand.vertex[3].x = left + 256;
            work->statsBand.vertex[0].y = work->statsBand.vertex[2].y = window->y + 63;
        }
        work->statsBand.vertex[1].y = work->statsBand.vertex[3].y = window->y + 96;
        work->statsBand.vertex[0].color = work->statsBand.vertex[1].color =
            work->statsBand.vertex[2].color = work->statsBand.vertex[3].color = window->color + 2;
        endPrintExtFunc(work->statsBand.vertex[0].color, 3, &work->statsBand);

        work->headerLine.vertex[0].x = left + 3;
        work->headerLine.vertex[1].x = left + (window->width - work->offsetX) - 3;
        if (work->chrNo == -2) {
            work->headerLine.vertex[1].x = left + 238;
        }
        work->headerLine.vertex[1].y = work->headerLine.vertex[0].y = window->y + 26;
        work->headerLine.vertex[0].color = work->headerLine.vertex[1].color = window->color + 6;
        endPrintExtFunc(work->headerLine.vertex[0].color, 0, &work->headerLine);

        work->statsLine.vertex[0].x = left + 3;
        work->statsLine.vertex[1].x = left + (window->width - work->offsetX) - 3;
        if (work->chrNo >= 0) {
            work->statsLine.vertex[1].y = work->statsLine.vertex[0].y = window->y + 61;
        } else {
            if (work->chrNo == -1) {
                work->statsLine.vertex[1].x = left + 304;
            } else {
                work->statsLine.vertex[1].x = left + 238;
            }
            work->statsLine.vertex[1].y = work->statsLine.vertex[0].y = window->y + 61;
        }
        work->statsLine.vertex[0].color = work->statsLine.vertex[1].color = window->color + 6;
        endPrintExtFunc(work->statsLine.vertex[0].color, 0, &work->statsLine);

        work->divider.vertex[1].x = work->divider.vertex[0].x = left + 80;
        work->divider.vertex[0].color = work->divider.vertex[1].color = window->color + 6;
        work->divider.vertex[0].y = window->y + 3;
        work->divider.vertex[1].y = window->y + 96;
        endPrintExtFunc(work->divider.vertex[0].color, 0, &work->divider);

        if (work->chrNo >= 0 || showStats != 1) {
            work->statsFill.vertex[0].x = work->statsFill.vertex[1].x = left + 3;
            work->statsFill.vertex[2].x = work->statsFill.vertex[3].x = left + (window->width - work->offsetX) - 3;
            if (work->chrNo >= 0) {
                work->statsFill.vertex[0].y = work->statsFill.vertex[2].y = window->y + 61;
            } else if (work->chrNo == -2) {
                work->statsFill.vertex[2].x = work->statsFill.vertex[3].x = left + 238;
            } else {
                work->statsFill.vertex[0].y = work->statsFill.vertex[2].y = window->y + 63;
            }
            work->statsFill.vertex[1].y = work->statsFill.vertex[3].y = window->y + 96;
            work->statsFill.vertex[0].color = work->statsFill.vertex[1].color =
                work->statsFill.vertex[2].color = work->statsFill.vertex[3].color = window->color + 7;
            endPrintExtFunc(work->statsFill.vertex[0].color, 3, &work->statsFill);
        }

        work->face.x = left + 16;
        work->face.color = window->color + 15;
        work->face.y = nameY;
        work->face.spriteId = MenuFaceEpidGet(chrNo, 0);
        endSpriteSet(&work->face, 255);
        endPrintExtFunc(work->face.color, 2, &work->face);
        endPrintExtFunc(work->box.color, 6, 0);
        break;
    }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_top_command_check", MenuInfoWindow);

INCLUDE_ASM("asm/main/nonmatchings/menu_top_command_check", MenuSelectWindow);

void MenuPasWindow(void) {

}