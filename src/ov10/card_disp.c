/*
 * OV10 original TU 6: 0x00a1e5d8..0x00a21348 (27 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov10/cgp.h"

typedef struct CardHelpPackedVector {
    u64 words[2];
} CardHelpPackedVector;

void xglMatrixScale(Matrix4 destination, const Matrix4 source,
                            const float scale[4]);
void xglMatrixTrans(Matrix4 destination, const Matrix4 source,
                            const float translation[4]);
void CardDispLight(Matrix4 matrix, int card, int enabled, int color);
extern void nmlModelEntry(u32 model);
extern void nmlModelSetPlace(const Vector4 *matrix);
extern void nmlModelSetRenderLevel(int level);
extern void nmlModelSetTexture(const char *texture);

extern const CardHelpPackedVector D_00A4E0A0;
extern const CardHelpPackedVector D_00A4E0D0;
extern const CardHelpPackedVector D_00A4E0E0;
extern const CardHelpPackedVector D_00A4E170;
extern const CardHelpPackedVector D_00A4E180;
extern const CardHelpPackedVector D_00A4E190;
extern const CardHelpPackedVector D_00A4E1A0;
extern const CardHelpPackedVector D_00A4E1C0;
extern const char *cardxtx_tbl[];
extern const u32 cardlex_tbl[];

extern const CardHelpPackedVector D_00A4E0B0;
extern const CardHelpPackedVector D_00A4E0C0;
extern void nmlModelSetLight(Matrix4 color, Matrix4 direction);
extern float xglSin(float angle);
extern int ChangeCnt;
extern Matrix4 asDir;

/*
 * Help-title list state CHPTitleInitSub resets when CardHelpProc
 * (0x00a20908) opens a help category; CHPMenuInitSub (0x00a1e640, not part
 * of this TU's claimed source) runs immediately after it. TitleMaxList
 * (config/symbols/ov10.txt, 7 bytes) holds one entry-count byte per
 * category, read here through a pointer captured early and dereferenced
 * once per field (count and limit each get their own read). Only the
 * members CHPTitleInitSub writes are named; +0x04 stays unmodeled because
 * this function does not touch it.
 */
typedef struct CHPTitleList {
    s16 cursor;      /* +0x00: selected row, reset to the top */
    s8 flagA;        /* +0x02 */
    s8 flagB;        /* +0x03 */
    u8 unmodeled_04;
    s8 flagC;        /* +0x05 */
    s16 x;           /* +0x06: screen column */
    s16 y;           /* +0x08: screen row */
    s16 rowHeight;   /* +0x0A */
    s16 scroll;      /* +0x0C: top row offset, reset */
    s16 step;        /* +0x0E: row step */
    s16 count;       /* +0x10: TitleMaxList[category], sign-extended */
    s16 limit;       /* +0x12: TitleMaxList[category], sign-extended */
} CHPTitleList;

extern const u8 TitleMaxList[7];

void CHPTitleInitSub(CHPTitleList *list, int category)
{
    const u8 *entry;
    u8 raw;

    list->x = 266;
    list->y = 50;
    entry = &TitleMaxList[category];
    list->rowHeight = 24;
    list->cursor = 0;
    list->flagA = 0;
    list->flagB = 0;
    list->flagC = 0;
    list->scroll = 0;
    raw = *entry;
    list->step = 1;
    list->count = (s8)raw;
    raw = *entry;
    list->limit = (s8)raw;
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CHPMenuInitSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardHelpFileLoad);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CHPMenuBackDispSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispHelpCurrySet);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispHelpSlide1);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispHelpSlide2);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispHelpSlide3);

void CardDispCtr(Matrix4 matrix)
{
    Matrix4 transformed;
    CardHelpPackedVector translation;

    translation = D_00A4E0A0;
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   (const float *)translation.words);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture(cardxtx_tbl[0x34 / 4]);
    nmlModelEntry(cardlex_tbl[0x34 / 4]);
}

void CardDispField(Matrix4 matrix, const float translation[4])
{
    Matrix4 transformed;

    xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture(cardxtx_tbl[0x48 / 4]);
    nmlModelEntry(cardlex_tbl[0x48 / 4]);
}

void CardDispDeckMake(Matrix4 matrix)
{
    Matrix4 transformed;
    CardHelpPackedVector translation;
    CardHelpPackedVector scale;

    translation = D_00A4E0B0;
    scale = D_00A4E0C0;
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   (const float *)translation.words);
    xglMatrixScale(transformed, (const float (*)[4])transformed,
                   (const float *)scale.words);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture(cardxtx_tbl[0x64 / 4]);
    nmlModelEntry(cardlex_tbl[0x64 / 4]);
}

void CardDispH1P2P(Matrix4 matrix, const float translation[4])
{
    Matrix4 transformed;

    xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture(cardxtx_tbl[0x3C / 4]);
    nmlModelEntry(cardlex_tbl[0x3C / 4]);
}

void CardDispTurn(Matrix4 matrix)
{
    Matrix4 transformed;
    CardHelpPackedVector translation;
    CardHelpPackedVector scale;

    translation = D_00A4E0D0;
    scale = D_00A4E0E0;
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   (const float *)translation.words);
    xglMatrixScale(transformed, (const float (*)[4])transformed,
                   (const float *)scale.words);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture(cardxtx_tbl[0x38 / 4]);
    nmlModelEntry(cardlex_tbl[0x38 / 4]);
}

void CardDispFieldPointScale(Matrix4 matrix, const float translation[4],
                              const float scale[4], int kind)
{
    Matrix4 transformed;
    Matrix4 color;
    float light;

    light = (xglSin((((float)ChangeCnt * 11.25f) / 180.0f) * 3.1415927f) * 0.5f) + 1.0f;
    color[3][0] = color[3][1] = color[3][2] = light;
    xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
    xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
    nmlModelSetLight(color, asDir);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture(cardxtx_tbl[kind]);
    nmlModelEntry(cardlex_tbl[kind]);
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispTejyun);

void CardDispFieldPoint(Matrix4 matrix, const float translation[4], int kind)
{
    Matrix4 transformed;
    Matrix4 color;
    float light;

    light = (xglSin((((float)ChangeCnt * 11.25f) / 180.0f) * 3.1415927f) * 0.5f) + 1.0f;
    color[3][0] = color[3][1] = color[3][2] = light;
    xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
    nmlModelSetLight(color, asDir);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture(cardxtx_tbl[kind]);
    nmlModelEntry(cardlex_tbl[kind]);
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispGun);

void CardDispShineCurry(Matrix4 matrix, const float translation[4])
{
    Matrix4 transformed;
    CardHelpPackedVector scale;

    scale = D_00A4E170;
    xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
    xglMatrixScale(transformed, (const float (*)[4])transformed,
                   (const float *)scale.words);
    CardDispLight(transformed, 84, 1, 255);
}

void CardDispAttr(Matrix4 matrix)
{
    Matrix4 transformed;
    CardHelpPackedVector translation;
    const float step = -0.38f;

    translation = D_00A4E180;
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   (const float *)translation.words);
    nmlModelSetPlace((const Vector4 *)transformed);

    nmlModelSetTexture((const char *)0x01C91000);
    nmlModelSetRenderLevel(2);
    nmlModelEntry(0x01C87400);
    transformed[3][1] += step;

    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture((const char *)0x01C91000);
    nmlModelSetRenderLevel(2);
    nmlModelEntry(0x01C87C00);
    transformed[3][1] += step;

    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture((const char *)0x01C91000);
    nmlModelSetRenderLevel(2);
    nmlModelEntry(0x01C88000);
    transformed[3][1] += step;

    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture((const char *)0x01C91000);
    nmlModelSetRenderLevel(2);
    nmlModelEntry(0x01C87800);
    transformed[3][1] += step;

    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture((const char *)0x01C91000);
    nmlModelSetRenderLevel(2);
    nmlModelEntry(0x01C88800);
    transformed[3][1] += -0.57f;

    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture((const char *)0x01C91000);
    nmlModelSetRenderLevel(2);
    nmlModelEntry(0x01C88400);
}

void CardDispNazo(Matrix4 matrix)
{
    Matrix4 transformed;
    CardHelpPackedVector translation;

    translation = D_00A4E190;
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   (const float *)translation.words);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture((const char *)0x01C91000);
    nmlModelSetRenderLevel(2);
    nmlModelEntry(0x01C89000);
}

void CardDispGuno(Matrix4 matrix)
{
    Matrix4 transformed;
    CardHelpPackedVector translation;

    translation = D_00A4E1A0;
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   (const float *)translation.words);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture((const char *)0x01C91000);
    nmlModelSetRenderLevel(2);
    nmlModelEntry(0x01C88000);
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispOnce);

void CardDispComm(Matrix4 matrix, int kind)
{
    Matrix4 transformed;
    CardHelpPackedVector translation;

    translation = D_00A4E1C0;
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   (const float *)translation.words);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture(cardxtx_tbl[kind]);
    nmlModelEntry(cardlex_tbl[kind]);
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispPhase);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispHelpCard);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardHelpProc);

/*
 * Clears the 256-byte help page-state buffer at +0x59c0..+0x5abf, then the
 * active-page byte at +0x59be. Both fall inside CardGameWork's unmodeled
 * +0x0002..+0x5af0 span (include/ov10/cgp.h, ov10/tu008); naming them there
 * needs that TU's own allocation, so this TU uses the named-offset fallback
 * instead of indexing the unmodeled byte array with raw literals.
 */
#define CGP_HELP_PAGE_ACTIVE(w)  (((u8 *)(w)) + 0x59BE)
#define CGP_HELP_PAGE_STATE(w)   (((u8 *)(w)) + 0x59C0)
#define CGP_HELP_PAGE_STATE_SIZE 256

void CardHelpPageInit(CardGameWork *work)
{
    int i;

    for (i = CGP_HELP_PAGE_STATE_SIZE - 1; i >= 0; i--) {
        CGP_HELP_PAGE_STATE(work)[i] = 0;
    }
    *CGP_HELP_PAGE_ACTIVE(work) = 0;
}
