/*
 * OV12 original TU 77: 0x00a434b8..0x00a44f08 (35 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_announce.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a57bc0 contains the source filename "../rg_announce.euc.c".
 * ov12:0x00a57bb0 contains the assertion expression "pAnn != NIL".
 */
const char D_00A57BB0[] = "pAnn != NIL";

const char D_00A57BC0[] = "../rg_announce.euc.c";

extern void XrgPaint2DUseTexture(XrgPaint2D *paint, int texture);
extern void XrgPaint2DAlpha(XrgPaint2D *paint, int blend);
extern void XrgPaint2DDrawXYWH(XrgPaint2D *paint, int mode, int x, int y,
                               int width, int height);
extern void XrgPaint2DSetUVOffset(XrgPaint2D *paint, int u, int v);
extern void XrgPaint2DSetUVSize(XrgPaint2D *paint, int width, int height);
extern void XrgPaint2DFlush(XrgPaint2D *paint);
extern void DisposeXrgPaint2D_sub(XrgPaint2D *paint, const char *source_file,
                                  int line);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern RgHeap *InstanceOfRgHeap(void);
static void _InitAnn(RgAnnounce *pAnn);
extern void *InstanceOfRgBattleCommonData(void);
extern void *RgBattleCommonDataAnnounceAdd(void *battleCommon);
extern void *RgBattleCommonDataAnnounceSub(void *battleCommon);
extern int RgBxxGetPic(void *battleCommon, const char *name);
extern XrgPaint2D *CreateXrgPaint2D_sub(const char *source_file, int line);
extern void XrgPaint2DSetDrawPrio(XrgPaint2D *paint, int prio);
/* Scaffold-owned strings of _InitAnn: texture names, assert expressions and the
 * asset file name. Their bytes stay in the original .rodata. */
extern const char D_00A57C10[];
extern const char D_00A57BD8[];
extern const char D_00A57BE8[];
extern const char D_00A57C28[];
extern const char D_00A57C38[];
extern const char D_00A57C60[];
extern const char D_00A57C70[];
extern const char D_00A57C98[];
extern const char D_00A57CA0[];
extern const char D_00A57CC8[];
extern const char D_00A57CD0[];
extern const char D_00A57CF8[];
extern const char D_00A57D08[];
extern const char D_00A57D30[];
extern const char D_00A57D40[];
extern const char D_00A57D68[];
extern const char D_00A57D70[];
extern const char D_00A57D98[];
extern const char D_00A57DA8[];
extern const char D_00A57DD0[];
extern const char D_00A57DE0[];
extern const char D_00A57E08[];
extern const char D_00A57E18[];
extern const char D_00A57E40[];
extern const char D_00A57E50[];
extern const char D_00A57E78[];
extern const char D_00A57E88[];
extern const char D_00A57EB0[];
extern const char D_00A57EC0[];
extern const char D_00A57EE8[];
extern const char D_00A57EF8[];
extern const char D_00A57F20[];
extern const char D_00A57F30[];
extern const char D_00A57F58[];
extern const char D_00A57F68[];
extern const char D_00A57F90[];
extern const char D_00A57FA0[];
extern const char D_00A57FC8[];
extern const char D_00A57FD8[];
extern const char D_00A58000[];
extern const char D_00A58010[];
extern const char D_00A58038[];
extern const char D_00A58048[];
extern const char D_00A58070[];
extern const char D_00A58080[];
extern const char D_00A580A8[];
extern const char D_00A580B8[];
extern const char D_00A580E0[];
extern const char D_00A580E8[];
extern const char D_00A58110[];
extern const char D_00A58120[];
extern const char D_00A58148[];
extern const char D_00A58158[];
extern const char D_00A58180[];
extern const char D_00A58188[];
extern const char D_00A581B0[];
extern const char D_00A581C0[];
extern const char D_00A581E8[];
extern const char D_00A581F8[];
extern const char D_00A58220[];
extern const char D_00A58230[];
extern const char D_00A58258[];
extern const char D_00A58268[];
extern const char D_00A58290[];
extern const char D_00A582A0[];
extern const char D_00A582C8[];
extern const char D_00A582D8[];
extern const char D_00A58300[];
extern const char D_00A58310[];
extern const char D_00A58338[];
extern const char D_00A58348[];
extern const char D_00A58370[];
extern const char D_00A58380[];
extern const char D_00A583A8[];
extern const char D_00A583B8[];
extern const char D_00A583E0[];
extern const char D_00A583F0[];
extern const char D_00A58418[];
extern const char D_00A58420[];
extern const char D_00A58448[];
extern const char D_00A58450[];
extern const char D_00A58478[];
extern const char D_00A58488[];
extern const char D_00A584B0[];
extern const char D_00A584C0[];
extern const char D_00A584E8[];
extern const char D_00A584F0[];
extern const char D_00A58518[];
extern const char D_00A58528[];
extern const char D_00A58550[];
extern const char D_00A58560[];
extern const char D_00A58588[];
extern const char D_00A58598[];
extern const char D_00A585C0[];
extern const char D_00A585D0[];
extern const char D_00A585F8[];
extern const char D_00A58608[];
extern const char D_00A58630[];
extern const char D_00A58640[];
extern const char D_00A58668[];
extern const char D_00A58678[];

static void _InitAnn(RgAnnounce *pAnn)
{
    void *common;
    void *addedCommon;
    void *subCommon;

    if (pAnn == 0) {
        assert_prog(D_00A57BB0, D_00A57BC0, 76);
    }
    common = InstanceOfRgBattleCommonData();
    addedCommon = RgBattleCommonDataAnnounceAdd(common);
    subCommon = RgBattleCommonDataAnnounceSub(common);

    pAnn->textures[0] = RgBxxGetPic(addedCommon, D_00A57BD8);
    if (pAnn->textures[0] == 0) {
        assert_prog(D_00A57BE8, D_00A57C10, 1);
    }
    pAnn->textures[1] = RgBxxGetPic(addedCommon, D_00A57C28);
    if (pAnn->textures[1] == 0) {
        assert_prog(D_00A57C38, D_00A57C10, 2);
    }
    pAnn->textures[2] = RgBxxGetPic(addedCommon, D_00A57C60);
    if (pAnn->textures[2] == 0) {
        assert_prog(D_00A57C70, D_00A57C10, 3);
    }
    pAnn->textures[3] = RgBxxGetPic(addedCommon, D_00A57C98);
    if (pAnn->textures[3] == 0) {
        assert_prog(D_00A57CA0, D_00A57C10, 4);
    }
    pAnn->textures[4] = RgBxxGetPic(addedCommon, D_00A57CC8);
    if (pAnn->textures[4] == 0) {
        assert_prog(D_00A57CD0, D_00A57C10, 5);
    }
    pAnn->textures[5] = RgBxxGetPic(addedCommon, D_00A57CF8);
    if (pAnn->textures[5] == 0) {
        assert_prog(D_00A57D08, D_00A57C10, 6);
    }
    pAnn->textures[6] = RgBxxGetPic(addedCommon, D_00A57D30);
    if (pAnn->textures[6] == 0) {
        assert_prog(D_00A57D40, D_00A57C10, 7);
    }
    pAnn->textures[7] = RgBxxGetPic(addedCommon, D_00A57D68);
    if (pAnn->textures[7] == 0) {
        assert_prog(D_00A57D70, D_00A57C10, 8);
    }
    pAnn->textures[8] = RgBxxGetPic(addedCommon, D_00A57D98);
    if (pAnn->textures[8] == 0) {
        assert_prog(D_00A57DA8, D_00A57C10, 9);
    }
    pAnn->textures[9] = RgBxxGetPic(addedCommon, D_00A57DD0);
    if (pAnn->textures[9] == 0) {
        assert_prog(D_00A57DE0, D_00A57C10, 10);
    }
    pAnn->textures[10] = RgBxxGetPic(addedCommon, D_00A57E08);
    if (pAnn->textures[10] == 0) {
        assert_prog(D_00A57E18, D_00A57C10, 11);
    }
    pAnn->textures[11] = RgBxxGetPic(addedCommon, D_00A57E40);
    if (pAnn->textures[11] == 0) {
        assert_prog(D_00A57E50, D_00A57C10, 12);
    }
    pAnn->textures[12] = RgBxxGetPic(addedCommon, D_00A57E78);
    if (pAnn->textures[12] == 0) {
        assert_prog(D_00A57E88, D_00A57C10, 13);
    }
    pAnn->textures[13] = RgBxxGetPic(addedCommon, D_00A57EB0);
    if (pAnn->textures[13] == 0) {
        assert_prog(D_00A57EC0, D_00A57C10, 14);
    }
    pAnn->textures[14] = RgBxxGetPic(addedCommon, D_00A57EE8);
    if (pAnn->textures[14] == 0) {
        assert_prog(D_00A57EF8, D_00A57C10, 15);
    }
    pAnn->textures[15] = RgBxxGetPic(addedCommon, D_00A57F20);
    if (pAnn->textures[15] == 0) {
        assert_prog(D_00A57F30, D_00A57C10, 16);
    }
    pAnn->textures[16] = RgBxxGetPic(addedCommon, D_00A57F58);
    if (pAnn->textures[16] == 0) {
        assert_prog(D_00A57F68, D_00A57C10, 17);
    }
    pAnn->textures[17] = RgBxxGetPic(addedCommon, D_00A57F90);
    if (pAnn->textures[17] == 0) {
        assert_prog(D_00A57FA0, D_00A57C10, 18);
    }
    pAnn->textures[18] = RgBxxGetPic(addedCommon, D_00A57FC8);
    if (pAnn->textures[18] == 0) {
        assert_prog(D_00A57FD8, D_00A57C10, 19);
    }
    pAnn->textures[19] = RgBxxGetPic(addedCommon, D_00A58000);
    if (pAnn->textures[19] == 0) {
        assert_prog(D_00A58010, D_00A57C10, 20);
    }
    pAnn->textures[20] = RgBxxGetPic(addedCommon, D_00A58038);
    if (pAnn->textures[20] == 0) {
        assert_prog(D_00A58048, D_00A57C10, 21);
    }
    pAnn->textures[21] = RgBxxGetPic(addedCommon, D_00A58070);
    if (pAnn->textures[21] == 0) {
        assert_prog(D_00A58080, D_00A57C10, 22);
    }
    pAnn->textures[22] = RgBxxGetPic(addedCommon, D_00A580A8);
    if (pAnn->textures[22] == 0) {
        assert_prog(D_00A580B8, D_00A57C10, 23);
    }
    pAnn->textures[23] = RgBxxGetPic(addedCommon, D_00A580E0);
    if (pAnn->textures[23] == 0) {
        assert_prog(D_00A580E8, D_00A57C10, 24);
    }
    pAnn->textures[24] = RgBxxGetPic(addedCommon, D_00A58110);
    if (pAnn->textures[24] == 0) {
        assert_prog(D_00A58120, D_00A57C10, 25);
    }
    pAnn->textures[25] = RgBxxGetPic(addedCommon, D_00A58148);
    if (pAnn->textures[25] == 0) {
        assert_prog(D_00A58158, D_00A57C10, 26);
    }
    pAnn->textures[26] = RgBxxGetPic(addedCommon, D_00A58180);
    if (pAnn->textures[26] == 0) {
        assert_prog(D_00A58188, D_00A57C10, 27);
    }

    pAnn->textures[27] = RgBxxGetPic(subCommon, D_00A581B0);
    if (pAnn->textures[27] == 0) {
        assert_prog(D_00A581C0, D_00A57C10, 29);
    }
    pAnn->textures[28] = RgBxxGetPic(subCommon, D_00A581E8);
    if (pAnn->textures[28] == 0) {
        assert_prog(D_00A581F8, D_00A57C10, 30);
    }
    pAnn->textures[29] = RgBxxGetPic(subCommon, D_00A58220);
    if (pAnn->textures[29] == 0) {
        assert_prog(D_00A58230, D_00A57C10, 31);
    }
    pAnn->textures[30] = RgBxxGetPic(subCommon, D_00A58258);
    if (pAnn->textures[30] == 0) {
        assert_prog(D_00A58268, D_00A57C10, 32);
    }
    pAnn->textures[31] = RgBxxGetPic(subCommon, D_00A58290);
    if (pAnn->textures[31] == 0) {
        assert_prog(D_00A582A0, D_00A57C10, 33);
    }
    pAnn->textures[32] = RgBxxGetPic(subCommon, D_00A582C8);
    if (pAnn->textures[32] == 0) {
        assert_prog(D_00A582D8, D_00A57C10, 34);
    }
    pAnn->textures[33] = RgBxxGetPic(subCommon, D_00A58300);
    if (pAnn->textures[33] == 0) {
        assert_prog(D_00A58310, D_00A57C10, 35);
    }
    pAnn->textures[34] = RgBxxGetPic(subCommon, D_00A58338);
    if (pAnn->textures[34] == 0) {
        assert_prog(D_00A58348, D_00A57C10, 36);
    }
    pAnn->textures[35] = RgBxxGetPic(subCommon, D_00A58370);
    if (pAnn->textures[35] == 0) {
        assert_prog(D_00A58380, D_00A57C10, 37);
    }
    pAnn->textures[36] = RgBxxGetPic(subCommon, D_00A583A8);
    if (pAnn->textures[36] == 0) {
        assert_prog(D_00A583B8, D_00A57C10, 38);
    }
    pAnn->textures[37] = RgBxxGetPic(subCommon, D_00A583E0);
    if (pAnn->textures[37] == 0) {
        assert_prog(D_00A583F0, D_00A57C10, 39);
    }
    pAnn->textures[38] = RgBxxGetPic(subCommon, D_00A58418);
    if (pAnn->textures[38] == 0) {
        assert_prog(D_00A58420, D_00A57C10, 40);
    }
    pAnn->textures[39] = RgBxxGetPic(subCommon, D_00A58448);
    if (pAnn->textures[39] == 0) {
        assert_prog(D_00A58450, D_00A57C10, 41);
    }
    pAnn->textures[40] = RgBxxGetPic(subCommon, D_00A58478);
    if (pAnn->textures[40] == 0) {
        assert_prog(D_00A58488, D_00A57C10, 42);
    }
    pAnn->textures[41] = RgBxxGetPic(subCommon, D_00A584B0);
    if (pAnn->textures[41] == 0) {
        assert_prog(D_00A584C0, D_00A57C10, 43);
    }
    pAnn->textures[42] = RgBxxGetPic(subCommon, D_00A584E8);
    if (pAnn->textures[42] == 0) {
        assert_prog(D_00A584F0, D_00A57C10, 44);
    }
    pAnn->textures[43] = RgBxxGetPic(subCommon, D_00A58518);
    if (pAnn->textures[43] == 0) {
        assert_prog(D_00A58528, D_00A57C10, 45);
    }
    pAnn->textures[44] = RgBxxGetPic(subCommon, D_00A58550);
    if (pAnn->textures[44] == 0) {
        assert_prog(D_00A58560, D_00A57C10, 46);
    }
    pAnn->textures[45] = RgBxxGetPic(subCommon, D_00A58588);
    if (pAnn->textures[45] == 0) {
        assert_prog(D_00A58598, D_00A57C10, 47);
    }
    pAnn->textures[46] = RgBxxGetPic(subCommon, D_00A585C0);
    if (pAnn->textures[46] == 0) {
        assert_prog(D_00A585D0, D_00A57C10, 48);
    }
    pAnn->textures[47] = RgBxxGetPic(subCommon, D_00A585F8);
    if (pAnn->textures[47] == 0) {
        assert_prog(D_00A58608, D_00A57C10, 49);
    }
    pAnn->textures[48] = RgBxxGetPic(subCommon, D_00A58630);
    if (pAnn->textures[48] == 0) {
        assert_prog(D_00A58640, D_00A57C10, 50);
    }
    pAnn->textures[49] = RgBxxGetPic(subCommon, D_00A58668);
    if (pAnn->textures[49] == 0) {
        assert_prog(D_00A58678, D_00A57C10, 51);
    }

    pAnn->timer = RG_ANNOUNCE_TIMER_INFINITE;
    pAnn->paint = CreateXrgPaint2D_sub(D_00A57BC0, 91);
    XrgPaint2DSetDrawPrio(pAnn->paint, 5);
    pAnn->drawFunc = 0;
    pAnn->pendingDispY = RG_ANNOUNCE_DISP_Y_DEFAULT;
    pAnn->dispY = RG_ANNOUNCE_DISP_Y_DEFAULT;
    pAnn->stageClearActive = 0;
    pAnn->gameClearActive = 0;
}

static void _DestructAnn(RgAnnounce *pAnn)
{
    if (pAnn == 0) {
        assert_prog(D_00A57BB0, D_00A57BC0, 102);
    }
    DisposeXrgPaint2D_sub(pAnn->paint, D_00A57BC0, 103);
}

RgAnnounce *CreateRgAnnounce(void)
{
    RgAnnounce *pAnn;

    pAnn = RgHeapAlloc(InstanceOfRgHeap(), RG_ANNOUNCE_SIZE, D_00A57BC0, 110);
    _InitAnn(pAnn);
    return pAnn;
}

extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);

void DisposeRgAnnounce(RgAnnounce *pAnn)
{
    if (pAnn == 0) {
        assert_prog(D_00A57BB0, D_00A57BC0, 118);
    }
    _DestructAnn(pAnn);
    RgHeapFree(InstanceOfRgHeap(), pAnn, D_00A57BC0, 120);
}

void RgAnnouncePassTime(RgAnnounce *pAnn, float deltaTime)
{
    if (pAnn == 0) {
        assert_prog(D_00A57BB0, D_00A57BC0, 129);
    }
    if (pAnn->timer != RG_ANNOUNCE_TIMER_INFINITE) {
        pAnn->timer -= deltaTime;
        if (pAnn->timer < 0.0f) {
            pAnn->timer = 0.0f;
            pAnn->drawFunc = 0;
        }
    }
}

typedef struct RgAnnounceColor {
    int r;
    int g;
    int b;
    int a;
} RgAnnounceColor;

extern void XrgPaint2DColor(XrgPaint2D *paint, const RgAnnounceColor *color);

/*
 * The 16-byte fixed color the total line copies before drawing, as two
 * doublewords so the copy is an aligned ld/sd pair.
 */
typedef struct RgAnnounceColorHalves {
    long long low;
    long long high;
} RgAnnounceColorHalves;

/* ov12:0x00a586a0: the fixed color the total line copies before drawing (scaffold .rodata). */
extern const RgAnnounceColorHalves D_00A586A0;
static void _time_disp_title(RgAnnounce *pAnn, int x, int y);
static void _time_disp(RgAnnounce *pAnn, int x, int y, unsigned int time);
static void _damage_disp_title(RgAnnounce *pAnn, int x, int y);
static void _damage_disp(RgAnnounce *pAnn, int x, int y, float damage);
static void _paint_flush(RgAnnounce *pAnn);

void RgAnnounceDisp(RgAnnounce *pAnn)
{
    unsigned int totalTime;
    float totalDamage;
    unsigned int i;
    int y;

    if (pAnn == 0) {
        assert_prog(D_00A57BB0, D_00A57BC0, 154);
    }
    if (pAnn->drawFunc != 0) {
        pAnn->drawFunc(pAnn);
    }
    if (pAnn->stageClearActive != 0) {
        XrgPaint2D *paint;
        RgAnnounceColor color;
        int alpha;
        int fadeAlpha;
        int stageTick;

        paint = pAnn->paint;
        memset(&color, 0, sizeof(color));
        color.a = 80;
        alpha = 0;
        stageTick = pAnn->stageClearTick + 1;
        fadeAlpha = stageTick * 8;
        pAnn->stageClearTick = stageTick;
        if (fadeAlpha >= 0) {
            if (fadeAlpha <= color.a) {
                alpha = fadeAlpha;
            } else {
                alpha = color.a;
            }
        }
        color.a = alpha;
        XrgPaint2DColor(paint, &color);
        XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_LINEAR);
        XrgPaint2DDrawXYWH(paint, 0, 0, 218, 512, 128);
        if (color.a >= 80) {
            _time_disp_title(pAnn, 0, 232);
            _time_disp(pAnn, 0, 263, pAnn->stageClearTime);
            _damage_disp_title(pAnn, 0, 289);
            _damage_disp(pAnn, 0, 313, pAnn->stageClearDamage);
        }
    }
    if (pAnn->gameClearActive != 0) {
        XrgPaint2D *paint;
        RgAnnounceColor color;
        RgAnnounceColorHalves fixedColor;
        int alpha;
        int fadeAlpha;
        int stageTick;

        paint = pAnn->paint;
        memset(&color, 0, sizeof(color));
        color.a = 80;
        totalTime = 0;
        totalDamage = 0.0f;
        stageTick = pAnn->stageClearTick + 1;
        pAnn->stageClearTick = stageTick;
        fadeAlpha = stageTick * 8;
        alpha = 0;
        if (fadeAlpha >= 0) {
            if (fadeAlpha <= color.a) {
                alpha = fadeAlpha;
            } else {
                alpha = color.a;
            }
        }
        color.a = alpha;
        XrgPaint2DColor(paint, &color);
        XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_LINEAR);
        XrgPaint2DDrawXYWH(paint, 0, 0, 200, 512, 150);
        if (color.a >= 80) {
            fixedColor = D_00A586A0;
            i = 0;
            _time_disp_title(pAnn, -128, 202);
            _damage_disp_title(pAnn, 128, 206);
            if (pAnn->m_uStage != 0) {
                do {
                    y = 232 + i * 20;
                    _time_disp(pAnn, -128, y, pAnn->m_auTotalTime[i]);
                    _damage_disp(pAnn, 128, y, pAnn->m_afTotalDamage[i]);
                    totalTime += pAnn->m_auTotalTime[i];
                    totalDamage += pAnn->m_afTotalDamage[i];
                    i++;
                } while (i < pAnn->m_uStage);
            }
            XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_LINEAR);
            XrgPaint2DColor(paint, (const RgAnnounceColor *) &fixedColor);
            XrgPaint2DDrawXYWH(paint, 0, 0, 316, 512, 1);
            _time_disp(pAnn, -128, 322, totalTime);
            _damage_disp(pAnn, 128, 322, totalDamage);
        }
    }
    _paint_flush(pAnn);
}

/*
 * The stack copy _time_disp_title and _damage_disp_title hand to _paint_uvwh.
 * Both titles move their rectangle a word pair at a time: uvOffset is the
 * u/v pair _paint_uvwh feeds to XrgPaint2DSetUVOffset, uvSize the w/h pair it
 * feeds to XrgPaint2DSetUVSize and to the draw call's own width/height.
 */
typedef struct RgAnnounceUvRectHalves {
    long long uvOffset;
    long long uvSize;
} RgAnnounceUvRectHalves;

/*
 * The two fixed title rectangles, asm-owned scaffold data (splat names, no
 * config/symbols/ov12.txt entry): ov12:0x00a586b0 belongs to the time title
 * and ov12:0x00a586c0 to the damage title.
 */
const RgAnnounceUvRectHalves D_00A586B0 = {
    0, 0x0000001800000200LL
};

const RgAnnounceUvRectHalves D_00A586C0 = {
    0x0000003800000000LL, 0x0000001200000200LL
};

static void _paint_uvwh(RgAnnounce *pAnn, int index, int x, int y,
                        const RgAnnounceUvRect *rect);

static void _time_disp_title(RgAnnounce *pAnn, int x, int y)
{
    RgAnnounceUvRectHalves rect;

    rect.uvSize = D_00A586B0.uvSize;
    rect.uvOffset = D_00A586B0.uvOffset;
    XrgPaint2DAlpha(pAnn->paint, XRG_PAINT2D_BLEND_ADD);
    _paint_uvwh(pAnn, 0x17, x, y, (const RgAnnounceUvRect *) &rect);
}

extern int RgBattleCommonDataAnnounceFont(void *pEnv);
extern int RgFontStr(XrgPaint2D *paint, int fontId, const char *text, int x,
                     int y, int color);
static void _paint_add(RgAnnounce *pAnn, int index, int x, int y);

static void _time_disp(RgAnnounce *pAnn, int x, int y, unsigned int time)
{
    XrgPaint2D *paint;
    int fontId;
    unsigned int minutes;
    unsigned int rest;
    unsigned int centiseconds;
    char text[16];

    paint = pAnn->paint;
    fontId = RgBattleCommonDataAnnounceFont(InstanceOfRgBattleCommonData());
    minutes = (time / 60000) % 10;
    text[0] = '0' + minutes;
    text[1] = '0' + (time / 6000) % 10;
    text[2] = '\0';
    RgFontStr(paint, fontId, text, x + 227, y, 0);
    rest = time % 6000;
    text[0] = '0' + (rest / 1000u) % 10u;
    text[1] = '0' + (rest / 100u) % 10u;
    RgFontStr(paint, fontId, text, x + 256, y, 0);
    centiseconds = rest % 100;
    text[0] = '0' + (centiseconds / 10) % 10;
    text[1] = '0' + centiseconds % 10;
    RgFontStr(paint, fontId, text, x + 290, y, 0);
    _paint_add(pAnn, 0x18, x, y - 8);
}

static void _damage_disp_title(RgAnnounce *pAnn, int x, int y)
{
    RgAnnounceUvRectHalves rect;

    rect.uvSize = D_00A586C0.uvSize;
    rect.uvOffset = D_00A586C0.uvOffset;
    XrgPaint2DAlpha(pAnn->paint, XRG_PAINT2D_BLEND_ADD);
    _paint_uvwh(pAnn, 0x17, x, y, (const RgAnnounceUvRect *) &rect);
}

extern float floorf(float value);

/* ov12:0x00a586d0 is the scaffold-owned "%d" format string. */
const char D_00A586D0[] = "%d";

static void _damage_disp(RgAnnounce *pAnn, int x, int y, float damage)
{
    XrgPaint2D *paint;
    int fontId;
    unsigned int damageValue;
    char damageText[32];

    paint = pAnn->paint;
    fontId = RgBattleCommonDataAnnounceFont(InstanceOfRgBattleCommonData());
    damageValue = (unsigned int) floorf(damage);
    sprintf(damageText, D_00A586D0, damageValue);
    RgFontStr(paint, fontId, damageText, x + 0x11F, y, 1);
    _paint_add(pAnn, 0x19, x, y - 0x10);
}

static void _paint_sub(RgAnnounce *pAnn, int index, int x, int y);

static void _round1(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x2B, 0, pAnn->dispY);
    _paint_add(pAnn, 0, 0, pAnn->dispY);
}

static void _round2(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x2C, 0, pAnn->dispY);
    _paint_add(pAnn, 1, 0, pAnn->dispY);
}

static void _round3(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x2D, 0, pAnn->dispY);
    _paint_add(pAnn, 2, 0, pAnn->dispY);
}

static void _ready(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x2A, 0, pAnn->dispY);
    _paint_add(pAnn, 3, 0, pAnn->dispY);
}

static void _fight(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x27, 0, pAnn->dispY);
    _paint_add(pAnn, 4, 0, pAnn->dispY);
}

static void _youwin(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x1B, 0, pAnn->dispY);
    _paint_add(pAnn, 5, 0, pAnn->dispY);
}

static void _youlose(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x30, 0, pAnn->dispY);
    _paint_add(pAnn, 6, 0, pAnn->dispY);
}

static void _draw(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x26, 0, pAnn->dispY);
    _paint_add(pAnn, 7, 0, pAnn->dispY);
}

static void _gameover(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x29, 0, pAnn->dispY);
    _paint_add(pAnn, 9, 0, pAnn->dispY);
}

static void _timeover(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x2F, 0, pAnn->dispY);
    _paint_add(pAnn, 8, 0, pAnn->dispY);
}

static void _stageclear(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x2E, 0, pAnn->dispY);
    _paint_add(pAnn, 0xA, 0, pAnn->dispY);
}

static void _gameclear(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x28, 0, pAnn->dispY);
    _paint_add(pAnn, 0xB, 0, pAnn->dispY);
}

static void _you_win_you_lose(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x1B, -RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
    _paint_add(pAnn, 5, -RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
    _paint_sub(pAnn, 0x30, RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
    _paint_add(pAnn, 6, RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
}

static void _you_lose_you_win(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x1B, RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
    _paint_add(pAnn, 5, RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
    _paint_sub(pAnn, 0x30, -RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
    _paint_add(pAnn, 6, -RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
}

static void _paint(XrgPaint2D *paint, int texture, int blend, int x, int y)
{
    if (texture != 0) {
        XrgPaint2DUseTexture(paint, texture);
        XrgPaint2DAlpha(paint, blend);
        XrgPaint2DDrawXYWH(paint, XRG_PAINT2D_MODE_USE_PIC_SIZE, x, y, 0, 0);
    }
}

static void _paint_uvwh(RgAnnounce *pAnn, int index, int x, int y,
                        const RgAnnounceUvRect *rect)
{
    XrgPaint2D *paint = pAnn->paint;

    XrgPaint2DUseTexture(paint, pAnn->textures[index]);
    XrgPaint2DSetUVOffset(paint, rect->u, rect->v);
    XrgPaint2DSetUVSize(paint, rect->w, rect->h);
    XrgPaint2DDrawXYWH(paint, 0, x, y, rect->w, rect->h);
}

static void _paint_lin(RgAnnounce *pAnn, int index, int x, int y)
{
    _paint(pAnn->paint, pAnn->textures[index], XRG_PAINT2D_BLEND_LINEAR, x, y);
}

static void _paint_add(RgAnnounce *pAnn, int index, int x, int y)
{
    _paint(pAnn->paint, pAnn->textures[index], XRG_PAINT2D_BLEND_ADD, x, y);
}

static void _paint_sub(RgAnnounce *pAnn, int index, int x, int y)
{
    _paint(pAnn->paint, pAnn->textures[index], XRG_PAINT2D_BLEND_SUB, x, y);
}

static void _paint_flush(RgAnnounce *pAnn)
{
    XrgPaint2DFlush(pAnn->paint);
}

void RgAnnounceDispInit(RgAnnounce *pAnn, int kind)
{
    static RgAnnounceDrawFunc s_apFuncs[RG_ANNOUNCE_DRAW_KIND_COUNT] = {
        _round1, _round2, _round3, _ready, _fight, _youwin, _youlose,
        _draw, _gameover, _timeover, _stageclear, _gameclear,
        _you_win_you_lose, _you_lose_you_win
    };
    if (pAnn == 0) {
        assert_prog(D_00A57BB0, D_00A57BC0, 466);
    }
    if ((unsigned int) kind < RG_ANNOUNCE_DRAW_KIND_COUNT) {
        pAnn->timer = RG_ANNOUNCE_TIMER_INFINITE;
        pAnn->drawFunc = s_apFuncs[kind];
    } else {
        pAnn->drawFunc = 0;
    }
    pAnn->dispY = pAnn->pendingDispY;
    pAnn->pendingDispY = RG_ANNOUNCE_DISP_Y_DEFAULT;
}

void RgAnnounceDispInitTime(RgAnnounce *pAnn, int kind, float duration)
{
    RgAnnounceDispInit(pAnn, kind);
    if (pAnn->drawFunc != 0) {
        pAnn->timer = duration;
    }
}

void RgAnnounceDispOff(RgAnnounce *pAnn)
{
    if (pAnn == 0) {
        assert_prog(D_00A57BB0, D_00A57BC0, 493);
    }
    pAnn->drawFunc = 0;
    pAnn->stageClearActive = 0;
}

void RgAnnounceSetStageClear(RgAnnounce *pAnn, int time, float damage)
{
    if (pAnn == 0) {
        assert_prog(D_00A57BB0, D_00A57BC0, 505);
    }
    pAnn->stageClearTime = time;
    pAnn->stageClearDamage = damage;
    pAnn->gameClearActive = 0;
    pAnn->stageClearActive = 1;
    pAnn->stageClearTick = 0;
}

/* "uStage < sizeof(pAnn->m_auTotalTime) / sizeof(pAnn->m_auTotalTime[0])" */
const char D_00A586D8[] =
    "uStage < sizeof(pAnn->m_auTotalTime) / sizeof(pAnn->m_auTotalTime[0])";

void RgAnnounceSetGameClear(RgAnnounce *pAnn, const unsigned int *auTime,
                            const float *afDamage, unsigned int uStage)
{
    unsigned int i;

    if (pAnn == 0) {
        assert_prog(D_00A57BB0, D_00A57BC0, 521);
    }
    if (uStage >= RG_ANNOUNCE_GAMECLEAR_MAX) {
        assert_prog(D_00A586D8, D_00A57BC0, 522);
    }
    for (i = 0; i < uStage; i++) {
        pAnn->m_auTotalTime[i] = auTime[i];
        pAnn->m_afTotalDamage[i] = afDamage[i];
    }
    pAnn->m_uStage = uStage;
    pAnn->gameClearActive = 1;
    pAnn->stageClearActive = 0;
    pAnn->stageClearTick = 0;
}
