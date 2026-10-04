/*
 * OV11 original TU 1: 0x00a00100..0x00a091f8 (135 functions)
 */
#include "common.h"
#include "shared.h"
static struct CasinoResourcePrefix *ResData;
#include "res.h"
#include "mini_g.h"

/* xglCdReadFile is main:0x0021e310 (src/main/xgl_cd.c); every call below
 * passes it an asset path string and a fixed EE main RAM staging address. */
extern int xglCdReadFile(const char *name, void *buffer, int mode, int flags);

typedef char *va_list;
#define va_start(ap, last) ((ap) = (va_list)__builtin_next_arg(last) - (8 - __builtin_args_info(2)) * 8)
#define va_end(ap) ((void)0)

/* Partial newlib struct _reent: only the _stderr file pointer this debug
 * printf forwards to is touched; the preceding _errno/_stdin/_stdout
 * fields (newlib 1.9.0 libc/include/sys/reent.h field order) stay
 * unmodeled. _impure_ptr is the reentrant global newlib state pointer. */
struct Reent {
    unsigned char unmodeled_00[0xC];
    void *stderr_file;
};
extern struct Reent *_impure_ptr;
extern int vfprintf(void *stream, const char *format, va_list args);

void dprintf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(_impure_ptr->stderr_file, fmt, args);
    va_end(args);
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", MakeSprite);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SetDrawStatus);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SetTest);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SetRegAD);

static short BoxRGBA[4];

static void ResetRGBA(void)
{
    int index;
    short reset_color;

    reset_color = 0x80;
    for (index = 3; index >= 0; index--) {
        BoxRGBA[index] = reset_color;
    }
}

/* ov11:0x00a007e0. Stores the four caller-supplied components in the
 * shared untextured box color state (original LOCAL data symbol
 * BoxRGBA, ov11:0x00a0df78). */
static short BoxRGBA[4];

static void SetRGBA(short red, short green, short blue, short alpha)
{
    BoxRGBA[0] = red;
    BoxRGBA[1] = green;
    BoxRGBA[2] = blue;
    BoxRGBA[3] = alpha;
}

/* Candidate-backed OV11/tu001 initialized DATA objects; function bodies are retained. */
typedef unsigned int CasinoSpriteWords[7];
typedef unsigned int CasinoRectWords[4];
static short real11array[21];
static short real12array[21];
static short real13array[21];
static short real21array[21];
static short real22array[21];
static short real23array[21];
static short real31array[21];
static short real32array[21];
static short real33array[21];
static short real41array[21];
static short real42array[21];
static short real43array[21];
static short real51array[21];
static short real52array[21];
static short real53array[21];
extern short real61array[];
extern short real62array[];
extern short real63array[];
static char *SaveWork  = (char *)SaveData + 0x15190;
static short real01array[21];
static short real02array[21];
static short real03array[21];
static short *realpt[18]  = {
    real01array, real02array, real03array, real11array, real12array, real13array,
    real21array, real22array, real23array, real31array, real32array, real33array,
    real41array, real42array, real43array, real51array, real52array, real53array
};
static CasinoSpriteWords pay_window[2]  = {
    {224, 0, 32, 32, 0, 572, 0},
    {224, 32, 32, 32, 0, 572, 0}
};
static CasinoRectWords pay_window_rect_4[5]  = {
    {36, 33, 32, 32}, {36, 82, 32, 32}, {36, 131, 32, 32},
    {36, 180, 32, 32}, {36, 229, 32, 32}
};
static int status_keep_5  = 0;
static CasinoSpriteWords BoxSpr  = {0, 0, 511, 447, 0, 1020, 0};
static CasinoRectWords BoxRect  = {0, 0, 511, 447};
static CasinoSpriteWords NineGameBG_Tex_43  = {0, 0, 511, 447, 0, 1020, 0};
static CasinoRectWords NineGameBG_Rect_44  = {0, 0, 512, 448};
static CasinoSpriteWords tex_68  = {0, 0, 512, 448, 0, 1020, 0};
static CasinoRectWords rect_69  = {0, 0, 512, 448};
static CasinoSpriteWords tex_80  = {0, 0, 352, 176, 0, 1020, 0};
static CasinoRectWords rect_81  = {16, 256, 352, 176};
static CasinoRectWords result_rect_97[9]  = {
    {204, 362, 160, 32}, {204, 330, 160, 32}, {204, 298, 160, 32},
    {204, 266, 160, 32}, {0, 394, 244, 32}, {0, 362, 244, 32},
    {0, 330, 244, 32}, {0, 298, 244, 32}, {0, 266, 244, 32}
};
static CasinoSpriteWords tex_98  = {96, 224, 160, 32, 0, 756, 0};
static CasinoSpriteWords SubWindow_Tex  = {0, 0, 176, 120, 0, 564, 2};
static CasinoSpriteWords Level_Tex[4]  = {
    {176, 0, 72, 16, 0, 564, 2}, {176, 16, 72, 16, 0, 564, 2},
    {176, 32, 72, 16, 0, 564, 2}, {176, 48, 72, 16, 0, 564, 2}
};
static CasinoSpriteWords ExChange_Tex[3]  = {
    {176, 64, 64, 16, 0, 564, 2}, {176, 80, 64, 16, 0, 564, 2},
    {176, 96, 64, 16, 0, 564, 2}
};
static CasinoSpriteWords BGTEST_Tex  = {0, 0, 511, 447, 0, 1136, 0};
static CasinoRectWords BGTEST_Rect  = {0, 0, 512, 448};
static int flg_104  = 0;

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

/* The sound-effect ID table sits between the known CasinoResourcePrefix
 * prefix and CASINO_DEBUG_SETTING_OFFSET (0x1d3c); its own extent past this
 * indexed access is not proven, so it is reached by byte offset rather than
 * a named CasinoResourcePrefix member (matching RES_IsDebugMode above). */
#define CASINO_SOUND_ID_TABLE_OFFSET 0x193C

extern void xglSoundEffectNormalID(int soundEffectId, int variant);
static const char D_00A0BBB0[] = "Sound Req = SE_%.3d:Sound num = %x\n";

static void RES_SoundEffect(int soundId)
{
    int soundEffectId = *(int *)((unsigned char *)ResData + soundId * 4 + CASINO_SOUND_ID_TABLE_OFFSET);

    dprintf(D_00A0BBB0, soundId, soundEffectId);
    xglSoundEffectNormalID(soundEffectId, 0);
}

extern void xglSoundEffectStopDirect(int soundEffectId);
static const char D_00A0BBD8[] = "Sound Req = SE_%.3d:Sound Stop num = %x\n";

static void RES_SoundEffectStop(int soundId)
{
    int soundEffectId = *(int *)((unsigned char *)ResData + soundId * 4 + CASINO_SOUND_ID_TABLE_OFFSET);

    dprintf(D_00A0BBD8, soundId, soundEffectId);
    xglSoundEffectStopDirect(soundEffectId);
}

/* ov11:0x00a00af0. Loads the casino resource table (CASINO.res) into the
 * fixed asset staging address and records it as ResData, the same
 * (void *)FIXED_ADDRESS pattern PokerInit and the other loaders below use. */
static const char D_00A0BC08[] = "data\\tanaka\\CASINO.res";

#define CASINO_RES_BUFFER 0x01000000

static void RES_Load(void)
{
    xglCdReadFile(D_00A0BC08, (void *)CASINO_RES_BUFFER, 0, 0);
    ResData = (CasinoResourcePrefix *)CASINO_RES_BUFFER;
}

/* The coin balance is the word at offset 0x50 in the persistent save block. */
struct CasinoSaveWork {
    unsigned char unmodeled_00[0x50];
    int coin_balance;
};
extern int dataMoneyBoxInc(int amount);

static int AddCoin(int amount)
{
    struct CasinoSaveWork *save;
    int total;
    int excess;

    save = (struct CasinoSaveWork *)SaveWork;
    total = save->coin_balance + amount;
    if (total > 9999999) {
        excess = total - 9999999;
        dataMoneyBoxInc(excess * 8);
        amount -= excess;
    }
    save = (struct CasinoSaveWork *)SaveWork;
    save->coin_balance += amount;
    return 1;
}

extern int D_00A0DD34[];

static short SlGetPic(int reel, int positionOffset, int row)
{
    int position;
    int pictureIndex;
    short *pictures;

    position = D_00A0DD34[reel] / 64;
    pictures = realpt[reel + row * 3];
    position -= positionOffset;
    pictureIndex = position + 2;
    if (pictureIndex < 0) {
        pictureIndex = position + 23;
    }
    if (pictureIndex >= 21) {
        pictureIndex -= 21;
    }
    return pictures[pictureIndex];
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlRealDraw);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", decprint);

static void decprint(int value, int digits, int x, int y);
extern int D_00A0DDC8;

static void SlCashPrint(void)
{
    decprint(((struct CasinoSaveWork *)SaveWork)->coin_balance, -1, 440, 309);
    decprint(D_00A0DDC8, -1, 448, 373);
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", pay_print);

/* ov11:0x00a01060. Draws the three-digit slot total stake at its fixed
 * screen position through decprint (ov11:0x00a00de0, LOCAL asm sibling). */
static void decprint(int value, int digits, int x, int y);
extern short D_00A0DD74;

static void total_pay_print(void)
{
    decprint(D_00A0DD74, 3, 0x1DC, 0x1B);
}

static int check_mode(void)
{
    int index;

    for (index = 0; index < 3; index++) {
        if (Gwork[index + 8] == 1) {
            return 1;
        }
    }
    return 0;
}

static void MakeSprite(void *sprite, void *rect);
static void SetDrawStatus(int mode, int enable);

static void SlPayWindowPrint(void)
{
    int rectIndex;
    int windowIndex;

    SetDrawStatus(0, 0);
    windowIndex = check_mode() != 1;
    for (rectIndex = 0; rectIndex < 5; rectIndex++) {
        MakeSprite(pay_window[windowIndex], pay_window_rect_4[rectIndex]);
    }
}


static int check_spin(void)
{
    int active;

    active = check_mode();
    if (active == 0) {
        status_keep_5 = 0;
        return active;
    }
    if (active != status_keep_5) {
        status_keep_5 = active;
        return 1;
    }
    status_keep_5 = active;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SpinBtnPrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlStopBtnPrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlDecPrintSmall);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlRateTableDraw);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlResultDraw);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlModePrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlGuidPrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", point_control);

/* ov11:0x00a01e28. Configures background draw state, then emits the fixed
 * slot background sprite built from BoxRect (ov11:0x00a0a0e0) into BoxSpr
 * (ov11:0x00a0a0c0). MakeSprite, SetDrawStatus and SetTest are LOCAL asm
 * siblings whose parameter roles beyond this one call site are not evidenced. */
static void MakeSprite(void *sprite, void *rect);
static void SetDrawStatus(int mode, int enable);
static void SetTest(int value);

static void SlBgDraw(void)
{
    SetDrawStatus(3, 0);
    SetTest(0x5000D);

    /* MakeSprite is the last statement of this void function and gcc 2.96
     * compiles a trailing call like that as a sibling jump; the original
     * keeps a real jal plus the shared epilogue (ld $31/jr $31). Wrapping
     * only this call in do{}while(0) defeats the sibcall pass instead
     * (CP-0288, config/compiler-patterns.json). */
    do {
        MakeSprite(BoxSpr, BoxRect);
    } while (0);
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlCoinEntry);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlPrizeCheck);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlLineCheck);

/* ov11:0x00a02460. Runs the complete slot line evaluation and reports
 * whether the resulting payout is nonzero. SlLineCheck is a LOCAL asm
 * sibling. */
static void SlLineCheck(void);
extern int D_00A0DDC8;

static int check_real(void)
{
    SlLineCheck();
    return D_00A0DDC8 != 0;
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", paid_action);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", linepay_check);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", slot_key);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlPrizeEffect);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlChangeRealEffect);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlEvent);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlHelpMode);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", slot_main);

/* ov11:0x00a034d8. Loads the five slot-machine texture resources into their
 * fixed rendering buffers through the resource loader. */
static const char D_00A0BE10[] = "data\\tanaka\\slot_1.xtx";
static const char D_00A0BE28[] = "data\\tanaka\\slot_2.xtx";
static const char D_00A0BE40[] = "data\\tanaka\\base.xtx";
static const char D_00A0BE58[] = "data\\tanaka\\w1.xtx";
static const char D_00A0BE70[] = "data\\tanaka\\help_all.xtx";

#define SLOT_1_TEX_BUFFER 0x01008000
#define SLOT_2_TEX_BUFFER 0x01048800
#define SLOT_BASE_TEX_BUFFER 0x01089000
#define SLOT_W1_TEX_BUFFER 0x010C9800
#define SLOT_HELP_TEX_BUFFER 0x0110A000

static void init_slot(void)
{
    xglCdReadFile(D_00A0BE10, (void *)SLOT_1_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0BE28, (void *)SLOT_2_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0BE40, (void *)SLOT_BASE_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0BE58, (void *)SLOT_W1_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0BE70, (void *)SLOT_HELP_TEX_BUFFER, 0, 0);
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgTimePrint);

/* ov11:0x00a03610. Draws the pending nine-reel payout at its fixed screen
 * position, multiplied by the current power-of-two streak factor. pow
 * (main/tu284) computes the streak factor; litodp and dptoli (main/tu330,
 * main/tu331) are this compiler's own int<->double conversion routines.
 * They pass a double as a raw 64-bit value in an integer register (this
 * target has no hardware double support); a genuinely double-typed call
 * chain here schedules the calls' delay slots differently from the
 * original bytes, so the encoded form is required. DOUBLE_TWO is the
 * IEEE-754 double-precision encoding of the literal 2.0. */
#define DOUBLE_TWO 0x4000000000000000ULL

extern unsigned long long litodp(int value);
extern int dptoli(unsigned long long value);
extern int pow(unsigned long long base, unsigned long long exponent);

static void NgPaidPrint(void)
{
    decprint(Gwork[42] * dptoli(pow(DOUBLE_TWO, litodp(Gwork[46]))), 7, 0x1B2, 0x182);
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgGetsDownBtnPrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgSpinBtnPrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgLamp);

/* ov11:0x00a038c0. Selects the loaded nine-game slot background texture
 * (SLOT_2_TEX_BUFFER, the "slot_2.xtx" buffer init_slot fills), sets the
 * packet's GS register write and draw state, then emits the fixed
 * nine-game background sprite. SetRegAD and Pkt are this overlay's own
 * LOCAL data/sibling; xglPacketTextureTrans is main:0x0022c268
 * (src/main/xgl_packet.c, still INCLUDE_ASM there). */
extern void xglPacketTextureTrans(void *buffer);
static void SetRegAD(XglPacket *packet, int reg, int value);
static XglPacket *Pkt;

static void NineGameBG_Draw(void)
{
    xglPacketTextureTrans((void *)SLOT_2_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    SetDrawStatus(3, 0);
    MakeSprite(NineGameBG_Tex_43, NineGameBG_Rect_44);
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgStopGuidPt);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgFwdGuidPt);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgStopBtnPrint);

extern short D_00A0DDE2[];

static void NgAllStop(void)
{
    int forced_stop;
    short *reel_status;
    int remaining;

    forced_stop = 3;
    reel_status = D_00A0DDE2;
    remaining = 8;
    do {
        if (reel_status[2] == 0) {
            reel_status[2] = forced_stop;
        }
        reel_status += 4;
    } while (--remaining >= 0);
    RES_SoundEffectStop(47);
}

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

/* ov11:0x00a04dc0. Selects the loaded poker table-1 background texture
 * (0x0138f000, the "poker_1.xtx" buffer PokerInit below fills as
 * POKER_TABLE1_TEX_BUFFER) and emits the fixed poker background sprite. */

static void PoBgDraw(void)
{
    SetDrawStatus(3, 0);
    xglPacketTextureTrans((void *)0x0138F000);
    SetRegAD(Pkt, 0x3F, 0);
    SetDrawStatus(3, 0);
    MakeSprite(tex_68, rect_69);
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoCardDraw);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoCardDrawAnime);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoBonusCardAnime);

/* ov11:0x00a05358. Emits the single fixed poker payout table sprite.
 * MakeSprite is a LOCAL asm sibling. */
static void MakeSprite(void *sprite, void *rect);

static void PoRateTableDraw(void)
{
    MakeSprite(tex_80, rect_81);
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoBonusBGDraw);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoResetCard);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoShuffleCard);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoSelectMarkDraw);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoKey);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoCardFlip);

/* ov11:0x00a05a78. Maps a 0..0x33 standard 52-card value (13 ranks per
 * suit) to its suit index. Callers (PoChk4 and PokerMain) read cardValue
 * as a signed `lh` from the sorted hand in Gwork; any value outside
 * 0..0x33 reports 0, matching the pre-switch init below. */
static int PoGetMark(int cardValue)
{
    int mark;

    mark = 0;
    switch (cardValue) {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7:
    case 0x8:
    case 0x9:
    case 0xA:
    case 0xB:
    case 0xC:
        mark = 0;
        break;
    case 0xD:
    case 0xE:
    case 0xF:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
        mark = 1;
        break;
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
    case 0x1F:
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:
    case 0x26:
        mark = 2;
        break;
    case 0x27:
    case 0x28:
    case 0x29:
    case 0x2A:
    case 0x2B:
    case 0x2C:
    case 0x2D:
    case 0x2E:
    case 0x2F:
    case 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
        mark = 3;
        break;
    }
    return mark;
}

static short PoGetBase(int cardValue);

static short PoGetBase(int cardValue)
{
    short base;

    base = 0;
    if (cardValue != 1) {
        if (cardValue >= 2) {
            switch (cardValue) {
            case 2:
                base = 26;
                break;
            case 3:
                base = 39;
                break;
            }
        }
    } else {
        base = 13;
    }
    return base;
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoSortCard);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoMultiChk);

struct PokerHandWork {
    unsigned char unmodeled_00[0x1A2];
    short cards[5];
};

static int PoFlushChk(void)
{
    struct PokerHandWork *hand;
    short *card;
    int suit;
    int index;

    hand = (struct PokerHandWork *)Gwork;
    card = &hand->cards[1];
    suit = PoGetMark(hand->cards[0]);
    for (index = 1; index < 5; index++) {
        if (suit != PoGetMark(*card++)) {
            return 0;
        }
    }
    return 1;
}

/* PoMultiChk classifies the sorted hand's rank multiplicities and
 * PoFlushChk classifies same-suit runs. dprintf is the overlay's debug
 * printf. */
static int PoMultiChk(void);
static int PoFlushChk(void);
extern void dprintf(const char *fmt, ...);

/* ov11:0x00a05de0. Reports category 1, printing D_00A0C078 through
 * dprintf, when PoMultiChk's multiplicity equals 1. */
static const char D_00A0C078[16] = "\xA5\xEF\xA5\xF3\xA5\xDA\xA5\xA2\n";

static int PoChk1(void)
{
    if (PoMultiChk() == 1) {
        dprintf(D_00A0C078);
        return 1;
    }
    return 0;
}

/* ov11:0x00a05e20. Reports category 2, printing D_00A0C088 through
 * dprintf, when PoMultiChk's multiplicity equals 2. */
static const char D_00A0C088[16] = "\xA5\xC4\xA1\xBC\xA5\xDA\xA5\xA2\n";

static int PoChk2(void)
{
    if (PoMultiChk() == 2) {
        dprintf(D_00A0C088);
        return 2;
    }
    return 0;
}

/* ov11:0x00a05e60. Reports category 3, printing D_00A0C098 through
 * dprintf, when PoMultiChk's multiplicity equals 3. */
static const char D_00A0C098[16] = "\xA5\xB9\xA5\xEA\xA1\xBC\xA5\xAB\xA1\xBC\xA5\xC9\n";

static int PoChk3(void)
{
    if (PoMultiChk() == 3) {
        dprintf(D_00A0C098);
        return 3;
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoChk4);

/* ov11:0x00a05fb0. Substitutes category 5 and prints D_00A0C0B8 through
 * dprintf whenever PoFlushChk returns nonzero; PoFlushChk's own nonzero
 * value is discarded rather than reused as the category. */
static const char D_00A0C0B8[16] = "\xA5\xD5\xA5\xE9\xA5\xC3\xA5\xB7\xA5\xE5\n";

static int PoChk5(void)
{
    int category = PoFlushChk();

    if (category != 0) {
        dprintf(D_00A0C0B8);
        category = 5;
    }
    return category;
}

/* ov11:0x00a05fe8. Reports category 6, printing D_00A0C0C8 through
 * dprintf, when PoMultiChk's multiplicity equals 4. */
static const char D_00A0C0C8[16] = "\xA5\xD5\xA5\xEB\xA5\xCF\xA5\xA6\xA5\xB9\n";

static int PoChk6(void)
{
    if (PoMultiChk() == 4) {
        dprintf(D_00A0C0C8);
        return 6;
    }
    return 0;
}

/* ov11:0x00a06028. Reports category 7, printing D_00A0C0D8 through
 * dprintf, when PoMultiChk's multiplicity equals 5. */
static const char D_00A0C0D8[16] = "\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xAB\xA1\xBC\xA5\xC9\n";

static int PoChk7(void)
{
    if (PoMultiChk() == 5) {
        dprintf(D_00A0C0D8);
        return 7;
    }
    return 0;
}

static const char D_00A0C0E8[24] = "\xA5\xB9\xA5\xC8\xA5\xEC\xA1\xBC\xA5\xC8\xA5\xD5\xA5\xE9\xA5\xC3\xA5\xB7\xA5\xE5\n";

static int PoChk8(void)
{
    struct PokerHandWork *hand;
    int first;

    if (PoFlushChk() == 0) {
        return 0;
    }
    hand = (struct PokerHandWork *)Gwork;
    first = hand->cards[0];
    if (hand->cards[1] != first + 1 ||
        hand->cards[2] != first + 2 ||
        hand->cards[3] != first + 3) {
        return 0;
    }
    if (hand->cards[4] == first + 4) {
        dprintf(D_00A0C0E8);
        return 8;
    }
    return 0;
}

/* ov11:0x00a060e8. Reports category 9 when PoFlushChk finds a flush and
 * the sorted hand's ranks, read from the Gwork work block, form a
 * consecutive run starting at the lowest card: base, base+9, base+0xA,
 * base+0xB, base+0xC. PoGetBase is a LOCAL asm sibling; the double
 * PoGetBase(PoGetBase(base)) round trip is the original's own check, not
 * a simplification. Any other outcome, including a flush that is not
 * this run, reports 0. */
#define POKER_HAND_RANK_INDEX 0xD1 /* short index; 0x1A2 / sizeof(short) */

static const char D_00A0C100[32] = "\xA5\xED\xA5\xA4\xA5\xE4\xA5\xEB\xA5\xB9\xA5\xC8\xA5\xEC\xA1\xBC\xA5\xC8\xA5\xD5\xA5\xE9\xA5\xC3\xA5\xB7\xA5\xE5\n";

static int PoChk9(void)
{
    short *hand;
    short base;

    if (PoFlushChk() == 0) {
        return 0;
    }

    hand = (short *)Gwork;
    base = hand[POKER_HAND_RANK_INDEX];
    if (base == PoGetBase(PoGetBase(base))) {
        if (hand[POKER_HAND_RANK_INDEX + 1] == base + 9 &&
            hand[POKER_HAND_RANK_INDEX + 2] == base + 0xA &&
            hand[POKER_HAND_RANK_INDEX + 3] == base + 0xB &&
            hand[POKER_HAND_RANK_INDEX + 4] == base + 0xC) {
            dprintf(D_00A0C100);
            return 9;
        }
    }
    return 0;
}

/* ov11:0x00a06190. Sorts the hand, then checks hand-rank categories from
 * nine downward until one reports nonzero, plays one of three sound tiers
 * for the result, and reports it through the same debug printf as the
 * PoChk* family (D_00A0C120, "result = %d\n").
 *
 * The nested PoChk9..PoChk1 chain and the shared-tail-block `goto
 * defaultSound` are the shape recorded as CP-0254 in
 * config/compiler-patterns.json: a flat "if (category == 0) category =
 * PoChkN();" per-line chain lets cc1 2.96's range analysis fold the
 * repeated zero tests together with the sound-tier comparisons instead of
 * keeping each PoChkN call, and a fully structured if/else-if/else sound
 * dispatch duplicates the shared RES_SoundEffect(0x3A) call into two
 * physical blocks instead of the original's single call reached from both
 * the category<=0 and category>=0xA paths. */
static void PoSortCard(void);
static int PoChk4(void);
static int PoChk8(void);
static int PoChk9(void);
static void RES_SoundEffect(int soundId);
static const char D_00A0C120[] = "result = %d\n";

static int PoCardCheck(void)
{
    int category;

    PoSortCard();
    category = PoChk9();
    if (category == 0) {
        category = PoChk8();
        if (category == 0) {
            category = PoChk7();
            if (category == 0) {
                category = PoChk6();
                if (category == 0) {
                    category = PoChk5();
                    if (category == 0) {
                        category = PoChk4();
                        if (category == 0) {
                            category = PoChk3();
                            if (category == 0) {
                                category = PoChk2();
                                if (category == 0) {
                                    category = PoChk1();
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (category > 0) {
        if (category >= 6) {
            if (category < 0xA) {
                RES_SoundEffect(0x39);
            } else {
                goto defaultSound;
            }
        } else {
            RES_SoundEffect(0x38);
        }
    } else {
defaultSound:
        RES_SoundEffect(0x3A);
    }

    dprintf(D_00A0C120, category);
    return category;
}

/* ov11:0x00a062b8. Draws the poker result lamp sprite for hand category
 * index category, selecting its fixed rect from a shared row of 0x10-byte
 * rects (the same tex/rect MakeSprite pair shape used throughout this TU;
 * the row's entry count beyond this stride is not separately evidenced). */

static void PoResultLamp(int category)
{
    SetDrawStatus(0, 0);
    MakeSprite(tex_98, result_rect_97[category]);
    SetDrawStatus(3, 0);
}

/* ov11:0x00a06308. Stores the caller-supplied poker sequence state in the
 * global sequence word. */
extern int D_00A0DEA8;

static void PoSeqChange(int sequenceState)
{
    D_00A0DEA8 = sequenceState;
}

/* ov11:0x00a06318. Looks up the currently selected bet level's ready
 * message and message-box width from two fixed four-entry tables and
 * forwards both to the message window renderer. D_00A0DD2C is the level
 * index level_select stores (0..3); the four strings are the "Play N
 * coins" prompts for the four bet levels. The compiled form copies each
 * table onto the stack as a 16-byte aggregate before indexing it, which is
 * why each table is typed as a one-member array struct rather than a bare
 * array (a bare array cannot be copied by value in C). PoWindowMessage is
 * a LOCAL asm sibling. */
typedef struct {
    const char *text[4];
} PokerReadyMessageTable;

typedef struct {
    int width[4];
} PokerReadyWidthTable;

static void PoWindowMessage(const char *message, int width);
extern int D_00A0DD2C;
extern const char D_00A0C130[];
extern const char D_00A0C150[];
extern const char D_00A0C170[];
extern const char D_00A0C190[];
extern const PokerReadyMessageTable D_00A0C1B8; /* "Play 5/10/30/100 coins" */
extern const PokerReadyWidthTable D_00A0C1C8;

static void PokerReadyMes(void)
{
    PokerReadyMessageTable readyMessage = D_00A0C1B8;
    PokerReadyWidthTable readyWidth = D_00A0C1C8;

    PoWindowMessage(readyMessage.text[D_00A0DD2C], readyWidth.width[D_00A0DD2C]);
}

const PokerReadyMessageTable D_00A0C1B8 = {{
    D_00A0C130, D_00A0C150, D_00A0C170, D_00A0C190,
}};
const PokerReadyWidthTable D_00A0C1C8 = {{29, 30, 30, 31}};

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PokerMain);

/* ov11:0x00a06c08. Resets five Gwork work-state words used by the poker
 * minigame: its only caller, GameModeChange, uses it to clear the main
 * sequence, display flags, bonus mode and bonus round index before a
 * poker session starts. Field identities beyond their byte offsets are
 * not separately evidenced. */
static void PoInitWork(void)
{
    Gwork[96] = 0;
    Gwork[97] = 0;
    Gwork[100] = 0;
    Gwork[101] = 0;
    Gwork[98] = 0;
}

/* ov11:0x00a06c28. Loads the six poker minigame textures (four suits plus
 * two poker-table backgrounds; the "data\\tanaka\\*.xtx" paths are these
 * D_00A0Cxxx strings themselves) into fixed EE main RAM staging addresses,
 * following the same (void *)FIXED_ADDRESS pattern as src/main/game_over.c's
 * IMAGE_LOAD_BUFFER. */
static const char D_00A0C270[] = "data\\tanaka\\spade.xtx";
static const char D_00A0C288[] = "data\\tanaka\\clover.xtx";
static const char D_00A0C2A0[] = "data\\tanaka\\heart.xtx";
static const char D_00A0C2B8[] = "data\\tanaka\\dia.xtx";
static const char D_00A0C2D0[] = "data\\tanaka\\poker_1.xtx";
static const char D_00A0C2E8[] = "data\\tanaka\\poker_2.xtx";

#define POKER_SPADE_TEX_BUFFER  0x0128D000
#define POKER_CLOVER_TEX_BUFFER 0x012CD800
#define POKER_HEART_TEX_BUFFER  0x0130E000
#define POKER_DIA_TEX_BUFFER    0x0134E800
#define POKER_TABLE1_TEX_BUFFER 0x0138F000
#define POKER_TABLE2_TEX_BUFFER 0x013CF800

static void PokerInit(void)
{
    xglCdReadFile(D_00A0C270, (void *)POKER_SPADE_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0C288, (void *)POKER_CLOVER_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0C2A0, (void *)POKER_HEART_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0C2B8, (void *)POKER_DIA_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0C2D0, (void *)POKER_TABLE1_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0C2E8, (void *)POKER_TABLE2_TEX_BUFFER, 0, 0);
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", GameModeChange);

/* ov11:0x00a06df0. Builds a fixed 0xB8x0x70 rect at the caller-supplied
 * (y, x) origin on the stack and draws the submenu window sprite into it;
 * this is the same {y, x, width, height} rect layout MakeSprite's other
 * (data-supplied) rect arguments use elsewhere in this TU. windowId itself
 * is not read here; level_select and exchange_select forward it on to
 * submenu_select right after calling sub_window. */
typedef struct {
    int y;
    int x;
    int width;
    int height;
} SubWindowRect;


static void sub_window(int windowId, int y, int x)
{
    SubWindowRect rect;

    SetDrawStatus(1, 0);
    rect.y = y;
    rect.x = x;
    rect.width = 0xB8;
    rect.height = 0x70;
    MakeSprite(SubWindow_Tex, &rect);
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", submenu_select);

/* ov11:0x00a07000. Draws a four-choice submenu bound to the shared game
 * level selection. sub_window's own body (asm/nonmatchings/ov11/mini_g/
 * sub_window.s) overwrites its first argument before using it, so windowId
 * only reaches submenu_select; y and x are the row/column submenu_select
 * stores for the first icon (offset there by 0x34 and 0xC before laying the
 * remaining icons out, per its and exchange_select's own disassembly).
 * level_select's own tail call (`j submenu_select`) is why its return value
 * is submenu_select's, and menu_mode branches on it (`beqz $2` right after
 * `jal level_select`). */
static void sub_window(int windowId, int y, int x);
static int submenu_select(int windowId, int y, int x, int iconCount,
                          void *icons, int *selected);
extern int D_00A0DD2C;

static int level_select(int windowId, int y, int x)
{
    sub_window(windowId, y, x);
    return submenu_select(windowId, y, x, 4, Level_Tex, &D_00A0DD2C);
}

/* ov11:0x00a07060. Draws a three-choice submenu bound to the shared exchange
 * mode selection, the same sub_window()+submenu_select() shape level_select
 * uses above; exchange_select's own tail call (`j submenu_select`) is why its
 * return value is submenu_select's. */
extern int D_00A0DD24;

static int exchange_select(int windowId, int y, int x)
{
    sub_window(windowId, y, x);
    return submenu_select(windowId, y, x, 3, ExChange_Tex, &D_00A0DD24);
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", menu_mode);

/* ov11:0x00a07410. Loads the menu background and window textures, then
 * resets the menu mode and game level selection in the Gwork work
 * block. */
static const char D_00A0C320[] = "data\\tanaka\\BG.xtx";
static const char D_00A0C338[] = "data\\tanaka\\window.xtx";

#define MENU_BG_TEX_BUFFER 0x0114A800
#define MENU_WINDOW_TEX_BUFFER 0x0118B000
#define MENU_LEVEL_INDEX 8 /* short index; 0x10 / sizeof(short) */

static void init_menu_mode(void)
{
    xglCdReadFile(D_00A0C320, (void *)MENU_BG_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0C338, (void *)MENU_WINDOW_TEX_BUFFER, 0, 0);

    Gwork[3] = 0;
    ((short *)Gwork)[MENU_LEVEL_INDEX] = 0;
}

/* ov11:0x00a07470. Loads the casino test-mode background texture. */
extern const char D_00A0BE40[]; /* "data\\tanaka\\base.xtx" */

#define BASE_TEX_BUFFER            0x01089000
#define TEST_MODE_BASE_TEX_BUFFER  0x01593000

static void init_test_mode(void)
{
    xglCdReadFile(D_00A0BE40, (void *)TEST_MODE_BASE_TEX_BUFFER, 0, 0);
}

/* ov11:0x00a074a0. Lazily loads the test-mode background texture on first
 * entry (flg_104 latches this), then selects it and draws the fixed
 * test-mode background sprite. xglPacketGetCurrent is main:0x0022c450
 * (src/main/xgl_packet.c). */
extern XglPacket *xglPacketGetCurrent(void);

static void test_mode(void)
{
    if (flg_104 == 0) {
        init_test_mode();
        flg_104 = 1;
    }
    Pkt = xglPacketGetCurrent();
    xglPacketTextureTrans((void *)TEST_MODE_BASE_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    SetDrawStatus(3, 0);
    MakeSprite(BGTEST_Tex, BGTEST_Rect);
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", DecPrint2);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", UtlCurPrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", coin_main);

/* ov11:0x00a07c38. Loads the casino coin-exchange minigame's background and
 * exchange textures (D_00A0BE40 is the same "base.xtx" path init_test_mode
 * uses, into a different fixed buffer address here). */
static const char D_00A0C390[] = "data\\tanaka\\exchange2.xtx";

#define COIN_EXCHANGE_TEX_BUFFER 0x011CB800

static void init_coin(void)
{
    xglCdReadFile(D_00A0BE40, (void *)BASE_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0C390, (void *)COIN_EXCHANGE_TEX_BUFFER, 0, 0);
}

/* ov11:0x00a07c80. Loads the casino shop's exchange and background textures
 * (D_00A0BE40 "base.xtx" again, into the same fixed buffer init_coin uses). */
static const char D_00A0C3B0[] = "data\\tanaka\\exchange1.xtx";

#define SHOP_EXCHANGE_TEX_BUFFER 0x0120C000

static void init_shop(void)
{
    xglCdReadFile(D_00A0C3B0, (void *)SHOP_EXCHANGE_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0BE40, (void *)BASE_TEX_BUFFER, 0, 0);
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", init_shpmenu);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", shop_main);

/* ov11:0x00a083f8. Loads the data-viewer minigame's dataviewer and sampler
 * textures. */
static const char D_00A0C590[] = "data\\tanaka\\dataviewer.xtx";
static const char D_00A0C5B0[] = "data\\tanaka\\sam.xtx";

#define VW_DATAVIEWER_TEX_BUFFER 0x0124C800
#define VW_SAM_TEX_BUFFER        0x01410000

static void VW_Init(void)
{
    xglCdReadFile(D_00A0C590, (void *)VW_DATAVIEWER_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0C5B0, (void *)VW_SAM_TEX_BUFFER, 0, 0);
}

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

/* xglFlagsSet1 is main:0x00222938 (src/main/xgl_flags.c, external linkage
 * there). dataMoneyBoxInc is main:0x002BCE40, still INCLUDE_ASM there
 * (src/main/data.c); src/main/runtime.c already forwards to it with this
 * same signature. AddCoin (ov11:0x00a00b28) and MUSIC_CALL (ov11:0x00a08dc0)
 * are LOCAL asm siblings of this TU. */
extern int xglFlagsSet1(int bit_offset, int value);
extern int dataMoneyBoxInc(int amount);
static int AddCoin(int amount);
static void MUSIC_CALL(int musicId);

/* ov11:0x00a08f40. Resets the four leading Gwork words and the six Gwork
 * shorts at 0x240-0x24A, loads the casino resource table and starts the
 * background sound queue, applies four debug-mode config-flag unlocks and
 * grants a near-maximum bank balance plus a matching coin total when
 * RES_IsDebugMode() is set, then seeds six more Gwork shorts starting at
 * 0x54 (a leading sentinel of 5 followed by a descending fill of the next
 * five with 4..0). Field identities beyond their byte offsets are not
 * separately evidenced. Its only caller, MiniG_Init, runs this once. */
/* The original materializes Gwork's address once (%lo(Gwork)=0, so
 * `addiu s0,v0,0`) and reaches every field below off that same register
 * with an immediate byte offset; indexing Gwork as a word array and a
 * short array through separate expressions instead recomputes the base at
 * Gwork+0x240, which does not reproduce this. */
#define WORK_RESET_WORD_0 0x0
#define WORK_RESET_WORD_1 0x4
#define WORK_RESET_WORD_2 0x8
#define WORK_RESET_WORD_3 0xC
#define WORK_MODE_SHORT_0 0x240
#define WORK_MODE_SHORT_1 0x242
#define WORK_MODE_SHORT_2 0x244
#define WORK_MODE_SHORT_3 0x246
#define WORK_MODE_SHORT_4 0x248
#define WORK_MODE_SHORT_5 0x24A
#define WORK_STAKE_SENTINEL 0x54
#define WORK_STAKE_TABLE_END 0x5E

static void InitWork(void)
{
    unsigned char *work = (unsigned char *)Gwork;
    short *slot;
    int value;

    *(short *)(work + WORK_MODE_SHORT_0) = 0;
    *(short *)(work + WORK_MODE_SHORT_1) = 0;
    *(short *)(work + WORK_MODE_SHORT_2) = 0;
    *(short *)(work + WORK_MODE_SHORT_3) = 0;
    *(short *)(work + WORK_MODE_SHORT_4) = 0;
    *(short *)(work + WORK_MODE_SHORT_5) = 0;
    *(int *)(work + WORK_RESET_WORD_0) = 0;
    *(int *)(work + WORK_RESET_WORD_1) = 0;
    *(int *)(work + WORK_RESET_WORD_2) = 0;
    *(int *)(work + WORK_RESET_WORD_3) = 0;

    RES_Load();
    MUSIC_CALL(0);

    if (RES_IsDebugMode() != 0) {
        xglFlagsSet1(0x73, 1);
        xglFlagsSet1(0x12D, 1);
        xglFlagsSet1(0x185, 1);
        xglFlagsSet1(0x187, 1);
        dataMoneyBoxInc(0x3B9AC9FF);
        AddCoin(0x989298);
    }

    *(short *)(work + WORK_STAKE_SENTINEL) = 5;
    slot = (short *)(work + WORK_STAKE_TABLE_END);
    value = 4;
    do {
        *slot = value;
        value--;
        slot--;
    } while (value >= 0);
}

/* ov11:0x00a09020. Logs the configured heap address, then delegates all
 * casino state initialization to InitWork (LOCAL asm sibling). */
static void InitWork(void);
static const char D_00A0C6D0[] = "work size = %x\n";

void MiniG_Init(void)
{
    dprintf(D_00A0C6D0, 0x7CF000);
    InitWork();
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", MiniG_Main);



static short real01array[21] = { 1, 3, 1, 0, 4, 4, 0, 1, 3, 1, 0, 2, 1, 0, 1, 2, 1, 0, 5, 5, 0 };

static short real02array[21] = { 3, 0, 1, 3, 0, 0, 0, 1, 2, 0, 4, 0, 1, 2, 0, 1, 0, 0, 3, 5, 1 };

static short real03array[21] = { 1, 0, 0, 0, 2, 1, 0, 2, 1, 4, 2, 0, 0, 1, 5, 3, 1, 0, 5, 5, 3 };

static short real11array[21] = { 1, 3, 1, 0, 4, 4, 0, 1, 3, 1, 0, 2, 1, 0, 1, 2, 1, 0, 7, 7, 0 };

static short real12array[21] = { 3, 0, 1, 3, 0, 0, 0, 1, 2, 0, 4, 0, 1, 2, 0, 1, 0, 0, 3, 7, 1 };

static short real13array[21] = { 1, 0, 0, 0, 2, 1, 0, 2, 1, 4, 2, 0, 0, 1, 7, 3, 1, 0, 7, 7, 3 };

static short real21array[21] = { 1, 3, 1, 6, 4, 6, 0, 1, 3, 1, 6, 2, 1, 0, 1, 2, 1, 0, 5, 5, 0 };

static short real22array[21] = { 3, 0, 1, 3, 6, 0, 0, 1, 2, 0, 4, 0, 1, 2, 0, 1, 0, 0, 3, 5, 1 };

static short real23array[21] = { 1, 0, 0, 6, 2, 1, 6, 2, 1, 4, 2, 0, 0, 1, 5, 3, 1, 0, 5, 5, 3 };

static short real31array[21] = { 6, 3, 1, 6, 4, 6, 6, 1, 3, 1, 0, 2, 1, 0, 6, 2, 1, 6, 5, 5, 6 };

static short real32array[21] = { 3, 0, 1, 3, 0, 0, 0, 1, 2, 0, 4, 0, 1, 2, 0, 1, 0, 0, 3, 5, 1 };

static short real33array[21] = { 1, 0, 0, 0, 2, 1, 0, 2, 1, 4, 2, 0, 0, 1, 5, 3, 1, 0, 5, 5, 3 };

static short real41array[21] = { 8, 7, 8, 7, 8, 7, 8, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

static short real42array[21] = { 8, 8, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 8, 8, 7, 8, 8, 8, 8 };

static short real43array[21] = { 8, 8, 8, 8, 7, 8, 8, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

static short real51array[21] = { 1, 3, 8, 0, 4, 4, 0, 8, 3, 1, 0, 2, 8, 0, 8, 2, 1, 0, 5, 5, 0 };

static short real52array[21] = { 3, 0, 1, 8, 0, 8, 8, 1, 2, 8, 4, 0, 1, 2, 0, 8, 0, 8, 3, 5, 1 };

static short real53array[21] = { 1, 0, 8, 8, 2, 1, 0, 2, 1, 8, 2, 8, 0, 8, 5, 3, 1, 0, 5, 5, 3 };

const char D_00A0C130[32] = "Play 5 coins per round poker\241\251";

const char D_00A0C150[32] = "Play 10 coins per round poker\241\251";

const char D_00A0C170[32] = "Play 30 coins per round poker\241\251";

const char D_00A0C190[40] = "Play 100 coins per round poker\241\251";
