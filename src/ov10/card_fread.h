/*
 * TU-local declarations of ov10/tu002 (src/ov10/card_fread.c).
 */

#ifndef SRC_OV10_CARD_FREAD_H
#define SRC_OV10_CARD_FREAD_H

#include "shared.h"
#include "ov10/cgp.h"
#include "ov10/card_play_sound1.h"
#include "ov10/card_fread.h"

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
 * A card leaving a pile slides to its destination in CARD_SLIDE_FRAMES (ov10/cgp.h) frames: the
 * move record's timer (`addiu $9,$0,0xA`, 0x00a0bc7c) and the divisor of its
 * per-frame step (`div.s` by 10.0f, 0x00a0c294; folded into the +-0.1265f
 * constants 0x3E018937/0xBE018937 at 0x00a0c4c0 and 0x00a0bdc4).
 */
#define CARD_SUTE_PILE_X 1.265f      /* discard pile position, 0x3FA1EB85 (`lui`, 0x00a0bc60) */
#define CARD_SUTE_PILE_Y (-1.81f)    /* 0xBFE7AE14 (`lui`, 0x00a0bc70) */

/*
 * Generation markers drawn by CGCPGeneDispSub: one icon model per generation
 * slot of CardCountGeneration, CARD_MODEL_STRIDE bytes apart from CARD_ICON_MODEL_BASE (`lui`/`ori`
 * at 0x00a112ac, `sll 10` at 0x00a11274), all with the texture at 0x01C91000
 * (`lui`/`ori` at 0x00a112a0); the icons of a row are 0.155 apart (0x3E1EB852,
 * 0x00a11234). The slot loop runs while slot < 5 (`slti $2,$20,0x5`, 0x00a112e4).
 */
#define CARD_GENERATION_COUNT 5
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

#define CARD_SLOT_STRIDE_HALFWORDS (sizeof(CardBoardSlot) / sizeof(s16)) /* halfwords between the card ids of two neighbouring board slots (0xDE) */
#define CARD_JUNK_SLOT_COUNT 40      /* junk[] entries, `slti 0x28` at 0x00a0c2e8 */
#define CARD_BATTLE_LAYER_COUNT 40   /* CardLayerStack.layers[] entries (0x27 countdown, 0x00a0c354) */

/* CardPlayTypeSort sorts at most this many cards (two s16 arrays of 40 on its 0xB0 frame) and
 * runs one pass per card kind (`slti $2,$16,3`, 0x00a0c8c8). */
#define CARD_SORT_MAX_CARDS 40
#define CARD_SORT_KIND_COUNT 3

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

/* The card-model fade state in CardGameWork (+0x7100..+0x71A0, ov10/cgp.h):
 * four model slots, of whose four color channels the fades touch 0..2. */
#define CARD_MODEL_COLOR_CHANNELS 3

#define CARD_TEXT_SECOND_CHAR 1  /* byte after the backslash that CardNameConvert tests */
#define CARD_TEXT_THIRD_CHAR 2   /* byte after that: a backslash gives "#73", a '%' gives "#23" */
#define CARD_BYTE_BITS 8         /* bits of a byte: the colour value is stored high byte first */
#define CARD_COLOR_CODE_HIGH 3   /* colorCode[3]: high byte of the 16-bit colour value, after the 3-byte prefix 0x15 0x03 0x03 */
#define CARD_COLOR_CODE_LOW 4    /* colorCode[4]: low byte of the colour value */
#define CARD_COLOR_CODE_END 5    /* colorCode[5]: terminating zero */
#define CARD_ROW_PREFIX_BYTES 2  /* escape bytes 0x19 0x03 in front of the text of a list row */

/*
 * Card range, texture and model addresses shared by CardDispLight (asm 0x00a094b0),
 * CardDispCard (asm 0x00a0a068) and CardDisp (asm 0x00a0a2f0).
 */
#define CARD_DISP_ID_COUNT 0x94                     /* sltiu 0x94, 0x00a0a0a8 */
#define CARD_MASTER_TABLE ((CardDefinition *)0x01000010) /* 0x00a0a0d8 */
#define CARD_TEXTURE_BASE 0x0100C000                /* 0x00a0a0e4 */
#define CARD_TEXTURE_STRIDE 0x0000BAB0              /* 0x00a0a0b0 */
#define CARD_TEXTURE(cardId) ((const char *)(CARD_TEXTURE_BASE + (cardId) * CARD_TEXTURE_STRIDE)) /* texture of a card face */
#define CARD_DISP_ENABLED 0x01                      /* bit 0 of the flags: draw at all (`andi 0x1`, 0x00a0a350) */
#define CARD_DISP_OVERLAY 0x02                      /* andi 0x2, 0x00a0a2a0 */
#define CARD_DISP_LIGHT_MIDDLE 0x40                 /* andi 0x40, 0x00a0a0f8 */
#define CARD_DISP_LIGHT_CN 0x80                     /* andi 0x80, 0x00a0a120 */
#define CARD_MODEL_FRAME 0x016F8500                 /* 0x00a0a150 */
#define CARD_MODEL_FACE 0x016F8100                  /* 0x00a0a200 */
#define CARD_MODEL_DEFAULT 0x01701F80               /* 0x00a0a1f4 */
#define CARD_MODEL_KIND1 0x016F8900                 /* 0x00a0a230 */
#define CARD_MODEL_KIND2 0x016FD440                 /* ori 0x00a0a240 */
#define CARD_MODEL_OVERLAY 0x01706AC0               /* 0x00a0a2bc */
#define CARD_OVERLAY_TEXTURE ((const char *)0x016E0000) /* 0x00a0a250 */

#define CARD_BACK_TEXTURE CARD_FRAME_TEXTURE /* lui 0x00a0a000 */
#define CARD_BACK_MODEL 0x01713600                      /* lui 0x00a0a010 */

/* Hit points are shown only when no flag above bit 4 is set (andi 0xFFE0, 0x00a09628). */
#define CARD_FLAGS_NO_HIT_POINTS 0xFFE0

/* Icon textures and model bases of CardDispLight. */
#define CARD_ATTACK_TEXTURE ((const char *)0x01C94050)  /* lui 0x00a09e08 */
#define CARD_RARITY_TEXTURE ((const char *)0x01C9BB70) /* card_Rare.xtx (lui 0x00a09f1c) */
#define CARD_DIGIT_MODEL 0x01C80000         /* digit models 0..9 (lui 0x00a09674 as a float) */
#define CARD_NEED_MODEL 0x01C85000          /* requirement icons (float 0x4BE42800, lui 0x00a09c98) */
#define CARD_ATTACK_MODEL 0x01C89400        /* attack type icons (float 0x4BE44A00, lui 0x00a09e0c) */
#define CARD_MODEL_RARITY_P 0x01C90730      /* card_m_p.lex (lui 0x00a09ec8) */
#define CARD_MODEL_RARITY_R 0x01C90220      /* card_m_r.lex (lui 0x00a09ee4) */
#define CARD_MODEL_RARITY_U 0x01C8FD10      /* card_m_u.lex (lui 0x00a09ef4) */
#define CARD_MODEL_RARITY_C 0x01C8F800      /* card_m_c.lex, every other letter (lui 0x00a09ee0) */
#define CARD_RARITY_P 0x50                  /* rarity letter 'P' (`addiu $2,$0,0x50`, 0x00a09ecc) */
#define CARD_RARITY_R 0x52                  /* rarity letter 'R' (`addiu $2,$0,0x52`, 0x00a09ee8) */
#define CARD_RARITY_U 0x55                  /* rarity letter 'U' (`addiu $2,$0,0x55`, 0x00a09ef8) */
#define CARD_RARITY_O 0x4F                  /* letter 'O', compared in the original switch but never matches (`slti $3,0x51` bound, 0x00a09ed8) */
#define CARD_KIND_UNUSED_3 3                /* kind 3 is compared in the overlay switch but no card has it (`slti $3,0x3`, 0x00a09f6c) */
#define CARD_NEED_ICON_ANY 6                /* requirement icon index of CARD_NEED_ANY, 0x40C00000 (0x00a09c58) */
#define CARD_NEED_ICON_NONE 8               /* no requirement icon is drawn, 0x41000000 (0x00a09c68) */
#define CARD_ICON_INDEX_NONE (-1)           /* no attack icon is drawn, 0xBF800000 (0x00a09de0) */
#define CARD_FACE_X (-0.436f)               /* face model offset, 0xBEDF3B64 (0x00a09580) */
#define CARD_FACE_Y 0.847f                  /* 0x3F58D4FE (0x00a09590) */
#define CARD_FACE_Z 0.005f                  /* 0x3BA3D70A (0x00a0959c) */

#define CARD_DISP_MARKER 0x04                       /* andi 0x4, 0x00a0aaf0 */
#define CARD_UNOWNED_TEXTURE ((const char *)0x01C74000) /* lui 0x00a0a93c */
#define CARD_UNOWNED_MODEL 0x01C73000               /* lui 0x00a0a948 */
#define CARD_DISP_FACE_X (-0.41599998f)             /* face model offset of CardDisp, 0xBED4FDF3 (0x00a0a398); -0.416f encodes 0xBED4FDF4 */
#define CARD_DISP_MARKER_Y (-1.2f)                  /* selected marker offset, 0xBF99999A (0x00a0aafc) */
#define CARD_DISP_MARKER_Z 0.8f                     /* 0x3F4CCCCD (0x00a0ab0c) */
#define CARD_DISP_MARKER_SCALE_X 0.88f              /* 0x3F6147AE (0x00a0ab48) */
#define CARD_DISP_MARKER_SCALE_Y 0.92f              /* 0x3F6B851F (0x00a0ab54) */
#define CARD_TEXT_TITLE_X 0x78                      /* addiu 0x78, 0x00a0aa04 */
#define CARD_TEXT_TITLE_Y 0x120                     /* addiu 0x120, 0x00a0aa10 */
#define CARD_TEXT_X 0x18                            /* addiu 0x18, 0x00a0aa28 */
#define CARD_TEXT_CAPTION_Y 0x138                   /* addiu 0x138, 0x00a0aa2c */
#define CARD_TEXT_CAPTION_COLOR 0x17                /* addiu 0x17, 0x00a0aa3c */
#define CARD_TEXT_DESCRIPTION_Y 0x19C               /* addiu 0x19C, 0x00a0aa70 */


/* Definition flags that let a card skip the slot check (andi 0x64 at asm 0x00a0c938). */
#define CARD_FLAGS_NO_SLOT_CHECK 0x64

/* CardPlayChkCondition results other than 0 (the card may be played). */
#define CARD_CONDITION_NEED_UNMET 2       /* `addiu $2,$0,0x2` (0x00a0cd00) */
#define CARD_CONDITION_DISCARD_UNMET 4    /* `addiu $2,$0,0x4` (0x00a0ce74) */
#define CARD_CONDITION_IDENTITY_TAKEN 5   /* `addiu $2,$0,0x5` (0x00a0ceac) */
#define CARD_IDENTITY_COUNT 0x80          /* identity codes from here up are never checked */

#define CARD_CURSOR_EDGE_HIGHLIGHT 0.175f    /* highlight size at positions 0 and 3: 0x3E333333 (0x00a0cf80) */
#define CARD_CURSOR_INNER_HIGHLIGHT 0.355f   /* highlight size at positions 1 and 2: 0x3EB5C28F (0x00a0d048) */
#define CARD_CURSOR_INDEX_COUNT 4            /* positions inside a zone (`slti $2,$2,4`, 0x00a0cf50) */
#define CARD_CURSOR_SHOW_CARD_FLAG 0x10      /* drawFlags bit: also draw the card (`andi $2,$7,0x10`, 0x00a0cf8c) */
#define CARD_CURSOR_CARD_ID_MASK 0xF0FF      /* 0x00a0cfbc */
#define CARD_CURSOR_PAIR_ZONE_MASK 0xFE      /* zones 1 and 3 fold onto 0 and 2 (0x00a0d08c) */
#define CARD_CURSOR_PREVIEW_Z 0.1f           /* depth of the previewed card, 0x3DCCCCCD (0x00a0cfd0) */
#define CARD_CURSOR_ZONE_1 1                 /* cursor zone numbers (CardCursorPosition.zone, cursorZone) */
#define CARD_CURSOR_ZONE_2 2                 /* `addiu $2,$0,0x2` (0x00a0d06c) */
#define CARD_CURSOR_ZONE_3 3                 /* zone taken when the hand is empty is the same number (0x00a0d1bc) */
#define CARD_CURSOR_ZONE_4 4                 /* hand zone (0x00a0d170) */
#define CARD_CURSOR_ZONE_5 5                 /* 0x00a10188 */
#define CARD_CURSOR_ZONE_6 6                 /* mark zones 2..7 form the jump table of CardCursorActive (sltiu 0x6, 0x00a10130) */
#define CARD_CURSOR_ZONE_7 7
#define CARD_CURSOR_MARK_COUNT 24            /* marks in markTable and markTableB: six columns of four (counter 0x17, 0x00a100f8) */
#define CARD_CURSOR_MARK_1 1                 /* mark codes: the mode stored by CardCursorActive; CAMASub maps 2, 3 and 4 to mark models */
#define CARD_CURSOR_MARK_2 2                 /* `addiu $3,$0,0x2` (0x00a101e0) */
#define CARD_CURSOR_MARK_3 3                 /* 0x00a101f0 */
#define CARD_CURSOR_MARK_4 4                 /* 0x00a101fc */
#define CARD_CURSOR_JUMP_BUTTON 0x4000       /* button that jumps from position 1 or 2 to zone 2 (0x00a0d064) */

#define CARD_CURSOR_HAND_HIGHLIGHT 0.175f    /* hand cursor highlight size: 0x3E333333 (lui/ori at 0x00a0d190) */
#define CARD_CURSOR_ZONE_BACK_BUTTON 0x1000  /* steps the zone back (`andi $2,$17,0x1000`, 0x00a0d0d0) */
#define CARD_CURSOR_INDEX_FORWARD_BUTTON 0x2000 /* steps the index forward (`andi $2,$17,0x2000`, 0x00a0d0f0) */
#define CARD_CURSOR_ZONE_FORWARD_BUTTON 0x4000  /* steps the zone forward (`andi $2,$17,0x4000`, 0x00a0d0f8) */
#define CARD_CURSOR_INDEX_BACK_BUTTON 0x8000    /* steps the index back (`andi $2,$17,0x8000`, 0x00a0d11c) */
#define CARD_CURSOR_ZONE_LIMIT 5             /* zones 5 and above clamp to the hand zone (`slti $2,$2,5`, 0x00a0d150) */
#define CARD_CURSOR_HAND_ZONE 4              /* zone of the hand (`slti $2,$2,4`, 0x00a0d170) */
#define CARD_CURSOR_EMPTY_HAND_ZONE 3        /* zone taken when the hand is empty (`addiu $2,$0,3`, 0x00a0d1bc) */
#define CARD_CURSOR_ZONE_STRIDE 4            /* indices per zone in CursorTbl (`sll $2,$2,2`, 0x00a0d1e8) */

#define CARD_BATTLE_ARRIVE_SOUND 0x2000F      /* sound when a card lands (0x00a0db9c) */
#define CARD_BATTLE_MARKER_TEXTURE ((const char *)0x01F70200)
#define CARD_BATTLE_DIGIT_TEXTURE ((const char *)0x01C91000)
#define CARD_BATTLE_FRAME_MODEL 0x01714600
#define CARD_BATTLE_SCALE 0.355f              /* card size in the slot, 0x3EB5C28F (0x00a0dacc) */
#define CARD_BATTLE_STACK_SCALE 0.5070423f    /* stacked card size, 0x3F01CD86 (0x00a0dadc) */
#define CARD_BATTLE_PLATE_FRAME_MODEL 0x01E30000
#define CARD_BATTLE_PLATE_MODEL 0x01E38000
#define CARD_BATTLE_PLATE_X (-0.345f)         /* 0xBEB0A3D7 (0x00a0de64) */
#define CARD_BATTLE_PLATE_Y 0.192f            /* 0x3E449BA6 (0x00a0de70) */
#define CARD_BATTLE_ICON_Z 0.05f              /* 0x3D4CCCCD (0x00a0de4c) */
#define CARD_BATTLE_INFO_Z 0.01f              /* 0x3C23D70A (0x00a0df24) */
#define CARD_BATTLE_ATTACK_X 0.131f           /* 0x3E0624DD (0x00a0df58) */
#define CARD_BATTLE_ATTACK_Y (-0.0981f)       /* 0xBDC8E8A7 (0x00a0df64) */
#define CARD_BATTLE_ATTACK_SCALE 1.5f         /* 0x3FC00000 (0x00a0df50) */
#define CARD_BATTLE_ATTACK_MODEL 0x01C89400
#define CARD_BATTLE_ICON_TEXTURE ((const char *)0x01C94050)
#define CARD_BATTLE_POWER_MAX 99              /* `addiu $2,$0,99` (0x00a0e0c0) */
#define CARD_BATTLE_POWER_SCALE 1.2f          /* 0x3F99999A (0x00a0e0d4) */
#define CARD_BATTLE_TENS_X 0.371f             /* 0x3EBDF3B6 (0x00a0e10c) */
#define CARD_BATTLE_UNITS_X 0.455f            /* 0x3EE8F5C3 (0x00a0e178) */
#define CARD_BATTLE_POWER_Y 0.1079f           /* 0x3DDCFAAD (0x00a0e118) */
#define CARD_BATTLE_PIP_X (-0.15f)            /* 0xBE19999A (0x00a0e1f8) */
#define CARD_BATTLE_PIP_Y (-0.2621f)          /* 0xBE8631F9 (0x00a0e208) */
#define CARD_BATTLE_PIP_STEP 0.14f            /* 0x3E0F5C29 (0x00a0e244) */
#define CARD_BATTLE_PIP_ON_MODEL 0x01901000
#define CARD_BATTLE_PIP_OFF_MODEL 0x01901400
#define CARD_BATTLE_EQUIP_X (-0.3589f)        /* 0xBEB7C1BE (0x00a0e2f8) */
#define CARD_BATTLE_EQUIP_Y (-0.1832f)        /* 0xBE3B98C8 (0x00a0e304) */
#define CARD_BATTLE_LIFE_SCALE 1.4f           /* 0x3FB33333 (0x00a0e2e4) */
#define CARD_BATTLE_LIFE_Y 0.277f             /* 0x3E8DD2F2 (0x00a0e444) */
#define CARD_BATTLE_BAR_X (-0.297f)           /* 0xBE981062 (0x00a0e544) */
#define CARD_BATTLE_BAR_Y 0.274f              /* 0x3E8C49BA (0x00a0e564) */
#define CARD_BATTLE_BAR_Z (-0.01f)            /* 0xBC23D70A (0x00a0e570) */
#define CARD_BATTLE_BAR_FULL 10.0f            /* life shown at full bar width, 0x41200000 (0x00a0e550) */
#define CARD_BATTLE_BAR_TEXTURE ((const char *)0x01985400)
#define CARD_BATTLE_BAR_MODEL 0x01919800
#define CARD_BATTLE_MARKER_MODEL_BIT0 0x01900000
#define CARD_BATTLE_MARKER_MODEL_BIT1 0x01900800
#define CARD_BATTLE_REFLECT_CARD 15           /* a card of this id gains a power per marked slot (0x00a0e070) */
#define CARD_BATTLE_REFLECT_FLAG 8            /* CardDefinition.flags bit it counts (`andi $2,$2,8`, 0x00a0e0b0) */

/*
 * Pile counters shared by CardPlayDispHand, CardPlayDispJunk and CardPlayDispSute: the pile size is shown as
 * tens (model CARD_PILE_COUNT_MODEL + (tens << 10)) and units (+ (units << 10)) digit models
 * (asm 0x00a0ef24 / 0x00a0efac and 0x00a0fe30 / 0x00a0feb4), both with CARD_PILE_COUNT_TEXTURE.
 */
#define CARD_PILE_STACK_SIZE 10               /* `addiu $2,$0,10` before the div (0x00a0ee9c) */
#define CARD_PILE_COUNT_MODEL 0x01C80000
#define CARD_PILE_COUNT_TEXTURE ((const char *)0x01C91000)
#define CARD_PILE_COUNT_SCALE 1.4f            /* 0x3FB33333 (0x00a0eeac) */
#define CARD_PILE_COUNT_Y (-1.876f)           /* 0xBFF020C5 (0x00a0ef08) */
#define CARD_PILE_COUNT_Z 0.01f               /* 0x3C23D70A (0x00a0eeb8) */
#define CARD_PILE_CARD_SCALE 0.175f           /* size of a pile card in motion, 0x3E333333 (0x00a0eff4) */
#define CARD_MOVE_ARRIVE_SOUND 0x2000F        /* sound when a card lands (0x00a0eb68, 0x00a0f0e8, 0x00a0f304, 0x00a0f754, 0x00a0fc14, 0x00a0ffd8) */

#define CARD_HAND_TENS_X (-0.964f)           /* digit x of the tens count, 0xBF76C8B4 (0x00a0e854) */
#define CARD_HAND_UNITS_X (-0.85f)           /* digit x of the units count, 0xBF59999A (0x00a0e8c4) */
#define CARD_HAND_SETTLE_DEPTH 0.005f        /* depth step between the hand cards, 0x3BA3D70A (0x00a0ec20) */
#define CARD_HAND_FULL_SCALE 2.0f            /* full flip scale of a card, 0x40000000 (0x00a0e9d0) */
#define CARD_HAND_FLAG_BITS 0x0F00           /* flag bits cleared from a hand entry before drawing (addiu -0xF01, 0x00a0ed8c) */

#define CARD_JUNK_COUNT_TENS_X 1.919f         /* 0x3FF5A1CB (0x00a0eefc) */
#define CARD_JUNK_COUNT_UNITS_X 2.033f        /* 0x40021CAC (0x00a0ef78) */
#define CARD_JUNK_TOP_DROP 0.02f              /* resting top card sits this far below the pile, 0x3CA3D70A (0x00a0f044) */

#define CARD_DISPOSE_SCALE 0.175f             /* card size in the slot, 0x3E333333 (0x00a0f240) */
#define CARD_DISPOSE_STACK_SCALE 0.5070423f   /* stacked card size, 0x3F01CD86 (0x00a0f254) */
#define CARD_DISPOSE_DIGIT_Z 0.05f            /* digit depth, 0x3D4CCCCD (0x00a0f554) */
#define CARD_DISPOSE_MARKER_TEXTURE ((const char *)0x01F70200) /* flag markers (0x00a0f4a8, 0x00a0f4f8) */
#define CARD_DISPOSE_MARKER_MODEL_BIT0 0x01900400
#define CARD_DISPOSE_MARKER_MODEL_BIT1 0x01900C00

#define CARD_OPERATION_SCALE 0.175f           /* card size, 0x3E333333 (0x00a0f6ec) */
#define CARD_OPERATION_MARKER_Z 0.01f         /* marker depth (0x00a0f894) */
#define CARD_OPERATION_MARKER_TEXTURE ((const char *)0x01F70200)
#define CARD_OPERATION_MARKER_MODEL 0x01900400
#define CARD_OPERATION_FLAG_MARKER 1          /* operationFlags bit (`andi $2,1`, 0x00a0f87c) */

#define CARD_YAMA_COUNT_SCALE 1.7f            /* digit scale of the deck count, 0x3FD9999A (0x00a0f9b4) */
#define CARD_YAMA_COUNT_TENS_X 0.401f         /* 0x3ECD4FDF (0x00a0fa00) */
#define CARD_YAMA_COUNT_UNITS_X 0.547f        /* 0x3F0C0831 (0x00a0fa80) */
#define CARD_YAMA_COUNT_Y (-1.855f)           /* 0xBFED70A4 (0x00a0fa0c) */
#define CARD_YAMA_MARKER_Y (-1.81f)           /* 0xBFE7AE14 (0x00a0fafc) */
#define CARD_YAMA_MARKER_TEXTURE ((const char *)0x016E0000)   /* (0x00a0fb3c) */
#define CARD_YAMA_MARKER_MODEL 0x01911800     /* (0x00a0fb44) */
#define CARD_YAMA_MARKER_MODEL_ENEMY 0x01915800 /* (0x00a0fb5c) */
#define CARD_YAMA_MARKER2_X (-0.484f)         /* 0xBEF7CED9 (0x00a0fb60) */
#define CARD_YAMA_MARKER2_Y (-1.851f)         /* 0xBFECED91 (0x00a0fb70) */
#define CARD_YAMA_MARKER2_TEXTURE ((const char *)0x01D80000)  /* (0x00a0fb9c) */
#define CARD_YAMA_MARKER2_MODEL 0x01D18000    /* (0x00a0fba4) */
#define CARD_YAMA_MARKER2_MODEL_ENEMY 0x01D1C000 /* ori 0x00a0fbbc */

#define CARD_SUTE_COUNT_TENS_X 0.889f         /* 0x3F639581 (0x00a0fe08) */
#define CARD_SUTE_COUNT_UNITS_X 1.0f          /* units digit x offset */
#define CARD_SUTE_MARKER_TEXTURE ((const char *)0x016E0000)
#define CARD_SUTE_MARKER_MODEL 0x01901800

#define CARD_CURSOR_FRAME_MODEL 0x01D00000       /* own side (0x00a1031c) */
#define CARD_CURSOR_FRAME_MODEL_ENEMY 0x01D02000 /* (0x00a10328) */
#define CARD_CURSOR_MARK_MODEL_2 0x01D0C000      /* CAMASub: model of mark code 2 */
#define CARD_CURSOR_MARK_MODEL_3 0x01D10000      /* mark code 3 */
#define CARD_CURSOR_MARK_MODEL_4 0x01D12000      /* mark code 4 */
#define CARD_CURSOR_MARK_MODEL_DEFAULT 0x01D08000 /* every other mark code */
#define CARD_CURSOR_MARKER_MODEL 0x01D18000      /* (0x00a102d8) */
#define CARD_CURSOR_MARKER_MODEL_ENEMY 0x01D1C000 /* (0x00a1032c) */
#define CARD_CURSOR_MARKER_TEXTURE ((const char *)0x01D80000) /* (0x00a1047c) */
#define CARD_CURSOR_FRAME_STRIDE 0x510           /* bytes between two animation frames (built from shifts and an add: `sll 2` 0x00a10374, `sll 4` 0x00a10398) */
#define CARD_CURSOR_ANIM_TICKS 24                /* animFrame counts 0..23 (sltiu 0x18, 0x00a10350) */
#define CARD_CURSOR_ANIM_TICKS_PER_FRAME_SHIFT 2 /* four ticks per frame (srl 2, 0x00a10358) */
#define CARD_CURSOR_SCALE 1.15f                  /* 0x3F933333 (0x00a10368) */
#define CARD_CURSOR_MARKER_X 0.275f              /* 0x3E8CCCCD (0x00a1043c) */
#define CARD_CURSOR_MARKER_Y 0.4f                /* 0x3ECCCCCD (0x00a10448) */
#define CARD_CURSOR_PULSE_A_MASK 0x1F            /* andi 0x1F (0x00a104ac) */
#define CARD_CURSOR_PULSE_A_STEP 11.25f          /* degrees per step, 0x41340000 (0x00a104bc) */
#define CARD_CURSOR_LIGHT_A_SWING 0.5f           /* CLColor swing around 1.0, 0x3F000000 (0x00a10504) */
#define CARD_CURSOR_LIGHT_B_SWING 0.25f          /* CLP1Color blue / CLP2Color red swing, 0x3E800000 (0x00a104e4) */
#define CARD_CURSOR_LIGHT_B_BASE 1.75f           /* 0x3FE00000 (0x00a1053c) */
#define CARD_CURSOR_LIGHT_C_SWING 0.235f         /* CLP1Color red and green swing, 0x3E70A3D7 (0x00a10574) */
#define CARD_CURSOR_LIGHT_C_BASE 0.735f          /* 0x3F3C28F6 (0x00a10588) */
#define CARD_CURSOR_LIGHT_D_SWING 0.285f         /* CLP2Color green and blue swing, 0x3E91EB85 (0x00a105bc) */
#define CARD_CURSOR_LIGHT_D_BASE 0.685f          /* 0x3F2F5C29 (0x00a105c8) */
#define CARD_CURSOR_LIGHT_E_SWING 0.25f          /* CMColor swing around 1.0: the same word as the B swing (0x00a104e4) */
#define CARD_CURSOR_PULSE_B_MASK 0x3F            /* andi 0x3F (0x00a10620) */
#define CARD_CURSOR_PULSE_B_STEP 5.625f          /* degrees per step, 0x40B40000 (0x00a10630) */
#define CARD_CURSOR_MARK_Z_LIFT 0.015f           /* 0x3C75C28F (0x00a10648) */
#define CARD_CURSOR_MARK_SCALE_SMALL 0.175f      /* disposal and operation marks, 0x3E333333 (0x00a1065c) */
#define CARD_CURSOR_MARK_SCALE_LARGE 0.355f      /* battle marks, 0x3EB5C28F (0x00a10744) */

#define CARD_HAND3_BASE_Y 1.5f                 /* 0x3FC00000, 0x00a11370 */
#define CARD_HAND3_BASE_Y_OPTION 1.74f         /* 0x3FDEB852, 0x00a11384 */
#define CARD_HAND3_BASE_Z 0.15f                /* 0x3E19999A, 0x00a113a4 */
#define CARD_HAND3_BASE_SCALE_Y 0.97f          /* 0x3F7851EC, 0x00a113b4 */
#define CARD_HAND3_SCALE 0.23f                 /* 0x3E6B851F, 0x00a11474 */
#define CARD_HAND3_SELECTED_SCALE 0.3f         /* 0x3E99999A, 0x00a11480 */
#define CARD_HAND3_SELECTED_TEXTURE ((const char *)0x01D60000) /* 0x00a116a0 */
#define CARD_HAND3_SELECTED_MODEL 0x01D12000   /* 0x00a116a4 */
#define CARD_HAND3_BASE_TEXTURE ((const char *)0x019B0000)     /* 0x00a11788 */
#define CARD_HAND3_BASE_MODEL 0x019AC000       /* 0x00a1178c */

#define CARD_HAND2_BASE_Y (-1.7f)               /* 0xBFD9999A, 0x00a1187c */
#define CARD_HAND2_BASE_Z 0.1f                  /* 0x3DCCCCCD, 0x00a11888 */
#define CARD_HAND2_BASE_SCALE_Y 0.97f           /* 0x3F7851EC, 0x00a11898 */
#define CARD_HAND2_SCALE 0.23f                  /* 0x3E6B851F, 0x00a1195c */
#define CARD_HAND2_SELECTED_SCALE 0.3f          /* 0x3E99999A, 0x00a11968 */
#define CARD_HAND2_SELECTED_TEXTURE ((const char *)0x01D60000)  /* 0x00a11b8c */
#define CARD_HAND2_SELECTED_MODEL 0x01D12000    /* 0x00a11b90 */
#define CARD_HAND2_PILE_SCALE 1.2f              /* 0x3F99999A, 0x00a11c60 */
#define CARD_HAND2_PILE_Y (-0.005f)             /* 0xBBA3D70A, 0x00a11c70 */
#define CARD_HAND2_TENS_X 1.116f                /* 0x3F8ED917, 0x00a11cb8 */
#define CARD_HAND2_UNITS_X 1.229f               /* 0x3F9D4FDF, 0x00a11d24 */
#define CARD_HAND2_BASE_TEXTURE ((const char *)0x019B0000)      /* 0x00a11db4 */
#define CARD_HAND2_BASE_MODEL 0x019A8000        /* 0x00a11db8 */

#define CARD_YAMA_MARK_Y 0.369f
#define CARD_YAMA_MARK_Z 0.3f
#define CARD_YAMA_MODEL_BASE 0x01DA0000
#define CARD_YAMA_MODEL_PILE 0x01DAAC00

#define CARD_YAMA_SELECTED_MODE 5
#define CARD_YAMA_MARK_FLAG 0x10               /* flags bit: also draw the marker card (`andi $20,$20,0x10`, 0x00a12060) */
#define CARD_WINDOW_SINGLE_X (-2.0f)           /* x offset of the pile card with a single card, 0xC0000000 (0x00a12224) */
#define CARD_WINDOW_CAPTION_X 22               /* caption position (`addiu $4,$0,0x16`, 0x00a12338) */
#define CARD_WINDOW_CAPTION_Y 140              /* `addiu $5,$0,0x8C` (0x00a1233c) */
#define CARD_WINDOW_CAPTION_COLOR 0xFFF8       /* `ori $6,$0,0xFFF8` (0x00a12344) */

/*
 * main/xgl_2.h and main/nml_model_set.h declare these. main/xgl_2.h defines
 * XglQuadword with a mode(TI) attribute that the ordinary-C header check
 * refuses, so the prototypes below repeat the canonical ones.
 */
extern void xglMatrixUnit(float matrix[4][4]);
void xglMatrixScale(Matrix4 destination, Matrix4 source, const float scale[4]);
void xglMatrixRotY(Matrix4 destination, Matrix4 source, float angle);
void xglMatrixRotZ(Matrix4 destination, Matrix4 source, float angle);
void xglMatrixRotX(Matrix4 destination, Matrix4 source, float angle);
extern float xglSin(float angle);

/*
 * Window edge corners {x, y, z} of the card hand (asm 0x00A43628, 0x18 bytes in the
 * data section): win0pos[0] is the first corner, win0pos[1] the second.
 */
static float win0pos[2][3];
/* Hand fan corners of the in-battle window (0x00A43640, 0x30 bytes, 4x3 floats). */
static float NewWin0pos[4][3];


/*
 * Card display text and icon positions (data section): {x, y, z} offsets of the digit and
 * icon models. Declared here; defined, with the data below, after CardPlayCntFld in card_fread.c.
 */
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

/* Offset of a stacked card within a disposal slot (0x00A435C8) and the x, y positions of the tens (0x00A435D8) and
 * units (0x00A435F8) life digits per slot; read by CardPlayDispDispose. */
static float CardStackPos[3];
static float DisposeHP10pos[][2];
static float DisposeHP01pos[][3];

/* The ten 16-byte deck names the deck list prints (0x00A43508, 0xA0 bytes). */
extern char DeckListStr[10][16];

/*
 * Window strings of the original rodata, defined at the end of card_fread.c. Most are printed by
 * several functions of this TU and so share one label. cardDeckHeaderFontCode, cardDeckCaption
 * and cardJunkCaption have a single user but carry original bytes after the terminating NUL (a 0x0D
 * at +3, "\x19\x03Deck" and "\x19\x03Junk" from +2) that a plain literal followed by zero
 * padding does not reproduce.
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

/* Forward declaration: defined further down, in the original function order of this TU. */
int CDTSGetStrLen(unsigned char *text);


/* Declared here: defined after CardPlayCntFld in card_fread.c. */
static float *CursorTbl[16];

/* Info plate position of each battle slot (0x00A435A8, 0x20 bytes, x and y). */
extern float CPSInfoPos[][2];

#endif /* SRC_OV10_CARD_FREAD_H */
