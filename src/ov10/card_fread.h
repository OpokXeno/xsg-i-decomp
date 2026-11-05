/*
 * TU-local declarations of ov10/tu002 (src/ov10/card_fread.c).
 */

#ifndef SRC_OV10_CARD_FREAD_H
#define SRC_OV10_CARD_FREAD_H

#include "shared.h"
#include "ov10/cgp.h"

/*
 * Stride in bytes of the stored deck records inside the save block: a u16
 * checksum at +0x2A followed by the 40 deck-id bytes at +0x2C (the record
 * index is scaled by this in CardCopyCLD2Ram, 0x00a0af28).
 */
#define CARD_DECK_RECORD_SIZE 42
#define CARD_LIST_SIZE 40            /* entries of every card list of a side (loop test `slti 0x28`, 0x00a0b33c, 0x00a0b55c) */
#define CARD_OPENING_DRAW_COUNT 6    /* CardPlayDrawCard calls at game start (counter 5..0, 0x00a0b3d8..0x00a0b3ec) */
#define CARD_DECK_MAX_COPIES 3       /* copies of one card a deck group holds (`slti $3,$3,0x3`, 0x00a08b60) */
#define CARD_DIGIT_BASE 10           /* decimal digit extraction: count / 10 and count % 10 (`addiu $2,$0,0xA`, 0x00a09638) */
#define CARD_TEXT_CONTROL_LIMIT 0x20 /* bytes below this start a font control code in card text (`sltiu $2,$2,0x20`, 0x00a090d8) */
#define CARD_TEXT_CHAR_BYTES 2       /* bytes per displayed name character: textLength is in characters (`sll $2,$2,1`, 0x00a08f38) */
#define CARD_TEXT_COLOR_STEP 21      /* colour code units per colour index (x * 21 = shifts and adds, 0x00a091ac..0x00a091c8) */
#define CARD_STR_LIST_ROW_HEIGHT 25  /* pixels between rows of CardStrListDisp (`addiu $20,$20,0x19`, 0x00a0d86c) */
#define CARD_DEGREES_HALF_TURN 180.0f /* 0x43340000 (`lui`, 0x00a0eab8, 0x00a103bc) */
#define CARD_RADIANS(degrees) ((degrees) / CARD_DEGREES_HALF_TURN * CARD_HALF_TURN)
/*
 * Card move records (CardMove.kind) of the hand while a card is drawn: the byte at +0x0E counts down
 * through these phases (kind--) and +0x0F holds the frames of the current one (compare chain at
 * 0x00a0ea40..0x00a0ea68 while the timer runs, 0x00a0eb50..0x00a0eb80 when it expires).
 */
#define CARD_MOVE_KIND_GROW 4          /* slides out of the pile growing to full size (`addiu $3,$0,0x4`, 0x00a0bad4) */
#define CARD_MOVE_KIND_TURN 3          /* turns face up for CARD_HAND_TURN_FRAMES (0x00a0eb7c) */
#define CARD_MOVE_KIND_WAIT 2          /* waits CARD_HAND_WAIT_FRAMES at full size (0x00a0eb4c) */
#define CARD_MOVE_KIND_SLIDE 1         /* slides to its hand slot in CARD_SLIDE_FRAMES, then plays the arrive sound (0x00a0eb60) */
#define CARD_HAND_TURN_FRAMES 16       /* frames of CARD_MOVE_KIND_TURN (`addiu $3,$0,0x10`, 0x00a0eb88) */
#define CARD_HAND_WAIT_FRAMES 5        /* frames of CARD_MOVE_KIND_WAIT (`addiu $3,$0,0x5`, 0x00a0ebd4) */
#define CARD_HAND_TURN_STEP 11.25f     /* degrees per frame of the turn (180 / 16), 0x41340000 (0x00a0eaa4) */
#define CARD_DRAW_SOUND 0x2000E        /* sound when a card is drawn from the deck or discard pile (`ori $4,$4,0xE`, 0x00a0bb54) */
#define CARD_DRAW_START_Z 0.1f         /* depth a drawn card starts at, 0x3DCCCCCD (0x00a0bb00) */
#define CARD_DRAW_STEP_Y 0.15f         /* per-frame y step of a drawn card, 0x3E19999A (0x00a0bb10) */
#define CARD_DRAW_STEP_Z 0.025f        /* per-frame depth step of a drawn card, 0x3CCCCCCD (0x00a0bb20) */
#define CARD_PILE_MOVE_Z 0.02f         /* depth a card moved between deck and discard pile starts at, 0x3CA3D70A (0x00a0bdb4, 0x00a0c4b0) */

/*
 * A stored deck record: CardSaveData (ov10/cgp.h) reused at a 42-byte
 * stride, as the comment above CardCopyRam2Deck documents. The checksum
 * CardChkDeckRam validates sits 2 bytes before the 40 deck-id bytes
 * CardCopyRam2Deck already reads through CardSaveData.deck.
 */
typedef struct CardDeckRecord {
    u8 unmodeled_00[0x2A];
    u16 checksum;                /* +0x2A */
} CardDeckRecord;

/*
 * One line of a card list window as CardMakeDispList writes it: the card id
 * (sh at +0x00, stride 4). Same layout and name as the copies in
 * card_view_proc.h and card_play_sound1.h, whose TUs read the lists back.
 */
typedef struct CardListEntry {
    s16 cardId;
    u16 count;
} CardListEntry;

/*
 * A card leaving a pile slides to its destination in CARD_SLIDE_FRAMES (ov10/cgp.h) frames: the
 * move record's timer (`addiu $9,$0,0xA`, 0x00a0bc7c) and the divisor of its
 * per-frame step (`div.s` by 10.0f, 0x00a0c294; folded into the +-0.1265f
 * constants 0x3E018937/0xBE018937 at 0x00a0c4c0 and 0x00a0bdc4).
 */
#define CARD_SUTE_PILE_X 1.265f      /* discard pile position, 0x3FA1EB85 (`lui`, 0x00a0bc60) */
#define CARD_SUTE_PILE_Y (-1.81f)    /* 0xBFE7AE14 (`lui`, 0x00a0bc70) */

/*
 * main/xgl_2.h and main/nml_model_set.h declare these. main/xgl_2.h defines
 * XglQuadword with a mode(TI) attribute that the ordinary-C header check
 * refuses, so the prototypes below repeat the canonical ones.
 */
extern void xglMatrixUnit(float matrix[4][4]);
void xglMatrixTrans(Matrix4 destination, Matrix4 source, const float translation[4]);
void xglMatrixScale(Matrix4 destination, Matrix4 source, const float scale[4]);
void nmlModelSetTexture(const char *texture);
void nmlModelSetPlace(Matrix4 *place);
void xglMatrixRotY(Matrix4 destination, Matrix4 source, float angle);
void xglMatrixRotZ(Matrix4 destination, Matrix4 source, float angle);
void xglMatrixRotX(Matrix4 destination, Matrix4 source, float angle);
extern float xglSin(float angle);
/* main/nml_model_set.c defines it with an NmlModel pointer; the card models
 * are fixed addresses passed in $a0 as is (same declaration as card_view_proc.c). */
extern void nmlModelEntryCard(unsigned int model);

/*
 * Generation markers drawn by CGCPGeneDispSub: one icon model per generation
 * slot of CardCountGeneration, 0x400 bytes apart from 0x01C87400 (`lui`/`ori`
 * at 0x00a112ac, `sll 10` at 0x00a11274), all with the texture at 0x01C91000
 * (`lui`/`ori` at 0x00a112a0); the icons of a row are 0.155 apart (0x3E1EB852,
 * 0x00a11234). The slot loop runs while slot < 5 (`slti $2,$20,0x5`, 0x00a112e4).
 */
#define CARD_GENERATION_COUNT 5
#define CARD_GENERATION_MODEL_BASE 0x01C87400
#define CARD_GENERATION_MODEL_SIZE 0x400
#define CARD_GENERATION_TEXTURE ((const char *)0x01C91000)
#define CARD_GENERATION_ICON_STEP 0.155f

/*
 * The need row drawn by CGCPNeedDispSub (asm 0x00a10f00): four entries (`slti $2,$21,0x4`,
 * 0x00a1112c), starting at (-0.145, 0.408) or, when
 * flipped, at (0.212, -0.686) (`lui` at 0x00a10f38, 0x00a10f44, 0x00a10f58, 0x00a10f64), each
 * icon scaled by 0.9 (0x3F666666, 0x00a10f88). The switch on an entry minus one is bounded by
 * `sltiu 0x1F` (0x00a10fec): the masks below are its used cases.
 */
#define CARD_NEED_SLOT_COUNT 4
#define CARD_NEED_ROW_X -0.145f
#define CARD_NEED_ROW_Y 0.408f
#define CARD_NEED_ROW_FLIPPED_X 0.212f
#define CARD_NEED_ROW_FLIPPED_Y -0.686f
#define CARD_NEED_ICON_SCALE 0.9f
#define CARD_GENE_ROW_X -0.145f              /* first generation icon, 0xBE147AE1 (0x00a111a4) */
#define CARD_GENE_ROW_Y 0.675f               /* 0x3F2CCCCD (0x00a111b0) */
#define CARD_GENE_ROW_FLIPPED_X 0.212f       /* flipped side, 0x3E591687 (0x00a111c8) */
#define CARD_GENE_ROW_FLIPPED_Y -0.418f      /* 0xBED60419 (0x00a111d4) */
#define CARD_GENE_ICON_SCALE 0.9f            /* 0x3F666666 (0x00a111f8) */
#define CARD_NEED_GENERATION0 1
#define CARD_NEED_GENERATION1 2
#define CARD_NEED_GENERATION2 8
#define CARD_NEED_GENERATION3 4
#define CARD_NEED_GENERATION4 16
#define CARD_NEED_ANY 31

/*
 * The hand drawn by CGCPHandDispSubCore (asm 0x00a10ce0): cards are CARD_HAND_DEPTH_STEP
 * apart in depth (0x3C75C28F, `lui` at 0x00a10dd8), a normal card is scaled by
 * CARD_HAND_SCALE (0x3ECCCCCD, 0x00a10d94) and the selected card by
 * CARD_HAND_SELECTED_SCALE (0x3F000000, 0x00a10da0). The hand model is entered with
 * CARD_HAND_TEXTURE set (`lui` at 0x00a10eac, texture 0x01DB5800) and CARD_HAND_MODEL
 * (`lui` at 0x00a10ec4).
 */
#define CARD_HAND_DEPTH_STEP 0.015f
#define CARD_HAND_SCALE 0.4f
#define CARD_HAND_SELECTED_SCALE 0.5f
#define CARD_HAND_TEXTURE ((const char *)0x01DB5800)
#define CARD_HAND_MODEL 0x01DA0000
#define CARD_SINGLE_CARD_SPACING 0.5f   /* a single card sits halfway across the window: edge distance * 0.5, 0x3F000000 (0x00a10d30, 0x00a11410, 0x00a118f8, 0x00a11f78) */
#define CARD_HAND_PANEL_Y 0.95f         /* hand panel y, negated for the own side, 0x3F733333 (0x00a11e9c; 0xBF733333, 0x00a11e78) */
#define CARD_HAND_PANEL_Z 0.1f          /* hand panel depth, 0x3DCCCCCD (0x00a11eb0) */

/*
 * Window edge corners {x, y, z} of the card hand (asm 0x00A43628, 0x18 bytes in the
 * data section): win0pos[0] is the first corner, win0pos[1] the second.
 */
static float win0pos[2][3];
/* Hand fan corners of the in-battle window (0x00A43640, 0x30 bytes, 4x3 floats). */
static float NewWin0pos[4][3];
extern float OperationPos1P[4][3];

/*
 * Dim light colour (data at 0x00A43080, 0x40 bytes, 4x4 floats) and light
 * direction matrix (0x00A42DE0, 0x40 bytes) that CGCPNeedDispSub passes to
 * nmlModelSetLight (asm 0x00a110bc..0x00a110c8). The prototype is the canonical spelling of include/main/nml_model_set.h.
 */
extern Matrix4 HalfColor;
extern Matrix4 asDir;
/* Light colour matrix (0x00A43200, 0x40 bytes) that CardPlayDispYama passes to nmlModelSetLight. */
extern Matrix4 CMColor;
/* Light colour matrices (0x00A430C0, 0x00A43100, 0x00A43140, 0x40 bytes each) that CardDispCard and CGCPHandDispSub3 pass to nmlModelSetLight. */
extern Matrix4 MiddleColor;
extern Matrix4 CLColor;
/* Light colour matrices (0x00A43180, 0x00A431C0) of the two sides that CardPlayDispCursor pulses. */
extern Matrix4 CLP1Color;
extern Matrix4 CLP2Color;
extern Matrix4 CNColor;
extern int printf(const char *format, ...);
void nmlModelSetLight(Matrix4 *direction, Matrix4 *color);

/*
 * Card display colour, text and icon positions (data section):
 * CCColor is the light colour matrix CardDispLight passes to nmlModelSetLight (0x00A42EE0,
 * 0x40 bytes); the position triples are {x, y, z} offsets of the digit and icon models.
 */
extern Matrix4 CCColor;
static float CardHpPos01[3];          /* 0x00A43460: hit points, units digit */
static float CardHpPos10[3];          /* 0x00A43470: hit points, tens digit */
static float CardPowPos01[3];         /* 0x00A43480: power, units digit */
static float CardPowPos10[3];         /* 0x00A43490: power, tens digit */
static float CardStnbyPos[3];         /* 0x00A434A0: reserve cost digit */
static float CardAttriPos[3];         /* 0x00A434B0: attribute icon */
static float CardAttakPos[3];         /* 0x00A434C0: attack type icon */
extern float CardCondPos[4][3];       /* 0x00A434D0: requirement icons */
/* Last card CardDispLight drew (0x00A43500, initial value 0xFFFF). */
static int oldno_0;

/* Resting positions of the battle and disposal slots (x, y, z per slot); defined in ov10/card_play_sound1.c. */
extern float BattlePos1P[][3];
extern float DisposePos1P[][3];
/* Offset of a stacked card within a disposal slot (0x00A435C8) and the x, y positions of the tens (0x00A435D8) and
 * units (0x00A435F8) life digits per slot; read by CardPlayDispDispose. */
static float CardStackPos[3];
static float DisposeHP10pos[][2];
static float DisposeHP01pos[][3];

/*
 * Index of the first mark of each zone's column in CardCursorControl.markTable
 * (four marks per column). CardCursorActive picks it by zone with the jump table
 * at 0x00a4d620 (`addiu $7,$0,N` in the branch delay slots at 0x00a1015c..0x00a1017c, `daddu $7,$0,$0` at
 * 0x00a10180).
 */
#define CARD_MARK_COLUMN_ZONE2 12   /* 0x00a1016c */
#define CARD_MARK_COLUMN_ZONE3 20   /* 0x00a1015c */
#define CARD_MARK_COLUMN_ZONE4 16   /* 0x00a10164 */
#define CARD_MARK_COLUMN_ZONE5 4    /* 0x00a1017c */
#define CARD_MARK_COLUMN_ZONE6 0    /* 0x00a10180 */
#define CARD_MARK_COLUMN_ZONE7 8    /* 0x00a10174 */

/* Junk pile move records start at this height: 0.06f, 0x3D75C28F (`lui`/`ori`, 0x00a0c32c). */
#define CARD_JUNK_MOVE_Z 0.06f
/*
 * The junk pile display (CGCPJunkDispSub, asm 0x00a12380): cards sit CARD_WINDOW_DEPTH_STEP
 * (0x3C449BA6, 0x00a12484) apart in depth over a base plane CARD_WINDOW_BASE_DEPTH
 * (0x3DCCCCCD, 0x00a1240c) deep; the pile marker is placed at CARD_WINDOW_PILE_X/Y
 * (0xC0066666, 0x3F28F5C3 at 0x00a1259c, 0x00a125b4), scaled to CARD_WINDOW_PILE_SCALE_Y
 * (0x3F19999A, 0x00a125dc) in height and entered as model CARD_JUNK_PILE_MODEL
 * (`lui` at 0x00a12630).
 */
#define CARD_WINDOW_DEPTH_STEP 0.012f
#define CARD_WINDOW_BASE_DEPTH 0.1f
#define CARD_WINDOW_PILE_X -2.1f
#define CARD_WINDOW_PILE_Y 0.66f
#define CARD_WINDOW_PILE_SCALE_Y 0.6f
#define CARD_JUNK_PILE_MODEL 0x01DAAC00

/* Frame texture and model shared by the hand, disposal, operation and hand-window cards (`lui` at 0x00a0edd8, 0x00a0ede8, 0x00a0f3c4, 0x00a0f854). */
#define CARD_FRAME_TEXTURE ((const char *)0x01717600)
#define CARD_FRAME_MODEL 0x01716600
#define CARD_HALF_TURN 3.1415927f    /* 0x40490FDB, e.g. 0x00a0ea74 */

#define CARD_JUNK_SLOT_COUNT 40      /* junk[] entries, `slti 0x28` at 0x00a0c2e8 */
#define CARD_BATTLE_LAYER_COUNT 40   /* CardLayerStack.layers[] entries (0x27 countdown, 0x00a0c354) */

/* CardPlayTypeSort sorts at most this many cards (two s16 arrays of 40 on its 0xB0 frame) and
 * runs one pass per card kind (`slti $2,$16,3`, 0x00a0c8c8). */
#define CARD_SORT_MAX_CARDS 40
#define CARD_SORT_KIND_COUNT 3

/*
 * The card master record (CardDefinition, 0x24 bytes, include/ov10/cgp.h) as
 * CardDeckDisp reads it: only the name offset at +0x18 is used. A separate type because
 * giving CardDefinition a 32-bit member raises its alignment from 2 to 4 (no 32-bit
 * member keeps it at 2), which changes the code generated for ov10/tu008 (src/ov10/cgp.c).
 */
typedef struct CardDefinitionName {
    u8 unmodeled_00[0x18];
    u32 nameOffset;              /* +0x18: card name text, offset from CARD_DATA_BUFFER */
    u32 captionOffset;           /* +0x1C: caption text, offset from CARD_DATA_BUFFER (0 = none) */
    u32 descriptionOffset;       /* +0x20: description text, offset from CARD_DATA_BUFFER (0 = none) */
} CardDefinitionName;

typedef struct CardDeckEntry {
    s16 cardId;                  /* +0x00 */
    s16 copies;                  /* +0x02 */
} CardDeckEntry;

/*
 * Card list window of the deck screens as CardDeckListDisp and CardDeckDisp read it. Same
 * leading fields as CardDeckListWindow in card_deck_chk.c, which leaves
 * +0x06..+0x0B unmodeled and carries the entry array after rowCount; a
 * separate name because the two TUs model different parts of the record.
 */
typedef struct CardDeckListView {
    u16 flags;                   /* +0x00: bit 0 prints the copy count of each row (CardListDisp) */
    u8 unmodeled_02[3];
    s8 cursor;                   /* +0x05: selected row */
    s16 originX;                 /* +0x06 */
    s16 originY;                 /* +0x08 */
    u16 textLength;              /* +0x0A: bytes of a DeckListStr row that are printed */
    s16 scroll;                  /* +0x0C: first listed entry */
    s16 cursorLimit;             /* +0x0E: set to rowCount by CardDeckDisp (0x00a0906c) */
    s16 windowRows;              /* +0x10 */
    s16 rowCount;                /* +0x12 */
    CardDeckEntry entries[40];   /* +0x14 */
} CardDeckListView;

/*
 * Title list window as CardStrListDisp and CardEnemyListDisp read it. Same
 * layout as CHPTitleList in card_disp.c, which names +0x0A rowHeight and
 * leaves it signed; here it is the unsigned byte count (`lhu`) printed per row.
 */
typedef struct CHPTitleListView {
    s16 cursor;                  /* +0x00 */
    u8 unmodeled_02[4];          /* +0x02: flags of card_disp.c, not read here */
    s16 x;                       /* +0x06: screen column */
    s16 y;                       /* +0x08: screen row */
    u16 textLength;              /* +0x0A: characters of a row string that are printed */
    s16 scroll;                  /* +0x0C */
    s16 step;                    /* +0x0E */
    s16 count;                   /* +0x10: rows drawn */
    s16 limit;                   /* +0x12: rows at or past this print nothing */
} CHPTitleListView;

/* The ten 16-byte deck names the deck list prints (0x00A43508, 0xA0 bytes). */
extern char DeckListStr[10][16];

#define DECK_LIST_TEXT_COLOR 0xfff0   /* `ori $6,$0,0xFFF0` before every xglFontPrint (0x00a0d5ec) */
#define DECK_LIST_ROW_HEIGHT 24       /* pixels between rows (`addiu $19,$19,0x18`, 0x00a0d6fc) */
#define DECK_DISP_ROW_HEIGHT 25       /* pixels between rows of CardDeckDisp (`addiu $21,$21,0x19`, 0x00a0904c) */
#define DECK_DISP_CHAR_WIDTH 10       /* pixels per name character (textLength * 10: `sll 2`/`addu`/`sll 1`, 0x00a0901c) */
#define DECK_DISP_COUNT_GAP 6         /* gap before the copy count (`addiu $4,$4,0x6`, 0x00a09034) */
#define CARD_CONDITION_COUNT 4        /* requirement bytes per card (CardDefinition.need; loop `slti $4,$7,0x4`, 0x00a0b7a0) */
#define CARD_CHECK_MARK_COUNT 40      /* CardPlayChkCondition mark buffer size (clear loop `addiu $9,$0,0x27`, 0x00a0cb58) */
#define CARD_FAKE_PICK_COUNT 3        /* cards CardPlayInitWorkFake picks for the opening hand (`slti $2,$18,0x3`, 0x00a0b7c8) */
#define CARD_LIST_COPY_LIMIT 10       /* CardListDisp: owned counts from this value up are shown as CARD_LIST_COPY_CAP (`slti $2,$6,0xA`, 0x00a0d484) */
#define CARD_LIST_COPY_CAP 9          /* the count shown for owned counts of CARD_LIST_COPY_LIMIT or more (`movz $6,$3,$2` with $3 = 9, 0x00a0d48c) */
#define CARD_DATA_BUFFER 0x01000000   /* card text data; a definition's nameOffset is relative to it (0x00a08ef4) */

/*
 * Window strings of the original rodata (named by their address while it owns them,
 * docs/naming.md). None of the ones below is in use by an assembler function any more
 * (CardPlayDispBattle references none); they stay extern for two reasons. Most are
 * printed by several functions of this TU, so they need one shared label, while a C
 * literal would give every function its own copy and a data-carve row hands the label
 * to a single C user (data-carve rows are per C owner). The single-user ones (cardDeckHeaderFontCode in CardDeckDisp,
 * cardDeckCaption in CGCPYamaDispSub, cardJunkCaption in CGCPJunkDispSub) carry original bytes
 * after the terminating NUL (a 0x0D at +3, "\x19\x03Deck" and "\x19\x03Junk" from +2) that
 * a plain literal followed by zero padding does not reproduce.
 */
extern const char cardDeckHeaderFontCode[];    /* "\x0b\x19" (a 0x0D follows the NUL): CardDeckDisp header */
extern const char cardRowPrefixFontCode[];    /* "\x19\x03": row prefix printed at the origin */
extern const char cardCopyCountFontCode[];    /* "\x0c@@": marker before the copy count */
extern const char cardCopyCountFormat[];    /* "%d": copy count format */
extern const char cardSelectedRowFontCode[];    /* "\x0c\x80": cursor marker of the selected row */
extern const char cardOtherRowFontCode[];    /* "\x0b\r": marker of the other rows */
extern const char cardWindowFooterFontCode[];    /* "\x0b\x19": window footer */
extern const char cardWindowHeaderFontCode[];    /* "\x19\x03\r": window header */
extern const char cardJunkCaption[];    /* "\r" then, after the NUL, "\x19\x03Junk": caption printed by CGCPJunkDispSub */
extern const char cardTextLineFontCode[];    /* "\x0b\r": font escape pair printed before each CardDispTxtSub line */
extern const char cardDeckCaption[];    /* "\r" then, after the NUL, "\x19\x03Deck": caption printed by CGCPYamaDispSub */
extern const char cardEnemyListHeaderFontCode[];    /* enemy list window header (CardEnemyListDisp) */
extern const char cardUnownedRowText[];    /* placeholder of an unowned list row (CardListDisp) */

#endif /* SRC_OV10_CARD_FREAD_H */
