/* Casino settings, prize text/costs and coin-purchase offers from CASINO.res.
 * Original RES_* symbols: slus-20469-412d448de315 / ov11:0x00a00998..0x00a00a58.
 * See reports/readability-semantics.md for asset/consumer evidence.
 */
#include "xeno/casino/res.h"

int RES_IsDebugMode(void)
{
    /* The debug word is proven; the intervening sound region's extent is not. */
    int *debug_mode = (int *)((unsigned char *)ResData + CASINO_DEBUG_SETTING_OFFSET);
    return *debug_mode;
}

int RES_GetBonusTime(void)
{
    return ResData->bonus_duration;
}

int RES_GetRealSpeed(void)
{
    return ResData->reel_step;
}

char *RES_ShopDataInfo(int prize_index)
{
    return ResData->prizes[prize_index].description;
}

int RES_ShopDataCoin(int prize_index)
{
    CasinoPrize *prize = ResData->prizes;
    prize += prize_index;
    return prize->coin_cost;
}

char *RES_ShopDataName(int prize_index)
{
    return ResData->prizes[prize_index].name;
}

int RES_Coin_Rate(int offer_index)
{
    return ResData->coin_offers[offer_index].money_cost;
}

char *RES_Coin_Info(int offer_index)
{
    return ResData->coin_offers[offer_index].description;
}
