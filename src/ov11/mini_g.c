/*
 * OV11 original TU 1: 0x00a00100..0x00a091f8 (135 functions)
 */
#include "common.h"
#include "res.h"
#include "mini_g.h"

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", dprintf);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", MakeSprite);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SetDrawStatus);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SetTest);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SetRegAD);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", ResetRGBA);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SetRGBA);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", MakeBoxPos);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", DrawNonTextureBox);

/* Casino settings, prize text/costs and coin-purchase offers from CASINO.res.
 * Original RES_* symbols: slus-20469-412d448de315 / ov11:0x00a00998..0x00a00a58.
 * See reports/readability-semantics.md for asset/consumer evidence.
 *
 * All eight are LOCAL symbols in the original overlay, so they have internal
 * linkage and are defined `static` here. The compiled object binds them the
 * same way, which is what the linked comparison checks alongside the bytes.
 */

static int RES_IsDebugMode(void)
{
    /* The debug word is proven; the intervening sound region's extent is not. */
    int *debug_mode = (int *)((unsigned char *)ResData + CASINO_DEBUG_SETTING_OFFSET);
    return *debug_mode;
}

static int RES_GetBonusTime(void)
{
    return ResData->bonus_duration;
}

static int RES_GetRealSpeed(void)
{
    return ResData->reel_step;
}

static char *RES_ShopDataInfo(int prize_index)
{
    return ResData->prizes[prize_index].description;
}

static int RES_ShopDataCoin(int prize_index)
{
    CasinoPrize *prize = ResData->prizes;
    prize += prize_index;
    return prize->coin_cost;
}

static char *RES_ShopDataName(int prize_index)
{
    return ResData->prizes[prize_index].name;
}

static int RES_Coin_Rate(int offer_index)
{
    return ResData->coin_offers[offer_index].money_cost;
}

static char *RES_Coin_Info(int offer_index)
{
    return ResData->coin_offers[offer_index].description;
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", RES_SoundEffect);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", RES_SoundEffectStop);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", RES_Load);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", AddCoin);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlGetPic);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlRealDraw);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", decprint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlCashPrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", pay_print);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", total_pay_print);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", check_mode);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlPayWindowPrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", check_spin);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SpinBtnPrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlStopBtnPrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlDecPrintSmall);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlRateTableDraw);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlResultDraw);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlModePrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlGuidPrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", point_control);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlBgDraw);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlCoinEntry);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlPrizeCheck);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlLineCheck);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", check_real);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", paid_action);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", linepay_check);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", slot_key);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlPrizeEffect);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlChangeRealEffect);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlEvent);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlHelpMode);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", slot_main);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", init_slot);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgTimePrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgPaidPrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgGetsDownBtnPrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgSpinBtnPrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgLamp);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NineGameBG_Draw);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgStopGuidPt);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgFwdGuidPt);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgStopBtnPrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgAllStop);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgCheckReal);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", get_ngreal_shape);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", DrawNgPanel);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgReal);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgGame_GetsDown);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgGame_Select);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgGame_Key);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgGame_Paid);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgHelpMode);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NineGameMain);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", init_ninegame);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoMoneyPrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoCurDraw);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoWindowMessage);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoDialog);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoBgDraw);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoCardDraw);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoCardDrawAnime);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoBonusCardAnime);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoRateTableDraw);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoBonusBGDraw);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoResetCard);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoShuffleCard);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoSelectMarkDraw);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoKey);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoCardFlip);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoGetMark);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoGetBase);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoSortCard);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoMultiChk);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoFlushChk);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoChk1);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoChk2);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoChk3);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoChk4);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoChk5);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoChk6);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoChk7);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoChk8);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoChk9);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoCardCheck);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoResultLamp);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoSeqChange);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PokerReadyMes);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PokerMain);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoInitWork);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PokerInit);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", GameModeChange);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", sub_window);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", submenu_select);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", level_select);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", exchange_select);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", menu_mode);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", init_menu_mode);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", init_test_mode);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", test_mode);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", DecPrint2);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", UtlCurPrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", coin_main);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", init_coin);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", init_shop);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", init_shpmenu);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", shop_main);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", VW_Init);

/* Build the compact list of the 30 sampler slots enabled in SaveWork. */
static __inline void sampler_offset(int **base, int *count)
{
    *base += 114;
    *count = 0;
}

static int VW_SamlistInit(void)
{
    int count;
    int index;
    char *enabled;
    int *sampler;

    sampler = Gwork;
    sampler_offset(&sampler, &count);
    enabled = SaveWork;
    enabled += 6;
    index = 0;

    for (index = 0; index < 30; index++) {
        if (*enabled == 1) {
            *sampler = index;
            sampler++;
            count++;
        }
        enabled++;
    }
    return count;
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", VW_ViewModeInit);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", VW_ViewMode);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", VW_SamMode);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", VW_Main);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", MUSIC_CALL);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", InitWork);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", MiniG_Init);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", MiniG_Main);
