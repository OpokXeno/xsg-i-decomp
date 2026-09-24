/*
 * OV10 original TU 1: 0x00a00410..0x00a08670 (15 functions)
 */
#include "common.h"
#include "shared.h"
#include "card_play_sound1.h"

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardPlaySound1);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardPlaySound3);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardPlaySound2);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardPlaySound4);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardDataLoad);

typedef signed int s32;

extern s32 CardFread(u32 mode, char *filename);
extern char D_00A4BD00[];

u32 CardGraphicLoad(void) {
    return (u32) ~CardFread(0x0100C000, D_00A4BD00) >> 0x1F;
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardTitleInit);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CMIBGInit);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardMainInit);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardMainItemGetSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardMainAddItemSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", NewCMPGameModeExit);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CaedGameWinMode);

extern char D_00A4C788[];

void CMTSetFontColor(s32 count, s32 player)
{
    if (count == player) {
        xglFontPrint(0, 0, 0, D_00A4C788);
    } else if (count < player) {
        xglFontPrint(0, 0, 0, D_00A4C788);
    } else {
        xglFontPrint(0, 0, 0, D_00A4C788);
    }
}

static inline void CardHideModels(CardGameWork *work) {
    int model;

    for (model = 15; model >= 0; model--)
        work->models[model].visible = 0;
}

static inline void CardLoadBattleWindows(void) {
    CardFread(0x01930000, D_00A4C5A0);
    CardFread(0x01931800, D_00A4C5B8);
    CardFread(0x01933000, D_00A4C5D0);
    CardFread(0x01934800, D_00A4C5E8);
    CardFread(0x01936000, D_00A4C600);
    CardFread(0x01937800, D_00A4C618);
}

s32 CardMainProc(CardGameWork *work) {
    float matrix[4][4];
    float placement[4][4];
    float position[4];
    StudioLight *studioLight;
    int i;
    int win;
    float pulse;

    work->command.input[0].held = PadData[0].prefix.half_28;
    work->command.input[0].pressed = PadData[0].prefix.half_2a;
    work->command.input[0].directions = PadData[0].directions;
    work->command.input[0].repeat = PadData[0].prefix.half_2a;
    work->command.input[1].held = PadData[0].prefix.half_28;
    work->command.input[1].pressed = PadData[0].prefix.half_2a;
    work->command.input[1].directions = PadData[0].directions;
    work->command.input[1].repeat = PadData[0].prefix.half_2a;
    if (work->fase == 9) {
        work->command.input[1].held = PadData[1].prefix.half_28;
        work->command.input[1].pressed = PadData[1].prefix.half_2a;
        work->command.input[1].directions = PadData[1].directions;
        work->command.input[1].repeat = PadData[1].prefix.half_2a;
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
        case 0:
            CardFadeOut(work);
            if (work->fadeOutTimer == 0) {
                xglSoundLoadRequestSmd("Umnmode", (void *) 0x01900000);
                work->fase = 1;
                CardTitleInit(work);
                work->fadeOutTimer = 10;
            }
            break;

        case 1: {
            int previousFase;

                xglFontDebugPrintf(0x54, 0, "Menu");
                previousFase = work->fase;
                CardFadeIn(work);
                if (SubNo_2 < 0) {
                    if (PadData[0].prefix.half_2a & 0x1000) {
                        MenuNo_1--;
                        xglSoundEffectNormalID(3, 0);
                    } else if (PadData[0].prefix.half_2a & 0x4000) {
                        MenuNo_1++;
                        xglSoundEffectNormalID(3, 0);
                    }
                    if (MenuNo_1 < 0)
                        MenuNo_1 = 3;
                    if (MenuNo_1 >= 4)
                        MenuNo_1 = 0;
                    if (PadData[0].prefix.half_2a & 0x20) {
                        switch (MenuNo_1) {
                        case 0:
                        case 1:
                        case 2:
                            xglSoundEffectNormalID(1, 0);
                            SubNo_2 = 0;
                            break;
                        case 3:
                            xglSoundEffectNormalID(1, 0);
                            return 0;
                        case 4:
                            work->fase = 5;
                            break;
                        }
                    }
                } else {
                    if (PadData[0].prefix.half_2a & 0x1000) {
                        SubNo_2--;
                        xglSoundEffectNormalID(3, 0);
                    } else if (PadData[0].prefix.half_2a & 0x4000) {
                        SubNo_2++;
                        xglSoundEffectNormalID(3, 0);
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
                    if (PadData[0].prefix.half_2a & 0x40) {
                        xglSoundEffectNormalID(2, 0);
                        SubNo_2 = -1;
                    } else if (PadData[0].prefix.half_2a & 0x20) {
                        xglSoundEffectNormalID(1, 0);
                        if (MenuNo_1 == 0) {
                            switch (SubNo_2) {
                            case 0:
                                work->fase = 11;
                                break;
                            case 1:
                                work->fase = 10;
                                break;
                            case 2:
                                work->fase = 9;
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
                                xglSoundEffectNormalID(1, 0);
                                if (work->save->flags & CARD_SAVE_LARGE_CARDS) {
                                    work->models[5].model = (void *) 0x01E20000;
                                    work->models[5].texture = (void *) 0x01F70200;
                                    work->models[7].model = (void *) 0x01E20000;
                                    work->models[7].texture = (void *) 0x01F70200;
                                } else {
                                    work->models[5].model = (void *) 0x01996000;
                                    work->models[5].texture = (void *) 0x0199A000;
                                    work->models[7].model = (void *) 0x01996000;
                                    work->models[7].texture = (void *) 0x0199A000;
                                }
                                break;
                            }
                        } else if (MenuNo_1 == 1) {
                            switch (SubNo_2) {
                            case 0:
                                work->fase = 7;
                                break;
                            case 1:
                                work->faseStep = 0;
                                work->fase = 2;
                                break;
                            default:
                                work->fase = 3;
                                break;
                            }
                        } else {
                            switch (SubNo_2) {
                            case 0:
                                work->fase = 12;
                                work->fadeOutTimer = 10;
                                work->fadeOutStep = 16;
                                work->faseStep = 0;
                                break;
                            case 1:
                                work->helpState = 48;
                                work->fadeOutTimer = 10;
                                work->fadeOutStep = 16;
                                work->faseStep = 0;
                                break;
                            }
                        }
                    }
                }

                {
                    char menuColor[] = "\v\r\0\f\x10\x10\x10";
                    char cursorColor[] = "\f(((";
                    CardFontCode5 selectColor = D_00A4C780;
                    void *model;
                    void *texture;

                    position[0] = position[1] = position[2] = 0.0f;
                    position[3] = 1.0f;
                    xglMatrixUnit(matrix);

                    xglFontPrint(0, 0, 0xFFF0, menuColor);
                    if (SubNo_2 >= 0)
                        xglFontPrint(0, 0, 0xFFF0, cursorColor);
                    if (MenuNo_1 == 0) {
                        nmlModelSetLight(asColor, asDir);
                        xglFontPrint(0, 0, 0xFFF0, selectColor.text);
                    }
                    xglFontPrint(0, 0, 0, D_00A4C788);
                    xglFontPrint(0xDE, 0x8C, 0xFFF0, " Game");
                    position[1] = 0.6787f;
                    xglMatrixTrans(placement, matrix, position);
                    nmlModelSetPlace(placement);
                    nmlModelSetTexture((void *) 0x0170F600);
                    nmlModelEntryCard((void *) 0x0170B600);

                    xglFontPrint(0, 0, 0xFFF0, menuColor);
                    if (SubNo_2 >= 0)
                        xglFontPrint(0, 0, 0xFFF0, cursorColor);
                    if (MenuNo_1 == 1) {
                        nmlModelSetLight(asColor, asDir);
                        xglFontPrint(0, 0, 0xFFF0, selectColor.text);
                    }
                    xglFontPrint(0, 0, 0, D_00A4C788);
                    xglFontPrint(0xDE, 0xB1, 0xFFF0, " Card");
                    position[1] = 0.33f;
                    xglMatrixTrans(placement, matrix, position);
                    nmlModelSetPlace(placement);
                    nmlModelSetTexture((void *) 0x0170F600);
                    nmlModelEntryCard((void *) 0x0170B600);

                    xglFontPrint(0, 0, 0xFFF0, menuColor);
                    if (SubNo_2 >= 0)
                        xglFontPrint(0, 0, 0xFFF0, cursorColor);
                    if (MenuNo_1 == 2) {
                        xglFontPrint(0, 0, 0xFFF0, selectColor.text);
                        nmlModelSetLight(asColor, asDir);
                    }
                    xglFontPrint(0, 0, 0, D_00A4C788);
                    xglFontPrint(0xDE, 0xD6, 0xFFF0, D_00A4C790);
                    position[1] = -0.0202f;
                    xglMatrixTrans(placement, matrix, position);
                    nmlModelSetPlace(placement);
                    nmlModelSetTexture((void *) 0x0170F600);
                    nmlModelEntryCard((void *) 0x0170B600);

                    xglFontPrint(0, 0, 0xFFF0, menuColor);
                    if (SubNo_2 >= 0)
                        xglFontPrint(0, 0, 0xFFF0, cursorColor);
                    if (MenuNo_1 == 3) {
                        xglFontPrint(0, 0, 0xFFF0, selectColor.text);
                        nmlModelSetLight(asColor, asDir);
                    }
                    xglFontPrint(0, 0, 0, D_00A4C788);
                    xglFontPrint(0xDE, 0xFB, 0xFFF0, " Exit");
                    position[1] = -0.3692f;
                    xglMatrixTrans(placement, matrix, position);
                    nmlModelSetPlace(placement);
                    nmlModelSetTexture((void *) 0x0170F600);
                    nmlModelEntryCard((void *) 0x0170B600);

                    if (SubNo_2 >= 0) {
                        CardFontCode8 itemColor = D_00A4C7C8;
                        CardFontCode5 itemSelectColor = D_00A4C7D0;

                        position[0] = 1.8374f;
                        if (MenuNo_1 == 0) {
                            xglFontPrint(0, 0, 0xFFF0, itemColor.text);
                            if (SubNo_2 == 0) {
                                xglFontPrint(0, 0, 0xFFF0, itemSelectColor.text);
                                nmlModelSetLight(asColor, asDir);
                            }
                            xglFontPrint(0, 0, 0, D_00A4C788);
                            xglFontPrint(0x168, 0x7A, 0xFFF0, "Exhibition");
                            position[1] = 0.8521f;
                            xglMatrixTrans(placement, matrix, position);
                            nmlModelSetPlace(placement);
                            nmlModelSetTexture((void *) 0x0170F600);
                            nmlModelEntryCard((void *) 0x0170D600);

                            xglFontPrint(0, 0, 0xFFF0, itemColor.text);
                            if (SubNo_2 == 1) {
                                xglFontPrint(0, 0, 0xFFF0, itemSelectColor.text);
                                nmlModelSetLight(asColor, asDir);
                            }
                            xglFontPrint(0, 0, 0, D_00A4C788);
                            xglFontPrint(0x168, 0x9E, 0xFFF0, "Tournament");
                            position[1] = 0.5015f;
                            xglMatrixTrans(placement, matrix, position);
                            nmlModelSetPlace(placement);
                            nmlModelSetTexture((void *) 0x0170F600);
                            nmlModelEntryCard((void *) 0x0170D600);

                            xglFontPrint(0, 0, 0xFFF0, itemColor.text);
                            if (SubNo_2 == 2) {
                                xglFontPrint(0, 0, 0xFFF0, itemSelectColor.text);
                                nmlModelSetLight(asColor, asDir);
                            }
                            xglFontPrint(0, 0, 0, D_00A4C788);
                            xglFontPrint(0x168, 0xC2, 0xFFF0, "2P Battle");
                            position[1] = 0.1509f;
                            xglMatrixTrans(placement, matrix, position);
                            nmlModelSetPlace(placement);
                            nmlModelSetTexture((void *) 0x0170F600);
                            nmlModelEntryCard((void *) 0x0170D600);

                            xglFontPrint(0, 0, 0xFFF0, itemColor.text);
                            if (SubNo_2 == 3) {
                                xglFontPrint(0, 0, 0xFFF0, itemSelectColor.text);
                                nmlModelSetLight(asColor, asDir);
                            }
                            xglFontPrint(0, 0, 0, D_00A4C788);
                            xglFontPrint(0x168, 0xE8, 0xFFF0, "Change BG");
                            position[1] = -0.1976f;
                            xglMatrixTrans(placement, matrix, position);
                            nmlModelSetPlace(placement);
                            nmlModelSetTexture((void *) 0x0170F600);
                            nmlModelEntryCard((void *) 0x0170D600);
                        } else if (MenuNo_1 == 1) {
                            xglFontPrint(0, 0, 0xFFF0, itemColor.text);
                            if (SubNo_2 == 0) {
                                xglFontPrint(0, 0, 0xFFF0, itemSelectColor.text);
                                nmlModelSetLight(asColor, asDir);
                            }
                            xglFontPrint(0, 0, 0, D_00A4C788);
                            xglFontPrint(0x168, 0xC2, 0xFFF0, "Create deck");
                            position[1] = 0.1509f;
                            xglMatrixTrans(placement, matrix, position);
                            nmlModelSetPlace(placement);
                            nmlModelSetTexture((void *) 0x0170F600);
                            nmlModelEntryCard((void *) 0x0170D600);

                            xglFontPrint(0, 0, 0xFFF0, itemColor.text);
                            if (SubNo_2 == 1) {
                                xglFontPrint(0, 0, 0xFFF0, itemSelectColor.text);
                                nmlModelSetLight(asColor, asDir);
                            }
                            xglFontPrint(0, 0, 0, D_00A4C788);
                            xglFontPrint(0x168, 0xE8, 0xFFF0, "Open card");
                            position[1] = -0.1976f;
                            xglMatrixTrans(placement, matrix, position);
                            nmlModelSetPlace(placement);
                            nmlModelSetTexture((void *) 0x0170F600);
                            nmlModelEntryCard((void *) 0x0170D600);

                            xglFontPrint(0, 0, 0xFFF0, itemColor.text);
                            if (SubNo_2 == 2) {
                                xglFontPrint(0, 0, 0xFFF0, itemSelectColor.text);
                                nmlModelSetLight(asColor, asDir);
                            }
                            xglFontPrint(0, 0, 0, D_00A4C788);
                            xglFontPrint(0x168, 0x10D, 0xFFF0, "View cards");
                            position[1] = -0.5478f;
                            xglMatrixTrans(placement, matrix, position);
                            nmlModelSetPlace(placement);
                            nmlModelSetTexture((void *) 0x0170F600);
                            nmlModelEntryCard((void *) 0x0170D600);
                        } else {
                            xglFontPrint(0, 0, 0xFFF0, itemColor.text);
                            if (SubNo_2 == 0) {
                                xglFontPrint(0, 0, 0xFFF0, itemSelectColor.text);
                                nmlModelSetLight(asColor, asDir);
                            }
                            xglFontPrint(0, 0, 0, D_00A4C788);
                            xglFontPrint(0x168, 0xE8, 0xFFF0, "Tutorial");
                            position[1] = -0.1976f;
                            xglMatrixTrans(placement, matrix, position);
                            nmlModelSetPlace(placement);
                            nmlModelSetTexture((void *) 0x0170F600);
                            nmlModelEntryCard((void *) 0x0170D600);

                            xglFontPrint(0, 0, 0xFFF0, itemColor.text);
                            if (SubNo_2 == 1) {
                                xglFontPrint(0, 0, 0xFFF0, itemSelectColor.text);
                                nmlModelSetLight(asColor, asDir);
                            }
                            xglFontPrint(0, 0, 0, D_00A4C788);
                            xglFontPrint(0x168, 0x10D, 0xFFF0, "Help");
                            position[1] = -0.5478f;
                            xglMatrixTrans(placement, matrix, position);
                            nmlModelSetPlace(placement);
                            nmlModelSetTexture((void *) 0x0170F600);
                            nmlModelEntryCard((void *) 0x0170D600);
                        }
                    }

                    position[0] = -1.8833f;
                    model = TitleMdlLst[MenuNo_1 * 2];
                    position[1] = 0.1576f;
                    texture = TitleMdlLst[MenuNo_1 * 2 + 1];
                    if (model != 0) {
                        xglMatrixTrans(placement, matrix, position);
                        nmlModelSetPlace(placement);
                        nmlModelSetTexture(texture);
                        nmlModelEntryCard(model);
                    }
                }
                xglFontPrint(0, 0, 0xFFF0, D_00A4C868);
                xglFontPrint(0, 0, 0, D_00A4C788);
                {
                    char *guide;

                    if (SubNo_2 < 0)
                        guide = CardPlayTMessList[50 + MenuNo_1];
                    else
                        guide = CardPlayTMessList[55 + MenuNo_1 * 5 + SubNo_2];
                    CardDispTxtSub(0x18, 0x138, guide, 0x17);
                }
                xglFontPrint(0xD8, 0x90, 0xFFF0, D_00A4C870);
                if (previousFase != work->fase) {
                    D_00A56F18.mode = 2;
                    ModeExit_4 = 0;
                    xglSoundEffectNormalID(1, 0);
                    switch (work->fase) {
                    case 0:
                        break;
                    case 3:
                        work->fadeOutTimer = 10;
                        work->fadeOutStep = 16;
                        work->faseStep = 0;
                        work->command.step = 0;
                        break;
                    case 7:
                        work->fadeOutTimer = 10;
                        work->fadeOutStep = 16;
                        work->faseStep = 0;
                        work->command.step = 0;
                        break;
                    case 9:
                        work->fadeOutTimer = 10;
                        work->fadeOutStep = 16;
                        work->faseStep = 0;
                        work->command.step = 0;
                        break;
                    case 10:
                        work->fadeOutTimer = 10;
                        work->fadeOutStep = 16;
                        work->faseStep = 0;
                        work->command.step = 0;
                        break;
                    case 11:
                        work->fadeOutTimer = 10;
                        work->fadeOutStep = 16;
                        work->faseStep = 0;
                        work->command.step = 0;
                        break;
                    }
                }
                break;
        }

        case 2:
            CardOpenProc(work, &ModeExit_4);
            break;

        case 3:
            switch (work->faseStep) {
            case 0:
                CardFadeOut(work);
                if (work->fadeOutTimer == 0) {
                    work->fadeOutTimer = 10;
                    work->command.step = 0;
                    work->fadeOutStep = 16;
                    work->faseStep++;
                    CardHideModels(work);
                    work->models[1].visible = 1;
                    D_00A56F18.baseColor = 7;
                    D_00A56F18.cursorColor = 15;
                    D_00A56F18.x = 316;
                    D_00A56F18.y = 60;
                    D_00A56F18.lineHeight = 16;
                    D_00A56F18.rows = 9;
                    D_00A56F18.active = 0;
                    D_00A56F18.cursor = 0;
                    D_00A56F18.top = 0;
                    D_00A56F18.count = 0;
                }
                break;
            case 1:
                CardFadeIn(work);
                if (work->fadeOutTimer == 0)
                    work->faseStep++;
                break;
            default:
                CardViewProc(work, &D_00A56F18, &ModeExit_4);
                break;
            }
            break;

        case 9: {
            int previous;
            int result;
            int last;
            char *title;
            char *enemyTitle;
            int exiting;

                exiting = 0;
                switch (work->faseStep) {
                case 0:
                    CardFadeOut(work);
                    if (work->fadeOutTimer == 0) {
                        work->fadeOutTimer = 10;
                        work->fadeOutStep = 16;
                        work->command.step = 0;
                        work->faseStep++;
                        for (i = 0; i < 16; i++)
                            work->models[i].visible = 0;
                        work->models[2].visible = 1;
                        CardFread(0x01900000, "deckmake\\type1_B.lex");
                        CardFread(0x01900400, "deckmake\\type1_E.lex");
                        CardFread(0x01900800, "deckmake\\type1_S.lex");
                        CardFread(0x01900C00, "deckmake\\type2_0.lex");
                        CardFread(0x01901000, "deckmake\\type2_1.lex");
                        CardFread(0x01901400, "deckmake\\type2_2.lex");
                        CardFread(0x01901800, "deckmake\\type2_3.lex");
                        CardFread(0x01901C00, "deckmake\\type2_4.lex");
                        CardFread(0x01902000, "deckmake\\type2_5.lex");
                        CardFread(0x01902400, "deckmake\\type2_6.lex");
                        CardFread(0x01908000, "deckmake\\type1_B.xtx");
                        CardFread(0x01918100, "deckmake\\type1_E.xtx");
                        CardFread(0x01928200, "deckmake\\type1_S.xtx");
                        CardFread(0x01938300, "deckmake\\type2_0.xtx");
                        D_00A56F18.baseColor = 7;
                        D_00A56F18.cursorColor = 15;
                        D_00A56F18.cursor = 0;
                        D_00A57188.mode = 0;
                        D_00A57188.cursor = 0;
                        D_00A56F18.active = 1;
                        D_00A56F18.top = 0;
                        D_00A56F18.rows = 9;
                        D_00A56F18.lineHeight = 16;
                        D_00A57188.lineHeight = 16;
                        D_00A57188.top = 0;
                        D_00A56F18.x = 12;
                        D_00A56F18.y = 64;
                        D_00A56F18.count = 0;
                        D_00A57188.x = 312;
                        D_00A57188.y = 60;
                        D_00A57188.columns = 1;
                        D_00A57188.rows = 9;
                        previous = -1;
                        last = -1;
                        for (i = 0; i < 40; i++) {
                            if (previous != work->save->deck[i]) {
                                last++;
                                D_00A57188.entries[last].cardId = work->save->deck[i];
                                D_00A57188.entries[last].count = 1;
                            } else {
                                D_00A57188.entries[last].count++;
                            }
                            previous = work->save->deck[i];
                        }
                        D_00A57188.count = last + 1;
                    }
                    break;
                case 1:
                    CardFadeIn(work);
                    title = P1DeckLoadStr;
                    if (ModeExit_4) {
                        title = 0;
                        xglFontPrint(0x18, 0x138, 0xFFF0, ModeEndMess0);
                        xglFontPrint(0xE0, 0x168, 0xFFF0, ModeEndMess1);
                        exiting = 1;
                        if (PadData[0].prefix.half_2a & 0x20) {
                            work->fadeOutTimer = 10;
                            work->fadeOutStep = 16;
                            work->fase = 0;
                            ModeExit_4 = 0;
                        }
                        if (PadData[0].prefix.half_2a & 0x840) {
                            xglSoundEffectNormalID(2, 0);
                            ModeExit_4 = 0;
                        }
                    } else if (work->command.input[0].pressed & 0x800) {
                        xglSoundEffectNormalID(1, 0);
                        ModeExit_4 = 1;
                    }
                    result = CardLoadDeckProc(exiting, &D_00A56F18, &D_00A57188, work, title);
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
                case 2:
                    enemyTitle = P2DeckLoadStr;
                    if (ModeExit_4) {
                        enemyTitle = 0;
                        xglFontPrint(0x18, 0x138, 0xFFF0, ModeEndMess0);
                        xglFontPrint(0xE0, 0x168, 0xFFF0, ModeEndMess1);
                        exiting = 1;
                        if (PadData[0].prefix.half_2a & 0x20) {
                            work->fadeOutTimer = 10;
                            work->fadeOutStep = 16;
                            work->fase = 0;
                            ModeExit_4 = 0;
                        }
                        if (PadData[0].prefix.half_2a & 0x840) {
                            xglSoundEffectNormalID(2, 0);
                            ModeExit_4 = 0;
                        }
                    } else if (work->command.input[0].pressed & 0x800) {
                        xglSoundEffectNormalID(1, 0);
                        ModeExit_4 = 1;
                    }
                    result = CardLoadDeckProc(exiting, &D_00A56F18, &D_00A57188, work, enemyTitle);
                    if (result != 0) {
                        if (result < 0) {
                            work->command.step = 0;
                            work->faseStep--;
                        } else {
                            CardCopyRam2Deck(work->enemyDeck, work, result - 1);
                            work->command.step = 0;
                            work->fadeOutTimer = 10;
                            work->fadeOutStep = 16;
                            work->faseStep++;
                        }
                    }
                    break;
                case 3:
                    CardFadeOut(work);
                    if (work->fadeOutTimer == 0) {
                        work->fadeOutTimer = 10;
                        work->fadeOutStep = 16;
                        work->faseStep++;
                        work->command.step = 0;
                        CardPlaySound1();
                        CardGameInit(work);
                        CardPlayInitWork(&work->player, work->playerDeck);
                        CardPlayInitWork(&work->enemy, work->enemyDeck);
                    }
                    break;
                case 4:
                    CardFadeIn(work);
                    if (CardGameProc(0, &D_00A573F8, work, 0) != 0) {
                        work->resultWait = 90;
                        work->faseStep++;
                    }
                    break;
                case 5:
                    CardGameProc(0, &D_00A573F8, work, 0);
                    if (work->resultWait <= 0 &&
                        ((work->command.input[0].pressed & 0xF0) ||
                         (work->command.input[1].pressed & 0xF0))) {
                        xglSoundEffectNormalID(1, 0);
                        work->fadeOutTimer = 10;
                        work->fadeOutStep = 16;
                        work->fase = 0;
                        ModeExit_4 = 0;
                    }
                    break;
                }
                break;
        }

        case 10: {
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
                case 0:
                    D_00A57730[3] = 0;
                    D_00A57730[2] = 0;
                    D_00A57730[1] = 0;
                    D_00A57730[0] = 0;
                    CardFadeOut(work);
                    if (work->fadeOutTimer == 0) {
                        D_00A57728 = 0;
                        work->fadeOutTimer = 10;
                        work->faseStep++;
                        work->fadeOutStep = 16;
                        for (i = 0; i < 16; i++)
                            work->models[i].visible = 0;
                        work->models[2].visible = 1;
                        CardLoadBattleWindows();
                        D_00A56F18.baseColor = 7;
                        D_00A56F18.cursorColor = 15;
                        D_00A56F18.cursor = 0;
                        D_00A57188.mode = 0;
                        D_00A57188.cursor = 0;
                        D_00A56F18.active = 1;
                        D_00A56F18.top = 0;
                        D_00A56F18.rows = 9;
                        D_00A56F18.lineHeight = 16;
                        D_00A57188.lineHeight = 16;
                        D_00A57188.top = 0;
                        D_00A56F18.x = 12;
                        D_00A56F18.y = 64;
                        D_00A56F18.count = 0;
                        D_00A57188.x = 312;
                        D_00A57188.y = 60;
                        D_00A57188.columns = 1;
                        D_00A57188.rows = 9;
                        previous = -1;
                        last = -1;
                        for (i = 0; i < 40; i++) {
                            if (previous != work->save->deck[i]) {
                                last++;
                                D_00A57188.entries[last].cardId = work->save->deck[i];
                                D_00A57188.entries[last].count = 1;
                            } else {
                                D_00A57188.entries[last].count++;
                            }
                            previous = work->save->deck[i];
                        }
                        D_00A57188.count = last + 1;
                    }
                    break;
                case 1:
                    CardFadeIn(work);
                    if (work->fadeOutTimer == 0)
                        work->faseStep++;
                    break;
                case 2:
                    title = P1DeckLoadStr;
                    if (ModeExit_4) {
                        title = 0;
                        xglFontPrint(0x18, 0x138, 0xFFF0, ModeEndMess0);
                        xglFontPrint(0xE0, 0x180, 0xFFF0, ModeEndMess1);
                        exiting = 1;
                        if (PadData[0].prefix.half_2a & 0x20) {
                            work->fadeOutTimer = 10;
                            work->fadeOutStep = 16;
                            work->fase = 0;
                            ModeExit_4 = 0;
                        }
                        if (PadData[0].prefix.half_2a & 0x840) {
                            xglSoundEffectNormalID(2, 0);
                            ModeExit_4 = 0;
                        }
                    } else if (work->command.input[0].pressed & 0x800) {
                        xglSoundEffectNormalID(1, 0);
                        ModeExit_4 = 1;
                    }
                    result = CardLoadDeckProc(exiting, &D_00A56F18, &D_00A57188, work, title);
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
                case 3:
                    count = 40;
                    if (xglFlagsGet(0x186, 1) == 0)
                        count = xglFlagsGet(0x12D, 1) != 0 ? 19 : 10;
                    for (pick = 0; pick < count; pick++)
                        D_00A57668[pick] = -1;
                    for (pick = 0; pick < count;) {
                        int *slot = &D_00A57668[xglSRand() % count];

                        if (*slot < 0) {
                            *slot = pick;
                            pick++;
                        }
                    }
                    D_00A57188.count = 5;
                    work->faseStep++;
                    /* fall through */
                case 4:
                    if (ModeExit_4) {
                        xglFontPrint(0, 0, 0, D_00A4C788);
                        xglFontPrint(0x18, 0x138, 0xFFF0, "\r\0Exit game?");
                        exiting = 1;
                        switch (D_00A57728) {
                        case 1:
                            xglFontPrint(0, 0, 0, D_00A4C788);
                            xglFontPrint(0x18, 0x150, 0xFFF0, "The prize is 1 Card Pack if you stop here.");
                            break;
                        case 2:
                            xglFontPrint(0, 0, 0, D_00A4C788);
                            xglFontPrint(0x18, 0x150, 0xFFF0, "The prize is 2 Card Packs if you stop here.");
                            break;
                        case 3:
                            xglFontPrint(0, 0, 0, D_00A4C788);
                            xglFontPrint(0x18, 0x150, 0xFFF0, "The prize is 3 Card Packs if you stop here.");
                            break;
                        case 4:
                            xglFontPrint(0, 0, 0, D_00A4C788);
                            xglFontPrint(0x18, 0x150, 0xFFF0, "The prize is 4 Card Packs if you stop here.");
                            break;
                        }
                        xglFontPrint(0xE0, 0x180, 0xFFF0, ModeEndMess1);
                        if (work->command.input[0].pressed & 0x20) {
                            CardMainItemGetSub(D_00A57730, D_00A57728);
                            work->faseStep = 15;
                        } else if (work->command.input[0].pressed & 0x840) {
                            xglSoundEffectNormalID(2, 0);
                            ModeExit_4 = 0;
                        }
                    } else {
                        if (work->command.input[0].pressed & 0x800) {
                            xglSoundEffectNormalID(1, 0);
                            ModeExit_4 = 1;
                        }
                        if (work->command.input[0].pressed & 0x20) {
                            work->faseStep++;
                            xglSoundEffectNormalID(1, 0);
                        } else if (work->command.input[0].pressed & 0x40) {
                            if (D_00A57728 != 0) {
                                xglSoundEffectNormalID(5, 0);
                                ModeExit_4 = 1;
                            } else {
                                work->faseStep--;
                                xglSoundEffectNormalID(2, 0);
                            }
                        }
                        if (D_00A57728 == 0) {
                            xglFontPrint(0, 0, 0, D_00A4C788);
                            xglFontPrint(0x18, 0x138, 0xFFF0, "\v\x19\x03\r\0Select an opponent.");
                            xglFontPrint(0x18, 0x150, 0xFFF0, "\v\x19\x03\r\0Is this ok?");
                            xglFontPrint(0xE0, 0x168, 0xFFF0, ModeContMess1);
                        } else {
                            switch (D_00A57728) {
                            case 1:
                                xglFontPrint(0, 0, 0, D_00A4C788);
                                xglFontPrint(0x18, 0x138, 0xFFF0, "\v\x19\x03\r\0Entering the second round.");
                                break;
                            case 2:
                                xglFontPrint(0, 0, 0, D_00A4C788);
                                xglFontPrint(0x18, 0x138, 0xFFF0, "\v\x19\x03\r\0Entering the third round.");
                                break;
                            case 3:
                                xglFontPrint(0, 0, 0, D_00A4C788);
                                xglFontPrint(0x18, 0x138, 0xFFF0, "\v\x19\x03\r\0Entering the fourth round.");
                                break;
                            case 4:
                                xglFontPrint(0, 0, 0, D_00A4C788);
                                xglFontPrint(0x18, 0x138, 0xFFF0, "\v\x19\x03\r\0Entering the final round.");
                                break;
                            }
                            xglFontPrint(0xE0, 0x180, 0xFFF0, ModeContMess0);
                        }
                    }
                    CardHideModels(work);
                    work->models[15].visible = 1;
                    work->models[14].visible = 1;
                    position[1] = 1.751f;
                    position[3] = 1.0f;
                    position[0] = 0.0f;
                    position[2] = 0.0f;
                    xglMatrixUnit(placement);
                    xglMatrixTrans(matrix, placement, position);
                    nmlModelSetTexture((void *) 0x01937800);
                    nmlModelSetPlace(matrix);
                    nmlModelEntryCard((void *) 0x01930000);
                    position[1] -= 0.55f;
                    CMTSetFontColor(D_00A57728, 0);
                    xglFontPrint(0x6E, 0x25, 0xFFF0, EnemyDeckListStr[D_00A57668[0]]);
                    xglMatrixTrans(matrix, placement, position);
                    nmlModelSetTexture((void *) 0x01937800);
                    nmlModelSetPlace(matrix);
                    nmlModelEntryCard((void *) 0x01931800);
                    position[1] -= 0.55f;
                    CMTSetFontColor(D_00A57728, 1);
                    xglFontPrint(0x6E, 0x5E, 0xFFF0, EnemyDeckListStr[D_00A57668[1]]);
                    xglMatrixTrans(matrix, placement, position);
                    nmlModelSetTexture((void *) 0x01937800);
                    nmlModelSetPlace(matrix);
                    nmlModelEntryCard((void *) 0x01933000);
                    position[1] -= 0.55f;
                    CMTSetFontColor(D_00A57728, 2);
                    xglFontPrint(0x6E, 0x97, 0xFFF0, EnemyDeckListStr[D_00A57668[2]]);
                    xglMatrixTrans(matrix, placement, position);
                    nmlModelSetTexture((void *) 0x01937800);
                    nmlModelSetPlace(matrix);
                    nmlModelEntryCard((void *) 0x01934800);
                    position[1] -= 0.55f;
                    CMTSetFontColor(D_00A57728, 3);
                    xglFontPrint(0x6E, 0xD2, 0xFFF0, EnemyDeckListStr[D_00A57668[3]]);
                    xglMatrixTrans(matrix, placement, position);
                    nmlModelSetTexture((void *) 0x01937800);
                    nmlModelSetPlace(matrix);
                    nmlModelEntryCard((void *) 0x01936000);
                    position[1] -= 0.55f;
                    CMTSetFontColor(D_00A57728, 4);
                    xglFontPrint(0x6E, 0x10D, 0xFFF0, EnemyDeckListStr[D_00A57668[4]]);
                    work->fadeOutTimer = 10;
                    work->fadeOutStep = 16;
                    break;
                case 5:
                    CardFadeOut(work);
                    if (work->fadeOutTimer == 0) {
                        work->fadeOutStep = 16;
                        work->fadeOutTimer = 10;
                        if (D_00A57728 >= 4)
                            CardPlaySound2();
                        else
                            CardPlaySound1();
                        CardGameInit(work);
                        CardPlayInitWork(&work->player, work->playerDeck);
                        CardCopyRom2Deck(work->enemyDeck, D_00A57668[D_00A57728]);
                        work->enemyDeckId = D_00A57668[D_00A57728];
                        CardPlayInitWorkFake(&work->enemy, work->enemyDeck, work->definitions);
                        work->flags |= 0x20;
                        work->faseStep++;
                    }
                    break;
                case 6:
                    CardFadeIn(work);
                    outcome = CardGameProc(0, &D_00A573F8, work, D_00A57728);
                    if (outcome != 0) {
                        if (outcome == 1) {
                            D_00A57728++;
                            work->faseStep++;
                            work->resultWait = 90;
                        } else {
                            work->resultWait = 90;
                            work->faseStep = 20;
                        }
                    }
                    break;
                case 7:
                    CardGameProc(0, &D_00A573F8, work, D_00A57728);
                    if (work->resultWait <= 0 && (work->command.input[0].pressed & 0xF0)) {
                        if (D_00A57728 == 5) {
                            work->faseStep = 10;
                            work->resultWait = 90;
                            xglSoundEffectNormalID(6, 0);
                            work->command.step = 0;
                            work->command.flowRequest = 5;
                        } else {
                            CardHideModels(work);
                            work->models[2].visible = 1;
                            xglSoundEffectNormalID(1, 0);
                            work->faseStep = 4;
                        }
                    }
                    break;
                case 10:
                    CardGameProc(0, &D_00A573F8, work, D_00A57728);
                    if (work->resultWait == 0 && (work->command.input[0].pressed & 0xF0)) {
                        D_00A57730[3] = 0;
                        D_00A57730[2] = 0;
                        D_00A57730[1] = 0;
                        D_00A57730[0] = 0;
                        for (win = 0; win < D_00A57728; win++) {
                            int roll = xglSRand();

                            if (xglFlagsGet(0x12D, 1) == 0)
                                roll = 0;
                            if (roll & 1)
                                D_00A57730[1]++;
                            else
                                D_00A57730[0]++;
                        }
                        if (!(work->save->flags & CARD_SAVE_PROMO_GIVEN)) {
                            D_00A57730[3] = 1;
                            work->save->flags |= CARD_SAVE_PROMO_GIVEN;
                        } else {
                            D_00A57730[2] = (xglSRand() & 1) + 1;
                        }
                        xglSoundEffectNormalID(1, 0);
                        work->faseStep++;
                        work->command.step = -1;
                    }
                    break;
                case 11: {
                    char packCount[3];

                    CardGameProc(0, &D_00A573F8, work, D_00A57728);
                    work->models[14].visible = 1;
                    line = 0;
                    xglFontPrint(0, 0, 0, D_00A4C788);
                    xglFontPrint(0x14, 0x138, 0xFFF0, "\v\x19\x03\r\0Congratulations. Your prize is: ");
                    if (D_00A57730[0] != 0) {
                        xglFontPrint(0, 0, 0, D_00A4C788);
                        packCount[0] = ZenkakuListA[D_00A57730[0] * 2];
                        packCount[1] = ZenkakuListA[D_00A57730[0] * 2 + 1];
                        packCount[2] = 0;
                        line = 1;
                        xglFontPrint(0x14, 0x150, 0xFFF0, "\v\x19\x03\r\0Card Pack #821");
                        xglFontPrint(0xC8, 0x150, 0xFFF0, packCount);
                    }
                    if (D_00A57730[1] != 0) {
                        packCount[0] = ZenkakuListA[D_00A57730[1] * 2];
                        packCount[1] = ZenkakuListA[D_00A57730[1] * 2 + 1];
                        packCount[2] = 0;
                        xglFontPrint(0, 0, 0, D_00A4C788);
                        xglFontPrint(0x14, line * 24 + 0x150, 0xFFF0, "\v\x19\x03\r\0Card Pack #822");
                        xglFontPrint(0xC8, line * 24 + 0x150, 0xFFF0, packCount);
                        line++;
                    }
                    if (D_00A57730[2] != 0) {
                        xglFontPrint(0, 0, 0, D_00A4C788);
                        xglFontPrint(0x14, line * 24 + 0x150, 0xFFF0, "\v\x19\x03\r\0\xA3\xB1 Promotional Card");
                        line++;
                    }
                    if (D_00A57730[3] != 0) {
                        xglFontPrint(0, 0, 0, D_00A4C788);
                        xglFontPrint(0x14, line * 24 + 0x150, 0xFFF0, "\v\x19\x03\r\0Tuned Circuit");
                    }
                    if (work->command.input[0].pressed & 0xF0) {
                        xglSoundEffectNormalID(1, 0);
                        work->faseStep = 15;
                    }
                    break;
                }
                case 15:
                    CardMainAddItemSub(work, D_00A57730);
                    xglSoundEffectNormalID(1, 0);
                    work->fadeOutTimer = 10;
                    work->fadeOutStep = 16;
                    work->fase = 0;
                    ModeExit_4 = 0;
                    break;
                case 20:
                    CardGameProc(0, &D_00A573F8, work, D_00A57728);
                    if (work->resultWait <= 0) {
                        for (pack = 0; pack < D_00A57728; pack++) {
                            int roll = xglSRand();

                            if (xglFlagsGet(0x12D, 1) == 0)
                                roll = 0;
                            if (roll & 1)
                                D_00A57730[1]++;
                            else
                                D_00A57730[0]++;
                        }
                        work->faseStep++;
                    }
                    break;
                case 21: {
                    char packCount[3];

                    line = 0;
                    CardGameProc(0, &D_00A573F8, work, D_00A57728);
                    work->models[14].visible = 1;
                    xglFontPrint(0, 0, 0, D_00A4C788);
                    xglFontPrint(0x14, 0x138, 0xFFF0, "\v\x19\x03\r\0Sorry, you lost. Your prize is: ");
                    if (D_00A57728 != 0) {
                        if (D_00A57730[0] != 0) {
                            packCount[0] = ZenkakuListA[D_00A57730[0] * 2];
                            packCount[1] = ZenkakuListA[D_00A57730[0] * 2 + 1];
                            line = 1;
                            packCount[2] = 0;
                            xglFontPrint(0, 0, 0, D_00A4C788);
                            xglFontPrint(0x14, 0x150, 0xFFF0, "\v\x19\x03\r\0Card Pack #821");
                            xglFontPrint(0xC8, 0x150, 0xFFF0, packCount);
                        }
                        if (D_00A57730[1] != 0) {
                            xglFontPrint(0, 0, 0, D_00A4C788);
                            packCount[0] = ZenkakuListA[D_00A57730[1] * 2];
                            packCount[1] = ZenkakuListA[D_00A57730[1] * 2 + 1];
                            packCount[2] = 0;
                            xglFontPrint(0x14, line * 24 + 0x150, 0xFFF0, "\v\x19\x03\r\0Card Pack #822");
                            xglFontPrint(0xC8, line * 24 + 0x150, 0xFFF0, packCount);
                            line++;
                        }
                    } else {
                        xglFontPrint(0, 0, 0, D_00A4C788);
                    }
                    xglFontPrint(0x14, line * 24 + 0x150, 0xFFF0, "\v\x19\x03\r\0Nothing.");
                    if (work->command.input[0].pressed & 0xF0) {
                        xglSoundEffectNormalID(1, 0);
                        work->faseStep = 15;
                    }
                    break;
                }
                }
                break;
        }

        case 11: {
            int previous;
            int model;
            int result;
            int count;
            int last;
            char *title;
            int exiting;

                exiting = 0;
                switch (work->faseStep) {
                case 0:
                    CardFadeOut(work);
                    if (work->fadeOutTimer == 0) {
                        D_00A57744 = 0;
                        work->fadeOutTimer = 10;
                        work->faseStep++;
                        work->fadeOutStep = 16;
                        for (i = 0; i < 16; i++)
                            work->models[i].visible = 0;
                        work->models[2].visible = 1;
                        CardLoadBattleWindows();
                        D_00A56F18.baseColor = 7;
                        D_00A56F18.cursorColor = 15;
                        D_00A56F18.cursor = 0;
                        D_00A57188.mode = 0;
                        D_00A57188.cursor = 0;
                        D_00A56F18.active = 1;
                        D_00A56F18.top = 0;
                        D_00A56F18.rows = 9;
                        D_00A56F18.lineHeight = 16;
                        D_00A57188.lineHeight = 16;
                        D_00A57188.top = 0;
                        D_00A56F18.x = 12;
                        D_00A56F18.y = 64;
                        D_00A56F18.count = 0;
                        D_00A57188.x = 312;
                        D_00A57188.y = 60;
                        D_00A57188.columns = 1;
                        D_00A57188.rows = 9;
                        previous = -1;
                        last = -1;
                        for (i = 0; i < 40; i++) {
                            if (previous != work->save->deck[i]) {
                                last++;
                                D_00A57188.entries[last].cardId = work->save->deck[i];
                                D_00A57188.entries[last].count = 1;
                            } else {
                                D_00A57188.entries[last].count++;
                            }
                            previous = work->save->deck[i];
                        }
                        D_00A57188.count = last + 1;
                    }
                    break;
                case 1:
                    CardFadeIn(work);
                    if (work->fadeOutTimer == 0)
                        work->faseStep++;
                    break;
                case 2:
                    title = P1DeckLoadStr;
                    if (ModeExit_4) {
                        title = 0;
                        xglFontPrint(0x18, 0x138, 0xFFF0, ModeEndMess0);
                        xglFontPrint(0xE0, 0x168, 0xFFF0, ModeEndMess1);
                        exiting = 1;
                        if (PadData[0].prefix.half_2a & 0x20) {
                            work->fadeOutTimer = 10;
                            work->fadeOutStep = 16;
                            work->fase = 0;
                            ModeExit_4 = 0;
                        }
                        if (PadData[0].prefix.half_2a & 0x840) {
                            xglSoundEffectNormalID(2, 0);
                            ModeExit_4 = 0;
                        }
                    } else if (PadData[0].prefix.half_2a & 0x840) {
                        xglSoundEffectNormalID(2, 0);
                        ModeExit_4 = 1;
                    }
                    result = CardLoadDeckProc(exiting, &D_00A56F18, &D_00A57188, work, title);
                    if (result != 0) {
                        if (result < 0) {
                            ModeExit_4 = 1;
                        } else {
                            CardCopyRam2Deck(work->playerDeck, work, result - 1);
                            work->command.step = 0;
                            work->faseStep++;
                            for (model = 15; model >= 0; model--)
                                work->models[model].visible = 0;
                            work->models[15].visible = 1;
                            work->models[14].visible = 1;
                        }
                    }
                    break;
                case 3:
                    count = 40;
                    if (xglFlagsGet(0x186, 1) == 0)
                        count = xglFlagsGet(0x12D, 1) != 0 ? 19 : 10;
                    D_00A57740[0] = xglSRand() % count;
                    D_00A57188.count = 1;
                    work->faseStep++;
                    /* fall through */
                case 4:
                    if (ModeExit_4) {
                        xglFontPrint(0x18, 0x138, 0xFFF0, ModeEndMess0);
                        xglFontPrint(0xE0, 0x168, 0xFFF0, ModeEndMess1);
                        if (PadData[0].prefix.half_2a & 0x20) {
                            work->fadeOutTimer = 10;
                            work->fadeOutStep = 16;
                            work->fase = 0;
                            ModeExit_4 = 0;
                        }
                        if (PadData[0].prefix.half_2a & 0x840) {
                            xglSoundEffectNormalID(2, 0);
                            ModeExit_4 = 0;
                        }
                    } else {
                        xglFontPrint(0, 0, 0, D_00A4C788);
                        xglFontPrint(0x18, 0x138, 0xFFF0, "\v\x19\x03\r\0Select an opponent.");
                        xglFontPrint(0x18, 0x150, 0xFFF0, "\v\x19\x03\r\0Is this ok?");
                        xglFontPrint(0xE0, 0x168, 0xFFF0, ModeContMess1);
                        if (work->command.input[0].pressed & 0x20) {
                            work->faseStep++;
                            xglSoundEffectNormalID(1, 0);
                        } else if (work->command.input[0].pressed & 0x800) {
                            ModeExit_4 = 1;
                            xglSoundEffectNormalID(1, 0);
                        } else if (work->command.input[0].pressed & 0x40) {
                            work->faseStep--;
                            xglSoundEffectNormalID(2, 0);
                        }
                    }
                    xglMatrixUnit(placement);
                    position[1] = 1.751f;
                    position[3] = 1.0f;
                    position[0] = 0.0f;
                    position[2] = 0.0f;
                    xglMatrixTrans(placement, placement, position);
                    nmlModelSetTexture((void *) 0x01937800);
                    nmlModelSetPlace(placement);
                    nmlModelEntryCard((void *) 0x01930000);
                    xglFontPrint(0, 0, 0, D_00A4C788);
                    xglFontPrint(0x78, 0x25, 0xFFF0, EnemyDeckListStr[D_00A57740[0]]);
                    work->fadeOutTimer = 10;
                    work->fadeOutStep = 16;
                    break;
                case 5:
                    CardFadeOut(work);
                    if (work->fadeOutTimer == 0) {
                        work->fadeOutTimer = 10;
                        work->fadeOutStep = 16;
                        work->faseStep++;
                        CardPlaySound1();
                        CardGameInit(work);
                        CardPlayInitWork(&work->player, work->playerDeck);
                        CardCopyRom2Deck(work->enemyDeck, D_00A57740[0]);
                        work->enemyDeckId = D_00A57740[D_00A57744];
                        CardPlayInitWorkFake(&work->enemy, work->enemyDeck, work->definitions);
                        work->enemyDeckId = D_00A57740[0];
                        work->flags |= 0x20;
                    }
                    break;
                case 6:
                    CardFadeIn(work);
                    result = CardGameProc(0, &D_00A573F8, work, 0);
                    if (result != 0) {
                        if (result == 1)
                            D_00A57744++;
                        work->resultWait = 90;
                        work->faseStep++;
                    }
                    break;
                case 7:
                    CardGameProc(0, &D_00A573F8, work, 0);
                    if (work->resultWait <= 0 && (work->command.input[0].pressed & 0xF0)) {
                        xglSoundEffectNormalID(1, 0);
                        if (D_00A57744 != 0) {
                            int roll = xglSRand();

                            if (xglFlagsGet(0x12D, 1) == 0)
                                roll = 0;
                            if (roll & 1) {
                                D_00A57748 = 2;
                                dataEvtBoxInc(0x37);
                            } else {
                                D_00A57748 = 1;
                                dataEvtBoxInc(0x36);
                            }
                            work->faseStep++;
                        } else {
                            work->fadeOutTimer = 10;
                            work->fadeOutStep = 16;
                            work->fase = 0;
                            ModeExit_4 = 0;
                        }
                    }
                    break;
                case 8:
                    CardGameProc(0, &D_00A573F8, work, 0);
                    work->models[14].visible = 1;
                    xglFontPrint(0, 0, 0, D_00A4C788);
                    xglFontPrint(0x18, 0x138, 0xFFF0, "\v\x19\x03\r\0Congratulations!");
                    if (D_00A57748 == 1)
                        xglFontPrint(0x18, 0x150, 0xFFF0, "\v\x19\x03\r\0Your prize is 1 Card Pack #821.");
                    else
                        xglFontPrint(0x18, 0x150, 0xFFF0, "\v\x19\x03\r\0Your prize is 1 Card Pack #822.");
                    if (work->command.input[0].pressed & 0xF0) {
                        xglSoundEffectNormalID(1, 0);
                        work->fadeOutTimer = 10;
                        work->fadeOutStep = 16;
                        work->fase = 0;
                        ModeExit_4 = 0;
                    }
                    break;
                }
                break;
        }

        case 7: {
            int status;
            int entry;
            int card;
            int previous;
            int last;
            int exiting;

                exiting = 0;
                xglFontPrint(0, 0, 0, D_00A4C788);
                xglFontDebugPrintf(0x54, 0, "\v\x19\x03\r\0Deck building mode");
                switch (work->faseStep) {
                case 0:
                case 1:
                case 2:
                case 3:
                    break;
                default:
                    if (work->command.input[0].pressed & 0x800) {
                        ModeExit_4 = (ModeExit_4 + 1) & 1;
                        if (ModeExit_4)
                            xglSoundEffectNormalID(1, 0);
                        else
                            xglSoundEffectNormalID(2, 0);
                    }
                    break;
                }
                if (ModeExit_4) {
                    xglFontPrint(0, 0, 0, D_00A4C788);
                    xglFontPrint(0x18, 0x138, 0xFFF0, "\v\x19\x03\r\0Exit deck building mode?");
                    xglFontPrint(0xE0, 0x168, 0xFFF0, ModeEndMess1);
                    exiting = 1;
                    if (PadData[0].prefix.half_2a & 0x20) {
                        xglSoundEffectNormalID(1, 0);
                        work->fadeOutTimer = 10;
                        work->fadeOutStep = 16;
                        work->fase = 0;
                        ModeExit_4 = 0;
                    }
                    if (PadData[0].prefix.half_2a & 0x40) {
                        xglSoundEffectNormalID(2, 0);
                        ModeExit_4 = 0;
                    }
                } else if (Message_6 != 0) {
                    exiting = 1;
                    xglFontPrint(0x18, 0x138, 0xFFFF, "\v\r\0\f\x80@@");
                    xglFontPrint(0, 0, 0, D_00A4C788);
                    xglFontPrint(0x18, 0x138, 0xFFFF, CardErrorList[Message_6]);
                    if (work->command.input[0].pressed & 0xF0) {
                        xglSoundEffectNormalID(1, 0);
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
                case 0:
                    work->command.step = 0;
                    work->faseStep++;
                    for (entry = 0; entry < 40; entry++) {
                        D_00A57188.entries[entry].cardId = -1;
                        D_00A57188.entries[entry].count = 0;
                    }
                    work->fadeOutTimer = 10;
                    work->fadeOutStep = 16;
                    break;
                case 1:
                    CardFadeOut(work);
                    if (work->fadeOutTimer == 0)
                        work->faseStep++;
                    break;
                case 2:
                    work->fadeOutTimer = 10;
                    work->fadeOutStep = 16;
                    work->faseStep++;
                    for (i = 0; i < 16; i++)
                        work->models[i].visible = 0;
                    work->models[2].visible = 1;
                    CardFread(0x01900000, "deckmake\\type1_B.lex");
                    CardFread(0x01900400, "deckmake\\type1_E.lex");
                    CardFread(0x01900800, "deckmake\\type1_S.lex");
                    CardFread(0x01900C00, "deckmake\\type2_0.lex");
                    CardFread(0x01901000, "deckmake\\type2_1.lex");
                    CardFread(0x01901400, "deckmake\\type2_2.lex");
                    CardFread(0x01901800, "deckmake\\type2_3.lex");
                    CardFread(0x01901C00, "deckmake\\type2_4.lex");
                    CardFread(0x01902000, "deckmake\\type2_5.lex");
                    CardFread(0x01902400, "deckmake\\type2_6.lex");
                    CardFread(0x01908000, "deckmake\\type1_B.xtx");
                    CardFread(0x01918100, "deckmake\\type1_E.xtx");
                    CardFread(0x01928200, "deckmake\\type1_S.xtx");
                    CardFread(0x01938300, "deckmake\\type2_0.xtx");
                    D_00A56F18.baseColor = 7;
                    D_00A56F18.cursorColor = 15;
                    D_00A56F18.cursor = 0;
                    D_00A57188.mode = 0;
                    D_00A57188.cursor = 0;
                    D_00A56F18.active = 1;
                    D_00A56F18.y = 60;
                    D_00A56F18.top = 0;
                    D_00A56F18.rows = 9;
                    D_00A56F18.lineHeight = 16;
                    D_00A57188.lineHeight = 16;
                    D_00A57188.top = 0;
                    D_00A56F18.x = 12;
                    D_00A56F18.count = 0;
                    D_00A57188.x = 312;
                    D_00A57188.y = 60;
                    D_00A57188.columns = 1;
                    D_00A57188.rows = 9;
                    last = -1;
                    previous = -1;
                    for (i = 0; i < 40; i++) {
                        if (previous != work->save->deck[i]) {
                            last++;
                            D_00A57188.entries[last].cardId = work->save->deck[i];
                            D_00A57188.entries[last].count = 1;
                        } else {
                            D_00A57188.entries[last].count++;
                        }
                        previous = work->save->deck[i];
                    }
                    D_00A57188.count = last + 1;
                    break;
                case 3:
                    CardFadeIn(work);
                    if (work->fadeOutTimer == 0)
                        work->faseStep++;
                    break;
                case 4:
                    if (!exiting) {
                        xglFontPrint(0, 0, 0, D_00A4C788);
                        xglFontPrint(0x18, 0x138, 0xFFF0, "\v\x19\x03\r\0Load deck?");
                        xglFontPrint(0x18, 0x168, 0xFFF0,
                                     "\r\x02\f\x80@@\xA1\xFB \v\x19\x03\r\0Yes\xA1\xA1\r\x02\f@@\x80\xA1\xDF \v\x19\x03\r\0No");
                        xglFontPrint(0x78, 0x120, 0xFFF0, D_00A4C870);
                        if (work->command.input[0].pressed & 0x20) {
                            int prevId;
                            int lastEntry;

                            xglSoundEffectNormalID(1, 0);
                            prevId = -1;
                            lastEntry = -1;
                            for (card = 0; card < 40; card++) {
                                if (prevId != work->save->deck[card]) {
                                    lastEntry++;
                                    D_00A57188.entries[lastEntry].cardId = work->save->deck[card];
                                    D_00A57188.entries[lastEntry].count = 1;
                                } else {
                                    D_00A57188.entries[lastEntry].count++;
                                }
                                prevId = work->save->deck[card];
                            }
                            D_00A57188.count = lastEntry + 1;
                            work->faseStep++;
                        } else if (work->command.input[0].pressed & 0x40) {
                            xglSoundEffectNormalID(2, 0);
                            D_00A57188.count = 0;
                            work->faseStep = 10;
                        }
                    }
                    break;
                case 5:
                    status = CardLoadDeckProc(exiting, &D_00A56F18, &D_00A57188, work, 0);
                    if (status != 0) {
                        if (status < 0) {
                            work->faseStep--;
                        } else {
                            CardCopyRam2CLD(&D_00A57188, work, (status & 0x7F) - 1);
                            work->command.step = 0;
                            work->faseStep = 10;
                        }
                    }
                    break;
                case 10:
                    CardHideModels(work);
                    work->faseStep++;
                    work->models[3].visible = 1;
                    D_00A56F18.baseColor = 7;
                    D_00A56F18.cursorColor = 15;
                    D_00A56F18.active = 1;
                    D_00A56F18.lineHeight = 16;
                    D_00A56F18.cursor = 0;
                    D_00A56F18.top = 0;
                    D_00A56F18.rows = 9;
                    D_00A56F18.count = 0;
                    D_00A57188.x = 312;
                    D_00A57188.y = 60;
                    D_00A57188.lineHeight = 16;
                    D_00A57188.cursor = 0;
                    D_00A57188.columns = 1;
                    D_00A57188.top = 0;
                    D_00A573F8.cursor = 0;
                    D_00A573F8.top = 0;
                    D_00A56F18.x = 12;
                    D_00A56F18.y = 64;
                    D_00A56F18.mode = 2;
                    D_00A57188.rows = 9;
                    D_00A573F8.x = 312;
                    D_00A573F8.y = 60;
                    D_00A573F8.lineHeight = 16;
                    D_00A573F8.columns = 1;
                    D_00A573F8.rows = 9;
                    D_00A573F8.count = 0;
                    break;
                case 11:
                    status = CardMakeDeckProc(exiting, &D_00A56F18, &D_00A57188, work);
                    if (status != 0) {
                        xglSoundEffectNormalID(5, 0);
                        Message_6 = status;
                        MessageCnt_7 = 45;
                    }
                    if (work->command.input[0].pressed & 0x80) {
                        if (CardGetDeckAllCnt(&D_00A57188) == 40) {
                            xglSoundEffectNormalID(1, 0);
                            work->faseStep++;
                        } else {
                            xglSoundEffectNormalID(5, 0);
                            Message_6 = 2;
                            MessageCnt_7 = 45;
                        }
                    }
                    break;
                case 12:
                    work->command.step = 0;
                    work->faseStep++;
                    D_00A56F18.count = 5;
                    D_00A56F18.top = 0;
                    D_00A56F18.cursor = 0;
                    CardCopyRam2CLD(&D_00A573F8, work, 0);
                    CardHideModels(work);
                    work->models[2].visible = 1;
                    /* fall through */
                case 13: {
                    char saveTitle[] = "\v\x19\x03\r\0Save Mode";

                    status = CardSaveDeckProc(exiting, &D_00A56F18, &D_00A573F8, work, saveTitle);
                    if (status != 0) {
                        if (status < 0) {
                            xglSoundEffectNormalID(2, 0);
                            work->faseStep = 10;
                        } else {
                            CardCopyCLD2Ram(&D_00A57188, work, status - 1);
                            xglSoundEffectNormalID(1, 0);
                            work->fadeOutTimer = 10;
                            work->fadeOutStep = 16;
                            work->fase = 0;
                        }
                    }
                    break;
                }
                }
                break;
        }

        case 12: {
            int running = 1;

            switch (work->faseStep) {
            case 0:
                CardFadeOut(work);
                if (work->fadeOutTimer == 0)
                    work->faseStep++;
                break;
            case 1:
                work->faseStep = 2;
                work->fadeOutTimer = 10;
                work->fadeOutStep = 16;
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
                xglSoundEffectNormalID(1, 0);
                work->faseStep = 0;
                work->fadeOutTimer = 10;
                work->fadeOutStep = 16;
                work->fase = 0;
            }
            break;
        }

        case 5: {
            int soundList[25] = {
                0x20001, 0x20002, 0x20003, 0x20004, 0x20005, 0x20006, 0x20007,
                0x20008, 0x20009, 0x2000A, 0x2000D, 0x2000E, 0x2000F, 0x2000F,
                0x2000F, 0x2000F, 0x2000F, 0x2000F, 0x2000F, 0x2000F, 0x2000F,
                0x2000F, 0x2000F, 0x2000F, 0x2000F,
            };
            char volumeText[16];

            if ((PadData[0].prefix.half_2a & 8) && vol_19 < 127) {
                vol_19 += 10;
                if (vol_19 >= 128)
                    vol_19 = 127;
            }
            if ((PadData[0].prefix.half_2a & 4) && vol_19 > 0) {
                vol_19 -= 10;
                if (vol_19 < 0)
                    vol_19 = 0;
            }
            if (ModeExit_4) {
                xglFontPrint(0xC0, 0x90, 0xFFF8, ModeEndMess2);
                xglFontPrint(0xA8, 0xC0, 0xFFF8, ModeEndMess3);
                if (PadData[0].prefix.half_2a & 0x20) {
                    work->fadeOutTimer = 10;
                    work->fadeOutStep = 16;
                    work->fase = 0;
                    ModeExit_4 = 0;
                    xglSoundEffectNormalID(1, 0);
                }
                if (PadData[0].prefix.half_2a & 0x40) {
                    ModeExit_4 = 0;
                    xglSoundEffectNormalID(2, 0);
                }
            }
            xglFontPrint(0x30, 0x60, 0xFFF0, "1P Deck");
            xglFontPrint(0x48, 0x78, 0xFFF0, "T1");
            xglFontPrint(0x90, 0x78, 0xFFF0, "T2");
            xglFontPrint(0xD8, 0x78, 0xFFF0, "T3");
            xglFontPrint(0x120, 0x78, 0xFFF0, "T4");
            xglFontPrint(0x168, 0x78, 0xFFF0, "T5");
            xglFontPrint(0x48, 0x90, 0xFFF0, "T6");
            xglFontPrint(0x90, 0x90, 0xFFF0, "T7");
            xglFontPrint(0xD8, 0x90, 0xFFF0, "T8");
            xglFontPrint(0x120, 0x90, 0xFFF0, "T9");
            xglFontPrint(0x168, 0x90, 0xFFF0, "T0");
            xglFontPrint(0x30, 0xC0, 0xFFF0, "2P deck");
            xglFontPrint(0x48, 0xD8, 0xFFF0, "T1");
            xglFontPrint(0x90, 0xD8, 0xFFF0, "T2");
            xglFontPrint(0xD8, 0xD8, 0xFFF0, "T3");
            xglFontPrint(0x120, 0xD8, 0xFFF0, "T4");
            xglFontPrint(0x168, 0xD8, 0xFFF0, "T5");
            xglFontPrint(0x48, 0xF0, 0xFFF0, "T6");
            xglFontPrint(0x90, 0xF0, 0xFFF0, "T7");
            xglFontPrint(0xD8, 0xF0, 0xFFF0, "T8");
            xglFontPrint(0x120, 0xF0, 0xFFF0, "T9");
            xglFontPrint(0x168, 0xF0, 0xFFF0, "T0");
            xglFontPrint(0x30, 0x120, 0xFFF0, "Game play");
            xglFontPrint(0x18, (s16) (SubMenu_3 * 0x60 + 0x60), 0xFFF0, ">");
            if (p0num_17 < 5)
                xglFontPrint((s16) (p0num_17 * 0x48 + 0x30), 0x78, 0xFFF0, ">");
            else
                xglFontPrint((s16) (p0num_17 * 0x48 - 0x138), 0x90, 0xFFF0, ">");
            if (p1num_18 < 5)
                xglFontPrint((s16) (p1num_18 * 0x48 + 0x30), 0xD8, 0xFFF0, ">");
            else
                xglFontPrint((s16) (p1num_18 * 0x48 - 0x138), 0xF0, 0xFFF0, ">");
            if (PadData[0].prefix.half_2a & 0x1000) {
                SubMenu_3--;
                xglSoundEffectNormalID(3, 0);
            }
            if (PadData[0].prefix.half_2a & 0x4000) {
                SubMenu_3++;
                xglSoundEffectNormalID(3, 0);
            }
            if (SubMenu_3 < 0)
                SubMenu_3 = 0;
            if (SubMenu_3 >= 3)
                SubMenu_3 = 2;
            sprintf(volumeText, "%d", vol_19);
            xglFontDebugPrintf(0x18, 0x24, volumeText);
            switch (SubMenu_3) {
            case 0:
                if (PadData[0].prefix.half_2a & 0x2000) {
                    p0num_17++;
                    xglSoundEffectNormalID(3, 0);
                }
                if (PadData[0].prefix.half_2a & 0x8000) {
                    p0num_17--;
                    xglSoundEffectNormalID(3, 0);
                }
                if (PadData[0].prefix.half_2a & 0x20)
                    xglSoundEffectParamDirect(soundList[p0num_17], vol_19, 0x40);
                if (p0num_17 < 0)
                    p0num_17 = 0;
                if (p0num_17 >= 10)
                    p0num_17 = 9;
                break;
            case 1:
                if (PadData[0].prefix.half_2a & 0x2000) {
                    p1num_18++;
                    xglSoundEffectNormalID(3, 0);
                }
                if (PadData[0].prefix.half_2a & 0x8000) {
                    p1num_18--;
                    xglSoundEffectNormalID(3, 0);
                }
                if (PadData[0].prefix.half_2a & 0x20)
                    xglSoundEffectNormalID(soundList[p1num_18 + 7], 0);
                if (p1num_18 < 0)
                    p1num_18 = 0;
                if (p1num_18 >= 10)
                    p1num_18 = 9;
                break;
            case 2:
                if (PadData[0].prefix.half_2a & 0x20) {
                    ModeExit_4 = 0;
                    work->fase = 6;
                    xglSoundEffectNormalID(1, 0);
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

        case 6:
            if (CardGameProc(ModeExit_4, &D_00A573F8, work, 0) != 0 &&
                (work->command.input[0].pressed & 0xF0)) {
                xglSoundEffectNormalID(1, 0);
                work->fadeOutTimer = 10;
                work->fadeOutStep = 16;
                work->fase = 0;
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
        nmlModelSetLight(helpLight, asDir);
        nmlModelSetTexture(work->models[15].texture);
        xglMatrixUnit(placement);
        nmlModelSetPlace(placement);
        nmlModelEntryCard(work->models[15].model);
    } else {
        for (i = 0; i < 16; i++) {
            CardModelEntry *model = &work->models[i];

            if (model->visible) {
                if (i == 4)
                    nmlModelSetLight(Player1Color, asDir);
                if (i == 6)
                    nmlModelSetLight(Player2Color, asDir);
                if (i == 5 || i == 7) {
                    float cardLight[4][4];

                    memset(cardLight, 0, sizeof(cardLight));
                    cardLight[0][3] = 1.0f;
                    cardLight[1][3] = 1.0f;
                    cardLight[2][3] = 1.0f;
                    cardLight[3][0] = 0.5f;
                    cardLight[3][1] = 0.5f;
                    cardLight[3][2] = 0.5f;
                    cardLight[3][3] = 1.0f;
                    nmlModelSetLight(cardLight, asDir);
                } else if (i == 15) {
                    float dimLight[4][4];

                    memset(dimLight, 0, sizeof(dimLight));
                    dimLight[0][3] = 1.0f;
                    dimLight[1][3] = 1.0f;
                    dimLight[2][3] = 1.0f;
                    dimLight[3][0] = 0.3f;
                    dimLight[3][1] = 0.3f;
                    dimLight[3][2] = 0.3f;
                    dimLight[3][3] = 1.0f;
                    nmlModelSetLight(dimLight, asDir);
                }
                nmlModelSetTexture(model->texture);
                nmlModelSetPlace(work->models[i].matrix);
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
        if (pressed & 0x900) {
            xglSoundEffectNormalID(2, 0);
            work->helpState = 0;
        }
    } else if (ModeExit_4 == 0 && work->fase != 12) {
        if (work->command.input[0].pressed & 0x100) {
            xglSoundEffectNormalID(1, 0);
            work->helpState = 1;
            work->menuPlayer = 0;
            CardHelpPageInit(work);
        } else if (work->command.input[1].pressed & 0x100) {
            xglSoundEffectNormalID(1, 0);
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
        float *eye = &xglStudioSelectGetActiveCamera(0)->position.x;

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
