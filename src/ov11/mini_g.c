/*
 * OV11 original TU 1: 0x00a00100..0x00a091f8 (135 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_cd.h"
#include "main/xgl_flags.h"
#include "main/xgl_font.h"
#include "main/xgl_packet.h"
#include "main/xgl_sound.h"
#include "ov11/cajino_get.h"
#include "ov11/mini_g.h"
#include "mini_g_private.h"

static CasinoResourcePrefix *ResData;
static union CasinoWork Gwork;

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

/* GIF tags of the packet helpers below, stored in the overlay's .rodata (the
 * D_00A0BBxx constants). Each function copies its tag to a stack local
 * because the 128-bit argument is passed by value. */
typedef unsigned int Ov11Quadword __attribute__((mode(TI)));
typedef union Ov11GifTag {
    struct { u64 lo, hi; } part;
    Ov11Quadword quad;
} Ov11GifTag;

/* Declared here: every external function and datum the casino uses. Data
 * defined in this file (the static tables and the D_00A0Cxxx strings) keeps
 * its definition where it is. */

/* Packet helpers (sceVif1Pk*), GS packet tags and box vertices. */
extern void sceVif1PkCnt(XglPacket *packet, int mode);
extern void sceVif1PkOpenDirectCode(XglPacket *packet, int mode);
extern void sceVif1PkOpenGifTag(XglPacket *packet, Ov11Quadword tag);
extern void sceVif1PkAddGsAD(XglPacket *packet, int reg, u64 value);
extern void sceVif1PkAddGsData(XglPacket *packet, u64 value);
extern void sceVif1PkCloseGifTag(XglPacket *packet);
extern void sceVif1PkCloseDirectCode(XglPacket *packet);
extern const Ov11GifTag spriteTextureGifTag;
extern const Ov11GifTag spritePrimGifTag;
extern const Ov11GifTag drawStatusGifTag;
extern const Ov11GifTag setTestGifTag;
extern const Ov11GifTag setRegisterGifTag;
extern const Ov11GifTag nonTextureBoxGifTag;
static unsigned long long BoxPOS[4];

/* Main executable: texture upload, sound, the item box (read as ints by
 * the casino; src/main/data.h declares void for dataAccBoxInc and
 * dataEvtBoxInc, so the header cannot be included), debug printf. */
extern void xglPacketTextureTrans(void *buffer);
extern void xglSoundEffectStopDirect(int soundEffectId);
extern void xglSoundSendSmd(void *smd);
extern int SsdSpuDmaCompleted(int wait);
extern int sprintf(char *buffer, const char *format, ...);
extern int dataMoneyBoxInc(int amount);
extern int dataMoneyBoxChk(void);
extern int dataMoneyBoxDec(int amount);
extern int dataItmBoxChk(int id);
extern int dataItmBoxInc(int id);
extern int dataAccBoxInc(int id);
extern int dataEvtBoxInc(int id);
extern CasinoPadView PadData;

/* Soft-float helpers: they take raw 64-bit values, either a double or its
 * encoding, so these three stay unprototyped. */
extern unsigned long long litodp(int value);
extern int dptoli(unsigned long long value);
extern unsigned long long dpmul(unsigned long long a, unsigned long long b);
extern unsigned long long pow();
extern unsigned long long dpdiv();
extern int dpcmp();

/* Slot: reels, pictures, buttons, pay table, prize effects. */
extern int slotReelPositionTable[];
static struct SpriteTex Real_2[9];
static int xpos_3[3];
static struct SpriteTex slot_window_0;
static struct SpriteRect slot_window_rect_1[3];
static struct SpriteTex decfont[10];
extern int slotPayoutAmount;
extern const struct PayLayout { struct { short x; short y; } row[5]; } linePayPositionTable;
extern short slotTotalPay;
static struct SpriteTex spinbt[3];
static struct SpriteRect spinbt_rect_6;
static int pushcnt_7;
static struct SpriteTex stopbt[3];
static struct SpriteRect stopbt_rect_8[3];
static struct SpriteTex DeciList_9[10];
extern const struct SpriteRect rateTableOption1UpperRect;
extern const struct SpriteRect rateTableOption1MiddleRect;
extern const struct SpriteRect rateTableOption1LowerRect;
extern const struct SpriteRect rateTableOption2Rect;
extern const struct SpriteRect rateTableOption3Rect;
extern const struct SpriteRect rateTableOption4Rect;
static int pos_10[7][2];
static struct SpriteTex PicTable[7];
static struct SpriteTex OptionText[5];
static int rate_11[6];
static int double_rate_12[6];
static struct SpriteTex Pic_13[9];
static struct SpriteTex Total_14;
static struct SpriteTex linetex_15;
static struct SpriteRect linerect_16[9];
extern const struct SpriteRect resultTotalLabelRect;
static struct SpriteTex Text_17[3];
static struct SpriteTex Bar_18;
static struct SpriteTex TimeBar_19;
extern const struct SpriteRect bonusBannerRect;
extern const struct SpriteRect bonusIdleBannerRect;
extern const struct SpriteRect bonusTimeBarFrameRect;
extern const struct SpriteRect bonusTimeBarRect;
extern const struct StakeSteps lineStakeStepTable;
static struct SlotPatternPoint { short reel; short offset; } pat_22[5][3];
extern const char lineCheckFormat[];
extern const char prizeCheckFormat[];
extern const char paidCheckFormat[];
static int inter_23;
extern const struct W22LineIds {int index[5];} linePayCheckOrderTable;
extern const struct SpriteTex prizeEffectTexDefault;
extern const struct SpriteRect prizeEffectRectDefault;
static int mode_24;
static int count_25;
static int dx_26;
static int dy_27;
static struct SpriteTex tex_28;
static struct SpriteRect rect_29;
static struct SpriteTex tex_30;
static int mode_33;
static struct SlReelPanel sht_31[3];
static struct SlReelPanel *psht_32;
static int wait_34;
static int busy_35;
extern const struct SpriteTex slotHelpTex;
extern const struct SpriteRect slotHelpRect;

/* Nine-game. */
extern const struct SpriteRect ngTimeBarRect;
static struct SpriteTex tex_36;
static struct SpriteTex tex_37[3];
static struct SpriteRect rect_38;
static int count_39;
static struct SpriteRect rect_40;
static int count_41;
static struct SpriteRect RateLamp_Rect[9];
static int anm_42;
static int stopbtn_pos_47[9][2];
extern short ngReelStatusWork[];
static short *ng_realtbl[20];
static struct SpriteTex NgReal_Tex_48[2];
static struct NgPanelPos NgReal_49[9];
static unsigned short ptmove_50[11][4];
static int se_51;
extern const struct SpriteTex ngHelpTopTex;
extern const struct SpriteTex ngHelpBottomTex;
extern const struct SpriteRect ngHelpTopRect;
extern const struct SpriteRect ngHelpBottomRect;
extern const struct SpriteRect pokerMoneyDigitRect;
extern const struct SpriteRect pokerMoneyFrameRect;
static struct SpriteTex fontlist_52[10];
static struct SpriteTex waku_53[2];

/* Poker. */
static struct SpriteTex PoCurTex[8];
static int cnt_54;
static struct SpriteRect rect_55;
extern const struct SpriteRect pokerWindowRect;
static struct SpriteTex Ltex_56;
static struct SpriteTex Rtex_57;
static struct SpriteTex tex_58;
extern const char pokerWindowFontControlFirst[];
extern const char pokerWindowFontControlSecond[];
static struct SpriteTex tex_59;
static struct SpriteRect rect_60[2];
static struct SpriteTex type0_tex_61;
static struct SpriteRect type0_rect_62;
static struct SpriteTex type1_tex_63;
static struct SpriteRect type1_rect_64;
static short type0_pos_65[2][2];
static short type1_pos_66[3][2];
static int dialog_cur_67;
static void *CardBuf_70[4];
static struct SpriteTex CardTex[15];
static struct SpriteRect CardPos[5];
static struct PoCardRange { int lo; int hi; } range_71[4];
static struct SpriteRect card_74[5];
static int wait_75;
static int status_73;
static struct SpriteTex tex_72;
static struct SpriteRect card_78;
static int wait_79;
static int status_77;
static struct SpriteTex tex_76;
static int cnt_82;
static int add_83;
static int rate_84;
static struct SpriteTex tex_85;
static struct SpriteRect rect_86;
static struct SpriteTex MarkerTex_87;
static struct SpriteRect MarkerPos_88[5];
static struct SpriteTex PtrTex_89;
static struct SpriteRect PtrPos_90[4];
static int wait_91;
static struct SpriteTex PoSelecterTex[2];
static struct SpriteRect PoSelecterPos[6];
static int cur_92;
static int mode_93;
static short selpos_94[5][2];
static short chgpos_95[2];
static int status_96;
extern const char pokerSortedCardFormat[];
extern const char debugNewlineText[];
extern const char pokerRankMatch1Text[];
extern const char pokerRankMatch2Text[];
extern const char pokerRankMatch3Text[];
extern const char pokerRankSeparatorText[];
extern const char pokerStraightDebugText[];
extern const char pokerPlay5CoinsText[];
extern const char pokerPlay10CoinsText[];
extern const char pokerPlay30CoinsText[];
extern const char pokerPlay100CoinsText[];
extern const PokerReadyMessageTable pokerReadyTexts; /* "Play 5/10/30/100 coins" */
extern const PokerReadyWidthTable pokerReadyWidthTable;
static int init_99;
static int PokerCoinRate[4];
static int PokerRate[10];
extern const char pokerNoMoreCoinsText[];
extern const char pokerContinueGameText[];
extern const char pokerDoubleOrNothingText[];

/* Menu, coin exchange and test mode. */
static int anm_100;
static struct SpriteTex Cursor_Tex[8];
static struct SpriteTex EntryBG_Tex;
static struct SpriteRect EntryBG_Rect;
static struct SpriteRect Cursor_Rect[4];
static int curpos_101;
static int cur_int_102;
static int menu_mode_103;
static struct SpriteTex tex_105[11];
static struct SpriteTex Tex_106[8];
static int anm_107;
static struct SpriteTex BG_Tex;
static struct SpriteRect BG_Rect;
static int cur_108;
static struct CoinCursorPos { short x; short y; } curpt_109[5];
extern const char coinExchangeTitleFontControl[];
extern const char coinExchangeBuyCoinsText[];
extern const char coinExchangeAfterTitleFontControl[];
extern const char coinExchangeListFontControl[];
extern const char coinExchangeExitFontControl[];
extern const char coinExchangeExitText[];
extern const char coinExchangeAfterExitFontControl[];

/* Shop. */
static int shpmenu_work[51];
static int MenuNum;
static int init_110;
static int cur_111;
static int top_112;
extern const char fontModeControl[];
extern const char shopItemNameFontControl[];
extern const char infoFontControl[];

/* Data viewer and music. */
extern const char viewerImageY1PathFormat[];
extern const char tanakaDirectoryPath[];
extern const char viewerImageY2PathFormat[];
extern const char viewerImageW1PathFormat[];
extern const char viewerImageW2PathFormat[];
extern const char viewerImageW3PathFormat[];
extern const char viewerImageW4PathFormat[];
static char fileinfo[30];
static int old_113;
static struct SpriteTex tex_114;
static struct SpriteTex keytex_115;
static struct SpriteRect keyrect_116;
static struct SpriteTex back_tex_118;
static struct SpriteTex ViewSamTex[30];
static struct SpriteRect sam_rect_117;
static int cur_119;
static int ofs_120;
static int init_121;
static int cnt_122;
extern const char viewerCursorText[];
extern const char viewerTitleFontControl[];
extern const char viewerInfoFontControl[];
static int mode_123;
static int pic_124;
static int MUSICLIST_125[1];
static int oldreq_126;
extern const char miniSlotSedPath[];
extern const char miniSlotSwdPath[];
extern const char musicSmdPathFormat[];
extern const char soundDirectoryPath[];
extern const char musicSwdPathFormat[];
extern const char versionDebugFormat[];
extern const char casinoBuildDateText[];

/* Preserve original compiler-owned NOBITS ordering: the BoxRGBA storage
 * must precede Pkt in the source declaration order for this pinned TU. */
static short BoxRGBA[4];
static XglPacket *Pkt;


/* ov11:0x00a00168. Emits one textured sprite into Pkt. The sprite
 * descriptor gives the texture position (u, v), its size, two words for
 * the texture setup and a mode; the mode picks one of four texture-setup
 * words (modes 1 and 3 also double the v coordinate). Then the four
 * corners of rect (x, y, width, height) go out as UV and XYZ pairs with
 * the screen origin offset and z = 0x80. Not sure of the GS
 * register meaning of the four setup words. */
static void MakeSprite(const struct SpriteTex *tex, const struct SpriteRect *pos)
{
    Ov11GifTag tex_tag = spriteTextureGifTag;
    Ov11GifTag prim_tag = spritePrimGifTag;
    int u0 = tex->u + 1;
    int v0 = tex->v + 1;
    int du = tex->w - 1;
    int dv = tex->h - 1;
    u64 z;
    u64 ul;
    u64 ur;
    u64 vt;
    u64 vb;

    sceVif1PkCnt(Pkt, 0);
    sceVif1PkOpenDirectCode(Pkt, 0);
    sceVif1PkOpenGifTag(Pkt, tex_tag.quad);
    switch (tex->format) {
    case 1:
        sceVif1PkAddGsData(Pkt, (tex->texture_base + CASINO_GS_TEX_BLOCK) | 0x25440000 | ((u64)(tex->clut_base + CASINO_GS_TEX_BLOCK) << 37) | 0x2000000E40000000ULL);
        v0 = v0 * 2 - 1;
        break;
    case 2:
        sceVif1PkAddGsData(Pkt, ((u64)(tex->clut_base + CASINO_GS_TEX_BLOCK) << 37) | 0x4000000E21343800ULL);
        break;
    case 3:
        sceVif1PkAddGsData(Pkt, ((u64)(tex->clut_base + CASINO_GS_TEX_BLOCK) << 37) | 0x4000000EA9443800ULL);
        v0 = v0 * 2 - 1;
        break;
    default:
        sceVif1PkAddGsData(Pkt, ((u64)(tex->clut_base + CASINO_GS_TEX_BLOCK) << 37) | 0x4000000E65323800ULL);
        break;
    }
    ul = u0 << 4;
    vt = (u64)(v0 << 4) << 16;
    sceVif1PkCloseGifTag(Pkt);
    sceVif1PkOpenGifTag(Pkt, prim_tag.quad);
    z = 0x8000000000ULL;
    ur = (u0 + du) << 4;
    sceVif1PkAddGsData(Pkt, CASINO_PRIM_TEXTURED);
    sceVif1PkAddGsData(Pkt, ul | vt);
    sceVif1PkAddGsData(Pkt, ((pos->x << 4) + CASINO_GS_ORIGIN_X) | ((u64)((pos->y << 4) + CASINO_GS_ORIGIN_Y) << 16) | z);
    sceVif1PkAddGsData(Pkt, ur | vt);
    vb = (u64)((v0 + dv) << 4) << 16;
    sceVif1PkAddGsData(Pkt, (((pos->x + pos->w) << 4) + CASINO_GS_ORIGIN_X) | ((u64)((pos->y << 4) + CASINO_GS_ORIGIN_Y) << 16) | z);
    sceVif1PkAddGsData(Pkt, ul | vb);
    sceVif1PkAddGsData(Pkt, ((pos->x << 4) + CASINO_GS_ORIGIN_X) | ((u64)(((pos->y + pos->h) << 4) + CASINO_GS_ORIGIN_Y) << 16) | z);
    sceVif1PkAddGsData(Pkt, ur | vb);
    sceVif1PkAddGsData(Pkt, (((pos->x + pos->w) << 4) + CASINO_GS_ORIGIN_X) | ((u64)(((pos->y + pos->h) << 4) + CASINO_GS_ORIGIN_Y) << 16) | z);
    sceVif1PkCloseGifTag(Pkt);
    sceVif1PkCloseDirectCode(Pkt);
}


/* ov11:0x00a004d0. Builds the GS draw-state packet: it writes TEST_1,
 * TEX1_1 and CLAMP_1 with fixed values, then ALPHA_1 with a value chosen by
 * mode (0..6; modes 5 and 6 put the second argument in the upper word,
 * anything else writes 0), i.e. mode selects the blend. */
static void SetDrawStatus(int mode, int arg)
{
    Ov11GifTag tag = drawStatusGifTag;

    sceVif1PkCnt(Pkt, 0);
    sceVif1PkOpenDirectCode(Pkt, 0);
    sceVif1PkOpenGifTag(Pkt, tag.quad);
    sceVif1PkAddGsAD(Pkt, SCE_GS_TEST_1, 0x5001D);
    sceVif1PkAddGsAD(Pkt, SCE_GS_TEX1_1, 0x20);
    sceVif1PkAddGsAD(Pkt, SCE_GS_CLAMP_1, 0);
    switch (mode) {
    case 0:
        sceVif1PkAddGsAD(Pkt, SCE_GS_ALPHA_1, 0x48);
        break;
    case 2:
        sceVif1PkAddGsAD(Pkt, SCE_GS_ALPHA_1, 0x6000000062ULL);
        break;
    case 3:
        sceVif1PkAddGsAD(Pkt, SCE_GS_ALPHA_1, 0);
        break;
    case 4:
        sceVif1PkAddGsAD(Pkt, SCE_GS_ALPHA_1, 0x88);
        break;
    case 5:
        sceVif1PkAddGsAD(Pkt, SCE_GS_ALPHA_1, ((u64)arg << 32) | 0x29);
        break;
    case 6:
        sceVif1PkAddGsAD(Pkt, SCE_GS_ALPHA_1, ((u64)arg << 32) | 0x68);
        break;
    case 1:
        sceVif1PkAddGsAD(Pkt, SCE_GS_ALPHA_1, 0x44);
        break;
    default:
        sceVif1PkAddGsAD(Pkt, SCE_GS_ALPHA_1, 0);
        break;
    }
    sceVif1PkCloseGifTag(Pkt);
    sceVif1PkCloseDirectCode(Pkt);
}

/* Writes the GS TEST_1 register with the given value. */
static void SetTest(int value)
{
    Ov11GifTag tag = setTestGifTag;
    sceVif1PkCnt(Pkt, 0);
    sceVif1PkOpenDirectCode(Pkt, 0);
    sceVif1PkOpenGifTag(Pkt, tag.quad);
    sceVif1PkAddGsAD(Pkt, SCE_GS_TEST_1, value);
    sceVif1PkCloseGifTag(Pkt);
    sceVif1PkCloseDirectCode(Pkt);
}

static void SetRegAD(XglPacket *packet, int reg, int value)
{
    Ov11GifTag tag = setRegisterGifTag;
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectCode(packet, 0);
    sceVif1PkOpenGifTag(packet, tag.quad);
    sceVif1PkAddGsAD(packet, reg, value);
    sceVif1PkCloseGifTag(packet);
    sceVif1PkCloseDirectCode(packet);
}

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

/* Reel strips (real<reel><row>array) and the other casino data. */
static short real11array[SLOT_REEL_CELLS];
static short real12array[SLOT_REEL_CELLS];
static short real13array[SLOT_REEL_CELLS];
static short real21array[SLOT_REEL_CELLS];
static short real22array[SLOT_REEL_CELLS];
static short real23array[SLOT_REEL_CELLS];
static short real31array[SLOT_REEL_CELLS];
static short real32array[SLOT_REEL_CELLS];
static short real33array[SLOT_REEL_CELLS];
static short real41array[SLOT_REEL_CELLS];
static short real42array[SLOT_REEL_CELLS];
static short real43array[SLOT_REEL_CELLS];
static short real51array[SLOT_REEL_CELLS];
static short real52array[SLOT_REEL_CELLS];
static short real53array[SLOT_REEL_CELLS];

static struct CasinoSaveWork *SaveWork  = (struct CasinoSaveWork *)(SaveData + 0x15190);
static short real01array[SLOT_REEL_CELLS];
static short real02array[SLOT_REEL_CELLS];
static short real03array[SLOT_REEL_CELLS];
static short *realpt[18]  = {
    real01array, real02array, real03array, real11array, real12array, real13array,
    real21array, real22array, real23array, real31array, real32array, real33array,
    real41array, real42array, real43array, real51array, real52array, real53array
};
static struct SpriteTex pay_window[2]  = {
    {224, 0, 32, 32, 0, 572, 0},
    {224, 32, 32, 32, 0, 572, 0}
};
static struct SpriteRect pay_window_rect_4[5]  = {
    {36, 33, 32, 32}, {36, 82, 32, 32}, {36, 131, 32, 32},
    {36, 180, 32, 32}, {36, 229, 32, 32}
};
static int status_keep_5  = 0;
static struct SpriteTex GuidPtTex[10]  = {
    {256, 248, 8, 8, 0, 564, 0}, {264, 248, 8, 8, 0, 564, 0},
    {272, 248, 8, 8, 0, 564, 0}, {280, 248, 8, 8, 0, 564, 0},
    {288, 248, 8, 8, 0, 564, 0}, {296, 248, 8, 8, 0, 564, 0},
    {304, 248, 8, 8, 0, 564, 0}, {312, 248, 8, 8, 0, 564, 0},
    {320, 248, 8, 8, 0, 564, 0}, {328, 248, 8, 8, 0, 564, 0}
};
static struct SpriteRect point_rect_20[9]  = {
    {59, 62, 8, 8}, {59, 111, 8, 8}, {59, 160, 8, 8}, {59, 209, 8, 8},
    {59, 258, 8, 8}, {192, 271, 8, 8}, {241, 271, 8, 8}, {290, 271, 8, 8},
    {439, 144, 8, 8}
};
static int seq_21  = 0;
static struct SpriteTex BoxSpr  = {0, 0, 511, 447, 0, 1020, 0};
static struct SpriteRect BoxRect  = {0, 0, 511, 447};
static struct SpriteTex NineGameBG_Tex_43  = {0, 0, 511, 447, 0, 1020, 0};
static struct SpriteRect NineGameBG_Rect_44  = {0, 0, 512, 448};
static int seq_45  = 0;
static struct GuidPoint GuidPt_46[11]  = {
    {192, 136}, {241, 136}, {290, 136}, {204, 240}, {253, 240}, {302, 240},
    {216, 344}, {265, 344}, {314, 344}, {458, 27}, {183, 362}
};
static struct SpriteTex tex_68  = {0, 0, 512, 448, 0, 1020, 0};
static struct SpriteRect rect_69  = {0, 0, 512, 448};
static struct SpriteTex tex_80  = {0, 0, 352, 176, 0, 1020, 0};
static struct SpriteRect rect_81  = {16, 256, 352, 176};
static struct SpriteRect result_rect_97[9]  = {
    {204, 362, 160, 32}, {204, 330, 160, 32}, {204, 298, 160, 32},
    {204, 266, 160, 32}, {0, 394, 244, 32}, {0, 362, 244, 32},
    {0, 330, 244, 32}, {0, 298, 244, 32}, {0, 266, 244, 32}
};
static struct SpriteTex tex_98  = {96, 224, 160, 32, 0, 756, 0};
static struct SpriteTex SubWindow_Tex  = {0, 0, 176, 120, 0, 564, 2};
static struct SpriteTex Level_Tex[4]  = {
    {176, 0, 72, 16, 0, 564, 2}, {176, 16, 72, 16, 0, 564, 2},
    {176, 32, 72, 16, 0, 564, 2}, {176, 48, 72, 16, 0, 564, 2}
};
static struct SpriteTex ExChange_Tex[3]  = {
    {176, 64, 64, 16, 0, 564, 2}, {176, 80, 64, 16, 0, 564, 2},
    {176, 96, 64, 16, 0, 564, 2}
};
static struct SpriteTex BGTEST_Tex  = {0, 0, 511, 447, 0, 1136, 0};
static struct SpriteRect BGTEST_Rect  = {0, 0, 512, 448};
static int flg_104  = 0;

/* Four GS XYZ vertices for an untextured box. Coordinates use 12.4
 * fixed point around the casino's drawing origin;
 * the upper word supplies Z = 1. */

static void MakeBoxPos(int x, int y, int width, int height)
{
    int left;
    int right;
    unsigned long long top;
    unsigned long long bottom;
    unsigned long long depth;

    left = x * 16 + CASINO_GS_ORIGIN_X;
    top = (unsigned long long)(y * 16 + CASINO_GS_ORIGIN_Y);
    right = (x + width) * 16 + CASINO_GS_ORIGIN_X;
    bottom = (unsigned long long)((y + height) * 16 + CASINO_GS_ORIGIN_Y);
    depth = 1ULL << 32;

    BoxPOS[0] = left | (top << 16) | depth;
    BoxPOS[1] = right | (top << 16) | depth;
    BoxPOS[2] = left | (bottom << 16) | depth;
    BoxPOS[3] = right | (bottom << 16) | depth;
}


/* ov11:0x00a00878. Draws a filled box as a GS packet: MakeBoxPos builds
 * the corner positions, then four colour+position pairs are written using
 * the current box colour. */
static void DrawNonTextureBox(XglPacket *packet, int x, int y, int width, int height)
{
    Ov11GifTag tag = nonTextureBoxGifTag;
    u64 *pos;
    short *color;
    int count;
    u64 value;
    u64 low;
    u64 high;

    MakeBoxPos(x, y, width, height);
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectCode(packet, 0);
    sceVif1PkOpenGifTag(packet, tag.quad);
    sceVif1PkAddGsData(packet, CASINO_PRIM_FLAT);
    color = BoxRGBA;
    pos = BoxPOS;
    count = 3;
    do {
        high = ((u64)color[2] << 16) | ((u64)color[1] << 8);
        low = (u64)color[0] | 0x100000000ULL;
        value = low | high;
        value |= (u64)color[3] << 24;
        sceVif1PkAddGsData(packet, value);
        sceVif1PkAddGsData(packet, *pos);
        pos++;
    } while (--count >= 0);
    sceVif1PkCloseGifTag(packet);
    sceVif1PkCloseDirectCode(packet);
}

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
    return ResData->debug_mode;
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

static const char soundRequestFormat[] = "Sound Req = SE_%.3d:Sound num = %x\n";

static void RES_SoundEffect(int soundId)
{
    int soundEffectId = ResData->sound_effect_id[soundId];

    dprintf(soundRequestFormat, soundId, soundEffectId);
    xglSoundEffectNormalID(soundEffectId, 0);
}

static const char soundStopRequestFormat[] = "Sound Req = SE_%.3d:Sound Stop num = %x\n";

static void RES_SoundEffectStop(int soundId)
{
    int soundEffectId = ResData->sound_effect_id[soundId];

    dprintf(soundStopRequestFormat, soundId, soundEffectId);
    xglSoundEffectStopDirect(soundEffectId);
}

/* ov11:0x00a00af0. Loads the casino resource table (CASINO.res) into the
 * fixed asset staging address and records it as ResData. */
static const char casinoResPath[] = "data\\tanaka\\CASINO.res";

static void RES_Load(void)
{
    xglCdReadFile(casinoResPath, CASINO_RES_BUFFER, 0, 0);
    ResData = CASINO_RES_BUFFER;
}


static int AddCoin(int amount)
{
    struct CasinoSaveWork *save;
    int total;
    int excess;

    save = SaveWork;
    total = save->coin_balance + amount;
    if (total > CASINO_COIN_MAX) {
        excess = total - CASINO_COIN_MAX;
        dataMoneyBoxInc(excess * 8);
        amount -= excess;
    }
    save = SaveWork;
    save->coin_balance += amount;
    return 1;
}


static short SlGetPic(int reel, int positionOffset, int row)
{
    int position;
    int pictureIndex;
    short *pictures;

    position = slotReelPositionTable[reel] / SLOT_CELL_SIZE;
    pictures = realpt[reel + row * 3];
    position -= positionOffset;
    pictureIndex = position + 2;
    if (pictureIndex < 0) {
        pictureIndex = position + SLOT_REEL_CELLS + 2;
    }
    if (pictureIndex >= SLOT_REEL_CELLS) {
        pictureIndex -= SLOT_REEL_CELLS;
    }
    return pictures[pictureIndex];
}


/* ov11:0x00a00c10. Draws the three slot reels: wraps each reel position
 * (Gwork.slot.reel_pos) back by 0x540 when it passes 0x540, then for each reel
 * draws six picture cells (SlGetPic for the current row) scrolled by the
 * position, then the three reel windows and a dark box over the lower
 * area. Not sure what the box is for. */
static void SlRealDraw(void)
{
    struct SpriteRect rect;
    int i;
    int j;
    int n;

    SetDrawStatus(3, 0);
    for (n = 0; n < 3; n++) {
        if (Gwork.slot.reel_pos[n] > SLOT_REEL_LENGTH) {
            Gwork.slot.reel_pos[n] -= SLOT_REEL_LENGTH;
        }
    }
    rect.w = 0x70;
    rect.h = 0x40;
    for (i = 0; i < 3; i++) {
        rect.x = xpos_3[i];
        for (n = 4; n >= -1; n--) {
            j = 2 - n;
            rect.y = Gwork.slot.reel_pos[i] % SLOT_CELL_SIZE + j * SLOT_CELL_SIZE + 0x32;
            MakeSprite(&Real_2[SlGetPic(i, j, Gwork.slot.row)], &rect);
        }
    }
    SetDrawStatus(2, 0);
    for (n = 0; n < 3; n++) {
        MakeSprite(&slot_window_0, &slot_window_rect_1[n]);
    }
    SetDrawStatus(3, 0);
    SetTest(CASINO_TEST_DEPTH_ALWAYS);
    ResetRGBA();
    SetRGBA(0, 0, 0, 0x80);
    DrawNonTextureBox(Pkt, 0x50, 0x12C, 0x160, 0x40);
}

/* The double 10.0, as the raw 64-bit value the soft-float helpers take. */
#define DOUBLE_TEN 0x4024000000000000ULL

/* ov11:0x00a00de0. Prints value in decimal with 16x24 digit sprites from
 * decfont, right-aligned at x and moving left. A negative digits count
 * means as many digits as value needs (at most 11, found by comparing
 * against powers of ten); zero digits still draws one zero. */
static void decprint(int value, int digits, int x, int y)
{
    struct SpriteRect rect;
    double number;
    unsigned long long power;
    int i;

    if (digits < 0) {
        digits = 0;
        while (1) {
            if (digits < 11) {
                number = (double)value;
                if (dpcmp(number, pow(DOUBLE_TEN, litodp(digits))) < 0) {
                    break;
                }
                digits++;
            } else {
                break;
            }
        }
    }
    rect.w = 16;
    rect.h = 24;
    if (digits > 0) {
        for (i = 0; i < digits; ) {
            rect.x = x - i * 16;
            rect.y = y;
            power = pow(DOUBLE_TEN, litodp(i++));
            /* Empty loop: GCC 2.96 does not schedule across loop notes, which keeps
             * the pow result copy ahead of the next conversion argument. */
            do { } while (0);
            MakeSprite(&decfont[dptoli(dpdiv((double)value, power)) % 10], &rect);
        }
    } else {
        rect.x = x;
        rect.y = y;
        MakeSprite(decfont, &rect);
    }
}

static void SlCashPrint(void)
{
    decprint(SaveWork->coin_balance, -1, 440, 309);
    decprint(slotPayoutAmount, -1, 448, 373);
}

/* ov11:0x00a00fb0. Slot: prints the five per-line pay amounts with
 * decprint at five fixed screen positions (a 20-byte position table copied
 * to the stack). */
static void pay_print(XglPacket *packet)
{
    struct PayLayout layout = linePayPositionTable;
    int i;

    for (i = 0; i < 5; i++) {
        decprint(Gwork.slot.line_pay[i], 2, layout.row[i].x, layout.row[i].y);
    }
}

/* ov11:0x00a01060. Draws the three-digit slot total stake at its fixed
 * screen position through decprint. */

static void total_pay_print(XglPacket *packet)
{
    decprint(slotTotalPay, 3, 0x1DC, 0x1B);
}

static int check_mode(void)
{
    int index;

    for (index = 0; index < 3; index++) {
        if (Gwork.slot.reel_state[index] == SLOT_REEL_SPINNING) {
            return 1;
        }
    }
    return 0;
}

static void SlPayWindowPrint(void)
{
    int rectIndex;
    int windowIndex;

    SetDrawStatus(0, 0);
    windowIndex = check_mode() != 1;
    for (rectIndex = 0; rectIndex < 5; rectIndex++) {
        MakeSprite(&pay_window[windowIndex], &pay_window_rect_4[rectIndex]);
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


/* ov11:0x00a011b0. Slot: draws the spin button. The sprite comes from
 * check_spin()'s result plus a push counter (pushcnt_7); not sure of the
 * exact meaning of the counter. */
static void SpinBtnPrint(void)
{
    int index;

    switch (check_spin()) {
    case 0:
        index = 1;
        break;
    case -1:
        index = 0;
        break;
    case 1:
        index = 2;
        pushcnt_7 = 0;
        break;
    default:
        if (pushcnt_7 >= 0) {
            index = 2;
            pushcnt_7 = pushcnt_7 + 1;
            if (pushcnt_7 >= 5) {
                pushcnt_7 = -1;
            }
        } else {
            index = 3;
        }
        break;
    }
    if (index != 0) {
        MakeSprite(&spinbt[index - 1], &spinbt_rect_6);
    }
}


/* ov11:0x00a01278. Slot: draws the three stop-button sprites, choosing
 * each texture from that reel's state (1, 2 or anything else). */
static void SlStopBtnPrint(void)
{
    int i;
    int index;

    for (i = 0; i < 3; i++) {
        switch (Gwork.slot.reel_state[i]) {
        case SLOT_REEL_SPINNING:
            index = 0;
            break;
        case SLOT_REEL_STOPPING:
            index = 1;
            break;
        default:
            index = 2;
            break;
        }
        MakeSprite(&stopbt[index], &stopbt_rect_8[i]);
    }
}


/* ov11:0x00a01330. Prints value in decimal with 12x16 digit sprites from
 * DeciList_9 starting at x and moving left by w per digit, at height y.
 * The digit count is as many as value needs (at most 11); zero still
 * draws one zero. */
static void SlDecPrintSmall(int x, int y, int w, int value)
{
    struct SpriteRect rect;
    double number;
    unsigned long long power;
    int digits;
    int i;

    digits = 0;
    while (1) {
        if (digits < 11) {
            number = (double)value;
            if (dpcmp(number, pow(DOUBLE_TEN, litodp(digits))) < 0) {
                break;
            }
            digits++;
        } else {
            break;
        }
    }
    rect.w = 12;
    rect.h = 16;
    if (digits > 0) {
        for (i = 0; i < digits; ) {
            rect.x = x - i * w;
            rect.y = y;
            power = pow(DOUBLE_TEN, litodp(i++));
            /* Empty loop: GCC 2.96 does not schedule across loop notes, which keeps
             * the pow result copy ahead of the next conversion argument. */
            do { } while (0);
            MakeSprite(&DeciList_9[dptoli(dpdiv((double)value, power)) % 10], &rect);
        }
    } else {
        rect.x = x;
        rect.y = y;
        MakeSprite(DeciList_9, &rect);
    }
}


/* ov11:0x00a014b8. Draws the slot pay-rate table: six symbol pictures from
 * PicTable at the pos_10 positions, a seventh narrower one, six OptionText
 * labels from fixed rectangles, then the six rates (double_rate_12 when
 * Gwork.slot.double_pay is set, otherwise rate_11) with SlDecPrintSmall.
 * Not sure what the labels say or what SlDecPrintSmall's third argument
 * means. */
static void SlRateTableDraw(void)
{
    struct SpriteRect rect;
    int i;

    SetDrawStatus(3, 0);
    rect.w = 0x54;
    rect.h = 0x10;
    for (i = 0; i < 6; i++) {
        rect.x = pos_10[i][0];
        rect.y = pos_10[i][1];
        MakeSprite(&PicTable[i], &rect);
    }
    rect.y = pos_10[i][1];
    rect.x = pos_10[i][0];
    rect.w = 0x1C;
    MakeSprite(&PicTable[i], &rect);
    rect = rateTableOption1UpperRect;
    MakeSprite(&OptionText[1], &rect);
    rect = rateTableOption1MiddleRect;
    MakeSprite(&OptionText[1], &rect);
    rect = rateTableOption1LowerRect;
    MakeSprite(&OptionText[1], &rect);
    rect = rateTableOption2Rect;
    MakeSprite(&OptionText[2], &rect);
    rect = rateTableOption3Rect;
    MakeSprite(&OptionText[3], &rect);
    rect = rateTableOption4Rect;
    MakeSprite(&OptionText[4], &rect);
    for (i = 0; i < 6; i++) {
        if (Gwork.slot.double_pay != 0) {
            SlDecPrintSmall(0xAF, 0x125 + i * 0x14, 0xC, double_rate_12[i]);
        } else {
            SlDecPrintSmall(0xAF, 0x125 + i * 0x14, 0xC, rate_11[i]);
        }
    }
}


/* ov11:0x00a01750. Draws the slot result screen. For every pay line with a
 * prize it draws the three reel pictures (Pic_13), the prize and the
 * line's stake with SlDecPrintSmall, adds prize times stake to the total,
 * and either draws a label for the line's kind (OptionText 1, 2 or 0 for
 * kinds 1, 2, 3) or prints the line payout; rows move down 0x14 per
 * winning line. It then draws the total, and lights the winning pay lines
 * on the 3x3 grid (linetex_15 at linerect_16 positions). Not sure which
 * game labels the kinds stand for. */
static void SlResultDraw(void)
{
    int lit[9];
    struct SpriteRect rect;
    int total;
    int y;
    int i;
    int k;
    int x;
    struct SlotLine *line;

    rect.h = 0x10;
    SetDrawStatus(3, 0);
    y = 0x125;
    total = 0;
    for (i = 0; i < 5; i++) {
        line = &Gwork.slot.line[i];
        if (line->prize != 0) {
            rect.w = 0x1C;
            x = 0x21;
            for (k = 0; k < 3; k++) {
                rect.x = x;
                rect.y = y;
                MakeSprite(&Pic_13[line->pic[k]], &rect);
                x += 0x1C;
            }
            SlDecPrintSmall(0xAF, y, 0xC, line->prize);
            SlDecPrintSmall(0x8B, y, 0xC, Gwork.slot.line_pay[i]);
            total += line->prize * Gwork.slot.line_pay[i];
            if (line->kind != 0) {
                rect.x = 0xC5;
                rect.y = y;
                rect.h = 0x10;
                switch (line->kind) {
                case 1:
                    rect.w = 0x60;
                    MakeSprite(&OptionText[1], &rect);
                    break;
                case 2:
                    rect.w = 0x68;
                    MakeSprite(&OptionText[2], &rect);
                    break;
                case 3:
                    rect.w = 0x50;
                    MakeSprite(&OptionText[0], &rect);
                    break;
                }
            } else {
                SlDecPrintSmall(0x119, y, 0xC, line->prize * Gwork.slot.line_pay[i]);
            }
            y += 0x14;
        }
    }
    rect = resultTotalLabelRect;
    MakeSprite(&Total_14, &rect);
    SlDecPrintSmall(0x119, 0x19D, 0xC, total);
    for (i = 8; i >= 0; i--) {
        lit[i] = 0;
    }
    if (Gwork.slot.line[0].prize != 0) {
        lit[0] = 1;
        lit[4] = 1;
        lit[8] = 1;
    }
    if (Gwork.slot.line[1].prize != 0) {
        lit[0] = 1;
        lit[3] = 1;
        lit[6] = 1;
    }
    if (Gwork.slot.line[2].prize != 0) {
        lit[1] = 1;
        lit[4] = 1;
        lit[7] = 1;
    }
    if (Gwork.slot.line[3].prize != 0) {
        lit[2] = 1;
        lit[5] = 1;
        lit[8] = 1;
    }
    if (Gwork.slot.line[4].prize != 0) {
        lit[2] = 1;
        lit[4] = 1;
        lit[6] = 1;
    }
    SetDrawStatus(0, 0);
    for (i = 0; i < 9; i++) {
        if (lit[i] != 0) {
            MakeSprite(&linetex_15, &linerect_16[i]);
        }
    }
    SetDrawStatus(3, 0);
}


/* ov11:0x00a01a90. Draws the bonus banner. Text_17[1] shows while the
 * prize doubling is on without the bonus, Text_17[2] while the bonus is
 * on, and Text_17[0] when neither is. With either one on, the time bar
 * (Bar_18, then TimeBar_19 scaled by the remaining bonus time over
 * RES_GetBonusTime) is drawn and the bonus time counts down on idle
 * frames; at zero it restarts when the slot state is nonzero, otherwise
 * it raises event 7. */
static void SlModePrint(void)
{
    struct SpriteRect rect;
    struct SpriteRect bar;

    if (Gwork.slot.double_pay != 0 && Gwork.slot.bonus == 0) {
        rect = bonusBannerRect;
        MakeSprite(&Text_17[1], &rect);
    }
    if (Gwork.slot.bonus != 0) {
        rect = bonusBannerRect;
        MakeSprite(&Text_17[2], &rect);
    }
    if (Gwork.slot.bonus == 0 && Gwork.slot.double_pay == 0) {
        rect = bonusIdleBannerRect;
        MakeSprite(&Text_17[0], &rect);
    } else {
        float elapsed;

        rect = bonusTimeBarFrameRect;
        MakeSprite(&Bar_18, &rect);
        bar = bonusTimeBarRect;
        elapsed = Gwork.slot.bonus_time;
        elapsed = 180.0f * (elapsed / RES_GetBonusTime());
        bar.w = (int)elapsed;
        MakeSprite(&TimeBar_19, &bar);
        if (Gwork.slot.event == 0) {
            Gwork.slot.bonus_time--;
        }
        if (Gwork.slot.bonus_time == 0) {
            if (Gwork.slot.state != SLOT_STATE_IDLE) {
                Gwork.slot.bonus_time = 1;
            } else {
                Gwork.slot.event = 7;
            }
        }
    }
}



static void SlGuidPrint(int index)
{
    MakeSprite(&GuidPtTex[seq_21], &point_rect_20[index]);
    seq_21 = seq_21 + 1;
    seq_21 = seq_21 % 10;
}

/* ov11:0x00a01d20. Slot pay-line pointer. check_mode() 0 draws the guide
 * at Gwork.slot.guide; 1 draws it at Gwork.slot.pointer + 5 and right /
 * left (newly pressed) step the pointer forward / back (wrapping within
 * 0..2) with a sound. Not sure what the pointer is for in game terms. */
static void point_control(XglPacket *packet)
{
    switch (check_mode()) {
    case 0:
        SlGuidPrint(Gwork.slot.guide);
        break;
    case 1:
        SlGuidPrint(Gwork.slot.pointer + 5);
        if (PadData.canonical.half_2a & PAD_RIGHT) {
            Gwork.slot.pointer = Gwork.slot.pointer + 1;
            if (Gwork.slot.pointer >= 3) {
                Gwork.slot.pointer = 0;
            }
            RES_SoundEffect(CASINO_SE_SLOT_POINTER_RIGHT);
        }
        if (PadData.canonical.half_2a & PAD_LEFT) {
            Gwork.slot.pointer = Gwork.slot.pointer - 1;
            if (Gwork.slot.pointer < 0) {
                Gwork.slot.pointer = 2;
            }
            RES_SoundEffect(CASINO_SE_SLOT_POINTER_LEFT);
        }
        break;
    }
}

/* ov11:0x00a01e28. Configures background draw state, then emits the fixed
 * slot background sprite built from BoxRect (ov11:0x00a0a0e0) into BoxSpr
 * (ov11:0x00a0a0c0). */


static void SlBgDraw(XglPacket *packet)
{
    SetDrawStatus(3, 0);
    SetTest(CASINO_TEST_DEPTH_GEQUAL);

    /* MakeSprite is the last statement of this void function and gcc 2.96
     * compiles a trailing call like that as a sibling jump; the original
     * keeps a real jal plus the shared epilogue (ld $31/jr $31). Wrapping
     * only this call in do{}while(0) defeats the sibcall pass instead
     * (CP-0288, config/compiler-patterns.json). */
    do {
        MakeSprite(&BoxSpr, &BoxRect);
    } while (0);
}


/* Measured: SlCoinEntry's last loop only matches with the work addressed as
 * a struct SlotWork at each use; a pointer variable or the Gwork.slot member
 * path changes its register allocation and induction variables. */
#define SLOT_WORK ((struct SlotWork *)&Gwork)

/* ov11:0x00a01e68. Stake entry on the slot idle screen. Up / down move the
 * pay line (Gwork.slot.guide, wrapping within 0..4); right / left (with
 * auto-repeat) raise or lower that line's stake step (0..3) while the
 * total stays below the coin balance; any other button toggles the help
 * flag. line_pay is each step times the stake chosen by the menu level. */
static void SlCoinEntry(void)
{
    struct StakeSteps steps = lineStakeStepTable;
    int i;

    if (PadData.canonical.half_2a & PAD_UP) {
        Gwork.slot.guide = Gwork.slot.guide - 1;
        Gwork.slot.help = 0;
        if (Gwork.slot.guide < 0) {
            Gwork.slot.guide = 4;
        }
        RES_SoundEffect(CASINO_SE_SLOT_LINE_UP);
    }
    if (PadData.canonical.half_2a & PAD_DOWN) {
        Gwork.slot.guide = Gwork.slot.guide + 1;
        Gwork.slot.help = 0;
        if (Gwork.slot.guide >= 5) {
            Gwork.slot.guide = 0;
        }
        RES_SoundEffect(CASINO_SE_SLOT_LINE_DOWN);
    }
    if (PadData.wide.repeat & PAD_RIGHT) {
        Gwork.slot.help = 0;
        if (Gwork.slot.line_bet[Gwork.slot.guide] < 3) {
            if (Gwork.slot.total_bet + steps.step[Gwork.slot.level] <
                SaveWork->coin_balance) {
                Gwork.slot.line_bet[Gwork.slot.guide]++;
                RES_SoundEffect(CASINO_SE_SLOT_STAKE_UP);
            } else {
                RES_SoundEffect(CASINO_SE_SLOT_STAKE_REFUSED);
            }
        }
    }
    if (PadData.wide.repeat & PAD_LEFT) {
        Gwork.slot.help = 0;
        if (Gwork.slot.line_bet[Gwork.slot.guide] > 0) {
            Gwork.slot.line_bet[Gwork.slot.guide]--;
            RES_SoundEffect(CASINO_SE_SLOT_STAKE_DOWN);
        }
    }
    if (PadData.canonical.half_2a & (PAD_L2 | PAD_R2 | PAD_L1 | PAD_R1 | PAD_TRIANGLE |
                                      PAD_SQUARE | PAD_SELECT | PAD_START)) {
        if (Gwork.slot.help == 0) {
            Gwork.slot.help = 1;
        } else {
            Gwork.slot.help = 0;
        }
    }
    SLOT_WORK->total_bet = 0;
    for (i = 0; i < 5; i++) {
        SLOT_WORK->line_pay[i] = SLOT_WORK->line_bet[i] * steps.step[SLOT_WORK->level];
        SLOT_WORK->total_bet += SLOT_WORK->line_pay[i];
    }
}

/* The slot picture that counts as any other on a pay line (SlPrizeCheck). */
#define SLOT_PIC_WILD 6

/* ov11:0x00a02068. Scores one slot pay line. A line of three equal
 * pictures pays in full; picture 6 acts as a wild card (two equal pictures
 * plus a 6, or two 6s) and pays half, rounded up. The picture number then
 * selects the prize (5, 10, 20, 20, 1, 30, 0 or 30) and a kind (1, 2 or 3
 * for some pictures, otherwise 0); Gwork.slot.double_pay doubles the
 * prize. Not sure of the in-game picture names or what the kind values
 * mean. */
static void SlPrizeCheck(int index)
{
    struct SlotLine *line;
    int pic;

    line = &Gwork.slot.line[index];
    pic = -1;
    line->half = 1;
    if (line->pic[0] == line->pic[1] && line->pic[0] == line->pic[2]) {
        pic = line->pic[0];
        line->half = 0;
    }
    if (line->pic[0] == SLOT_PIC_WILD && line->pic[1] == line->pic[2]) {
        pic = line->pic[1];
    }
    if (line->pic[1] == SLOT_PIC_WILD && line->pic[0] == line->pic[2]) {
        pic = line->pic[0];
    }
    if (line->pic[2] == SLOT_PIC_WILD && line->pic[0] == line->pic[1]) {
        pic = line->pic[0];
    }
    if (line->pic[0] == SLOT_PIC_WILD) {
        if (line->pic[1] == SLOT_PIC_WILD) {
            pic = line->pic[2];
        }
        if (line->pic[2] == SLOT_PIC_WILD) {
            pic = line->pic[1];
        }
    }
    if (line->pic[1] == SLOT_PIC_WILD && line->pic[2] == SLOT_PIC_WILD) {
        pic = line->pic[0];
    }
    line->kind = 0;
    if (pic == -1) {
        return;
    }
    switch (pic) {
    case 0:
        line->prize = 5;
        break;
    case 1:
        line->prize = 10;
        line->kind = 1;
        break;
    case 2:
        line->prize = 20;
        line->kind = 1;
        break;
    case 3:
        line->prize = 20;
        line->kind = 1;
        break;
    case 4:
        line->prize = 1;
        line->kind = 2;
        break;
    case 5:
        line->prize = 30;
        break;
    case 7:
        line->prize = 30;
        line->kind = 3;
        break;
    default:
        line->prize = 0;
        break;
    }
    if (Gwork.slot.double_pay != 0) {
        line->prize *= 2;
    }
    if (line->half == 1) {
        line->prize = (line->prize + 1) / 2;
    }
}

/* The three reel cells read for each of the five pay lines (pat_22): the
 * reel and the cell offset passed to SlGetPic. The dprintf formats are
 * "line %d [%d,%d,%d]\n", "prize = %d [%d]\n" and "paid = %d\n"
 * (lineCheckFormat, prizeCheckFormat, paidCheckFormat). */

/* ov11:0x00a02260. Reads the three pictures of each pay line (pat_22),
 * then scores every line that has a stake (SlPrizeCheck), adds its prize
 * times the stake to the payout and keeps the highest kind 1-3. With a
 * payout, that kind becomes the slot event, or sound 0x42 plays when there
 * is none. The dprintf calls are the original's debug output. The second
 * read of the kind after its dprintf (kind and prize_kind) is what gives
 * the original's load and store order. */
static void SlLineCheck(void)
{
    int i;
    int k;
    int best;
    int amount;
    int kind;
    int prize_kind;

    for (i = 0; i < 5; i++) {
        struct SlotLine *ln = &Gwork.slot.line[i];

        ln->prize = 0;
        ln->kind = 0;
        for (k = 0; k < 3; k++) {
            int reel = pat_22[i][k].reel;

            ln->pic[reel] = SlGetPic(reel, pat_22[i][k].offset, Gwork.slot.row);
        }
    }
    for (i = 0; i < 5; i++) {
        short *pic = Gwork.slot.line[i].pic;

        dprintf(lineCheckFormat, i, pic[0], pic[1], pic[2]);
    }
    best = 0;
    for (i = 0; i < 5; i++) {
        struct SlotLine *ln = &Gwork.slot.line[i];

        if (Gwork.slot.line_pay[i] != 0) {
            SlPrizeCheck(i);
            amount = ln->prize * Gwork.slot.line_pay[i];
            prize_kind = ln->kind;
            Gwork.slot.payout += amount;
            dprintf(prizeCheckFormat, amount, prize_kind);
            kind = ln->kind;
            if (kind == 1) {
                if (best <= 0) {
                    best = kind;
                }
            }
            if (kind == 2) {
                if (best < 2) {
                    best = kind;
                }
            }
            if (kind == 3) {
                if (best < 3) {
                    best = kind;
                }
            }
        }
    }
    if (Gwork.slot.payout != 0) {
        if (best != 0) {
            Gwork.slot.event = best;
        } else {
            RES_SoundEffect(CASINO_SE_SLOT_WIN_NO_EVENT);
        }
    }
    dprintf(paidCheckFormat, Gwork.slot.payout);
}

/* ov11:0x00a02460. Runs the complete slot line evaluation and reports
 * whether the resulting payout is nonzero. */


static int check_real(void)
{
    SlLineCheck();
    return slotPayoutAmount != 0;
}

static void GameModeChange(int mode);

/* ov11:0x00a02488. Payout handler. When Gwork.slot.start_nine_game is
 * set it switches to the nine-game and stops the payout sound;
 * otherwise pressing circle pays the pending coins at once, else one coin per
 * frame while the pending amount counts down. Not sure of the exact game
 * wording. */
static int paid_action(void)
{
    if (Gwork.slot.start_nine_game == 1) {
        GameModeChange(CASINO_NINE_GAME);
        Gwork.slot.start_nine_game = 0;
        RES_SoundEffectStop(CASINO_SE_SLOT_PAYOUT);
        inter_23 = 0;
        return 1;
    }
    if (PadData.canonical.half_2a & PAD_CIRCLE) {
        AddCoin(Gwork.slot.payout);
        Gwork.slot.payout = 0;
        RES_SoundEffectStop(CASINO_SE_SLOT_PAYOUT);
    }
    if (Gwork.slot.payout == 0) {
        if (PadData.canonical.half_2a & PAD_CIRCLE) {
            inter_23 = 0;
            return 1;
        }
    } else if (Gwork.slot.payout > 0) {
        Gwork.slot.payout = Gwork.slot.payout - 1;
        if (Gwork.slot.payout == 0) {
            RES_SoundEffectStop(CASINO_SE_SLOT_PAYOUT);
        }
        AddCoin(1);
        inter_23 = inter_23 + 1;
        if (inter_23 == 1) {
            RES_SoundEffect(CASINO_SE_SLOT_PAYOUT);
        }
    }
    return 0;
}

static void linepay_check(void)
{
    struct W22LineIds ids = linePayCheckOrderTable;
    char *root = (char *)&Gwork;
    char *score_root = root + 6;
    int cap = SaveWork->coin_balance;
    int sum = 0;
    int *p = ids.index;
    int count = 4;
    /* Measured: the addresses of line_pay[line] (SlotWork +0x56) and
     * line_bet[line] (+0x60) must stay two separate sums off the work base;
     * a member path or a struct pointer folds them into one register and
     * changes the schedule. */
    do {
        int offset = (*p) * 2;
        int scoreoff = offset + 80;
        short *score = (short *)((unsigned)scoreoff + (unsigned)score_root);
        char *at = (char *)((unsigned)offset + (unsigned)root);
        sum += *score;
        if (cap < sum) { *score = 0; *(short *)(at + 96) = 0; }
        ++p; --count;
    } while (count >= 0);
}


/* ov11:0x00a02630. Slot input handler, by Gwork.slot.state. Idle (0):
 * cross leaves to the menu; otherwise it runs SlCoinEntry, and circle
 * with a stake starts a spin (help off, pointer and reels reset to
 * spinning, the stake taken from the coin balance, sound 0x22). Spinning
 * (1): circle stops the reel under the pointer, a stopped reel slides on
 * by 0x10 per frame until it can snap to the next 64-pixel cell, and once
 * all three are still the result is checked (check_real, linepay_check).
 * Result (2): runs paid_action. A stop moves the pointer to the next reel. */
static void slot_key(void)
{
    int i;
    int stopped;
    int pressed;
    int rest;

    stopped = 0;
    pressed = 0;
    switch (Gwork.slot.state) {
    case SLOT_STATE_IDLE:
        if (PadData.canonical.half_2a & PAD_CROSS) {
            RES_SoundEffect(CASINO_SE_SLOT_LEAVE);
            GameModeChange(CASINO_MENU);
            break;
        }
        SlCoinEntry();
        if ((PadData.canonical.half_2a & PAD_CIRCLE) && Gwork.slot.total_bet > 0) {
            Gwork.slot.help = 0;
            Gwork.slot.pointer = 0;
            for (i = 0; i < 3; i++) {
                Gwork.slot.reel_state[i] = SLOT_REEL_SPINNING;
            }
            Gwork.slot.state = SLOT_STATE_SPINNING;
            SaveWork->coin_balance -= Gwork.slot.total_bet;
            RES_SoundEffect(CASINO_SE_SLOT_SPIN);
        }
        break;
    case SLOT_STATE_SPINNING:
        for (i = 0; i < 3; i++) {
            switch (Gwork.slot.reel_state[i]) {
            case SLOT_REEL_SPINNING:
                if (Gwork.slot.pointer == i && (PadData.canonical.half_2a & PAD_CIRCLE)) {
                    Gwork.slot.reel_state[i] = SLOT_REEL_STOPPING;
                    Gwork.slot.stop_travel[i] = 0;
                    RES_SoundEffect(CASINO_SE_SLOT_REEL_STOP);
                    pressed = 1;
                }
                Gwork.slot.reel_pos[i] += RES_GetRealSpeed();
                break;
            case SLOT_REEL_STOPPING:
                if (Gwork.slot.stop_travel[i] >= 0) {
                    rest = SLOT_CELL_SIZE - Gwork.slot.reel_pos[i] % SLOT_CELL_SIZE;
                    if (rest < 0x11) {
                        Gwork.slot.reel_state[i] = SLOT_REEL_STOPPED;
                        Gwork.slot.reel_pos[i] += rest;
                        break;
                    }
                }
                Gwork.slot.stop_travel[i] += 0x10;
                Gwork.slot.reel_pos[i] += 0x10;
                break;
            case SLOT_REEL_STOPPED:
                if ((PadData.canonical.half_2a & PAD_CIRCLE) && Gwork.slot.pointer == i) {
                    RES_SoundEffect(CASINO_SE_SLOT_STOP_IGNORED);
                }
                stopped++;
                break;
            }
        }
        if (stopped == 3) {
            if (check_real() == 0) {
                Gwork.slot.state = SLOT_STATE_IDLE;
            } else {
                Gwork.slot.state = SLOT_STATE_PAYOUT;
            }
            linepay_check();
        }
        break;
    case SLOT_STATE_PAYOUT:
        if (paid_action() != 0) {
            Gwork.slot.state = SLOT_STATE_IDLE;
        }
        break;
    }
    if (pressed == 1) {
        Gwork.slot.pointer++;
        if (Gwork.slot.pointer >= 3) {
            Gwork.slot.pointer = 0;
        }
    }
}


/* ov11:0x00a02938. Slot win banner animation, one step per call. Mode 0
 * loads the banner sprite (its v offset chosen by kind 1-4), resets the
 * counters and plays sound 0x28. Mode 1 shrinks the width towards 0x140
 * and grows the height to 0x40, keeping the banner centred on (0x100,
 * 0x90). Mode 2 holds for 16 more calls. Mode 3 shrinks the width to 0
 * while stretching the height to 0x800, then returns 0 and goes back to
 * mode 0. Otherwise it draws the banner and returns 1. Not sure what the
 * four kinds are. */
static int SlPrizeEffect(int kind)
{
    switch (mode_24) {
    case 0:
        tex_28 = prizeEffectTexDefault;
        switch (kind) {
        case 1:
            tex_28.v = 0x40;
            break;
        case 2:
            tex_28.v = 0xC0;
            break;
        case 3:
            tex_28.v = 0x80;
            break;
        case 4:
            tex_28.v = 0;
            break;
        }
        rect_29 = prizeEffectRectDefault;
        count_25 = 0;
        dx_26 = -0x20;
        dy_27 = 0;
        mode_24 = 1;
        RES_SoundEffect(0x28);
        break;
    case 1:
        rect_29.w += dx_26;
        rect_29.h += dy_27;
        rect_29.x = 0x100 - rect_29.w / 2;
        rect_29.y = 0x90 - rect_29.h / 2;
        if (rect_29.w <= 0x140) {
            dx_26 = 0;
            rect_29.w = 0x140;
        } else {
            dx_26 -= 8;
        }
        if (rect_29.h >= 0x40) {
            dy_27 = 0;
            rect_29.h = 0x40;
        } else {
            dy_27++;
        }
        if (dx_26 == 0 && dy_27 == 0) {
            mode_24 = 2;
        }
        break;
    case 2:
        if (++count_25 > 0x10) {
            mode_24 = 3;
            dx_26 = -4;
            dy_27 = 0x10;
        }
        break;
    case 3:
        rect_29.w += dx_26;
        rect_29.h += dy_27;
        rect_29.x = 0x100 - rect_29.w / 2;
        rect_29.y = 0x90 - rect_29.h / 2;
        if (rect_29.w < 0) {
            dx_26 = 0;
            rect_29.w = 0;
        } else {
            dx_26 -= 4;
        }
        if (rect_29.h >= 0x800) {
            dy_27 = 0;
            rect_29.h = 0x800;
        } else {
            dy_27 += 8;
        }
        if (dx_26 == 0 && dy_27 == 0) {
            mode_24 = 0;
            return 0;
        }
        break;
    }
    SetDrawStatus(1, 0);
    xglPacketTextureTrans(SLOT_W1_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    MakeSprite(&tex_28, &rect_29);
    SetDrawStatus(3, 0);
    xglPacketTextureTrans(BASE_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    return 1;
}


/* ov11:0x00a02d28. Slot reel-change animation, one step per call
 * (mode_33). Mode 0 resets the three panels with staggered delays. Mode 1
 * grows each panel to 0xC4 once its delay runs out (sound 0x29 when one
 * fills) and draws it. Mode 2 holds the full panels for 31 calls, then
 * picks the new reel row: 0 without a bonus, otherwise a random row 1-5
 * different from the current one. Mode 3 shrinks the panels. Mode 4 clears
 * or sets the bonus state (setting the bonus time from RES_GetBonusTime),
 * returns 0 and starts over. Returns 1 while the animation runs. Not sure
 * of the in-game meaning of the bonus state. */
static int SlChangeRealEffect(int bonus)
{
    struct SpriteRect rect;
    struct SlReelPanel *panel;
    int done;
    int i;
    unsigned int number;

    switch (mode_33) {
    case 0:
        for (i = 0; i < 3; i++) {
            psht_32 = &sht_31[i];
            psht_32->delay = i * 4;
            psht_32->state = 0;
            psht_32->size = 0;
        }
        mode_33 = 1;
        return 1;
    case 1:
        done = 0;
        for (i = 0; i < 3; i++) {
            psht_32 = &sht_31[i];
            if (psht_32->state == 0) {
                if (psht_32->delay == 0) {
                    psht_32->state = 1;
                } else {
                    psht_32->delay--;
                }
            } else {
                if (psht_32->size < 0xC4) {
                    psht_32->size += 0x20;
                }
                if (psht_32->size > 0xC4) {
                    psht_32->size = 0xC4;
                    RES_SoundEffect(0x29);
                }
                if (psht_32->size == 0xC4) {
                    done++;
                }
                tex_30.h = psht_32->size;
                tex_30.v = 0x1D0 - psht_32->size;
                rect = (struct SpriteRect){ 0x50 + i * 0x78, 0x32, 0x70, psht_32->size };
                MakeSprite(&tex_30, &rect);
            }
        }
        if (done == 3) {
            mode_33 = 2;
            wait_34 = 0;
            RES_SoundEffect(0x29);
        }
        return 1;
    case 2:
        for (i = 0; i < 3; i++) {
            tex_30.h = 0xC4;
            tex_30.v = 0x10C;
            rect.x = 0x50 + i * 0x78;
            rect.y = 0x32;
            rect.w = 0x70;
            rect.h = 0xC4;
            MakeSprite(&tex_30, &rect);
        }
        if (++wait_34 >= 0x1F) {
            if (bonus == 0) {
                Gwork.slot.row = 0;
            } else {
                for (;;) {
                    number = xglSRand() % 5 + 1;
                    if (number != Gwork.slot.row) {
                        Gwork.slot.row = number;
                        break;
                    }
                }
            }
            mode_33 = 3;
        }
        return 1;
    case 3:
        done = 0;
        for (i = 0; i < 3; i++) {
            psht_32 = &sht_31[i];
            psht_32->size -= 0x10;
            if (psht_32->size < 0) {
                psht_32->size = 0;
                done++;
            }
            tex_30.h = psht_32->size;
            tex_30.v = 0x1D0 - psht_32->size;
            rect = (struct SpriteRect){ 0x50 + i * 0x78, 0x32, 0x70, psht_32->size };
            MakeSprite(&tex_30, &rect);
        }
        if (done == 3) {
            mode_33 = 4;
        }
        return 1;
    case 4:
        if (bonus == 0) {
            Gwork.slot.double_pay = 0;
            Gwork.slot.bonus = 0;
            Gwork.slot.row = 0;
        } else {
            Gwork.slot.bonus_time = RES_GetBonusTime();
            Gwork.slot.bonus = 1;
        }
        mode_33 = 0;
        return 0;
    }
    return 1;
}


/* ov11:0x00a03150. Runs the current slot event (Gwork.slot.event), one step
 * per call. Events 1-4 run the win banner (SlPrizeEffect with the event as
 * its kind) and then: 1 and 4 move on to events 6 and 5; 2 turns on double
 * pay with a bonus time and sound 0x2B; 3 sets Gwork.slot.start_nine_game and plays
 * sound 0x2C. Event 5 just ends. Events 6 and 7 run the reel-change
 * animation with and without a bonus, then end with sounds 0x2A and 0x2D.
 * Returns 0 when idle (no event and busy_35 clear), otherwise 1. Not sure
 * of the in-game names of the events. */
static int SlEvent(int event)
{
    if (busy_35 == 0) {
        if (event == 0) {
            return 0;
        }
    }
    switch (event) {
    case 1:
        if (SlPrizeEffect(event) == 0) {
            Gwork.slot.event = 6;
        }
        break;
    case 2:
        if (SlPrizeEffect(event) == 0) {
            Gwork.slot.double_pay = 1;
            Gwork.slot.event = 0;
            Gwork.slot.bonus_time = RES_GetBonusTime();
            RES_SoundEffect(0x2B);
        }
        break;
    case 3:
        if (SlPrizeEffect(event) == 0) {
            Gwork.slot.start_nine_game = 1;
            Gwork.slot.event = 0;
            RES_SoundEffect(0x2C);
        }
        break;
    case 4:
        if (SlPrizeEffect(event) == 0) {
            Gwork.slot.event = 5;
        }
        break;
    case 5:
        Gwork.slot.event = 0;
        break;
    case 6:
        if (SlChangeRealEffect(1) == 0) {
            Gwork.slot.event = 0;
            RES_SoundEffect(0x2A);
        }
        break;
    case 7:
        if (SlChangeRealEffect(0) == 0) {
            Gwork.slot.event = 0;
            RES_SoundEffect(0x2D);
        }
        break;
    }
    return 1;
}


/* ov11:0x00a032c0. Slot help screen: uploads the help texture (0x0110A000)
 * and draws it as one sprite from the slotHelpTex descriptor and slotHelpRect
 * rectangle. Not sure what the help image shows. */
static void SlHelpMode(void)
{
    struct SpriteTex tex = slotHelpTex;
    struct SpriteRect rect = slotHelpRect;

    SetDrawStatus(1, 0);
    xglPacketTextureTrans(SLOT_HELP_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    MakeSprite(&tex, &rect);
}


/* ov11:0x00a03380. Slot main loop. Fetches the current packet, then draws
 * in order: the reels (SlRealDraw), cash, pay amounts and total stake, the
 * slot_1 background, the spin/stop/mode buttons and the pointer, the pay
 * window, and either the rate table or the result (Gwork.slot.state == 2
 * picks the result). Then runs SlEvent on Gwork.slot.event and, when it
 * returns 0, slot_key, and shows the help screen when Gwork.slot.help is
 * 1. Not sure of the exact game wording. */
static void slot_main(void)
{
    Pkt = xglPacketGetCurrent();
    xglPacketTextureTrans(BASE_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    SlRealDraw();
    SlCashPrint();
    pay_print(Pkt);
    total_pay_print(Pkt);
    xglPacketTextureTrans(SLOT_1_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    SlBgDraw(Pkt);
    xglPacketTextureTrans(BASE_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    SpinBtnPrint();
    SlStopBtnPrint();
    SlModePrint();
    point_control(Pkt);
    SlPayWindowPrint();
    if (Gwork.slot.state != SLOT_STATE_PAYOUT) {
        SlRateTableDraw();
    } else {
        SlResultDraw();
    }
    if (SlEvent(Gwork.slot.event) == 0) {
        slot_key();
    }
    if (Gwork.slot.help == 1) {
        SlHelpMode();
    }
}

/* ov11:0x00a034d8. Loads the five slot-machine texture resources into their
 * fixed rendering buffers through the resource loader. */
static const char slot1TexturePath[] = "data\\tanaka\\slot_1.xtx";
static const char slot2TexturePath[] = "data\\tanaka\\slot_2.xtx";
static const char baseTexturePath[] = "data\\tanaka\\base.xtx";
static const char slotW1TexturePath[] = "data\\tanaka\\w1.xtx";
static const char slotHelpTexturePath[] = "data\\tanaka\\help_all.xtx";

static void init_slot(void)
{
    xglCdReadFile(slot1TexturePath, SLOT_1_TEX_BUFFER, 0, 0);
    xglCdReadFile(slot2TexturePath, SLOT_2_TEX_BUFFER, 0, 0);
    xglCdReadFile(baseTexturePath, BASE_TEX_BUFFER, 0, 0);
    xglCdReadFile(slotW1TexturePath, SLOT_W1_TEX_BUFFER, 0, 0);
    xglCdReadFile(slotHelpTexturePath, SLOT_HELP_TEX_BUFFER, 0, 0);
}

static void NgTimePrint(int time)
{
    struct SpriteRect rect = ngTimeBarRect;
    if (time > 900) time = 900;
    rect.w = (time * 47 * 4) / 900;
    SetDrawStatus(3, 0);
    MakeSprite(&tex_36, &rect);
}


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


static void NgPaidPrint(void)
{
    decprint(Gwork.nine.payout * dptoli(pow(2.0, litodp(Gwork.nine.counter))), 7, 0x1B2, 0x182);
}


/* ov11:0x00a03668. Nine-game: draws the "gets down" button. A non-negative
 * argument restarts the countdown count_39; the sprite is picked from
 * three textures by the sign of the countdown, which then counts down and
 * wraps to -1 at zero. */
static void NgGetsDownBtnPrint(int button)
{
    int index;

    if (button >= 0) {
        count_39 = button;
    }
    index = 0;
    if (count_39 < 0) {
        index = 2;
    }
    if (count_39 > 0) {
        index = 1;
    }
    SetDrawStatus(3, 0);
    MakeSprite(&tex_37[index], &rect_38);
    if (count_39 != 0) {
        count_39 = count_39 - 1;
        if (count_39 == 0) {
            count_39 = -1;
        }
    }
}


/* ov11:0x00a03710. Nine-game: same shape as NgGetsDownBtnPrint, for the
 * spin button (spinbt, count_41). */
static void NgSpinBtnPrint(int button)
{
    int index;

    if (button >= 0) {
        count_41 = button;
    }
    index = 0;
    if (count_41 < 0) {
        index = 2;
    }
    if (count_41 > 0) {
        index = 1;
    }
    SetDrawStatus(3, 0);
    MakeSprite(&spinbt[index], &rect_40);
    if (count_41 != 0) {
        count_41 = count_41 - 1;
        if (count_41 == 0) {
            count_41 = -1;
        }
    }
}

/* ov11:0x00a037b8. Nine-game lamp row: draws one RateLamp_Rect cell per
 * counted streak level with pay_window[1], then the cell after them with
 * pay_window[anm_42 % 2], and advances anm_42. Nothing is drawn while the
 * counter is negative or the guide is 2. */

static void NgLamp(void)
{
    int i;

    if (Gwork.nine.counter < 0 || Gwork.nine.guide == 2) {
        return;
    }
    SetDrawStatus(0, 0);
    for (i = 0; i < Gwork.nine.counter; i++) {
        MakeSprite(&pay_window[1], &RateLamp_Rect[i]);
    }
    MakeSprite(&pay_window[anm_42 % 2], &RateLamp_Rect[i]);
    anm_42++;
}

/* ov11:0x00a038c0. Selects the loaded nine-game slot background texture
 * (SLOT_2_TEX_BUFFER, the "slot_2.xtx" buffer init_slot fills), sets the
 * packet's GS register write and draw state, then emits the fixed
 * nine-game background sprite. */

static void NineGameBG_Draw(void)
{
    xglPacketTextureTrans(SLOT_2_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    SetDrawStatus(3, 0);
    MakeSprite(&NineGameBG_Tex_43, &NineGameBG_Rect_44);
}

static void NgStopGuidPt(void);

static void NgStopGuidPt(void)
{
    struct SpriteRect rect;

    rect.x = GuidPt_46[Gwork.nine.mode].x;
    rect.y = GuidPt_46[Gwork.nine.mode].y;
    rect.h = 8;
    rect.w = 8;
    MakeSprite(&GuidPtTex[seq_45 % 10], &rect);
    seq_45 = seq_45 + 1;
}

/* ov11:0x00a039b0. Nine-game cursor: moves Gwork.nine.mode on to the next
 * reel after it that is still running (state 0); if there is none, wraps
 * to the first running reel from 0 up to the old cursor; if there is none
 * at all, the cursor goes to the gets-down button (10). */
static void NgFwdGuidPt(void)
{
    int i;
    int next;

    struct NineGameReel *r;

    next = 10;
    for (i = Gwork.nine.mode + 1; i < 9; i++) {
        r = &Gwork.nine.reel[i];
        if (r->state == 0) {
            next = i;
            break;
        }
    }
    if (next == 10) {
        for (i = 0; i < Gwork.nine.mode; i++) {
            r = &Gwork.nine.reel[i];
            if (r->state == 0) {
                next = i;
                break;
            }
        }
    }
    Gwork.nine.mode = next;
}


/* ov11:0x00a03a80. Nine-game: draws the nine reel stop buttons. Each
 * button sits at its stopbtn_pos_47 position, 16 by 24, and its texture
 * comes from the reel's state (0: stopbt[0], 1 or 3: stopbt[2], 2:
 * stopbt[1]); other states keep the previous texture. */
static void NgStopBtnPrint(void)
{
    struct SpriteRect rect;
    struct SpriteTex *tex = 0;
    struct NineGameReel *reel = Gwork.nine.reel;
    int (*pos)[2] = stopbtn_pos_47;
    int i;

    rect.w = 16;
    rect.h = 24;
    for (i = 0; i < 9; i++) {
        switch (reel->state) {
        case 1:
        case 3:
            tex = &stopbt[2];
            break;
        case 0:
            tex = &stopbt[0];
            break;
        case 2:
            tex = &stopbt[1];
            break;
        }
        /* the store to rect.x is overwritten below; it keeps i live */
        rect.x = i;
        rect.x = (*pos)[0];
        rect.y = (*pos)[1];
        MakeSprite(tex, &rect);
        reel++;
        pos++;
    }
}


static void NgAllStop(void)
{
    int forced_stop;
    short *reel_status;
    int remaining;

    forced_stop = 3;
    reel_status = ngReelStatusWork;
    remaining = 8;
    do {
        if (reel_status[2] == 0) {
            reel_status[2] = forced_stop;
        }
        reel_status += 4;
    } while (--remaining >= 0);
    RES_SoundEffectStop(47);
}


/* ov11:0x00a03be0. Nine-game: looks up the reel's result in ng_realtbl for
 * its current position; a hit (== 1) bumps the counter, otherwise it stops
 * all reels and resets the counter, amount and mode (10). */
static void NgCheckReal(int index)
{
    struct NineGameReel *reel = &Gwork.nine.reel[index];
    short *table = ng_realtbl[reel->number];
    int position;

    if (reel->position >= 0x3F0) {
        reel->position = 0;
    }
    position = reel->position / 0x30;
    if (table[position] == 1) {
        Gwork.nine.counter = Gwork.nine.counter + 1;
    } else {
        NgAllStop();
        Gwork.nine.counter = -1;
        Gwork.nine.payout = 0;
        Gwork.nine.mode = 10;
    }
}

/* ov11:0x00a03ca0. Returns the 28-byte sprite descriptor of one reel's
 * shape: the reel's index table in ng_realtbl picks the entry of
 * NgReal_Tex_48, with the shape number wrapped into 0..20. */

static struct SpriteTex get_ngreal_shape(int reel, int shape)
{
    short *indices;
    int selected;
    if (shape >= 21) shape -= 21;
    if (shape < 0) shape += 21;
    indices = ng_realtbl[reel];
    selected = indices[shape];
    return NgReal_Tex_48[selected];
}

/* ov11:0x00a03d58. Nine-game reel panel: splits the reel position into
 * 48-pixel cells and draws the visible panel pieces from the shape table
 * NgReal_49 (x, y per reel index) and the sprites of get_ngreal_shape. */

static void DrawNgPanel(int index, int y, int x)
{
    struct SpriteRect rect;
    struct SpriteTex tex;
    int quot;
    int rem;

    rect.w = 48;
    rect.h = 64;
    quot = y / 48;
    rem = y % 48;
    if (rem >= 25) {
        rem -= 48;
        quot += 1;
    }
    tex = get_ngreal_shape(x, quot);
    rect.y = NgReal_49[index].y;
    rect.x = NgReal_49[index].x + rem;
    MakeSprite(&tex, &rect);
    tex = get_ngreal_shape(x, quot - 1);
    rect.x = rect.x + 48;
    rect.w = 24 - rem;
    tex.w = rect.w;
    if (rect.w > 0) {
        MakeSprite(&tex, &rect);
    }
    tex = get_ngreal_shape(x, quot + 1);
    rect.x = rect.x - rem - 72;
    tex.u = tex.u - rem + 24;
    rect.w = rem + 24;
    tex.w = rem + 24;
    MakeSprite(&tex, &rect);
}


/* ov11:0x00a03eb0. Nine-game: draws the nine reel panels, then advances
 * each reel's state machine (+12 per frame, finishing when the remaining
 * distance runs out, wrapping at 0x3F0). Not sure of the exact game
 * wording. */
static void NgReal(void)
{
    int i;
    struct NineGameReel *reel;

    for (i = 0; i < 9; i++) {
        if (Gwork.nine.reel[i].state != 3) {
            DrawNgPanel(i, Gwork.nine.reel[i].position, Gwork.nine.reel[i].number);
        }
    }
    for (i = 0; i < 9; i++) {
        reel = &Gwork.nine.reel[i];
        if (reel->state == 0) {
            reel->position += 12;
        }
        if (reel->state == 2) {
            if (reel->remaining > 12) {
                reel->remaining -= 12;
                reel->position += 12;
            } else {
                reel->position += reel->remaining;
                reel->remaining = 0;
                reel->state = 1;
                NgCheckReal(i);
            }
        }
        if (reel->position >= 0x3F0) {
            reel->position = 0;
        }
    }
}


/* ov11:0x00a03fc0. Nine-game: scales the pending amount by 2^counter with
 * the double helpers, clears the counter, redraws the gets-down button and
 * stops all reels. */
static void NgGame_GetsDown(void)
{
    unsigned long long scale;

    Gwork.nine.guide = 2;
    scale = pow(2.0, litodp(Gwork.nine.counter));
    /* Empty loop: GCC 2.96 does not schedule across loop notes, which keeps
     * the original instruction order here (as in act_2.c). */
    do { } while (0);
    Gwork.nine.payout = dptoli(dpmul(litodp(Gwork.nine.payout), scale));
    Gwork.nine.counter = 0;
    NgGetsDownBtnPrint(16);
    NgAllStop();
}


/* ov11:0x00a04040. Nine-game select handler: up/down toggles the cursor
 * between the spin (9) and gets-down (10) buttons, circle starts the spin
 * or the gets-down action, and the shoulder, face, select and start buttons
 * toggle the flag. Not sure what the flag shows. */
static void NgGame_Select(void)
{
    int i;

    if (PadData.canonical.half_2a & (PAD_DOWN | PAD_UP)) {
        Gwork.nine.flag = 0;
        RES_SoundEffect(0x2E);
        switch (Gwork.nine.mode) {
        case 9:
            Gwork.nine.mode = 10;
            break;
        case 10:
            Gwork.nine.mode = 9;
            break;
        }
    }
    if (PadData.canonical.half_2a & PAD_CIRCLE) {
        Gwork.nine.flag = 0;
        switch (Gwork.nine.mode) {
        case 9:
            for (i = 8; i >= 0; i--) {
                Gwork.nine.reel[i].state = 0;
            }
            Gwork.nine.guide = 1;
            NgFwdGuidPt();
            NgSpinBtnPrint(16);
            RES_SoundEffect(0x2F);
            break;
        case 10:
            NgGame_GetsDown();
            RES_SoundEffect(0x30);
            break;
        }
    }
    if (PadData.canonical.half_2a & (PAD_START | PAD_SELECT | PAD_SQUARE | PAD_TRIANGLE | PAD_R1 | PAD_L1 | PAD_R2 | PAD_L2)) {
        if (Gwork.nine.flag == 0) {
            Gwork.nine.flag = 1;
        } else {
            Gwork.nine.flag = 0;
        }
    }
}

/* Cursor moves of the nine-game: for each cursor position (0-8 the reels
 * in a 3x3 grid, 0-2 the top row; 9 the spin button, 10 the gets-down
 * button) the position reached with each direction button. In the table,
 * left and right wrap within a row and up/down step a row, so the columns
 * are, in order, left, up, right and down. */
enum NgCursorMove {
    NG_MOVE_LEFT,
    NG_MOVE_UP,
    NG_MOVE_RIGHT,
    NG_MOVE_DOWN
};

/* ov11:0x00a04188. Nine-game input: the direction buttons (auto-repeat
 * word) move the cursor (Gwork.nine.mode) through ptmove_50, cross jumps to
 * the gets-down button (10), and circle acts on the cursor: on a reel that
 * is still running (state 0) it starts the stop (state 2, remaining = 96
 * minus position % 48) and moves the cursor on (NgFwdGuidPt); on 10 it
 * takes the winnings (NgGame_GetsDown). The case range is what gives the
 * original's compare order (below 0, below 9, equal to 10). */
static void NgGame_Key(void)
{
    struct NineGameReel *reel;

    if (PadData.wide.repeat & PAD_LEFT) {
        Gwork.nine.mode = ptmove_50[Gwork.nine.mode][NG_MOVE_LEFT];
        RES_SoundEffect(0x2E);
    }
    if (PadData.wide.repeat & PAD_UP) {
        Gwork.nine.mode = ptmove_50[Gwork.nine.mode][NG_MOVE_UP];
        RES_SoundEffect(0x2E);
    }
    if (PadData.wide.repeat & PAD_RIGHT) {
        Gwork.nine.mode = ptmove_50[Gwork.nine.mode][NG_MOVE_RIGHT];
        RES_SoundEffect(0x2E);
    }
    if (PadData.wide.repeat & PAD_DOWN) {
        Gwork.nine.mode = ptmove_50[Gwork.nine.mode][NG_MOVE_DOWN];
        RES_SoundEffect(0x2E);
    }
    if (PadData.canonical.half_2a & PAD_CROSS) {
        Gwork.nine.mode = 10;
        RES_SoundEffect(0x31);
    }
    if (PadData.canonical.half_2a & PAD_CIRCLE) {
        switch (Gwork.nine.mode) {
        case 0 ... 8:
            reel = &Gwork.nine.reel[Gwork.nine.mode];
            if (reel->state == 0) {
                reel->state = 2;
                reel->remaining = 96 - reel->position % 48;
                NgFwdGuidPt();
                RES_SoundEffect(0x32);
            } else {
                RES_SoundEffect(0x33);
            }
            break;
        case 10:
            NgGame_GetsDown();
            RES_SoundEffect(0x30);
            break;
        }
    }
}


/* ov11:0x00a04360. Nine-game payout step: when nothing is left to pay it
 * stops the payout sound, switches back to the slot game and clears
 * se_51; otherwise it starts the payout sound once and pays out coins, all
 * at once if circle is pressed and one per call otherwise. */
static void NgGame_Paid(void)
{
    int amount;

    if (Gwork.nine.payout == 0) {
        RES_SoundEffectStop(0x34);
        GameModeChange(CASINO_SLOT);
        se_51 = 0;
        return;
    }
    if (se_51 == 0) {
        RES_SoundEffect(0x34);
        se_51 = 1;
    }
    amount = Gwork.nine.payout;
    if (PadData.canonical.half_2a & PAD_CIRCLE) {
        AddCoin(amount);
        Gwork.nine.payout = 0;
    } else {
        Gwork.nine.payout = amount - 1;
        AddCoin(1);
    }
}


/* ov11:0x00a04410. Nine-game help screen: two textured sprites from fixed
 * tables. */
static void NgHelpMode(void)
{
    struct SpriteTex tex1 = ngHelpTopTex;
    struct SpriteTex tex2 = ngHelpBottomTex;
    struct SpriteRect rect1 = ngHelpTopRect;
    struct SpriteRect rect2 = ngHelpBottomRect;

    SetDrawStatus(1, 0);
    xglPacketTextureTrans(SLOT_HELP_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    MakeSprite(&tex1, &rect1);
    MakeSprite(&tex2, &rect2);
}


/* ov11:0x00a04560. Nine-game main loop. Fetches the current packet and
 * draws the reels, paid amount, background, lamps, the two buttons, the
 * countdown (time_left), the stop buttons and the guide pointer. Then runs
 * the handler for the current guide state (0 select, 1 key, 2 paid); in
 * state 1 it counts time_left down and calls NgGame_GetsDown when it runs
 * out, and it shows the help screen when the flag is 1. Not sure of the
 * exact game wording. */
static void NineGameMain(void)
{
    Pkt = xglPacketGetCurrent();
    xglPacketTextureTrans(BASE_TEX_BUFFER);
    NgReal();
    NgPaidPrint();
    NineGameBG_Draw();
    xglPacketTextureTrans(BASE_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    NgLamp();
    NgSpinBtnPrint(-1);
    NgGetsDownBtnPrint(-1);
    NgTimePrint(Gwork.nine.time_left);
    NgStopBtnPrint();
    NgStopGuidPt();
    switch (Gwork.nine.guide) {
    case 0:
        NgGame_Select();
        break;
    case 1:
        NgGame_Key();
        break;
    case 2:
        NgGame_Paid();
        break;
    }
    if (Gwork.nine.guide == 1) {
        Gwork.nine.time_left--;
        if (Gwork.nine.time_left <= 0) {
            NgGame_GetsDown();
        }
    }
    if (Gwork.nine.flag == 1) {
        NgHelpMode();
    }
}

/* ov11:0x00a046d0. Nine-game start-up: clears the counter and guide, sets
 * mode 9, resets all nine reels, then gives each reel a unique random
 * number in 0..19 (re-rolling on a duplicate) and redraws the two buttons.
 * Not sure of the exact game wording. */
static void init_ninegame(void)
{
    int i;
    int j;
    unsigned short number;

    Gwork.nine.counter = 0;
    Gwork.nine.guide = 0;
    Gwork.nine.mode = 9;
    Gwork.nine.time_left = 0x384;
    for (i = 0; i < 9; i++) {
        Gwork.nine.reel[i].number = -1;
        Gwork.nine.reel[i].position = 0;
        Gwork.nine.reel[i].state = 1;
    }
    for (i = 0; i < 9; i++) {
        number = xglSRand() % 20;
        for (j = 0; j < i; j++) {
            if (number == Gwork.nine.reel[j].number) {
                number = xglSRand() % 20;
                j = -1;
            }
        }
        Gwork.nine.reel[i].number = number;
    }
    NgGetsDownBtnPrint(0);
    NgSpinBtnPrint(0);
    Gwork.nine.flag = 1;
}


/* ov11:0x00a04828. Poker: draws a coin counter, a frame sprite picked by
 * mode and then the decimal digits of value right to left, 16 pixels apart
 * from x = 472 (a single 0 for a value below 1). The digit count comes from
 * comparing value with the powers of ten. */
static void PoMoneyPrint(int mode, int value)
{
    int digits = 0;
    struct SpriteRect digit_rect = pokerMoneyDigitRect;
    struct SpriteRect frame_rect = pokerMoneyFrameRect;
    int i;
    unsigned long long scale;
    unsigned long long amount;

    while (digits < 11) {
        double amount_d = value;
        if (dpcmp(amount_d, pow(10.0, litodp(digits))) >= 0) {
            digits++;
        } else {
            break;
        }
    }
    SetDrawStatus(3, 0);
    MakeSprite(&waku_53[mode], &frame_rect);
    if (digits > 0) {
        for (i = 0; i < digits;) {
            digit_rect.x = 472 - i * 16;
            scale = pow(10.0, litodp(i++));
            do { } while (0);
            amount = litodp(value);
            do { } while (0);
            MakeSprite(&fontlist_52[dptoli(dpdiv(amount, scale)) % 10], &digit_rect);
        }
    } else {
        MakeSprite(&fontlist_52[0], &digit_rect);
    }
}


/* ov11:0x00a04a08. Poker: draws the animated cursor at pos (x, y), a 24 by
 * 24 rectangle, using the frame counter cnt_54 to pick one of the eight
 * PoCurTex frames, then advances the counter. */
static void PoCurDraw(short *pos)
{
    struct SpriteRect *rect;

    xglPacketTextureTrans(POKER_TABLE2_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    rect = &rect_55;
    rect->h = 24;
    rect->w = 24;
    rect->x = pos[0];
    rect->y = pos[1];
    MakeSprite(PoCurTex + cnt_54 % 8, rect);
    cnt_54 = cnt_54 + 1;
}


/* ov11:0x00a04ac8. Poker: draws a message window sized by the text length
 * (left cap, right cap, middle) and prints the message. */
static void PoWindowMessage(const char *message, int length)
{
    struct SpriteRect rect = pokerWindowRect;

    SetDrawStatus(3, 0);
    SetTest(CASINO_TEST_DEPTH_GEQUAL);
    rect.x = 16;
    rect.w = 8;
    MakeSprite(&Ltex_56, &rect);
    rect.x = length * 9 + 28;
    MakeSprite(&Rtex_57, &rect);
    rect.x = 24;
    rect.w = length * 9 + 4;
    MakeSprite(&tex_58, &rect);
    xglFontPrint(0, 0, 0, pokerWindowFontControlFirst);
    xglFontPrint(0, 0, 0, pokerWindowFontControlSecond);
    xglFontPrint(24, 218, 0xFF00, message);
}

static short PoGetBase(int cardValue);

/* ov11:0x00a04be0. Poker yes/no style dialog. type 0 shows a two-choice
 * window and any other type a three-choice window. Up moves the cursor
 * (dialog_cur_67) up with wraparound and down moves it down, both with a
 * sound; circle confirms (plays a sound, resets the cursor and returns the
 * chosen entry) and cross cancels (returns -1); otherwise it
 * returns 0. Not sure of the exact game wording. */
static int PoDialog(int type)
{
    int limit;
    int selected;

    xglPacketTextureTrans(POKER_TABLE2_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    limit = 2;
    if (type != 0) {
        limit = 3;
    }
    if (PadData.canonical.half_2a & PAD_UP) {
        dialog_cur_67--;
        RES_SoundEffect(0x3C);
        if (dialog_cur_67 == 0) {
            dialog_cur_67 = limit;
        }
    }
    if (PadData.canonical.half_2a & PAD_DOWN) {
        RES_SoundEffect(0x3C);
        dialog_cur_67++;
        if (dialog_cur_67 == limit + 1) {
            dialog_cur_67 = 1;
        }
    }
    SetDrawStatus(3, 0);
    if (type == 0) {
        MakeSprite(&tex_59, &rect_60[0]);
        MakeSprite(&type0_tex_61, &type0_rect_62);
        PoCurDraw(type0_pos_65[dialog_cur_67 - 1]);
    } else {
        MakeSprite(&tex_59, &rect_60[1]);
        MakeSprite(&type1_tex_63, &type1_rect_64);
        PoCurDraw(type1_pos_66[dialog_cur_67 - 1]);
    }
    if (PadData.canonical.half_2a & PAD_CIRCLE) {
        RES_SoundEffect(0x3F);
        selected = dialog_cur_67;
        dialog_cur_67 = 1;
        return selected;
    }
    if (PadData.canonical.half_2a & PAD_CROSS) {
        RES_SoundEffect(0x40);
        dialog_cur_67 = 1;
        return -1;
    }
    return 0;
}

/* ov11:0x00a04dc0. Selects the loaded poker table-1 background texture
 * (0x0138f000, the "poker_1.xtx" buffer PokerInit below fills as
 * POKER_TABLE1_TEX_BUFFER) and emits the fixed poker background sprite. */

static void PoBgDraw(void)
{
    SetDrawStatus(3, 0);
    xglPacketTextureTrans(POKER_TABLE1_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    SetDrawStatus(3, 0);
    MakeSprite(&tex_68, &rect_69);
}


/* ov11:0x00a04e20. Poker: for each of the four suits selects the card
 * texture buffer and draws the held cards that fall in that suit's number
 * range. */
static void PoCardDraw(void)
{
    int suit;
    int slot;
    int base;

    base = 0;
    SetDrawStatus(3, 0);
    SetTest(CASINO_TEST_DEPTH_GEQUAL);
    for (suit = 0; suit < 4; suit++) {
        xglPacketTextureTrans(CardBuf_70[suit]);
        SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
        for (slot = 0; slot < 5; slot++) {
            int card = Gwork.poker_deal.cards.hand[slot];
            if (card >= range_71[suit].lo && card <= range_71[suit].hi) {
                MakeSprite(&CardTex[card - base], &CardPos[slot]);
            }
        }
        base += 13;
    }
}


/* ov11:0x00a04f60. Poker deal animation for all five cards at once, driven
 * by status_73. State 0 copies each card's slot rect and moves it up and
 * left by an amount that grows per card; state 1 draws the five card
 * sprites, moves each down 0x20 per call (a sound plays when a card's y
 * reaches 5) until all five arrive, then switches to state 2 with a 4-call
 * hold; state 2 keeps drawing and, when the hold ends, resets to state 0
 * and returns 1. Otherwise returns 0. Not sure of the exact game wording. */
static int PoCardDrawAnime(void)
{
    int i;
    int arrived;

    xglPacketTextureTrans(POKER_SPADE_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    switch (status_73) {
    case 0:
        for (i = 0; i < 5; i++) {
            card_74[i] = CardPos[i];
            card_74[i].y = card_74[i].y - i * 128 - 0xA0;
        }
        status_73 = 1;
        /* fall through */
    case 1:
        arrived = 0;
        for (i = 0; i < 5; i++) {
            MakeSprite(&tex_72, &card_74[i]);
            if (card_74[i].y == 5) {
                RES_SoundEffect(0x36);
            }
            if (card_74[i].y < CardPos[i].y) {
                card_74[i].y += 0x20;
            } else {
                arrived++;
            }
        }
        if (arrived == 5) {
            status_73 = 2;
            wait_75 = 4;
        }
        return 0;
    case 2:
        for (i = 0; i < 5; i++) {
            MakeSprite(&tex_72, &card_74[i]);
        }
        if (--wait_75 == 0) {
            status_73 = 0;
            return 1;
        }
        return 0;
    default:
        return 0;
    }
}


/* ov11:0x00a05180. Poker bonus card animation, one step per call, driven
 * by status_77 and the card index in Gwork.poker_deal.bonus_round. State 0 starts a card
 * sprite at the card's slot position moved up by 0xA0; state 1 draws it,
 * moves it down 0x20 per call (a sound plays at y == 5) until it reaches
 * the slot, then switches to state 2 with a 4-call hold; in state 2 it
 * keeps drawing and, when the hold runs out, sets that hand slot to 0x35
 * (face-down, not sure) and marks it selected, returning 1. Otherwise
 * returns 0. Not sure of the exact game wording. */
static int PoBonusCardAnime(void)
{
    int card;
    int blank;

    xglPacketTextureTrans(POKER_SPADE_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    card = Gwork.poker_deal.bonus_round;
    switch (status_77) {
    case 0:
        status_77 = 1;
        card_78 = CardPos[card];
        card_78.y -= 0xA0;
        /* fall through */
    case 1:
        MakeSprite(&tex_76, &card_78);
        card_78.y += 0x20;
        if (card_78.y == 5) {
            RES_SoundEffect(0x3D);
        }
        if (card_78.y >= CardPos[card].y) {
            status_77 = 2;
            wait_79 = 4;
        }
        return 0;
    case 2:
        MakeSprite(&tex_76, &card_78);
        blank = 0x35;
        if (--wait_79 == 0) {
            status_77 = 0;
            Gwork.poker_deal.cards.hand[card] = blank;
            Gwork.poker_deal.replace_selected[card] = 1;
            return 1;
        }
        return 0;
    default:
        return 0;
    }
}

/* ov11:0x00a05358. Emits the single fixed poker payout table sprite. */

static void PoRateTableDraw(void)
{
    MakeSprite(&tex_80, &rect_81);
}


/* ov11:0x00a05380. Poker bonus-round background: draws the background
 * sprite, then pulses rate_84 up and down by 8 over a 16-frame cycle
 * (cnt_82, add_83). It draws one marker per bonus card so far
 * (Gwork.poker_deal.bonus_round) and then one pointer per card, the last with the pulsing blend
 * value and the others with 0x30. Not sure what the markers and pointers
 * mean in the game, or why the pointer loop also stops at 5. */
static void PoBonusBGDraw(void)
{
    int i;

    xglPacketTextureTrans(POKER_TABLE2_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    MakeSprite(&tex_85, &rect_86);
    if (cnt_82 < 8) {
        add_83 = 8;
    } else {
        add_83 = -8;
    }
    cnt_82 = cnt_82 + 1;
    if (cnt_82 == 16) {
        cnt_82 = 0;
    }
    rate_84 = rate_84 + add_83;
    SetDrawStatus(6, 0x40);
    for (i = 0; i < Gwork.poker_deal.bonus_round; i++) {
        MakeSprite(&MarkerTex_87, &MarkerPos_88[i]);
    }
    for (i = 0; i < Gwork.poker_deal.bonus_round && i != 5; i++) {
        if (i == Gwork.poker_deal.bonus_round - 1) {
            SetDrawStatus(6, rate_84);
        } else {
            SetDrawStatus(6, 0x30);
        }
        MakeSprite(&PtrTex_89, &PtrPos_90[i]);
    }
}

/* Poker's deck-use flags, dealt cards and replacement choices occupy
 * these fields of Gwork. PoShuffleCard uses replacement value 1 to deal
 * a slot; card value 53 denotes an empty hand slot. */
static void PoResetCard(void)
{
    int index;
    for (index = 52; index >= 0; index--) {
        Gwork.poker_deal.cards.used[index] = 0;
    }
    for (index = 0; index < 5; index++) {
        Gwork.poker_deal.cards.hand[index] = 53;
        Gwork.poker_deal.replace_selected[index] = 1;
    }
}


/* Returns 1 once no replacement slot remains selected. */
static int PoShuffleCard(void)
{
    int i;
    unsigned short card = 0;
    int count;

    count = 0;
    for (i = 0; i < 5; i++) {
        if (Gwork.poker_deal.replace_selected[i] == 1) {
            count++;
        }
    }
    if (count == 0) {
        return 1;
    }
    if (--wait_91 != 0) {
        return 0;
    }
    i = 0;
    while (i < 5) {
        if (Gwork.poker_deal.replace_selected[i] == 1) {
            break;
        }
        i++;
    }
    for (;;) {
        if (Gwork.poker_deal.bonus_mode == 0) {
            card = xglSRand() % 52;
        } else {
            switch (i) {
            case 0:
                card = 0;
                break;
            case 1:
                card = 9;
                break;
            case 2:
                card = 10;
                break;
            case 3:
                card = 11;
                break;
            case 4:
                card = 12;
                break;
            }
        }
        if (Gwork.poker_deal.cards.used[card] == 1) {
            continue;
        }
        Gwork.poker_deal.cards.hand[i] = card;
        Gwork.poker_deal.cards.used[card] = 1;
        break;
    }
    RES_SoundEffect(65);
    Gwork.poker_deal.replace_selected[i] = 0;
    wait_91 = 8;
    return 0;
}


/* ov11:0x00a05758. Poker: draws the five selection marks, texture picked
 * from replace_selected[], then one fixed sprite. 0x013CF800 is the buffer
 * POKER_TABLE2_TEX_BUFFER names below. */
static void PoSelectMarkDraw(void)
{
    int i;

    xglPacketTextureTrans(POKER_TABLE2_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    SetDrawStatus(1, 0);
    for (i = 0; i < 5; i++) {
        MakeSprite(&PoSelecterTex[Gwork.poker_deal.replace_selected[i]], &PoSelecterPos[i]);
    }
    MakeSprite(&PoSelecterTex[1], &PoSelecterPos[5]);
}


/* ov11:0x00a05818. Poker: card selection. Up and down switch mode_93
 * between the card cursor (0) and the confirm button (1). In card mode
 * right and left (auto-repeat) move the cursor cur_92 over the five cards
 * with wraparound and circle toggles the card's replace flag. In button
 * mode circle plays a sound, resets both and returns 1; otherwise 0. */
static int PoKey(void)
{
    if (PadData.canonical.half_2a & PAD_UP) {
        RES_SoundEffect(0x3E);
        mode_93 = 0;
    }
    if (PadData.canonical.half_2a & PAD_DOWN) {
        RES_SoundEffect(0x3E);
        mode_93 = 1;
    }
    if (mode_93 == 1) {
        PoCurDraw(chgpos_95);
        if (PadData.canonical.half_2a & PAD_CIRCLE) {
            RES_SoundEffect(0x37);
            cur_92 = 0;
            mode_93 = 0;
            return 1;
        }
    } else {
        if (PadData.wide.repeat & PAD_RIGHT) {
            RES_SoundEffect(0x3E);
            cur_92++;
            if (cur_92 >= 5) {
                cur_92 = 0;
            }
        }
        if (PadData.wide.repeat & PAD_LEFT) {
            RES_SoundEffect(0x3E);
            cur_92--;
            if (cur_92 < 0) {
                cur_92 = 4;
            }
        }
        if (PadData.canonical.half_2a & PAD_CIRCLE) {
            RES_SoundEffect(0x37);
            if (Gwork.poker_deal.replace_selected[cur_92] == 0) {
                Gwork.poker_deal.replace_selected[cur_92] = 1;
            } else {
                Gwork.poker_deal.replace_selected[cur_92] = 0;
            }
        }
        PoCurDraw(selpos_94[cur_92]);
    }
    return 0;
}


/* ov11:0x00a059b8. Poker: sets cards the player chose to replace to 53
 * (face-down, not sure); when all five are chosen for 32 frames it sets
 * the flag at Gwork+0x190 and clears the used-card table. */
static void PoCardFlip(void)
{
    int i;
    int count;

    count = 0;
    for (i = 0; i < 5; i++) {
        if (Gwork.poker_deal.replace_selected[i] == 1) {
            Gwork.poker_deal.cards.hand[i] = 53;
            count++;
        }
    }
    if (count == 5) {
        status_96 = status_96 + 1;
    } else {
        status_96 = 0;
    }
    if (status_96 >= 32) {
        Gwork.poker_deal.bonus_mode = 1;
        for (i = 0; i < 53; i++) {
            Gwork.poker_deal.cards.used[i] = 0;
        }
    } else {
        Gwork.poker_deal.bonus_mode = 0;
    }
}

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


/* ov11:0x00a05b10. Poker: copies the five hand cards into sorted_hand,
 * bubble-sorts them and prints each with dprintf (debug output). */
static void PoSortCard(void)
{
    int i;
    int n;

    for (i = 0; i < 5; i++) {
        Gwork.poker_deal.sorted_hand[i] = Gwork.poker_deal.cards.hand[i];
    }
    for (n = 4; n != 0; n--) {
        for (i = 0; i < n; i++) {
            short prev = Gwork.poker_deal.sorted_hand[i];
            if (Gwork.poker_deal.sorted_hand[i + 1] < prev) {
                Gwork.poker_deal.sorted_hand[i] = Gwork.poker_deal.sorted_hand[i + 1];
                Gwork.poker_deal.sorted_hand[i + 1] = prev;
            }
        }
    }
    for (i = 0; i < 5; i++) {
        dprintf(pokerSortedCardFormat, Gwork.poker_deal.sorted_hand[i]);
    }
    dprintf(debugNewlineText);
}


/* ov11:0x00a05bf0. Poker: counts, over all ordered pairs of the five
 * sorted hand cards, how many differ by exactly 13, 26 or 39 (same rank in
 * another suit), printing a debug line for each hit. The match count
 * (1..4, 6) maps to the group POKER_MULTI_* (pair, two pair, three of a
 * kind, full house, four of a kind); anything else returns 0. */
static int PoMultiChk(void)
{
    int i;
    int j;
    int matches;
    int card;
    int other;

    matches = 0;
    for (i = 0; i < 5; i++) {
        card = Gwork.poker_deal.sorted_hand[i];
        for (j = 0; j < 5; j++) {
            if (i == j) {
                continue;
            }
            other = Gwork.poker_deal.sorted_hand[j];
            if (other == card + 13) {
                matches++;
                dprintf(pokerRankMatch1Text);
            }
            if (other == card + 26) {
                matches++;
                dprintf(pokerRankMatch2Text);
            }
            if (other == card + 39) {
                matches++;
                dprintf(pokerRankMatch3Text);
            }
        }
        dprintf(pokerRankSeparatorText);
    }
    dprintf(debugNewlineText);
    switch (matches) {
    case 6:
        return POKER_MULTI_FOUR_OF_A_KIND;
    case 4:
        return POKER_MULTI_FULL_HOUSE;
    case 3:
        return POKER_MULTI_THREE_OF_A_KIND;
    case 2:
        return POKER_MULTI_TWO_PAIR;
    case 1:
        return POKER_MULTI_ONE_PAIR;
    default:
        return 0;
    }
}

static int PoFlushChk(void)
{
    struct PokerDealWork *hand;
    short *card;
    int suit;
    int index;

    hand = &Gwork.poker_deal;
    card = &hand->sorted_hand[1];
    suit = PoGetMark(hand->sorted_hand[0]);
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

/* ov11:0x00a05de0. Reports category 1, printing pokerOnePairDebugText through
 * dprintf, when PoMultiChk's multiplicity equals 1. */
/* JP: ワンペア\n
 * EN: One pair\n */
static const char pokerOnePairDebugText[16] = "\xA5\xEF\xA5\xF3\xA5\xDA\xA5\xA2\n";

static int PoChk1(void)
{
    if (PoMultiChk() == POKER_MULTI_ONE_PAIR) {
        dprintf(pokerOnePairDebugText);
        return POKER_HAND_ONE_PAIR;
    }
    return 0;
}

/* ov11:0x00a05e20. Reports category 2, printing pokerTwoPairDebugText through
 * dprintf, when PoMultiChk's multiplicity equals 2. */
/* JP: ツーペア\n
 * EN: Two pair\n */
static const char pokerTwoPairDebugText[16] = "\xA5\xC4\xA1\xBC\xA5\xDA\xA5\xA2\n";

static int PoChk2(void)
{
    if (PoMultiChk() == POKER_MULTI_TWO_PAIR) {
        dprintf(pokerTwoPairDebugText);
        return POKER_HAND_TWO_PAIR;
    }
    return 0;
}

/* ov11:0x00a05e60. Reports category 3, printing pokerThreeOfAKindDebugText through
 * dprintf, when PoMultiChk's multiplicity equals 3. */
/* JP: スリーカード\n
 * EN: Three of a kind\n */
static const char pokerThreeOfAKindDebugText[16] = "\xA5\xB9\xA5\xEA\xA1\xBC\xA5\xAB\xA1\xBC\xA5\xC9\n";

static int PoChk3(void)
{
    if (PoMultiChk() == POKER_MULTI_THREE_OF_A_KIND) {
        dprintf(pokerThreeOfAKindDebugText);
        return POKER_HAND_THREE_OF_A_KIND;
    }
    return 0;
}

/* ov11:0x00a05ea0. Poker: category 4. Sorts the hands' rank offsets and,
 * when they are consecutive, prints pokerStraightDebugText with dprintf and returns 4
 * (looks like a straight; not sure of the label). */
static int PoChk4(void)
{
    int diff[5];
    int count;
    int i;
    int n;

    for (count = 0; count < 5; count++) {
        int value = Gwork.poker_deal.sorted_hand[count];
        diff[count] = value - PoGetBase(PoGetMark(value));
    }
    for (n = 4; n != 0; n--) {
        for (i = 0; i < n; i++) {
            int prev = diff[i];
            if (diff[i + 1] < prev) {
                diff[i] = diff[i + 1];
                diff[i + 1] = prev;
            }
        }
    }
    if (diff[1] == diff[0] + 1 && diff[2] == diff[0] + 2 &&
        diff[3] == diff[0] + 3 && diff[4] == diff[0] + 4) {
        dprintf(pokerStraightDebugText);
        return POKER_HAND_STRAIGHT;
    }
    return 0;
}

/* ov11:0x00a05fb0. Substitutes category 5 and prints pokerFlushDebugText through
 * dprintf whenever PoFlushChk returns nonzero; PoFlushChk's own nonzero
 * value is discarded rather than reused as the category. */
/* JP: フラッシュ\n
 * EN: Flush\n */
static const char pokerFlushDebugText[16] = "\xA5\xD5\xA5\xE9\xA5\xC3\xA5\xB7\xA5\xE5\n";

static int PoChk5(void)
{
    int category = PoFlushChk();

    if (category != 0) {
        dprintf(pokerFlushDebugText);
        category = POKER_HAND_FLUSH;
    }
    return category;
}

/* ov11:0x00a05fe8. Reports category 6, printing pokerFullHouseDebugText through
 * dprintf, when PoMultiChk's multiplicity equals 4. */
/* JP: フルハウス\n
 * EN: Full house\n */
static const char pokerFullHouseDebugText[16] = "\xA5\xD5\xA5\xEB\xA5\xCF\xA5\xA6\xA5\xB9\n";

static int PoChk6(void)
{
    if (PoMultiChk() == POKER_MULTI_FULL_HOUSE) {
        dprintf(pokerFullHouseDebugText);
        return POKER_HAND_FULL_HOUSE;
    }
    return 0;
}

/* ov11:0x00a06028. Reports category 7, printing pokerFourOfAKindDebugText through
 * dprintf, when PoMultiChk's multiplicity equals 5. */
/* JP: フォーカード\n
 * EN: Four of a kind\n */
static const char pokerFourOfAKindDebugText[16] = "\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xAB\xA1\xBC\xA5\xC9\n";

static int PoChk7(void)
{
    if (PoMultiChk() == POKER_MULTI_FOUR_OF_A_KIND) {
        dprintf(pokerFourOfAKindDebugText);
        return POKER_HAND_FOUR_OF_A_KIND;
    }
    return 0;
}

/* JP: ストレートフラッシュ\n
 * EN: Straight flush\n */
static const char pokerStraightFlushDebugText[24] = "\xA5\xB9\xA5\xC8\xA5\xEC\xA1\xBC\xA5\xC8\xA5\xD5\xA5\xE9\xA5\xC3\xA5\xB7\xA5\xE5\n";

static int PoChk8(void)
{
    struct PokerDealWork *hand;
    int first;

    if (PoFlushChk() == 0) {
        return 0;
    }
    hand = &Gwork.poker_deal;
    first = hand->sorted_hand[0];
    if (hand->sorted_hand[1] != first + 1 ||
        hand->sorted_hand[2] != first + 2 ||
        hand->sorted_hand[3] != first + 3) {
        return 0;
    }
    if (hand->sorted_hand[4] == first + 4) {
        dprintf(pokerStraightFlushDebugText);
        return POKER_HAND_STRAIGHT_FLUSH;
    }
    return 0;
}

/* ov11:0x00a060e8. Reports category 9 when PoFlushChk finds a flush and
 * the sorted hand's ranks, read from the Gwork work block, form a
 * consecutive run starting at the lowest card: base, base+9, base+0xA,
 * base+0xB, base+0xC. The double
 * PoGetBase(PoGetBase(base)) round trip is the original's own check, not
 * a simplification. Any other outcome, including a flush that is not
 * this run, reports 0. */
/* JP: ロイヤルストレートフラッシュ\n
 * EN: Royal flush\n */
static const char pokerRoyalStraightFlushDebugText[32] = "\xA5\xED\xA5\xA4\xA5\xE4\xA5\xEB\xA5\xB9\xA5\xC8\xA5\xEC\xA1\xBC\xA5\xC8\xA5\xD5\xA5\xE9\xA5\xC3\xA5\xB7\xA5\xE5\n";

static int PoChk9(void)
{
    struct PokerDealWork *hand;
    short base;

    if (PoFlushChk() == 0) {
        return 0;
    }

    hand = &Gwork.poker_deal;
    base = hand->sorted_hand[0];
    if (base == PoGetBase(PoGetBase(base))) {
        if (hand->sorted_hand[1] == base + 9 &&
            hand->sorted_hand[2] == base + 0xA &&
            hand->sorted_hand[3] == base + 0xB &&
            hand->sorted_hand[4] == base + 0xC) {
            dprintf(pokerRoyalStraightFlushDebugText);
            return POKER_HAND_ROYAL_STRAIGHT_FLUSH;
        }
    }
    return 0;
}

/* ov11:0x00a06190. Sorts the hand, then checks hand-rank categories from
 * nine downward until one reports nonzero, plays one of three sound tiers
 * for the result, and reports it through the same debug printf as the
 * PoChk* family (pokerResultFormat, "result = %d\n").
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
static const char pokerResultFormat[] = "result = %d\n";

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
        if (category >= POKER_HAND_FULL_HOUSE) {
            if (category < POKER_HAND_ROYAL_STRAIGHT_FLUSH + 1) {
                RES_SoundEffect(CASINO_SE_POKER_WIN_BIG);
            } else {
                goto defaultSound;
            }
        } else {
            RES_SoundEffect(CASINO_SE_POKER_WIN);
        }
    } else {
defaultSound:
        RES_SoundEffect(CASINO_SE_POKER_NO_WIN);
    }

    dprintf(pokerResultFormat, category);
    return category;
}

/* ov11:0x00a062b8. Draws the poker result lamp sprite for hand category
 * index category, selecting its fixed rect from a shared row of 0x10-byte
 * rects (the same tex/rect MakeSprite pair shape used throughout this TU;
 * the row's entry count beyond this stride is not separately evidenced). */

static void PoResultLamp(int category)
{
    SetDrawStatus(0, 0);
    MakeSprite(&tex_98, &result_rect_97[category]);
    SetDrawStatus(3, 0);
}

/* ov11:0x00a06308. Sets the poker sequence state (an enum PokerSeq
 * value). */
static void PoSeqChange(int sequenceState)
{
    Gwork.poker_deal.seq = sequenceState;
}

/* ov11:0x00a06318. Looks up the currently selected bet level's ready
 * message and message-box width from two fixed four-entry tables and
 * forwards both to the message window renderer. Gwork.modes.level is the
 * level index level_select stores (0..3); the four strings are the "Play N
 * coins" prompts for the four bet levels. The compiled form copies each
 * table onto the stack as a 16-byte aggregate before indexing it, which is
 * why each table is typed as a one-member array struct rather than a bare
 * array (a bare array cannot be copied by value in C). */

static void PokerReadyMes(void)
{
    PokerReadyMessageTable readyMessage = pokerReadyTexts;
    PokerReadyWidthTable readyWidth = pokerReadyWidthTable;

    PoWindowMessage(readyMessage.text[Gwork.modes.level], readyWidth.width[Gwork.modes.level]);
}

const PokerReadyMessageTable pokerReadyTexts = {{
    pokerPlay5CoinsText, pokerPlay10CoinsText, pokerPlay30CoinsText, pokerPlay100CoinsText,
}};
const PokerReadyWidthTable pokerReadyWidthTable = {{29, 30, 30, 31}};


/* ov11:0x00a063a0. Poker main loop, one call per frame, switching on
 * Gwork.poker_deal.seq (enum PokerSeq; jump table jtbl_00A0C220):
 *   READY: if the coin balance is below the level's stake (PokerCoinRate)
 *      go to NO_COINS, else show the "Play N coins" dialog; yes goes to
 *      BET, anything else back to the casino menu. NO_COINS: "There are no
 *      more coins.", any button leaves.
 *   BET: move the stake from the balance into the pot, 1, 5 or 10 coins
 *      per frame. DEAL to SCORE: deal (PoCardDrawAnime), fill the hand
 *      (PoShuffleCard), choose cards (PoKey), PoCardFlip and PoShuffleCard
 *      again, then score the hand (PoCardCheck; a win multiplies the pot by
 *      PokerRate).
 *   CONTINUE: "Continue game?". WIN/DOUBLE_UP: "Double or nothing?"; yes
 *      starts the bonus game, no pays out.
 *   BONUS_*: the bonus game deals five cards one at a time. The first card
 *      is only revealed; each later one is compared with the card before it
 *      against the guess from BONUS_GUESS (a tie also wins). Each of the
 *      four comparisons doubles the pot when the guess is right, and after
 *      the fifth card the pot is paid out; a wrong guess clears the pot
 *      (BONUS_LOST waits for circle).
 *   PAYOUT: pay the pot out, a step per frame by level (1, 5, 10, 100) or
 *      all at once with circle.
 * Not sure what enters CONTINUE (nothing in OV11 does). coins is kept apart
 * from step: one shared variable allocates PAYOUT differently. */
static void PokerMain(void)
{
    int sel;
    int step;
    int coins;
    int i;
    int previous_rank;
    int current_rank;

    Pkt = xglPacketGetCurrent();
    PoBgDraw();
    xglPacketTextureTrans(POKER_TABLE2_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    switch (Gwork.poker_deal.seq) {
    case POKER_SEQ_READY:
        Gwork.poker_deal.rates_hidden = 0;
        Gwork.poker_deal.in_round = 0;
        if (SaveWork->coin_balance < PokerCoinRate[Gwork.poker_deal.level]) {
            PoSeqChange(POKER_SEQ_NO_COINS);
            break;
        }
        PokerReadyMes();
        sel = PoDialog(0);
        if (init_99 != 0) {
            PoCardDraw();
        }
        if (sel == 0) {
            break;
        }
        if (sel == 1) {
            init_99 = sel;
            PoSeqChange(POKER_SEQ_BET);
            Gwork.poker_deal.pot = 0;
            PoResetCard();
            break;
        }
        GameModeChange(CASINO_MENU);
        init_99 = 0;
        RES_SoundEffect(CASINO_SE_POKER_LEAVE);
        break;
    case POKER_SEQ_NO_COINS:
        if (init_99 != 0) {
            PoCardDraw();
            xglPacketTextureTrans(POKER_TABLE2_TEX_BUFFER);
            SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
        }
        PoWindowMessage(pokerNoMoreCoinsText, 0x18);
        if (PadData.canonical.half_2a != 0) {
            GameModeChange(CASINO_MENU);
            init_99 = 0;
            RES_SoundEffect(CASINO_SE_POKER_LEAVE);
        }
        break;
    case POKER_SEQ_BET:
        if (Gwork.poker_deal.pot == PokerCoinRate[Gwork.poker_deal.level]) {
            Gwork.poker_deal.in_round = 1;
            PoSeqChange(POKER_SEQ_DEAL);
            break;
        }
        switch (Gwork.poker_deal.level) {
        case 2:
            step = 5;
            break;
        case 3:
            step = 10;
            break;
        default:
            step = 1;
            break;
        }
        SaveWork->coin_balance -= step;
        Gwork.poker_deal.pot += step;
        break;
    case POKER_SEQ_DEAL:
        Gwork.poker_deal.rates_hidden = 0;
        if (PoCardDrawAnime() != 0) {
            PoSeqChange(POKER_SEQ_DRAW_HAND);
        }
        break;
    case POKER_SEQ_DRAW_HAND:
        if (PoShuffleCard() == 1) {
            for (i = 0; i < 5; i++) {
                Gwork.poker_deal.replace_selected[i] = 1;
            }
            PoSeqChange(POKER_SEQ_SELECT);
        }
        PoCardDraw();
        break;
    case POKER_SEQ_SELECT:
        PoCardDraw();
        PoSelectMarkDraw();
        if (PoKey() == 1) {
            PoSeqChange(POKER_SEQ_REPLACE);
            PoCardFlip();
        }
        break;
    case POKER_SEQ_REPLACE:
        if (PoShuffleCard() == 1) {
            PoSeqChange(POKER_SEQ_SCORE);
        }
        PoCardDraw();
        break;
    case POKER_SEQ_SCORE:
        Gwork.poker_deal.hand_rank = PoCardCheck();
        if (Gwork.poker_deal.hand_rank > 0) {
            Gwork.poker_deal.pot *= PokerRate[Gwork.poker_deal.hand_rank];
            PoSeqChange(POKER_SEQ_WIN);
        } else {
            PoSeqChange(POKER_SEQ_READY);
        }
        PoCardDraw();
        break;
    case POKER_SEQ_CONTINUE:
        Gwork.poker_deal.rates_hidden = 1;
        PoWindowMessage(pokerContinueGameText, 10);
        sel = PoDialog(0);
        if (sel != 0) {
            if (sel == 1) {
                PoSeqChange(POKER_SEQ_DEAL);
                PoResetCard();
                break;
            }
            GameModeChange(CASINO_MENU);
            init_99 = 0;
        }
        PoCardDraw();
        break;
    case POKER_SEQ_WIN:
        PoSeqChange(POKER_SEQ_DOUBLE_UP);
        /* fall through */
    case POKER_SEQ_DOUBLE_UP:
        Gwork.poker_deal.bonus_round = 0;
        Gwork.poker_deal.rates_hidden = 1;
        PoRateTableDraw();
        PoResultLamp(Gwork.poker_deal.hand_rank - 1);
        PoWindowMessage(pokerDoubleOrNothingText, 0x11);
        sel = PoDialog(0);
        if (sel != 0) {
            if (sel == 1) {
                PoSeqChange(POKER_SEQ_BONUS_START);
                break;
            }
            RES_SoundEffect(CASINO_SE_POKER_PAYOUT);
            PoSeqChange(POKER_SEQ_PAYOUT);
        }
        PoCardDraw();
        break;
    case POKER_SEQ_BONUS_START:
        PoResetCard();
        for (i = 0; i < 5; i++) {
            Gwork.poker_deal.cards.hand[i] = 0x36;
            Gwork.poker_deal.replace_selected[i] = 0;
        }
        PoSeqChange(POKER_SEQ_BONUS_DEAL);
        /* fall through */
    case POKER_SEQ_BONUS_DEAL:
        if (PoBonusCardAnime() == 1) {
            PoSeqChange(POKER_SEQ_BONUS_DRAW);
        }
        PoCardDraw();
        PoBonusBGDraw();
        break;
    case POKER_SEQ_BONUS_DRAW:
        if (PoShuffleCard() == 1) {
            PoSeqChange(POKER_SEQ_BONUS_JUDGE);
        }
        PoBonusBGDraw();
        PoCardDraw();
        break;
    case POKER_SEQ_BONUS_JUDGE:
        if (Gwork.poker_deal.bonus_round == 0) {
            Gwork.poker_deal.bonus_round = 1;
            PoSeqChange(POKER_SEQ_BONUS_GUESS);
        } else {
            previous_rank = Gwork.poker_deal.cards.hand[Gwork.poker_deal.bonus_round - 1];
            previous_rank -= PoGetBase(PoGetMark(previous_rank));
            current_rank = Gwork.poker_deal.cards.hand[Gwork.poker_deal.bonus_round];
            current_rank -= PoGetBase(PoGetMark(current_rank));
            if ((current_rank < previous_rank) == Gwork.poker_deal.guess
                || previous_rank == current_rank) {
                RES_SoundEffect(CASINO_SE_POKER_WIN);
                Gwork.poker_deal.bonus_round++;
                Gwork.poker_deal.pot *= 2;
                if (Gwork.poker_deal.bonus_round != 5) {
                    PoSeqChange(POKER_SEQ_BONUS_GUESS);
                } else {
                    PoSeqChange(POKER_SEQ_PAYOUT);
                }
            } else {
                Gwork.poker_deal.bonus_round = 0;
                Gwork.poker_deal.pot = 0;
                RES_SoundEffect(CASINO_SE_POKER_NO_WIN);
                PoSeqChange(POKER_SEQ_BONUS_LOST);
            }
        }
        PoBonusBGDraw();
        PoCardDraw();
        break;
    case POKER_SEQ_BONUS_LOST:
        PoBonusBGDraw();
        PoCardDraw();
        if (PadData.canonical.half_2a & PAD_CIRCLE) {
            PoSeqChange(POKER_SEQ_READY);
        }
        break;
    case POKER_SEQ_BONUS_GUESS:
        PoBonusBGDraw();
        PoCardDraw();
        sel = PoDialog(1);
        if (sel != 0) {
            if (sel == 3 || sel == -1) {
                PoSeqChange(POKER_SEQ_PAYOUT);
                RES_SoundEffect(CASINO_SE_POKER_PAYOUT);
                break;
            }
            if (sel == 1) {
                Gwork.poker_deal.guess = 0;
            } else {
                Gwork.poker_deal.guess = 1;
            }
            PoSeqChange(POKER_SEQ_BONUS_DEAL);
        }
        break;
    case POKER_SEQ_PAYOUT:
        if (Gwork.poker_deal.bonus_round != 0) {
            PoBonusBGDraw();
        } else {
            PoRateTableDraw();
            PoResultLamp(Gwork.poker_deal.hand_rank - 1);
        }
        PoCardDraw();
        if (PadData.canonical.half_2a & PAD_CIRCLE) {
            AddCoin(Gwork.poker_deal.pot);
            PoSeqChange(POKER_SEQ_READY);
            Gwork.poker_deal.in_round = 0;
            RES_SoundEffectStop(CASINO_SE_POKER_PAYOUT);
            break;
        }
        coins = 1;
        switch (Gwork.poker_deal.level) {
        case 1:
            coins = 1;
            break;
        case 2:
            coins = 5;
            break;
        case 3:
            coins = 10;
            break;
        case 4:
            coins = 100;
            break;
        }
        if (Gwork.poker_deal.pot >= coins) {
            Gwork.poker_deal.pot -= coins;
            AddCoin(coins);
        } else {
            AddCoin(Gwork.poker_deal.pot);
            Gwork.poker_deal.pot = 0;
        }
        if (Gwork.poker_deal.pot == 0) {
            Gwork.poker_deal.in_round = 0;
            RES_SoundEffectStop(CASINO_SE_POKER_PAYOUT);
            PoSeqChange(POKER_SEQ_READY);
        }
        break;
    }
    SetDrawStatus(3, 0);
    xglPacketTextureTrans(POKER_TABLE2_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    if (Gwork.poker_deal.rates_hidden == 0) {
        PoRateTableDraw();
    }
    if (Gwork.poker_deal.in_round == 0) {
        PoMoneyPrint(0, SaveWork->coin_balance);
    } else {
        PoMoneyPrint(1, Gwork.poker_deal.pot);
    }
}

/* ov11:0x00a06c08. Resets five poker words before a session (its only
 * caller is GameModeChange): the sequence state, rates_hidden, bonus_mode,
 * bonus_round, and the word at 0x180, which nothing else in the TU reads
 * or writes. */
static void PoInitWork(void)
{
    Gwork.poker_deal.unknown_180 = 0;
    Gwork.poker_deal.rates_hidden = 0;
    Gwork.poker_deal.bonus_mode = 0;
    Gwork.poker_deal.bonus_round = 0;
    Gwork.poker_deal.seq = POKER_SEQ_READY;
}

/* ov11:0x00a06c28. Loads the six poker minigame textures (four suits plus
 * two poker-table backgrounds; the "data\\tanaka\\*.xtx" paths are these
 * D_00A0Cxxx strings themselves) into fixed EE main RAM staging addresses. */
static const char pokerSpadeTexturePath[] = "data\\tanaka\\spade.xtx";
static const char pokerCloverTexturePath[] = "data\\tanaka\\clover.xtx";
static const char pokerHeartTexturePath[] = "data\\tanaka\\heart.xtx";
static const char pokerDiamondTexturePath[] = "data\\tanaka\\dia.xtx";
static const char pokerTable1TexturePath[] = "data\\tanaka\\poker_1.xtx";
static const char pokerTable2TexturePath[] = "data\\tanaka\\poker_2.xtx";

static void PokerInit(void)
{
    xglCdReadFile(pokerSpadeTexturePath, POKER_SPADE_TEX_BUFFER, 0, 0);
    xglCdReadFile(pokerCloverTexturePath, POKER_CLOVER_TEX_BUFFER, 0, 0);
    xglCdReadFile(pokerHeartTexturePath, POKER_HEART_TEX_BUFFER, 0, 0);
    xglCdReadFile(pokerDiamondTexturePath, POKER_DIA_TEX_BUFFER, 0, 0);
    xglCdReadFile(pokerTable1TexturePath, POKER_TABLE1_TEX_BUFFER, 0, 0);
    xglCdReadFile(pokerTable2TexturePath, POKER_TABLE2_TEX_BUFFER, 0, 0);
}

/* ov11:0x00a06ce0. Switches the casino to another game and resets that
 * game's state. The menu clears the bet level; the nine-game runs
 * init_ninegame; poker runs PoInitWork. For the slot, unless it is coming
 * back from the nine-game it clears the per-line stakes and turns the help
 * screen on; it always clears the reel positions and states, the pointer,
 * state, total stake, the word at 0x40, double pay, the bonus flag and the
 * row. Finally it stores the new game in Gwork.modes.game. */
static void GameModeChange(int mode)
{
    int i;

    switch (mode) {
    case CASINO_MENU:
        Gwork.modes.level = 0;
        break;
    case CASINO_NINE_GAME:
        init_ninegame();
        break;
    case CASINO_SLOT:
        if (Gwork.modes.game != CASINO_NINE_GAME) {
            for (i = 0; i < 5; i++) {
                Gwork.slot.line_bet[i] = 0;
                Gwork.slot.line_pay[i] = 0;
            }
            Gwork.slot.help = 1;
        }
        for (i = 0; i < 3; i++) {
            Gwork.slot.reel_pos[i] = 0;
            Gwork.slot.reel_state[i] = 0;
        }
        Gwork.slot.pointer = 0;
        Gwork.slot.state = 0;
        Gwork.slot.total_bet = 0;
        Gwork.slot.unknown_40 = 0;
        Gwork.slot.double_pay = 0;
        Gwork.slot.bonus = 0;
        Gwork.slot.row = 0;
        break;
    case CASINO_POKER:
        PoInitWork();
        break;
    case CASINO_COIN_EXCHANGE:
    case CASINO_SHOP:
    case CASINO_VIEWER:
        break;
    }
    Gwork.modes.game = mode;
}

/* ov11:0x00a06df0. Draws the submenu window sprite as a fixed 0xB8 x 0x70
 * rectangle with its top-left corner at (x, y). packet itself is not read
 * here; level_select and exchange_select forward it on to submenu_select
 * right after calling sub_window. */

static void sub_window(XglPacket *packet, int x, int y)
{
    struct SpriteRect rect;

    SetDrawStatus(1, 0);
    rect.x = x;
    rect.y = y;
    rect.w = 0xB8;
    rect.h = 0x70;
    MakeSprite(&SubWindow_Tex, &rect);
}


/* ov11:0x00a06e50. Draws a submenu and handles its input: the window
 * (sub_window), one 0x48 x 0x10 icon per choice in a column at x + 0x34,
 * starting at y + 0xC and 0x18 apart, and an animated 0x10 x 0x10 cursor
 * (Cursor_Tex, 8 frames by anm_100) just left of the selected icon. Up and
 * down on the auto-repeat pad word move *selected with wraparound (sounds 5
 * and 6). Returns 1 when circle is pressed, else 0. */
static int submenu_select(XglPacket *packet, int x, int y, int iconCount,
                          const struct SpriteTex *icons, int *selected)
{
    struct SpriteRect rect;
    struct SpriteRect cursor;
    const struct SpriteTex *icon;
    int cursor_y;
    int i;

    sub_window(packet, x, y);
    SetDrawStatus(1, 0);
    cursor_y = 0;
    rect.x = x + 0x34;
    rect.w = 0x48;
    rect.h = 0x10;
    icon = icons;
    for (i = 0; i < iconCount; i++) {
        rect.y = y + 0xC + i * 0x18;
        if (i == *selected) {
            cursor_y = rect.y;
        }
        MakeSprite(&icon[i], &rect);
    }
    cursor.x = rect.x - 0x10;
    cursor.y = cursor_y;
    cursor.h = 0x10;
    cursor.w = 0x10;
    MakeSprite(&Cursor_Tex[anm_100 % 8], &cursor);
    anm_100++;
    if (PadData.wide.repeat & PAD_UP) {
        (*selected)--;
        RES_SoundEffect(CASINO_SE_SUBMENU_UP);
    }
    if (PadData.wide.repeat & PAD_DOWN) {
        (*selected)++;
        RES_SoundEffect(CASINO_SE_SUBMENU_DOWN);
    }
    if (*selected > iconCount - 1) {
        *selected = 0;
    }
    if (*selected < 0) {
        *selected = iconCount - 1;
    }
    if (PadData.canonical.half_2a & PAD_CIRCLE) {
        return 1;
    }
    return 0;
}

/* ov11:0x00a07000. Draws a four-choice submenu bound to the shared game
 * level selection, with the window's top-left corner at (x, y). sub_window
 * does not read packet, so it only reaches submenu_select. level_select's
 * own tail call (`j submenu_select`) is why its return value is
 * submenu_select's, and menu_mode branches on it (`beqz $2` right after
 * `jal level_select`). */
static int level_select(XglPacket *packet, int x, int y)
{
    sub_window(packet, x, y);
    return submenu_select(packet, x, y, 4, Level_Tex, &Gwork.modes.level);
}

/* ov11:0x00a07060. Draws a three-choice submenu bound to the shared exchange
 * mode selection, the same sub_window()+submenu_select() shape level_select
 * uses above; exchange_select's own tail call (`j submenu_select`) is why its
 * return value is submenu_select's. */
static int exchange_select(XglPacket *packet, int x, int y)
{
    sub_window(packet, x, y);
    return submenu_select(packet, x, y, 3, ExChange_Tex, &Gwork.modes.exchange_choice);
}



/* ov11:0x00a070c0. Casino menu: draws the background, then in state 0 moves
 * the cursor over the four entries with the d-pad and picks one with
 * circle; state 2 shows the exchange submenu (entry 0), a bet level submenu
 * (entries 1 and 2) or leaves the menu (entry 3). Returns 1 on leaving. */
static int menu_mode(void)
{
    XglPacket **slot = &Pkt;

    *slot = xglPacketGetCurrent();
    xglPacketTextureTrans(MENU_BG_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    SetDrawStatus(3, 0);
    SetTest(CASINO_TEST_DEPTH_GEQUAL);
    MakeSprite(&EntryBG_Tex, &EntryBG_Rect);
    xglPacketTextureTrans(MENU_WINDOW_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    switch (menu_mode_103) {
    case 0:
        if (PadData.canonical.half_2a & PAD_UP) {
            curpos_101 = 0;
            RES_SoundEffect(CASINO_SE_MENU_UP);
        }
        if (PadData.canonical.half_2a & PAD_LEFT) {
            curpos_101 = 1;
            RES_SoundEffect(CASINO_SE_MENU_LEFT);
        }
        if (PadData.canonical.half_2a & PAD_RIGHT) {
            curpos_101 = 2;
            RES_SoundEffect(CASINO_SE_MENU_RIGHT);
        }
        if (PadData.canonical.half_2a & PAD_DOWN) {
            curpos_101 = 3;
            RES_SoundEffect(CASINO_SE_MENU_DOWN);
        }
        if (PadData.canonical.half_2a & PAD_CIRCLE) {
            menu_mode_103 = 2;
            RES_SoundEffect(CASINO_SE_MENU_DECIDE);
        }
        MakeSprite(&Cursor_Tex[cur_int_102 % 8], &Cursor_Rect[curpos_101]);
        break;
    case 1:
        break;
    case 2:
        switch (curpos_101) {
        case 0:
            if (exchange_select(Pkt, 151, 117)) {
                switch (Gwork.modes.exchange_choice) {
                case 0:
                    GameModeChange(CASINO_COIN_EXCHANGE);
                    RES_SoundEffect(CASINO_SE_ENTER_EXCHANGE);
                    break;
                case 1:
                    GameModeChange(CASINO_SHOP);
                    RES_SoundEffect(CASINO_SE_ENTER_SHOP);
                    break;
                case 2:
                    GameModeChange(CASINO_VIEWER);
                    RES_SoundEffect(CASINO_SE_ENTER_VIEWER);
                    break;
                }
            }
            break;
        case 1:
            if (level_select(Pkt, 33, 243)) {
                GameModeChange(CASINO_SLOT);
                RES_SoundEffect(CASINO_SE_ENTER_SLOT);
                menu_mode_103 = 0;
            }
            break;
        case 2:
            if (level_select(Pkt, 325, 243)) {
                GameModeChange(CASINO_POKER);
                RES_SoundEffect(CASINO_SE_ENTER_POKER);
                menu_mode_103 = 0;
            }
            break;
        case 3:
            return 1;
        }
        if (PadData.canonical.half_2a & (PAD_LEFT | PAD_RIGHT | PAD_CROSS)) {
            RES_SoundEffect(CASINO_SE_MENU_CANCEL);
            menu_mode_103 = 0;
        }
        break;
    }
    cur_int_102++;
    return 0;
}

/* ov11:0x00a07410. Loads the menu background and window textures, then
 * clears the bet level (Gwork.modes.level) and the short at 0x10 (not
 * sure what that one holds). */
static const char menuBackgroundTexturePath[] = "data\\tanaka\\BG.xtx";
static const char menuWindowTexturePath[] = "data\\tanaka\\window.xtx";

static void init_menu_mode(void)
{
    xglCdReadFile(menuBackgroundTexturePath, MENU_BG_TEX_BUFFER, 0, 0);
    xglCdReadFile(menuWindowTexturePath, MENU_WINDOW_TEX_BUFFER, 0, 0);

    Gwork.modes.level = 0;
    Gwork.modes.unknown_10 = 0;
}

/* ov11:0x00a07470. Loads the casino test-mode background texture
 * ("data\\tanaka\\base.xtx", baseTexturePath above). */


static void init_test_mode(void)
{
    xglCdReadFile(baseTexturePath, TEST_MODE_BASE_TEX_BUFFER, 0, 0);
}

/* ov11:0x00a074a0. Lazily loads the test-mode background texture on first
 * entry (flg_104 latches this), then selects it and draws the fixed
 * test-mode background sprite. xglPacketGetCurrent is main:0x0022c450
 * (src/main/xgl_packet.c). */

static void test_mode(void)
{
    if (flg_104 == 0) {
        init_test_mode();
        flg_104 = 1;
    }
    Pkt = xglPacketGetCurrent();
    xglPacketTextureTrans(TEST_MODE_BASE_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    SetDrawStatus(3, 0);
    MakeSprite(&BGTEST_Tex, &BGTEST_Rect);
}


/* ov11:0x00a07528. Prints value like decprint with 14x24 digit sprites
 * from tex_105, stepping left by 14 per digit; a nonzero flag adds the
 * eleventh sprite of the table, the one past the digits, 16 pixels right
 * of x. */
static void DecPrint2(int value, int digits, int x, int y, int flag)
{
    struct SpriteRect rect;
    double number;
    unsigned long long power;
    int i;

    if (digits < 0) {
        digits = 0;
        while (1) {
            if (digits < 11) {
                number = (double)value;
                if (dpcmp(number, pow(DOUBLE_TEN, litodp(digits))) < 0) {
                    break;
                }
                digits++;
            } else {
                break;
            }
        }
    }
    rect.w = 14;
    rect.h = 24;
    if (digits > 0) {
        for (i = 0; i < digits; ) {
            rect.x = x - i * 14;
            rect.y = y;
            power = pow(DOUBLE_TEN, litodp(i++));
            /* Empty loop: GCC 2.96 does not schedule across loop notes, which keeps
             * the pow result copy ahead of the next conversion argument. */
            do { } while (0);
            MakeSprite(&tex_105[dptoli(dpdiv((double)value, power)) % 10], &rect);
        }
    } else {
        rect.x = x;
        rect.y = y;
        MakeSprite(tex_105, &rect);
    }
    if (flag != 0) {
        rect.x = x + 16;
        rect.y = y;
        MakeSprite(&tex_105[10], &rect);
    }
}

static void UtlCurPrint(int x, int y)
{
    struct SpriteRect rect;
    int n;
    xglPacketTextureTrans(MENU_WINDOW_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    rect.h = 24;
    rect.w = 24;
    rect.x = x;
    rect.y = y;
    n = anm_107;
    MakeSprite(&Tex_106[n % 8], &rect);
    anm_107 = anm_107 + 1;
}




/* ov11:0x00a077a0. Coin-exchange screen, one frame per call. Draws the
 * background, the price list (RES_Coin_Info) and labels, moves the cursor
 * (cur_108, five rows) with up/down on the auto-repeat word, and when
 * circle is pressed or R1 repeats (one doubleword test) it trades one money
 * box for 10, 100, 500 or 1000 coins on rows 0-3. If the balance is already
 * 9999999 or more, the box is put back. Circle on the last row, or cross,
 * leaves to the menu. Then it draws the cursor and the money-box and coin
 * totals. */
static void coin_main(void)
{
    int leave;

    Pkt = xglPacketGetCurrent();
    xglPacketTextureTrans(COIN_EXCHANGE_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    SetDrawStatus(3, 0);
    MakeSprite(&BG_Tex, &BG_Rect);
    xglFontPrint(0, 0, 0xFF00, coinExchangeTitleFontControl);
    xglFontPrint(0xD2, 0x20, 0xFF00, coinExchangeBuyCoinsText);
    xglFontPrint(0, 0, 0xFF00, coinExchangeAfterTitleFontControl);
    xglFontPrint(0, 0, 0xFF00, coinExchangeListFontControl);
    xglFontPrint(0x104, 0x40, 0xFF00, RES_Coin_Info(0));
    xglFontPrint(0x104, 0x58, 0xFF00, RES_Coin_Info(1));
    xglFontPrint(0x104, 0x70, 0xFF00, RES_Coin_Info(2));
    xglFontPrint(0x104, 0x88, 0xFF00, RES_Coin_Info(3));
    xglFontPrint(0, 0, 0, coinExchangeExitFontControl);
    xglFontPrint(0x19A, 0xD0, 0xFF00, coinExchangeExitText);
    xglFontPrint(0, 0, 0xFF00, coinExchangeAfterExitFontControl);
    if (PadData.wide.repeat & PAD_UP) {
        RES_SoundEffect(CASINO_SE_EXCHANGE_UP);
        cur_108--;
    }
    if (PadData.wide.repeat & PAD_DOWN) {
        RES_SoundEffect(CASINO_SE_EXCHANGE_DOWN);
        cur_108++;
    }
    if (cur_108 < 0) {
        cur_108 = 4;
    }
    if (cur_108 >= 5) {
        cur_108 = 0;
    }
    leave = 0;
    if (PadData.canonical.half_2a & PAD_CROSS) {
        leave = 1;
    }
    if (PadData.dword.words & (PAD_WORDS_PRESSED(PAD_CIRCLE) | PAD_WORDS_REPEAT(PAD_R1))) {
        switch (cur_108) {
        case 0:
            if (dataMoneyBoxDec(RES_Coin_Rate(0)) == 1) {
                if (SaveWork->coin_balance < CASINO_COIN_MAX) {
                    AddCoin(10);
                    RES_SoundEffect(CASINO_SE_EXCHANGE_BOUGHT);
                    break;
                }
                dataMoneyBoxInc(RES_Coin_Rate(0));
            }
            RES_SoundEffect(CASINO_SE_EXCHANGE_REFUSED);
            break;
        case 1:
            if (dataMoneyBoxDec(RES_Coin_Rate(1)) == 1) {
                if (SaveWork->coin_balance < CASINO_COIN_MAX) {
                    AddCoin(100);
                    RES_SoundEffect(CASINO_SE_EXCHANGE_BOUGHT);
                    break;
                }
                dataMoneyBoxInc(RES_Coin_Rate(1));
            }
            RES_SoundEffect(CASINO_SE_EXCHANGE_REFUSED);
            break;
        case 2:
            if (dataMoneyBoxDec(RES_Coin_Rate(2)) == 1) {
                if (SaveWork->coin_balance < CASINO_COIN_MAX) {
                    AddCoin(500);
                    RES_SoundEffect(CASINO_SE_EXCHANGE_BOUGHT);
                    break;
                }
                dataMoneyBoxInc(RES_Coin_Rate(2));
            }
            RES_SoundEffect(CASINO_SE_EXCHANGE_REFUSED);
            break;
        case 3:
            if (dataMoneyBoxDec(RES_Coin_Rate(3)) == 1) {
                if (SaveWork->coin_balance < CASINO_COIN_MAX) {
                    AddCoin(1000);
                    RES_SoundEffect(CASINO_SE_EXCHANGE_BOUGHT);
                    break;
                }
                dataMoneyBoxInc(RES_Coin_Rate(3));
            }
            RES_SoundEffect(CASINO_SE_EXCHANGE_REFUSED);
            break;
        default:
            if (PadData.canonical.half_2a & PAD_CIRCLE) {
                leave = 1;
            }
            break;
        }
    }
    UtlCurPrint(curpt_109[cur_108].x, curpt_109[cur_108].y);
    xglPacketTextureTrans(MENU_WINDOW_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    SetDrawStatus(6, 0x60);
    DecPrint2(dataMoneyBoxChk(), -1, 0xE6, 0x104, 0);
    DecPrint2(SaveWork->coin_balance, -1, 0x1D4, 0x104, 0);
    if (leave == 1) {
        cur_108 = 0;
        GameModeChange(CASINO_MENU);
        RES_SoundEffect(0x12);
    }
}

/* ov11:0x00a07c38. Loads the casino coin-exchange minigame's background and
 * exchange textures (baseTexturePath is the same "base.xtx" path init_test_mode
 * uses, into a different fixed buffer address here). */
static const char coinExchangeTexturePath[] = "data\\tanaka\\exchange2.xtx";


static void init_coin(void)
{
    xglCdReadFile(baseTexturePath, BASE_TEX_BUFFER, 0, 0);
    xglCdReadFile(coinExchangeTexturePath, COIN_EXCHANGE_TEX_BUFFER, 0, 0);
}

/* ov11:0x00a07c80. Loads the casino shop's exchange and background textures
 * (baseTexturePath "base.xtx" again, into the same fixed buffer init_coin uses). */
static const char shopExchangeTexturePath[] = "data\\tanaka\\exchange1.xtx";


static void init_shop(void)
{
    xglCdReadFile(shopExchangeTexturePath, SHOP_EXCHANGE_TEX_BUFFER, 0, 0);
    xglCdReadFile(baseTexturePath, BASE_TEX_BUFFER, 0, 0);
}


static void init_shpmenu(void)
{
    int *entry;
    int i;
    int enabled;

    entry = shpmenu_work;
    MenuNum = 0;
    for (i = 0; i < 51; i++) {
        enabled = 0;
        switch (i) {
        case 0:
        case 1:
            enabled = 1;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
        case 35:
            if (SaveWork->prize_bought[i] == 0) {
                enabled = 1;
            }
            break;
        case 36:
        case 37:
            if (xglFlagsGet1(115) == 1) {
                enabled = 1;
            }
            break;
        case 38:
            if (xglFlagsGet1(301) == 1) {
                enabled = 1;
            }
            break;
        case 39:
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:
        case 47:
        case 48:
        case 49:
            if (xglFlagsGet1(389) == 1 && SaveWork->prize_bought[i] == 0) {
                enabled = 1;
            }
            break;
        case 50:
            if (xglFlagsGet1(391) == 1 && SaveWork->prize_bought[i] == 0) {
                enabled = 1;
            }
            break;
        }
        if (enabled == 1) {
            *entry = i;
            MenuNum++;
            entry = shpmenu_work + MenuNum;
        }
    }
}


#define SHOP_PRIZE_COUNT 8

static void shop_main(void)
{
    int prize;
    int box_id;
    int item;
    int shown;
    int i;
    int *init;

    Pkt = xglPacketGetCurrent();
    xglPacketTextureTrans(SHOP_EXCHANGE_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    {
        float current = *(float *)&Pkt;
        *(float *)&Pkt = current;
    }
    init = &init_110;
    SetDrawStatus(3, 0);
    {
        int done = *init;
        *init = done;
    }
    MakeSprite(&BG_Tex, &BG_Rect);
    xglPacketTextureTrans(MENU_WINDOW_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    DecPrint2(SaveWork->coin_balance, -1, 468, 260, 0);
    if (*init == 0) {
        init_shpmenu();
        *init = 1;
    }
    shown = 0;
    i = 0;
    if (i < MenuNum) {
        do {
            if (i >= top_112 && shown != SHOP_PRIZE_COUNT) {
                item = shpmenu_work[i];
                xglFontPrint(0, 0, 0, fontModeControl);
                xglFontPrint(0, 0, 0, shopItemNameFontControl);
                xglFontPrint(240, shown * 24 + 32, 0xFF00, RES_ShopDataName(item));
                shown++;
            }
            i++;
        } while (i < MenuNum);
    }
    SetDrawStatus(3, 0);
    UtlCurPrint(216, cur_111 * 24 + 34);
    if (PadData.wide.repeat & PAD_UP) {
        RES_SoundEffect(19);
        if (cur_111 == 0) {
            top_112--;
        } else {
            cur_111--;
        }
    }
    if (PadData.wide.repeat & PAD_DOWN) {
        RES_SoundEffect(20);
        if (cur_111 + top_112 < MenuNum - 1 && cur_111 == SHOP_PRIZE_COUNT - 1) {
            top_112++;
        } else {
            cur_111++;
        }
    }
    if (top_112 + cur_111 >= MenuNum) {
        cur_111 = MenuNum - top_112 - 1;
    }
    if (top_112 < 0) {
        top_112 = 0;
    }
    if (MenuNum < cur_111) {
        cur_111 = MenuNum - 1;
    }
    prize = shpmenu_work[top_112 + cur_111];
    SetDrawStatus(0, 0);
    DecPrint2(RES_ShopDataCoin(prize), -1, 128, 260, 1);
    xglFontPrint(0, 0, 0xFF00, fontModeControl);
    xglFontPrint(0, 0, 0, infoFontControl);
    xglFontPrint(32, 310, 0xFF00, RES_ShopDataInfo(prize));
    if (PadData.dword.words & PAD_WORDS_REPEAT(PAD_CIRCLE | PAD_R1)) {
        if (RES_ShopDataCoin(prize) <= SaveWork->coin_balance) {
        SaveWork->coin_balance -= RES_ShopDataCoin(prize);
        if (prize >= 2 && prize <= 50) {
            SaveWork->prize_bought[prize] = 1;
            init_110 = 0;
            if (shown == 1) {
                top_112--;
            }
        }
        switch (prize) {
        case 0:
            if (dataItmBoxChk(1) == 99 && dataItmBoxChk(5) == 99 && dataItmBoxChk(10) == 99 && dataItmBoxChk(12) == 99) {
                RES_SoundEffect(CASINO_SE_SHOP_REFUSED);
                SaveWork->coin_balance += RES_ShopDataCoin(prize);
            } else {
                RES_SoundEffect(CASINO_SE_SHOP_BOUGHT);
                dataItmBoxInc(1);
                dataItmBoxInc(5);
                dataItmBoxInc(10);
                dataItmBoxInc(12);
            }
            break;
        case 1:
            if (dataItmBoxChk(16) == 99 && dataItmBoxChk(36) == 99) {
                RES_SoundEffect(CASINO_SE_SHOP_REFUSED);
                SaveWork->coin_balance += RES_ShopDataCoin(prize);
            } else {
                dataItmBoxInc(16);
                dataItmBoxInc(36);
                RES_SoundEffect(CASINO_SE_SHOP_BOUGHT);
            }
            break;
        case 2:
            if (dataAccBoxInc(100)) {
                RES_SoundEffect(CASINO_SE_SHOP_BOUGHT);
            } else {
                RES_SoundEffect(CASINO_SE_SHOP_REFUSED);
                SaveWork->coin_balance += RES_ShopDataCoin(prize);
            }
            break;
        case 3:
            if (dataAccBoxInc(109)) {
                RES_SoundEffect(CASINO_SE_SHOP_BOUGHT);
            } else {
                RES_SoundEffect(CASINO_SE_SHOP_REFUSED);
                SaveWork->coin_balance += RES_ShopDataCoin(prize);
            }
            break;
        case 4:
            if (dataItmBoxInc(11)) {
                RES_SoundEffect(CASINO_SE_SHOP_BOUGHT);
            } else {
                RES_SoundEffect(CASINO_SE_SHOP_REFUSED);
                SaveWork->coin_balance += RES_ShopDataCoin(prize);
            }
            break;
        case 5:
            if (dataItmBoxInc(18)) {
                RES_SoundEffect(CASINO_SE_SHOP_BOUGHT);
            } else {
                RES_SoundEffect(CASINO_SE_SHOP_REFUSED);
                SaveWork->coin_balance += RES_ShopDataCoin(prize);
            }
            break;
        case 36:
        case 37:
        case 38:
            box_id = prize + 17;
            goto event;
        case 39:
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:
        case 47:
        case 48:
        case 49:
        case 50:
            box_id = prize + 2;
        event:
            if (dataEvtBoxInc(box_id)) {
                RES_SoundEffect(CASINO_SE_SHOP_BOUGHT);
            } else {
                RES_SoundEffect(CASINO_SE_SHOP_REFUSED);
                SaveWork->coin_balance += RES_ShopDataCoin(prize);
            }
            break;
        default:
            RES_SoundEffect(CASINO_SE_SHOP_BOUGHT);
            break;
        }
        } else {
            RES_SoundEffect(CASINO_SE_SHOP_REFUSED);
        }
    }
    if (PadData.canonical.half_2a & PAD_CROSS) {
        top_112 = 0;
        cur_111 = 0;
        init_110 = 0;
        GameModeChange(CASINO_MENU);
        RES_SoundEffect(23);
    }
}

/* ov11:0x00a083f8. Loads the data-viewer minigame's dataviewer and sampler
 * textures. */
static const char viewerDataViewerTexturePath[] = "data\\tanaka\\dataviewer.xtx";
static const char viewerSampleTexturePath[] = "data\\tanaka\\sam.xtx";


static void VW_Init(void)
{
    xglCdReadFile(viewerDataViewerTexturePath, VW_DATAVIEWER_TEX_BUFFER, 0, 0);
    xglCdReadFile(viewerSampleTexturePath, VW_SAM_TEX_BUFFER, 0, 0);
}

/* Start of Gwork.viewer.sample (word 114) and a zero count, for
 * VW_SamlistInit. Measured: they have to come out of this helper (the
 * original's inline function), because written in line the count and the
 * loop index become one shared zero and the pointer setup is rescheduled. */
static __inline void sampler_offset(int **base, int *count)
{
    *base += 114;
    *count = 0;
}

/* Build the compact list of the 30 sampler slots enabled in SaveWork. */
static int VW_SamlistInit(void)
{
    int count;
    int index;
    char *enabled;
    int *sampler;

    sampler = Gwork.words;
    sampler_offset(&sampler, &count);
    enabled = SaveWork->prize_bought;
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


/* Viewer image pages: VW_ViewModeInit loads NN_Y1/Y2.xtx into the first
 * two and NN_W1..W4.xtx into all four. */

/* ov11:0x00a08498. Viewer: when the selected entry changes, builds its
 * file names (data\\tanaka\\NN_Y1/Y2.xtx for type 0, NN_W1..W4.xtx
 * otherwise, with the type taken from fileinfo) and loads them into fixed
 * buffers, then resets the scroll offsets (Gwork.viewer). Returns the entry's type.
 * Not sure what the Y and W image sets are. */
static int VW_ViewModeInit(int index)
{
    char path[256];
    int info;

    info = fileinfo[index];
    if (index != old_113) {
        if (info == 0) {
            sprintf(path, viewerImageY1PathFormat, tanakaDirectoryPath, index + 1);
            xglCdReadFile(path, VW_IMAGE_1_BUFFER, 0, 0);
            sprintf(path, viewerImageY2PathFormat, tanakaDirectoryPath, index + 1);
            xglCdReadFile(path, VW_IMAGE_2_BUFFER, 0, 0);
        } else {
            sprintf(path, viewerImageW1PathFormat, tanakaDirectoryPath, index + 1);
            xglCdReadFile(path, VW_IMAGE_1_BUFFER, 0, 0);
            sprintf(path, viewerImageW2PathFormat, tanakaDirectoryPath, index + 1);
            xglCdReadFile(path, VW_IMAGE_2_BUFFER, 0, 0);
            sprintf(path, viewerImageW3PathFormat, tanakaDirectoryPath, index + 1);
            xglCdReadFile(path, VW_IMAGE_3_BUFFER, 0, 0);
            sprintf(path, viewerImageW4PathFormat, tanakaDirectoryPath, index + 1);
            xglCdReadFile(path, VW_IMAGE_4_BUFFER, 0, 0);
        }
        Gwork.viewer.scroll_y = 0;
        Gwork.viewer.scroll_x = 0;
    }
    old_113 = index;
    return info;
}


/* ov11:0x00a08650. Viewer image display: loads the entry's images with
 * VW_ViewModeInit, draws them as a 2x2 grid of 578-wide pages at the scroll
 * offsets (the second row is 448 high, one page when the type is not 1 is
 * shown only as the first two), scrolls with the larger of the two stick
 * axes (the x offset only when the entry type is 1), clamps the offsets,
 * then draws the key sprite. Returns 1 when cross is newly pressed (sound
 * 0x1C), else 0. Not sure which stick bytes are which axis. */
static int VW_ViewMode(int arg)
{
    struct SpriteRect rect[4];
    int type;
    int dx;
    signed char dy;
    int ax;
    int ay;
    int bx;
    int by;
    signed char abs_ax;
    signed char abs_ay;
    signed char abs_bx;
    signed char abs_by;
    signed char by_char;

    type = VW_ViewModeInit(arg);
    rect[3].w = 578;
    rect[2].w = 578;
    rect[1].w = 578;
    rect[0].w = 578;
    rect[2].h = 512;
    rect[3].h = 448;
    rect[0].h = 512;
    rect[1].h = 448;
    rect[0].x = Gwork.viewer.scroll_x;
    rect[1].x = Gwork.viewer.scroll_x;
    rect[2].y = Gwork.viewer.scroll_y;
    rect[0].y = Gwork.viewer.scroll_y;
    rect[3].y = Gwork.viewer.scroll_y + 512;
    rect[1].y = Gwork.viewer.scroll_y + 512;
    rect[2].x = Gwork.viewer.scroll_x + 578;
    rect[3].x = Gwork.viewer.scroll_x + 578;
    SetDrawStatus(3, 0);
    xglPacketTextureTrans(VW_IMAGE_1_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    MakeSprite(&tex_114, &rect[0]);
    xglPacketTextureTrans(VW_IMAGE_2_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    MakeSprite(&tex_114, &rect[1]);
    if (type == 1) {
        xglPacketTextureTrans(VW_IMAGE_3_BUFFER);
        SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
        MakeSprite(&tex_114, &rect[2]);
        xglPacketTextureTrans(VW_IMAGE_4_BUFFER);
        SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
        MakeSprite(&tex_114, &rect[3]);
    }
    ax = PadData.wide.stick[0] / 2;
    abs_ax = ax;
    if (ax < 0) {
        abs_ax = -ax;
    }
    ay = PadData.wide.stick[1] / 2;
    abs_ay = ay;
    if (abs_ay < 0) {
        abs_ay = -abs_ay;
    }
    bx = PadData.wide.stick[2] / 2;
    abs_bx = bx;
    if (bx < 0) {
        abs_bx = -bx;
    }
    by = PadData.wide.stick[3] / 2;
    by_char = by;
    if (by_char < 0) {
        abs_by = -by_char;
    } else {
        abs_by = by_char;
    }
    dy = by;
    if (abs_ay >= abs_by) {
        dy = ay;
    }
    Gwork.viewer.scroll_y -= dy;
    dx = ax;
    if (abs_ax < abs_bx) {
        dx = bx;
    }
    if (type == 1) {
        Gwork.viewer.scroll_x -= dx;
    }
    if (Gwork.viewer.scroll_x < -578) {
        Gwork.viewer.scroll_x = -578;
    }
    if (Gwork.viewer.scroll_x > 0) {
        Gwork.viewer.scroll_x = 0;
    }
    if (Gwork.viewer.scroll_y < -592) {
        Gwork.viewer.scroll_y = -592;
    }
    if (Gwork.viewer.scroll_y > 0) {
        Gwork.viewer.scroll_y = 0;
    }
    xglPacketTextureTrans(VW_DATAVIEWER_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    MakeSprite(&keytex_115, &keyrect_116);
    if (PadData.canonical.half_2a & PAD_CROSS) {
        RES_SoundEffect(0x1C);
        return 1;
    }
    return 0;
}


/* ov11:0x00a08960. Viewer sample list. With arg 0 it resets the cursor and
 * scroll offset and returns 0. Otherwise it builds the list of enabled
 * samples once (VW_SamlistInit), draws the background and up to eight
 * titles from the scroll offset, the cursor, and moves the cursor or
 * scroll offset with up/down on the auto-repeat word (sounds 0x18 and
 * 0x19). If the list is not
 * empty it prints the selected sample's title and info and draws its
 * picture, returning the sample id; otherwise it returns -1. Not sure what
 * the font control strings select. */
static int VW_SamMode(int arg)
{
    int i;
    int y;
    int sample;

    if (arg == 0) {
        cur_119 = ofs_120 = init_121 = 0;
        return 0;
    }
    if (init_121 == 0) {
        init_121 = 1;
        cnt_122 = VW_SamlistInit();
    }
    xglPacketTextureTrans(VW_DATAVIEWER_TEX_BUFFER);
    SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
    SetDrawStatus(3, 0);
    MakeSprite(&back_tex_118, &BG_Rect);
    y = 0x14;
    for (i = 0; i < cnt_122; i++) {
        if (i >= ofs_120 && i <= ofs_120 + 7) {
            xglFontPrint(0, 0, 0, fontModeControl);
            xglFontPrint(0, 0, 0, infoFontControl);
            xglFontPrint(0x154, y * 2, 0xFF00, Cajino_GetTitle(Gwork.viewer.sample[i]));
            y += 0xC;
        }
    }
    if (cnt_122 > 0) {
        xglFontPrint(0x140, cur_119 * 24 + 0x2E, 0xFF00, viewerCursorText);
    }
    if (PadData.wide.repeat & PAD_UP) {
        RES_SoundEffect(0x18);
        if (cur_119 != 0) {
            cur_119--;
        } else if (ofs_120 > 0) {
            ofs_120--;
        }
    }
    if (PadData.wide.repeat & PAD_DOWN) {
        RES_SoundEffect(0x19);
        if (cur_119 == 7) {
            if (ofs_120 + cur_119 < cnt_122 - 1) {
                ofs_120++;
            }
        } else if (cur_119 < cnt_122 - 1) {
            cur_119++;
        }
    }
    sample = -1;
    if (cnt_122 > 0) {
        sample = Gwork.viewer.sample[ofs_120 + cur_119];
        xglFontPrint(0, 0, 0, fontModeControl);
        xglFontPrint(0, 0, 0, viewerTitleFontControl);
        xglFontPrint(0x1C, 0x100, 0xFF00, Cajino_GetTitle(sample));
        xglFontPrint(0, 0, 0, viewerInfoFontControl);
        xglFontPrint(0x46, 0x118, 0xFF00, Cajino_GetInfo(sample));
        xglPacketTextureTrans(VW_SAM_TEX_BUFFER);
        SetRegAD(Pkt, SCE_GS_TEXFLUSH, 0);
        MakeSprite(&ViewSamTex[sample], &sam_rect_117);
    }
    return sample;
}


/* ov11:0x00a08ce0. View mode main: switches between VW_ViewMode and
 * VW_SamMode on mode_123; triangle enters the sample list and cross leaves
 * it (calls GameModeChange(CASINO_MENU)). */
static void VW_Main(void)
{
    int result;
    XglPacket **slot = &Pkt;

    *slot = xglPacketGetCurrent();
    if (mode_123 == 1) {
        if (VW_ViewMode(pic_124)) {
            mode_123 = 0;
        }
        return;
    }
    result = VW_SamMode(1);
    pic_124 = result;
    if (result != -1) {
        if (PadData.canonical.half_2a & PAD_TRIANGLE) {
            RES_SoundEffect(0x1A);
            mode_123 = 1;
        }
    }
    if (PadData.canonical.half_2a & PAD_CROSS) {
        VW_SamMode(0);
        GameModeChange(CASINO_MENU);
        RES_SoundEffect(0x1B);
    }
}


/* ov11:0x00a08dc0. Starts a background music request. -1 and a repeat of
 * the previous request (oldreq_126) do nothing. Otherwise it clears the
 * two sound banks, loads the shared sound data, waits for the SPU DMA,
 * then loads the sequence (.smd) and wave (.swd) files for the requested
 * track from MUSICLIST_125 and starts the normal sequence at volume 0x7F.
 * Note the function never updates oldreq_126 itself. Not sure of the exact
 * sound API semantics. */
static void MUSIC_CALL(int musicId)
{
    char path[256];

    if (musicId == -1) {
        return;
    }
    if (oldreq_126 == musicId) {
        return;
    }
    xglSoundSendEffect(0, 0, 1);
    xglSoundSendEffect(0, 0, 2);
    xglCdReadFile(miniSlotSedPath, CASINO_EFFECT_1_BUFFER, 0, 0);
    xglCdReadFile(miniSlotSwdPath, CASINO_EFFECT_2_BUFFER, 0, 0);
    xglSoundSendEffect(CASINO_EFFECT_2_BUFFER, CASINO_EFFECT_1_BUFFER, 1);
    while (SsdSpuDmaCompleted(0) != 0) {
    }
    xglSoundSendEffect(CASINO_EFFECT_2_BUFFER, CASINO_EFFECT_1_BUFFER, 2);
    sprintf(path, musicSmdPathFormat, soundDirectoryPath, MUSICLIST_125[musicId]);
    xglCdReadFile(path, CASINO_MUSIC_SMD_BUFFER, 0, 0);
    xglSoundSendSmd(CASINO_MUSIC_SMD_BUFFER);
    sprintf(path, musicSwdPathFormat, soundDirectoryPath, MUSICLIST_125[musicId]);
    xglCdReadFile(path, CASINO_MUSIC_SWD_BUFFER, 0, 0);
    xglSoundSendSwd(CASINO_MUSIC_SWD_BUFFER, -1);
    while (SsdSpuDmaCompleted(0) != 0) {
    }
    xglSoundSequenceNormal(0x7F);
}

/* ov11:0x00a08f40. Resets the four leading Gwork words and the six
 * per-game initialised flags, loads the casino resource table and starts the
 * background sound queue, applies four debug-mode config-flag unlocks and
 * grants a near-maximum bank balance plus a matching coin total when
 * RES_IsDebugMode() is set, then seeds the slot stake fields (total_bet 5,
 * line_pay 0..4). Its only caller, MiniG_Init, runs this once. */
/* The original materializes Gwork's address once (%lo(Gwork)=0, so
 * `addiu s0,v0,0`) and reaches every field below off that same register
 * with an immediate byte offset, which needs the base held in a pointer to
 * the whole work union. */
static void InitWork(void)
{
    union CasinoWork *work = &Gwork;
    short *slot;
    int value;

    work->modes.initialized[0] = 0;
    work->modes.initialized[1] = 0;
    work->modes.initialized[2] = 0;
    work->modes.initialized[3] = 0;
    work->modes.initialized[4] = 0;
    work->modes.initialized[5] = 0;
    work->words[0] = 0;
    work->words[1] = 0;
    work->words[2] = 0;
    work->words[3] = 0;

    RES_Load();
    MUSIC_CALL(0);

    if (RES_IsDebugMode() != 0) {
        xglFlagsSet1(0x73, 1);
        xglFlagsSet1(0x12D, 1);
        xglFlagsSet1(0x185, 1);
        xglFlagsSet1(0x187, 1);
        dataMoneyBoxInc(999999999);
        AddCoin(9999000);
    }

    work->slot.total_bet = 5;
    slot = &work->slot.line_pay[4];
    value = 4;
    do {
        *slot = value;
        value--;
        slot--;
    } while (value >= 0);
}

/* ov11:0x00a09020. Logs the configured heap address, then delegates all
 * casino state initialization to InitWork. */
static const char workSizeFormat[] = "work size = %x\n";

void MiniG_Init(void)
{
    dprintf(workSizeFormat, 0x7CF000);
    InitWork();
}


/* ov11:0x00a09048. Casino main dispatcher. Gwork.modes.game selects the game:
 * 0 menu (returns 1 when the menu handler asks to leave), 1 slot, 2 poker,
 * 3 coin exchange, 4 shop, 5 viewer, 6 nine-game, 7 test mode. Each game's
 * init routine runs once, guarded by a per-game flag. After a game runs it
 * prints a debug line and returns 0; an unknown mode returns 0 without
 * printing. Not sure of the exact game wording. */
int MiniG_Main(void)
{
    switch (Gwork.modes.game) {
    case CASINO_MENU:
        if (Gwork.modes.initialized[0] == 0) {
            init_menu_mode();
            Gwork.modes.initialized[0] = 1;
        }
        if (menu_mode()) {
            return 1;
        }
        break;
    case CASINO_SLOT:
        if (Gwork.modes.initialized[1] == 0) {
            init_slot();
            Gwork.modes.initialized[1] = 1;
        }
        slot_main();
        break;
    case CASINO_POKER:
        if (Gwork.modes.initialized[2] == 0) {
            PokerInit();
            Gwork.modes.initialized[2] = 1;
        }
        PokerMain();
        break;
    case CASINO_COIN_EXCHANGE:
        if (Gwork.modes.initialized[3] == 0) {
            init_coin();
            Gwork.modes.initialized[3] = 1;
        }
        coin_main();
        break;
    case CASINO_SHOP:
        if (Gwork.modes.initialized[4] == 0) {
            init_shop();
            Gwork.modes.initialized[4] = 1;
        }
        shop_main();
        break;
    case CASINO_VIEWER:
        if (Gwork.modes.initialized[5] == 0) {
            VW_Init();
            Gwork.modes.initialized[5] = 1;
        }
        VW_Main();
        break;
    case CASINO_NINE_GAME:
        NineGameMain();
        break;
    case CASINO_TEST_MODE:
        test_mode();
        break;
    default:
        return 0;
    }
    xglFontDebugPrintf(4, 0, versionDebugFormat, casinoBuildDateText);
    return 0;
}



static short real01array[SLOT_REEL_CELLS] = { 1, 3, 1, 0, 4, 4, 0, 1, 3, 1, 0, 2, 1, 0, 1, 2, 1, 0, 5, 5, 0 };

static short real02array[SLOT_REEL_CELLS] = { 3, 0, 1, 3, 0, 0, 0, 1, 2, 0, 4, 0, 1, 2, 0, 1, 0, 0, 3, 5, 1 };

static short real03array[SLOT_REEL_CELLS] = { 1, 0, 0, 0, 2, 1, 0, 2, 1, 4, 2, 0, 0, 1, 5, 3, 1, 0, 5, 5, 3 };

static short real11array[SLOT_REEL_CELLS] = { 1, 3, 1, 0, 4, 4, 0, 1, 3, 1, 0, 2, 1, 0, 1, 2, 1, 0, 7, 7, 0 };

static short real12array[SLOT_REEL_CELLS] = { 3, 0, 1, 3, 0, 0, 0, 1, 2, 0, 4, 0, 1, 2, 0, 1, 0, 0, 3, 7, 1 };

static short real13array[SLOT_REEL_CELLS] = { 1, 0, 0, 0, 2, 1, 0, 2, 1, 4, 2, 0, 0, 1, 7, 3, 1, 0, 7, 7, 3 };

static short real21array[SLOT_REEL_CELLS] = { 1, 3, 1, 6, 4, 6, 0, 1, 3, 1, 6, 2, 1, 0, 1, 2, 1, 0, 5, 5, 0 };

static short real22array[SLOT_REEL_CELLS] = { 3, 0, 1, 3, 6, 0, 0, 1, 2, 0, 4, 0, 1, 2, 0, 1, 0, 0, 3, 5, 1 };

static short real23array[SLOT_REEL_CELLS] = { 1, 0, 0, 6, 2, 1, 6, 2, 1, 4, 2, 0, 0, 1, 5, 3, 1, 0, 5, 5, 3 };

static short real31array[SLOT_REEL_CELLS] = { 6, 3, 1, 6, 4, 6, 6, 1, 3, 1, 0, 2, 1, 0, 6, 2, 1, 6, 5, 5, 6 };

static short real32array[SLOT_REEL_CELLS] = { 3, 0, 1, 3, 0, 0, 0, 1, 2, 0, 4, 0, 1, 2, 0, 1, 0, 0, 3, 5, 1 };

static short real33array[SLOT_REEL_CELLS] = { 1, 0, 0, 0, 2, 1, 0, 2, 1, 4, 2, 0, 0, 1, 5, 3, 1, 0, 5, 5, 3 };

static short real41array[SLOT_REEL_CELLS] = { 8, 7, 8, 7, 8, 7, 8, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

static short real42array[SLOT_REEL_CELLS] = { 8, 8, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 8, 8, 7, 8, 8, 8, 8 };

static short real43array[SLOT_REEL_CELLS] = { 8, 8, 8, 8, 7, 8, 8, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

static short real51array[SLOT_REEL_CELLS] = { 1, 3, 8, 0, 4, 4, 0, 8, 3, 1, 0, 2, 8, 0, 8, 2, 1, 0, 5, 5, 0 };

static short real52array[SLOT_REEL_CELLS] = { 3, 0, 1, 8, 0, 8, 8, 1, 2, 8, 4, 0, 1, 2, 0, 8, 0, 8, 3, 5, 1 };

static short real53array[SLOT_REEL_CELLS] = { 1, 0, 8, 8, 2, 1, 0, 2, 1, 8, 2, 8, 0, 8, 5, 3, 1, 0, 5, 5, 3 };

const char pokerPlay5CoinsText[32] = "Play 5 coins per round poker\241\251";

const char pokerPlay10CoinsText[32] = "Play 10 coins per round poker\241\251";

const char pokerPlay30CoinsText[32] = "Play 30 coins per round poker\241\251";

const char pokerPlay100CoinsText[40] = "Play 100 coins per round poker\241\251";



int casinoUnusedWord = 0;

static struct SpriteTex slot_window_0 = { .u = 256, .v = 48, .w = 112, .h = 200, .texture_base = 0, .clut_base = 316, .format = 0 };

static struct SpriteRect slot_window_rect_1[3] = { { .x = 80, .y = 50, .w = 112, .h = 200 }, { .x = 200, .y = 50, .w = 112, .h = 200 }, { .x = 320, .y = 50, .w = 112, .h = 200 } };

static struct SpriteTex Real_2[9] = { { .u = 0, .v = 0, .w = 112, .h = 64, .texture_base = 128, .clut_base = 188, .format = 0 }, { .u = 0, .v = 64, .w = 112, .h = 64, .texture_base = 149, .clut_base = 188, .format = 0 }, { .u = 0, .v = 128, .w = 112, .h = 64, .texture_base = 384, .clut_base = 188, .format = 0 }, { .u = 112, .v = 0, .w = 112, .h = 64, .texture_base = 384, .clut_base = 188, .format = 0 }, { .u = 112, .v = 64, .w = 112, .h = 64, .texture_base = 640, .clut_base = 188, .format = 0 }, { .u = 112, .v = 192, .w = 112, .h = 64, .texture_base = 661, .clut_base = 188, .format = 0 }, { .u = 0, .v = 192, .w = 112, .h = 64, .texture_base = 405, .clut_base = 188, .format = 0 }, { .u = 0, .v = 256, .w = 112, .h = 64, .texture_base = 896, .clut_base = 188, .format = 0 }, { .u = 112, .v = 128, .w = 112, .h = 64, .texture_base = 917, .clut_base = 188, .format = 0 } };

static int xpos_3[3] = { 80, 200, 320 };

static struct SpriteTex decfont[10] = { { .u = 496, .v = 48, .w = 16, .h = 24, .texture_base = 0, .clut_base = 692, .format = 0 }, { .u = 496, .v = 72, .w = 16, .h = 24, .texture_base = 0, .clut_base = 692, .format = 0 }, { .u = 496, .v = 96, .w = 16, .h = 24, .texture_base = 0, .clut_base = 692, .format = 0 }, { .u = 496, .v = 120, .w = 16, .h = 24, .texture_base = 0, .clut_base = 692, .format = 0 }, { .u = 496, .v = 144, .w = 16, .h = 24, .texture_base = 0, .clut_base = 692, .format = 0 }, { .u = 496, .v = 168, .w = 16, .h = 24, .texture_base = 0, .clut_base = 692, .format = 0 }, { .u = 464, .v = 192, .w = 16, .h = 24, .texture_base = 0, .clut_base = 692, .format = 0 }, { .u = 480, .v = 192, .w = 16, .h = 24, .texture_base = 0, .clut_base = 692, .format = 0 }, { .u = 496, .v = 192, .w = 16, .h = 24, .texture_base = 0, .clut_base = 692, .format = 0 }, { .u = 464, .v = 216, .w = 16, .h = 24, .texture_base = 0, .clut_base = 692, .format = 0 } };

static struct SpriteTex spinbt[3] = { { .u = 340, .v = 256, .w = 40, .h = 88, .texture_base = 0, .clut_base = 436, .format = 0 }, { .u = 380, .v = 256, .w = 40, .h = 88, .texture_base = 0, .clut_base = 436, .format = 0 }, { .u = 420, .v = 256, .w = 40, .h = 88, .texture_base = 0, .clut_base = 436, .format = 0 } };

static struct SpriteRect spinbt_rect_6 = { .x = 439, .y = 155, .w = 40, .h = 88 };

static int pushcnt_7 = -1;

static struct SpriteTex stopbt[3] = { { .u = 480, .v = 216, .w = 16, .h = 24, .texture_base = 0, .clut_base = 564, .format = 0 }, { .u = 496, .v = 216, .w = 16, .h = 24, .texture_base = 0, .clut_base = 564, .format = 0 }, { .u = 460, .v = 288, .w = 16, .h = 24, .texture_base = 0, .clut_base = 564, .format = 0 } };

static struct SpriteRect stopbt_rect_8[3] = { { .x = 199, .y = 258, .w = 16, .h = 24 }, { .x = 248, .y = 258, .w = 16, .h = 24 }, { .x = 297, .y = 258, .w = 16, .h = 24 } };

static struct SpriteTex PicTable[7] = { { .u = 256, .v = 256, .w = 84, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 256, .v = 272, .w = 84, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 256, .v = 288, .w = 84, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 256, .v = 320, .w = 84, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 256, .v = 336, .w = 84, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 256, .v = 304, .w = 84, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 460, .v = 312, .w = 28, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 } };

static struct SpriteTex OptionText[5] = { { .u = 104, .v = 464, .w = 80, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 160, .v = 480, .w = 96, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 104, .v = 496, .w = 104, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 208, .v = 496, .w = 48, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 192, .v = 464, .w = 64, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 } };

static struct SpriteTex DeciList_9[10] = { { .u = 464, .v = 240, .w = 12, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 476, .v = 240, .w = 12, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 488, .v = 240, .w = 12, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 500, .v = 240, .w = 12, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 464, .v = 256, .w = 12, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 476, .v = 256, .w = 12, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 488, .v = 256, .w = 12, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 500, .v = 256, .w = 12, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 464, .v = 272, .w = 12, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 476, .v = 272, .w = 12, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 } };

static int pos_10[7][2] = { { 33, 373 }, { 33, 353 }, { 33, 333 }, { 33, 313 }, { 33, 393 }, { 33, 293 }, { 33, 413 } };

static int rate_11[6] = { 30, 20, 20, 10, 5, 1 };

static int double_rate_12[6] = { 60, 40, 40, 20, 10, 2 };

static struct SpriteTex Pic_13[9] = { { .u = 256, .v = 256, .w = 28, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 256, .v = 272, .w = 28, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 256, .v = 288, .w = 28, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 256, .v = 320, .w = 28, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 256, .v = 336, .w = 28, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 256, .v = 304, .w = 28, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 460, .v = 312, .w = 28, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 392, .v = 360, .w = 28, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 460, .v = 328, .w = 28, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 } };

static struct SpriteTex Total_14 = { .u = 352, .v = 360, .w = 40, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 };

static struct SpriteTex linetex_15 = { .u = 0, .v = 320, .w = 112, .h = 64, .texture_base = 0, .clut_base = 180, .format = 0 };

static struct SpriteRect linerect_16[9] = { { .x = 80, .y = 50, .w = 112, .h = 64 }, { .x = 80, .y = 114, .w = 112, .h = 64 }, { .x = 80, .y = 178, .w = 112, .h = 64 }, { .x = 200, .y = 50, .w = 112, .h = 64 }, { .x = 200, .y = 114, .w = 112, .h = 64 }, { .x = 200, .y = 178, .w = 112, .h = 64 }, { .x = 320, .y = 50, .w = 112, .h = 64 }, { .x = 320, .y = 114, .w = 112, .h = 64 }, { .x = 320, .y = 178, .w = 112, .h = 64 } };

static struct SpriteTex Text_17[3] = { { .u = 104, .v = 480, .w = 56, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 104, .v = 496, .w = 104, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 }, { .u = 160, .v = 480, .w = 96, .h = 16, .texture_base = 0, .clut_base = 444, .format = 0 } };

static struct SpriteTex Bar_18 = { .u = 256, .v = 0, .w = 256, .h = 32, .texture_base = 0, .clut_base = 180, .format = 0 };

static struct SpriteTex TimeBar_19 = { .u = 256, .v = 352, .w = 128, .h = 8, .texture_base = 0, .clut_base = 564, .format = 0 };

static struct SlotPatternPoint pat_22[5][3] = {
    { { 0, 0 }, { 1, 1 }, { 2, 2 } },
    { { 0, 0 }, { 1, 0 }, { 2, 0 } },
    { { 0, 1 }, { 1, 1 }, { 2, 1 } },
    { { 0, 2 }, { 1, 2 }, { 2, 2 } },
    { { 0, 2 }, { 1, 1 }, { 2, 0 } }
};

static int inter_23 = 0;

static int mode_24 = 0;

static struct SpriteTex tex_30 = { .u = 112, .v = 256, .w = 112, .h = 208, .texture_base = 0, .clut_base = 180, .format = 0 };

static int mode_33 = 0;

static int busy_35 = 0;

static struct SpriteTex tex_36 = { .u = 256, .v = 352, .w = 192, .h = 8, .texture_base = 0, .clut_base = 564, .format = 0 };

static struct SpriteTex tex_37[3] = { { .u = 368, .v = 48, .w = 128, .h = 48, .texture_base = 0, .clut_base = 436, .format = 0 }, { .u = 368, .v = 96, .w = 128, .h = 48, .texture_base = 0, .clut_base = 436, .format = 0 }, { .u = 368, .v = 144, .w = 128, .h = 48, .texture_base = 0, .clut_base = 436, .format = 0 } };

static struct SpriteRect rect_38 = { .x = 192, .y = 356, .w = 128, .h = 48 };

static int count_39 = 0;

static struct SpriteRect rect_40 = { .x = 439, .y = 28, .w = 40, .h = 88 };

static int count_41 = 0;

static struct SpriteRect RateLamp_Rect[9] = { { .x = 36, .y = 329, .w = 32, .h = 32 }, { .x = 36, .y = 292, .w = 32, .h = 32 }, { .x = 36, .y = 255, .w = 32, .h = 32 }, { .x = 36, .y = 218, .w = 32, .h = 32 }, { .x = 36, .y = 181, .w = 32, .h = 32 }, { .x = 36, .y = 144, .w = 32, .h = 32 }, { .x = 36, .y = 107, .w = 32, .h = 32 }, { .x = 36, .y = 70, .w = 32, .h = 32 }, { .x = 36, .y = 33, .w = 32, .h = 32 } };

static int anm_42 = 0;

static int stopbtn_pos_47[9][2] = { { 199, 123 }, { 248, 123 }, { 297, 123 }, { 211, 227 }, { 260, 227 }, { 309, 227 }, { 223, 331 }, { 272, 331 }, { 321, 331 } };

static short ng_real01array[21] = { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 };

static short ng_real02array[21] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

static short ng_real03array[21] = { 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1 };

static short ng_real04array[21] = { 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1 };

static short ng_real05array[21] = { 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

static short ng_real06array[21] = { 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1 };

static short ng_real07array[21] = { 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1 };

static short ng_real08array[21] = { 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1 };

static short ng_real09array[21] = { 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1 };

static short ng_real10array[21] = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1 };

static short ng_real11array[21] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

static short ng_real12array[21] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1 };

static short ng_real13array[21] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

static short ng_real14array[21] = { 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

static short ng_real15array[21] = { 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1 };

static short ng_real16array[21] = { 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

static short ng_real17array[21] = { 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

static short ng_real18array[21] = { 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1 };

static short ng_real19array[21] = { 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

static short ng_real20array[21] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

static short *ng_realtbl[20] = {
    ng_real01array,
    ng_real02array,
    ng_real03array,
    ng_real04array,
    ng_real05array,
    ng_real06array,
    ng_real07array,
    ng_real08array,
    ng_real09array,
    ng_real10array,
    ng_real11array,
    ng_real12array,
    ng_real13array,
    ng_real14array,
    ng_real15array,
    ng_real16array,
    ng_real17array,
    ng_real18array,
    ng_real19array,
    ng_real20array
};

static struct SpriteTex NgReal_Tex_48[2] = { { .u = 368, .v = 192, .w = 48, .h = 64, .texture_base = 0, .clut_base = 308, .format = 0 }, { .u = 416, .v = 192, .w = 48, .h = 64, .texture_base = 0, .clut_base = 308, .format = 0 } };

static struct NgPanelPos NgReal_49[9] = { { .x = 109, .y = 50 }, { .x = 232, .y = 50 }, { .x = 355, .y = 50 }, { .x = 121, .y = 154 }, { .x = 244, .y = 154 }, { .x = 367, .y = 154 }, { .x = 133, .y = 258 }, { .x = 256, .y = 258 }, { .x = 379, .y = 258 } };

static unsigned short ptmove_50[11][4] = { { 2, 10, 1, 3 }, { 0, 11, 2, 4 }, { 1, 10, 0, 5 }, { 5, 0, 4, 6 }, { 3, 1, 5, 7 }, { 4, 2, 3, 8 }, { 8, 3, 7, 10 }, { 6, 4, 8, 10 }, { 7, 5, 6, 10 }, { 2, 10, 9, 10 }, { 10, 6, 10, 10 } };

static int se_51 = 0;

static struct SpriteTex fontlist_52[10] = { { .u = 144, .v = 456, .w = 16, .h = 24, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 0, .v = 456, .w = 16, .h = 24, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 16, .v = 456, .w = 16, .h = 24, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 32, .v = 456, .w = 16, .h = 24, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 48, .v = 456, .w = 16, .h = 24, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 64, .v = 456, .w = 16, .h = 24, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 80, .v = 456, .w = 16, .h = 24, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 96, .v = 456, .w = 16, .h = 24, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 112, .v = 456, .w = 16, .h = 24, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 128, .v = 456, .w = 16, .h = 24, .texture_base = 0, .clut_base = 1016, .format = 0 } };

static struct SpriteTex waku_53[2] = { { .u = 352, .v = 352, .w = 129, .h = 56, .texture_base = 0, .clut_base = 636, .format = 0 }, { .u = 352, .v = 416, .w = 129, .h = 56, .texture_base = 0, .clut_base = 636, .format = 0 } };

static struct SpriteTex PoCurTex[8] = { { .u = 0, .v = 432, .w = 24, .h = 24, .texture_base = 0, .clut_base = 1000, .format = 0 }, { .u = 24, .v = 432, .w = 24, .h = 24, .texture_base = 0, .clut_base = 1000, .format = 0 }, { .u = 48, .v = 432, .w = 24, .h = 24, .texture_base = 0, .clut_base = 1000, .format = 0 }, { .u = 72, .v = 432, .w = 24, .h = 24, .texture_base = 0, .clut_base = 1000, .format = 0 }, { .u = 96, .v = 432, .w = 24, .h = 24, .texture_base = 0, .clut_base = 1000, .format = 0 }, { .u = 120, .v = 432, .w = 24, .h = 24, .texture_base = 0, .clut_base = 1000, .format = 0 }, { .u = 144, .v = 432, .w = 24, .h = 24, .texture_base = 0, .clut_base = 1000, .format = 0 }, { .u = 168, .v = 432, .w = 24, .h = 24, .texture_base = 0, .clut_base = 1000, .format = 0 } };

static int cnt_54 = 0;

static struct SpriteTex Ltex_56 = { .u = 18, .v = 176, .w = 8, .h = 48, .texture_base = 0, .clut_base = 1020, .format = 0 };

static struct SpriteTex Rtex_57 = { .u = 486, .v = 176, .w = 8, .h = 48, .texture_base = 0, .clut_base = 1020, .format = 0 };

static struct SpriteTex tex_58 = { .u = 22, .v = 176, .w = 464, .h = 48, .texture_base = 0, .clut_base = 1020, .format = 0 };

static struct SpriteTex tex_59 = { .u = 352, .v = 0, .w = 144, .h = 112, .texture_base = 0, .clut_base = 1020, .format = 0 };

static struct SpriteRect rect_60[2] = { { .x = 362, .y = 256, .w = 144, .h = 88 }, { .x = 362, .y = 256, .w = 144, .h = 112 } };

static struct SpriteTex type0_tex_61 = { .u = 192, .v = 432, .w = 64, .h = 64, .texture_base = 0, .clut_base = 988, .format = 0 };

static struct SpriteRect type0_rect_62 = { .x = 406, .y = 269, .w = 64, .h = 64 };

static struct SpriteTex type1_tex_63 = { .u = 352, .v = 256, .w = 64, .h = 96, .texture_base = 0, .clut_base = 988, .format = 0 };

static struct SpriteRect type1_rect_64 = { .x = 406, .y = 269, .w = 64, .h = 96 };

static short type0_pos_65[2][2] = { { 379, 270 }, { 379, 302 } };

static short type1_pos_66[3][2] = { { 379, 270 }, { 379, 302 }, { 379, 334 } };

static int dialog_cur_67 = 1;

static struct SpriteTex CardTex[15] = { { .u = 0, .v = 0, .w = 80, .h = 120, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 80, .v = 0, .w = 80, .h = 120, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 160, .v = 0, .w = 80, .h = 120, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 0, .v = 120, .w = 80, .h = 120, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 80, .v = 120, .w = 80, .h = 120, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 160, .v = 120, .w = 80, .h = 120, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 0, .v = 256, .w = 80, .h = 120, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 80, .v = 256, .w = 80, .h = 120, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 160, .v = 256, .w = 80, .h = 120, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 0, .v = 376, .w = 80, .h = 120, .texture_base = 0, .clut_base = 1016, .format = 0 }, { .u = 80, .v = 376, .w = 80, .h = 120, .texture_base = 0, .clut_base = 1020, .format = 0 }, { .u = 160, .v = 376, .w = 80, .h = 120, .texture_base = 0, .clut_base = 1020, .format = 0 }, { .u = 256, .v = 0, .w = 80, .h = 120, .texture_base = 0, .clut_base = 1020, .format = 0 }, { .u = 336, .v = 0, .w = 80, .h = 120, .texture_base = 0, .clut_base = 1020, .format = 0 }, { .u = 416, .v = 0, .w = 80, .h = 120, .texture_base = 0, .clut_base = 1004, .format = 0 } };

static struct SpriteRect CardPos[5] = { { .x = 32, .y = 37, .w = 80, .h = 120 }, { .x = 124, .y = 37, .w = 80, .h = 120 }, { .x = 216, .y = 37, .w = 80, .h = 120 }, { .x = 308, .y = 37, .w = 80, .h = 120 }, { .x = 400, .y = 37, .w = 80, .h = 120 } };

static void *CardBuf_70[4] = {
    POKER_SPADE_TEX_BUFFER,
    POKER_CLOVER_TEX_BUFFER,
    POKER_HEART_TEX_BUFFER,
    POKER_DIA_TEX_BUFFER
};

static struct PoCardRange range_71[4] = { { 0, 12 }, { 13, 25 }, { 26, 38 }, { 39, 53 } };

static struct SpriteTex tex_72 = { .u = 416, .v = 0, .w = 80, .h = 120, .texture_base = 0, .clut_base = 1004, .format = 0 };

static int status_73 = 0;

static struct SpriteTex tex_76 = { .u = 416, .v = 0, .w = 80, .h = 120, .texture_base = 0, .clut_base = 1004, .format = 0 };

static int status_77 = 0;

static int cnt_82 = 0;

static int add_83 = 1;

static int rate_84 = 0;

static struct SpriteTex tex_85 = { .u = 0, .v = 256, .w = 352, .h = 176, .texture_base = 0, .clut_base = 1020, .format = 0 };

static struct SpriteRect rect_86 = { .x = 16, .y = 256, .w = 352, .h = 176 };

static struct SpriteTex MarkerTex_87 = { .u = 416, .v = 256, .w = 64, .h = 80, .texture_base = 0, .clut_base = 1004, .format = 0 };

static struct SpriteRect MarkerPos_88[5] = { { .x = 20, .y = 304, .w = 64, .h = 80 }, { .x = 88, .y = 304, .w = 64, .h = 80 }, { .x = 156, .y = 304, .w = 64, .h = 80 }, { .x = 224, .y = 304, .w = 64, .h = 80 }, { .x = 292, .y = 304, .w = 64, .h = 80 } };

static struct SpriteTex PtrTex_89 = { .u = 480, .v = 256, .w = 24, .h = 24, .texture_base = 0, .clut_base = 1004, .format = 0 };

static struct SpriteRect PtrPos_90[4] = { { .x = 75, .y = 331, .w = 24, .h = 24 }, { .x = 143, .y = 331, .w = 24, .h = 24 }, { .x = 206, .y = 331, .w = 24, .h = 24 }, { .x = 279, .y = 331, .w = 24, .h = 24 } };

static int wait_91 = 8;

static struct SpriteTex PoSelecterTex[2] = { { .u = 0, .v = 224, .w = 80, .h = 16, .texture_base = 0, .clut_base = 988, .format = 0 }, { .u = 0, .v = 240, .w = 80, .h = 16, .texture_base = 0, .clut_base = 988, .format = 0 } };

static struct SpriteRect PoSelecterPos[6] = { { .x = 36, .y = 176, .w = 80, .h = 16 }, { .x = 128, .y = 176, .w = 80, .h = 16 }, { .x = 220, .y = 176, .w = 80, .h = 16 }, { .x = 312, .y = 176, .w = 80, .h = 16 }, { .x = 404, .y = 176, .w = 80, .h = 16 }, { .x = 220, .y = 216, .w = 80, .h = 16 } };

static int cur_92 = 0;

static int mode_93 = 0;

static short selpos_94[5][2] = { { 16, 172 }, { 108, 172 }, { 200, 172 }, { 292, 172 }, { 384, 172 } };

static short chgpos_95[2] = { 200, 212 };

static int status_96 = 0;

static int PokerCoinRate[4] = { 5, 10, 30, 100 };

static int PokerRate[10] = { 0, 1, 2, 3, 5, 7, 10, 20, 50, 100 };

static int init_99 = 0;

static struct SpriteTex EntryBG_Tex = { .u = 0, .v = 0, .w = 511, .h = 447, .texture_base = 0, .clut_base = 1020, .format = 0 };

static struct SpriteRect EntryBG_Rect = { .x = 0, .y = 0, .w = 512, .h = 448 };

static struct SpriteTex Cursor_Tex[8] = { { .u = 0, .v = 144, .w = 24, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 24, .v = 144, .w = 24, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 48, .v = 144, .w = 24, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 72, .v = 144, .w = 24, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 96, .v = 144, .w = 24, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 120, .v = 144, .w = 24, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 144, .v = 144, .w = 24, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 168, .v = 144, .w = 24, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 } };

static struct SpriteRect Cursor_Rect[4] = { { .x = 162, .y = 92, .w = 24, .h = 24 }, { .x = 44, .y = 218, .w = 24, .h = 24 }, { .x = 336, .y = 218, .w = 24, .h = 24 }, { .x = 244, .y = 360, .w = 24, .h = 24 } };

static int anm_100 = 0;

static int curpos_101 = 0;

static int cur_int_102 = 0;

static int menu_mode_103 = 0;

static struct SpriteTex BG_Tex = { .u = 0, .v = 0, .w = 511, .h = 447, .texture_base = 0, .clut_base = 1020, .format = 0 };

static struct SpriteRect BG_Rect = { .x = 0, .y = 0, .w = 512, .h = 448 };

static struct SpriteTex tex_105[11] = { { .u = 144, .v = 120, .w = 16, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 0, .v = 120, .w = 16, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 16, .v = 120, .w = 16, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 32, .v = 120, .w = 16, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 48, .v = 120, .w = 16, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 64, .v = 120, .w = 16, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 80, .v = 120, .w = 16, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 96, .v = 120, .w = 16, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 112, .v = 120, .w = 16, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 128, .v = 120, .w = 16, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 160, .v = 120, .w = 20, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 } };

static struct SpriteTex Tex_106[8] = { { .u = 0, .v = 144, .w = 24, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 24, .v = 144, .w = 24, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 48, .v = 144, .w = 24, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 72, .v = 144, .w = 24, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 96, .v = 144, .w = 24, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 120, .v = 144, .w = 24, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 144, .v = 144, .w = 24, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 }, { .u = 168, .v = 144, .w = 24, .h = 24, .texture_base = 0, .clut_base = 564, .format = 2 } };

static int anm_107 = 0;

static int cur_108 = 0;

static struct CoinCursorPos curpt_109[5] = { { 240, 64 }, { 240, 88 }, { 240, 112 }, { 240, 136 }, { 384, 208 } };

static int init_110 = 0;

static int cur_111 = 0;

static int top_112 = 0;

static struct SpriteTex ViewSamTex[30] = { { .u = 0, .v = 0, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 256, .v = 0, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 512, .v = 0, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 768, .v = 0, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 0, .v = 64, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 256, .v = 64, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 512, .v = 64, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 768, .v = 64, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 0, .v = 128, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 256, .v = 128, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 512, .v = 128, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 768, .v = 128, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 0, .v = 192, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 256, .v = 192, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 512, .v = 192, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 768, .v = 192, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 0, .v = 256, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 256, .v = 256, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 512, .v = 256, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 768, .v = 256, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 0, .v = 320, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 256, .v = 320, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 512, .v = 320, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 768, .v = 320, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 0, .v = 384, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 256, .v = 384, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 512, .v = 384, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 768, .v = 384, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 0, .v = 448, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 }, { .u = 256, .v = 448, .w = 255, .h = 127, .texture_base = 0, .clut_base = 2046, .format = 3 } };

static char fileinfo[30] = { 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0 };

static int old_113 = -1;

static struct SpriteTex tex_114 = { .u = 0, .v = 0, .w = 512, .h = 448, .texture_base = 0, .clut_base = 896, .format = 0 };

static struct SpriteTex keytex_115 = { .u = 0, .v = 448, .w = 480, .h = 64, .texture_base = 0, .clut_base = 1012, .format = 0 };

static struct SpriteRect keyrect_116 = { .x = 17, .y = 381, .w = 480, .h = 64 };

static struct SpriteRect sam_rect_117 = { .x = 32, .y = 71, .w = 256, .h = 168 };

static struct SpriteTex back_tex_118 = { .u = 0, .v = 0, .w = 512, .h = 448, .texture_base = 0, .clut_base = 1012, .format = 0 };

static int cur_119 = 0;

static int ofs_120 = 0;

static int init_121 = 0;

static int mode_123 = 0;

static int MUSICLIST_125[1] = { 0x00a0c658U };

static int oldreq_126 = -1;

const Ov11GifTag spriteTextureGifTag = { { 0x1400000000008000ULL, 0x6ULL } };

const Ov11GifTag spritePrimGifTag = { { 0x9400000000008000ULL, 0x434343430ULL } };

const Ov11GifTag drawStatusGifTag = { { 0x1000000000008000ULL, 0xeULL } };

const Ov11GifTag setTestGifTag = { { 0x1000000000008000ULL, 0xeULL } };

const Ov11GifTag setRegisterGifTag = { { 0x1000000000008000ULL, 0xeULL } };

const Ov11GifTag nonTextureBoxGifTag = { { 0x9400000000008000ULL, 0x414141410ULL } };

const struct PayLayout linePayPositionTable = { { { 52, 37 }, { 52, 86 }, { 52, 135 }, { 52, 184 }, { 52, 233 } } };

const struct SpriteRect rateTableOption1UpperRect = { .x = 197, .y = 313, .w = 96, .h = 16 };

const struct SpriteRect rateTableOption1MiddleRect = { .x = 197, .y = 333, .w = 96, .h = 16 };

const struct SpriteRect rateTableOption1LowerRect = { .x = 197, .y = 353, .w = 96, .h = 16 };

const struct SpriteRect rateTableOption2Rect = { .x = 197, .y = 393, .w = 104, .h = 16 };

const struct SpriteRect rateTableOption3Rect = { .x = 197, .y = 413, .w = 48, .h = 16 };

const struct SpriteRect rateTableOption4Rect = { .x = 127, .y = 413, .w = 64, .h = 16 };

const struct SpriteRect resultTotalLabelRect = { .x = 129, .y = 414, .w = 40, .h = 16 };

const struct SpriteRect bonusBannerRect = { .x = 78, .y = 28, .w = 96, .h = 16 };

const struct SpriteRect bonusIdleBannerRect = { .x = 78, .y = 28, .w = 56, .h = 16 };

const struct SpriteRect bonusTimeBarFrameRect = { .x = 180, .y = 21, .w = 256, .h = 32 };

const struct SpriteRect bonusTimeBarRect = { .x = 189, .y = 33, .w = 0, .h = 8 };

const struct StakeSteps lineStakeStepTable = { .step = { 1, 5, 10, 30 } };

const char lineCheckFormat[24] = "line %d [%d,%d,%d]\n";

const char prizeCheckFormat[24] = "prize = %d [%d]\n";

const char paidCheckFormat[16] = "paid = %d\n";

const struct W22LineIds linePayCheckOrderTable = { { 2, 1, 3, 0, 4 } };

const struct SpriteTex prizeEffectTexDefault = { .u = 0, .v = 0, .w = 256, .h = 64, .texture_base = 0, .clut_base = 64, .format = 0 };

const struct SpriteRect prizeEffectRectDefault = { .x = 128, .y = 112, .w = 1024, .h = 0 };

const struct SpriteTex slotHelpTex = { .u = 0, .v = 0, .w = 480, .h = 256, .texture_base = 0, .clut_base = 508, .format = 0 };

const struct SpriteRect slotHelpRect = { .x = 16, .y = 32, .w = 480, .h = 256 };

const struct SpriteRect ngTimeBarRect = { .x = 190, .y = 35, .w = 192, .h = 8 };

const struct SpriteTex ngHelpTopTex = { 0, 0x100, 0x100, 0x100, 0, 0x1fc, 0 };

const struct SpriteTex ngHelpBottomTex = { 0x100, 0x100, 0x100, 0x80, 0, 0x1fc, 0 };

const struct SpriteRect ngHelpTopRect = { .x = 144, .y = 48, .w = 256, .h = 256 };

const struct SpriteRect ngHelpBottomRect = { .x = 144, .y = 304, .w = 256, .h = 128 };

const struct SpriteRect pokerMoneyDigitRect = { .x = 472, .y = 399, .w = 16, .h = 24 };

const struct SpriteRect pokerMoneyFrameRect = { .x = 366, .y = 371, .w = 129, .h = 56 };

const struct SpriteRect pokerWindowRect = { .x = 0, .y = 207, .w = 512, .h = 48 };

const char pokerWindowFontControlFirst[8] = "\r\002";

const char pokerWindowFontControlSecond[8] = "\031\003";

const char pokerSortedCardFormat[8] = "%d ";

const char debugNewlineText[8] = "\n";

const char pokerRankMatch1Text[8] = "1";

const char pokerRankMatch2Text[8] = "2";

const char pokerRankMatch3Text[8] = "3";

const char pokerRankSeparatorText[8] = "/";

const char pokerStraightDebugText[16] = "\245\271\245\310\245\354\241\274\245\310\n";

const char pokerNoMoreCoinsText[32] = "There are no more coins.";

const char pokerContinueGameText[16] = "Continue game\?";

const char pokerDoubleOrNothingText[24] = "Double or nothing\?";

const char coinExchangeTitleFontControl[8] = "\r\002\014d\301{";

const char coinExchangeBuyCoinsText[16] = "\031\003Buy Coins";

const char coinExchangeAfterTitleFontControl[8] = "\r\002\014\377\200";

const char coinExchangeListFontControl[8] = "\025\002\004\001";

const char coinExchangeExitFontControl[8] = "\r\002\014\377";

const char coinExchangeExitText[8] = "EXIT";

const char coinExchangeAfterExitFontControl[8] = "\025\002\004";

const char fontModeControl[8] = "\r\003";

const char shopItemNameFontControl[8] = "\031\003\025\002\002\010";

const char infoFontControl[16] = "\031\003\025\002\002\010\025\002\001\014\025\003\003\002";

const char viewerImageY1PathFormat[16] = "%s\\%.2d_Y1.xtx";

const char tanakaDirectoryPath[16] = "data\\tanaka";

const char viewerImageY2PathFormat[16] = "%s\\%.2d_Y2.xtx";

const char viewerImageW1PathFormat[16] = "%s\\%.2d_W1.xtx";

const char viewerImageW2PathFormat[16] = "%s\\%.2d_W2.xtx";

const char viewerImageW3PathFormat[16] = "%s\\%.2d_W3.xtx";

const char viewerImageW4PathFormat[16] = "%s\\%.2d_W4.xtx";

const char viewerCursorText[8] = "\r\003\033\034";

const char viewerTitleFontControl[16] = "\031\003\025\002\002\010\025\002\001\014\025\003\003";

const char viewerInfoFontControl[8] = "\025\002\001\013";

const char casinoUmnModeText[8] = "Umnmode";

const char miniSlotSedPath[32] = "data\\sound\\sed\\Minislot.sed";

const char miniSlotSwdPath[32] = "data\\sound\\sed\\Minislot.swd";

const char musicSmdPathFormat[16] = "%s\\%s.SMD";

const char soundDirectoryPath[16] = "data\\sound\\smd";

const char musicSwdPathFormat[16] = "%s\\%s.SWD";

const char versionDebugFormat[16] = "\r\002 Ver %s";

const char casinoBuildDateText[16] = "Nov 24 2002";

static int count_25;

static int dx_26;

static int dy_27;

static struct SpriteTex tex_28;

static struct SpriteRect rect_29;

static struct SlReelPanel sht_31[3];

static struct SlReelPanel * psht_32;

static int wait_34;

static struct SpriteRect rect_55;

static struct SpriteRect card_74[5];

static int wait_75;

static struct SpriteRect card_78;

static int wait_79;

static int cnt_122;

static int pic_124;

static unsigned long long BoxPOS[4];

static int shpmenu_work[51];

static int MenuNum;
