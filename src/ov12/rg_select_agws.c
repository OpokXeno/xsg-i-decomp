/*
 * OV12 original TU 69: 0x00a380d8..0x00a3c6a0 (103 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_select_agws.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _disp_pic);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _disp_pic_pl);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _CreateSelectWepFont);

static void _SinInit(void)
{
    s_fSinRad = 0.0f;
}

static void _SinPassTime(void)
{
    float phase = s_fSinRad + 0.3926991f;
    while (phase > 3.1415928f)
        phase -= 6.2831855f;
    while (phase < -3.1415928f)
        phase += 6.2831855f;
    s_fSinRad = phase;
}

static int _SinGet(int amplitude)
{
    return (int)(sinf(s_fSinRad) * (float)amplitude);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _CheckEquipWeps);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _CharCursorToCharID);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _CharIDToCharCursor);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _InitSelectChar);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetSelectChar);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetSelectCursor);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetSelectableSelectChar);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetCharSelectChar);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _IsExistItemUpper);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _IsExistItemLower);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetSelectableSelectChar);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _PassTimeSelectChar);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _InitSelWep);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _InitCharTypeSelWep);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetEssSelWep);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetEssListSelWep);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetEssListSizeSelWep);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetCursorSelWep);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetCursorSelWep);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetCursorByNameSelWep);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetNameOnCursorSelWep);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _PassTimeSelWep);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _EquipCurIDToEquipType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetSelTypeCursor);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _ResumeSelTypeOldCursor);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetCursorSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetTypeSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetCurSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetSelWepSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetConfrictSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetDetermModeSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetSpecModeSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetCurrentConfrictSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetCurrentWeaponSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetAllSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetAllNameOnCursorSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _CheckConfrictSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetAllCursorByNameSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _InitSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _InitByCharSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _PassTimeSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _DetachWeaponIfConfrictSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _InitWepListDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _CreateWepListDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _DestructWepListDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _DisposeWepListDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetSelectorWepListDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _DrawWepListDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _InitSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _DumpSelect__);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SaveCharSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SaveWepSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _LoadCharSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _LoadWepSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetBaseSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetBaseSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetModeSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _CreateSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _DisposeSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _TransitToWeaponSelect);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _TransitToCharSelect);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SaveToBaseData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _TransitToOffSelect);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetPlayerModeSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetPlayerModeSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _PassTimeSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetSelectCharID);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetSelectWeps);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetWepSelection);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _DrawPlayerMode);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetCharSelDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _InitDetermWin);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _DispDetermWindow);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _InitFloatArrowHori);

static void _SetFloatArrowPos(FloatArrow *arrow, int first_x, int first_y,
                              int second_x, int second_y, int offset_x,
                              int offset_y)
{
    arrow->x0 = first_x + offset_x;
    arrow->y0 = first_y + offset_y;
    arrow->x1 = second_x + offset_x;
    arrow->y1 = second_y + offset_y;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _DispFloatArrow);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetWepSelPicList);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _DispWepSelOverlaps);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _init_camera);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _set_camera);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _Destruct_00A3B940);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _PassTime_00A3B9E8);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _Disp_00A3BE70);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _LoadPicList);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _InitSelect);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", InitRgSelectAGWSData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", RgSelectAGWSSetSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", RgSelectAGWSGetSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", RgSelectAGWSGetCollectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", RgSelectAGWSSetMode);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", RgSelectAGWSIsEnd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", RgSelectAGWSGetCharacter);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", RgSelectAGWSGetWeapon);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", CreateRgSelectAGWS);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", DisposeRgSelectAGWS);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", RgSelectAGWSPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", RgSelectAGWSDisp);
