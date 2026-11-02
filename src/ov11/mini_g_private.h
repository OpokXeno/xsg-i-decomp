/*
 * TU-local declarations of ov11/tu001 (src/ov11/mini_g.c).
 */

#ifndef SRC_OV11_MINI_G_PRIVATE_H
#define SRC_OV11_MINI_G_PRIVATE_H

#include "shared.h"

/* Screen rectangle (x, y, width, height) passed to MakeSprite. */
struct SpriteRect {
    int x;
    int y;
    int w;
    int h;
};

/* Texture descriptor read by MakeSprite (28 bytes, the casino's sprite
 * tables): the source rectangle in the texture, two GS buffer addresses and
 * the texture format. Both addresses are in GS 64-word blocks, relative to
 * block 0x3800: MakeSprite adds 0x3800 before placing them in TEX0. */
struct SpriteTex {
    int u;
    int v;
    int w;
    int h;
    /* TBP0 (texture base) for format 1. The other formats ignore it and use
     * block 0x3800 itself. */
    int texture_base;
    /* CBP (CLUT base), placed at bit 37 of TEX0 for every format. */
    int clut_base;
    /* Picks the rest of the TEX0 word. Decoded from MakeSprite's constants
     * (if I have the GS register layout right): 1 = 4-bit 512x512 at
     * texture_base, 2 = 8-bit 256x256, 3 = 4-bit 1024x1024, anything else =
     * 8-bit 512x512. Formats 1 and 3 also double v. */
    int format;
};

/* Screen origin of the casino's sprites and boxes in GS primitive
 * coordinates (12.4 fixed point: 1792 and 1824 pixels), and the first GS
 * memory block (64 words each) of the casino's texture area, which the
 * SpriteTex buffer addresses are relative to. */
#define CASINO_GS_ORIGIN_X 0x7000
#define CASINO_GS_ORIGIN_Y 0x7200
#define CASINO_GS_TEX_BLOCK 0x3800

/* PRIM values the casino draws with: a textured, alpha-blended triangle
 * strip with fixed (FST) UVs, and the same untextured. */
#define CASINO_PRIM_TEXTURED (GS_PRIM_TRISTRIP | GS_PRIM_TME | GS_PRIM_ABE | GS_PRIM_FST)
#define CASINO_PRIM_FLAT (GS_PRIM_TRISTRIP | GS_PRIM_ABE | GS_PRIM_FST)

/* Slot reels: every reel strip has 21 pictures of 64 pixels, so a reel
 * position (Gwork.slot.reel_pos) wraps after 21 * 64 = 0x540. */
#define SLOT_REEL_CELLS 21
#define SLOT_CELL_SIZE 64
#define SLOT_REEL_LENGTH 0x540

/* The most coins the balance can hold; AddCoin banks the excess. */
#define CASINO_COIN_MAX 9999999

/* GS TEST_1 values passed to SetTest: alpha test GREATER than 0 (pixels
 * with alpha 0 are dropped), with the depth test on and set to GEQUAL or to
 * ALWAYS. */
#define CASINO_TEST_DEPTH_GEQUAL 0x5000D
#define CASINO_TEST_DEPTH_ALWAYS 0x3000D

/* Poker hand categories (the result of PoChk1..PoChk9 and of PoCardCheck,
 * PokerDealWork.hand_rank), in order of rank; 0 is no win. */
#define POKER_HAND_NONE 0
#define POKER_HAND_ONE_PAIR 1
#define POKER_HAND_TWO_PAIR 2
#define POKER_HAND_THREE_OF_A_KIND 3
#define POKER_HAND_STRAIGHT 4
#define POKER_HAND_FLUSH 5
#define POKER_HAND_FULL_HOUSE 6
#define POKER_HAND_FOUR_OF_A_KIND 7
#define POKER_HAND_STRAIGHT_FLUSH 8
#define POKER_HAND_ROYAL_STRAIGHT_FLUSH 9

/* The groups PoMultiChk finds among the five cards, from the number of card
 * pairs that share a rank (1, 2, 3, 4 or 6 pairs). */
#define POKER_MULTI_ONE_PAIR 1
#define POKER_MULTI_TWO_PAIR 2
#define POKER_MULTI_THREE_OF_A_KIND 3
#define POKER_MULTI_FULL_HOUSE 4
#define POKER_MULTI_FOUR_OF_A_KIND 5

/* Slot machine states (Gwork.slot.state, handled by slot_key) and reel
 * states (Gwork.slot.reel_state). */
#define SLOT_STATE_IDLE 0     /* stake entry */
#define SLOT_STATE_SPINNING 1 /* reels spinning or stopping */
#define SLOT_STATE_PAYOUT 2   /* result shown, paid_action runs */
#define SLOT_REEL_STOPPED 0
#define SLOT_REEL_SPINNING 1
#define SLOT_REEL_STOPPING 2  /* stop pressed, sliding to the next cell */

/* Sound effect ids of RES_SoundEffect (indexes of the CASINO.res table),
 * named for the action they accompany in the menu, slot and shop code. */
#define CASINO_SE_MENU_UP 0
#define CASINO_SE_MENU_LEFT 1
#define CASINO_SE_MENU_RIGHT 2
#define CASINO_SE_MENU_DOWN 3
#define CASINO_SE_MENU_DECIDE 4
#define CASINO_SE_SUBMENU_UP 5
#define CASINO_SE_SUBMENU_DOWN 6
#define CASINO_SE_MENU_CANCEL 8
#define CASINO_SE_ENTER_EXCHANGE 9
#define CASINO_SE_ENTER_SHOP 10
#define CASINO_SE_ENTER_VIEWER 11
#define CASINO_SE_ENTER_SLOT 12
#define CASINO_SE_ENTER_POKER 13
#define CASINO_SE_SLOT_LEAVE 0x17
#define CASINO_SE_SLOT_LINE_UP 0x1D
#define CASINO_SE_SLOT_LINE_DOWN 0x1E
#define CASINO_SE_SLOT_STAKE_UP 0x1F
#define CASINO_SE_SLOT_STAKE_DOWN 0x20
#define CASINO_SE_SLOT_STAKE_REFUSED 0x21
#define CASINO_SE_SLOT_SPIN 0x22
#define CASINO_SE_SLOT_REEL_STOP 0x23
#define CASINO_SE_SLOT_POINTER_RIGHT 0x24
#define CASINO_SE_SLOT_POINTER_LEFT 0x25
#define CASINO_SE_SLOT_STOP_IGNORED 0x26
#define CASINO_SE_SLOT_PAYOUT 0x27
#define CASINO_SE_SLOT_WIN_NO_EVENT 0x42
#define CASINO_SE_POKER_WIN 0x38     /* PoCardCheck: pair to flush */
#define CASINO_SE_POKER_WIN_BIG 0x39 /* PoCardCheck: full house and better */
#define CASINO_SE_POKER_NO_WIN 0x3A
#define CASINO_SE_POKER_LEAVE 0x35
#define CASINO_SE_POKER_PAYOUT 0x3B
#define CASINO_SE_EXCHANGE_UP 0xE
#define CASINO_SE_EXCHANGE_DOWN 0xF
#define CASINO_SE_EXCHANGE_BOUGHT 0x10
#define CASINO_SE_EXCHANGE_REFUSED 0x11
#define CASINO_SE_SHOP_BOUGHT 21
#define CASINO_SE_SHOP_REFUSED 22

/* PadData's button words. xglPadRead (src/main/xgl_pad.c) stores four
 * halfwords from +0x28: the buttons held (+0x28), newly pressed (+0x2A),
 * newly pressed with auto-repeat (+0x2C: the newly pressed set, replaced by
 * the held buttons in xglPadSetRepeat's mask whenever the repeat timer
 * fires) and released (+0x2E). All four use the PAD_* bits of shared.h. */

/* Bits of the four words read as one doubleword from +0x28. */
#define PAD_WORDS_PRESSED(bits) ((u64)(bits) << 16)
#define PAD_WORDS_REPEAT(bits) ((u64)(bits) << 32)

/* PadData as the casino reads it. The shared PadPrefix stops at +0x36; this
 * TU also needs the auto-repeat word at +0x2C and the four analog stick
 * bytes at +0x64, so it uses a union with the canonical prefix, the same way
 * src/ov02/umn_guno_data_base_get.c widens it. */
typedef union CasinoPadView {
    PadPrefix canonical;
    /* The four button words from +0x28 read as one doubleword; coin_main
     * tests a pressed bit and a repeat bit together this way. */
    struct {
        unsigned char unmodeled_00[0x28];
        u64 words;
    } dword;
    struct {
        unsigned char unmodeled_00[0x2C];
        u16 repeat;
        unsigned char unmodeled_2e[0x36];
        signed char stick[4];
    } wide;
} CasinoPadView;

/* The poker "Play N coins" message and box width tables. The compiled form
 * copies each table onto the stack as a 16-byte aggregate before indexing
 * it, which is why each table is typed as a one-member array struct rather
 * than a bare array (a bare array cannot be copied by value in C). */
typedef struct {
    const char *text[4];
} PokerReadyMessageTable;

typedef struct {
    int width[4];
} PokerReadyWidthTable;

/* A screen position (x, y) of the nine-game's cursor points. */
struct GuidPoint {
    int x;
    int y;
};

/* A panel position (x, y) of the nine-game's reel shapes. */
struct NgPanelPos {
    short x;
    short y;
};

/* The four stake steps (1, 5, 10, 30) SlCoinEntry copies from the original
 * .rodata to its stack. */
struct StakeSteps {
    short step[4];
};

/* 0x10C in PokerDealWork: the deck (one flag per card) and the hand. */
struct PokerCards {
    short used[53];
    short hand[5];
};

/* Poker sequence states (Gwork.poker_deal.seq), as PokerMain handles them.
 * Nothing in OV11 sets 8 or 10. */
enum PokerSeq {
    POKER_SEQ_READY = 0,         /* stake check, "Play N coins" dialog */
    POKER_SEQ_NO_COINS = 1,      /* "There are no more coins." */
    POKER_SEQ_BET = 2,           /* move the stake into the pot */
    POKER_SEQ_DEAL = 3,          /* deal animation (PoCardDrawAnime) */
    POKER_SEQ_DRAW_HAND = 4,     /* fill the hand (PoShuffleCard) */
    POKER_SEQ_SELECT = 5,        /* choose the cards to replace (PoKey) */
    POKER_SEQ_REPLACE = 6,       /* replace them (PoShuffleCard) */
    POKER_SEQ_SCORE = 7,         /* score the hand (PoCardCheck) */
    POKER_SEQ_CONTINUE = 8,      /* "Continue game?" */
    POKER_SEQ_WIN = 9,           /* goes straight on to 11 */
    POKER_SEQ_DOUBLE_UP = 11,    /* "Double or nothing?" */
    POKER_SEQ_BONUS_START = 12,  /* reset the hand (every card set to 0x36) */
    POKER_SEQ_BONUS_DEAL = 13,   /* slide in the next card (PoBonusCardAnime) */
    POKER_SEQ_BONUS_DRAW = 14,   /* give it a value (PoShuffleCard) */
    POKER_SEQ_BONUS_JUDGE = 15,  /* compare it with the previous card */
    POKER_SEQ_BONUS_LOST = 16,   /* wrong guess: wait for circle */
    POKER_SEQ_BONUS_GUESS = 17,  /* three-choice dialog: guess or collect */
    POKER_SEQ_PAYOUT = 18        /* pay the pot out */
};

struct PokerDealWork {
    unsigned char unmodeled_000[0xC];
    /* 0x0C: the bet level chosen in the menu (see MiniGModeWork). */
    int level;
    unsigned char unmodeled_010[0xFC];
    struct PokerCards cards;
    /* 0x180: PoInitWork clears this word; nothing else in the TU reads or
     * writes it, so its role is not modeled. */
    int unknown_180;
    /* 0x184: nonzero hides the rate table PokerMain draws every frame. */
    int rates_hidden;
    /* 0x188: PokerMain's sequence state, an enum PokerSeq value (stored by
     * PoSeqChange). */
    int seq;
    unsigned char unmodeled_18c[0x4];
    /* 0x190: set by PoCardFlip once all five cards have stayed selected
     * for 32 frames, cleared otherwise. Not sure of the game meaning;
     * PoInitWork's notes call it the bonus mode. */
    int bonus_mode;
    /* 0x194: index of the bonus card PoBonusCardAnime deals next. */
    int bonus_round;
    short replace_selected[5];
    short sorted_hand[5];
    unsigned char unmodeled_1ac[0x4];
    /* 0x1B0: hand category from PoCardCheck (0 = no win). */
    int hand_rank;
    /* 0x1B4: the bonus game's guess: 0 = the next card is not lower than
     * the previous one, 1 = it is lower (a tie wins either way). */
    int guess;
    /* 0x1B8: coins at stake: the bet, then the winnings. */
    int pot;
    /* 0x1BC: 1 while a round is in progress (PokerMain then shows the pot
     * instead of the coin balance). */
    int in_round;
};

/* One slot pay line: the three pictures on it and the result SlPrizeCheck
 * computes for it (prize, kind, and whether the prize is halved). */
struct SlotLine {
    short pic[3];
    short prize;
    short kind;
    short half;
};

/* Slot work. Only the fields the recovered slot code uses are named;
 * the rest stays as unmodeled byte ranges. */
struct SlotWork {
    unsigned char unmodeled_000[0xC];
    /* 0x0C: the bet level chosen in the menu (level_select), which picks
     * the stake step in SlCoinEntry. */
    int level;
    unsigned char unmodeled_010[0x4];
    /* 0x14: reel scroll positions, wrapped back by 0x540 in SlRealDraw. */
    int reel_pos[3];
    /* 0x20: one state per reel. check_mode treats 1 as "a reel can be
     * stopped"; SlStopBtnPrint picks the stop-button texture from 1, 2 or
     * anything else. Not sure of the full set of values. */
    int reel_state[3];
    /* 0x2C: how far each reel has slid since its stop button was pressed
     * (reset by the stop, +0x10 per frame in slot_key). */
    int stop_travel[3];
    /* 0x38: pay-line pointer (0..2) moved by the pad in point_control. */
    int pointer;
    /* 0x3C: guide position drawn by point_control when no reel can be
     * stopped. */
    int guide;
    /* 0x40: one word GameModeChange clears; nothing else in the TU reads or
     * writes it, so its role is not modeled. */
    int unknown_40;
    /* 0x44: 1 shows the help screen (slot_main). */
    int help;
    /* 0x48: reel row used to read the pay lines (SlGetPic), re-rolled by
     * SlChangeRealEffect. */
    short row;
    /* 0x4A: nonzero doubles every prize (SlPrizeCheck). */
    short double_pay;
    /* 0x4C, 0x4E: bonus flag and remaining bonus time, set by SlEvent and
     * SlChangeRealEffect. Not sure of the in-game name of the bonus. */
    short bonus;
    short bonus_time;
    /* 0x50: 2 makes slot_main draw the result table instead of the rate
     * table. Not sure of the other values. */
    int state;
    /* 0x54: total stake, printed by total_pay_print and taken from the
     * coin balance on a spin (slot_key). */
    short total_bet;
    /* 0x56: per-line stakes, printed by pay_print. */
    short line_pay[5];
    /* 0x60: stake steps on each line (0..3), set with left/right in
     * SlCoinEntry; line_pay is this times the current step. */
    short line_bet[5];
    /* 0x6A: the five pay lines (SlLineCheck, SlPrizeCheck, SlResultDraw). */
    struct SlotLine line[5];
    unsigned char unmodeled_0a6[0x2];
    /* 0xA8: coins still to be paid out (paid_action). */
    int payout;
    /* 0xAC: 1 makes paid_action switch to the nine-game. */
    int start_nine_game;
    /* 0xB0: current slot event, passed to SlEvent by slot_main. */
    int event;
};

/* Nine-game reel state: one 8-byte record per reel starting at 0xC2. */
struct NineGameReel {
    short number;
    short position;
    short state;
    short remaining;
};

struct NineGameWork {
    unsigned char unmodeled_000[0xA8];
    /* 0xA8: coins to be paid out, as in the slot view. */
    int payout;
    unsigned char unmodeled_0ac[0x8];
    int flag;
    int counter;
    short mode;
    short guide;
    short time_left;
    struct NineGameReel reel[9];
};

/* Casino game selected by Gwork.modes.game (see MiniG_Main). */
enum CasinoGame {
    CASINO_MENU = 0,
    CASINO_SLOT = 1,
    CASINO_POKER = 2,
    CASINO_COIN_EXCHANGE = 3,
    CASINO_SHOP = 4,
    CASINO_VIEWER = 5,
    CASINO_NINE_GAME = 6,
    CASINO_TEST_MODE = 7
};

/* Game selector and per-game "initialised" flags used by MiniG_Main. */
struct MiniGModeWork {
    unsigned char unmodeled_000[0x4];
    /* 0x04: the row (0..2) exchange_select's three-choice submenu stores.
     * Not sure what the three choices are. */
    int exchange_choice;
    /* 0x08: the current game, an enum CasinoGame value. */
    int game;
    /* 0x0C: the bet level level_select stores (0..3). init_menu_mode and
     * GameModeChange reset it. */
    int level;
    /* 0x10: init_menu_mode clears this halfword; nothing else in the TU
     * reads or writes it, so its role is not modeled. */
    short unknown_10;
    unsigned char unmodeled_012[0x22E];
    short initialized[6];
};

/* Viewer work: the image scroll offsets VW_ViewMode moves and clamps
 * (x to -0x242..0, y to -0x250..0) and VW_ViewModeInit resets, and the
 * sample list VW_SamMode shows. */
struct ViewerWork {
    unsigned char unmodeled_000[0x1C0];
    int scroll_x;
    int scroll_y;
    /* 0x1C8: the enabled sound samples, as VW_SamlistInit lists them. */
    int sample[30];
};

/* One reel-change panel of SlChangeRealEffect. */
struct SlReelPanel {
    int state;
    int delay;
    int size;
};

/* The casino's 0x250-byte local work block (Gwork in mini_g.c), shared by the
 * games; each game reads it through its own view. The word view reaches the
 * fields that are not modeled. */
union CasinoWork {
    int words[148];
    struct PokerDealWork poker_deal;
    struct SlotWork slot;
    struct NineGameWork nine;
    struct MiniGModeWork modes;
    struct ViewerWork viewer;
};

/* Fixed EE RAM buffers the casino loads its assets into (CASINO.res and the
 * textures, in address order); the drawing code uploads each texture from
 * the same address. */
#define CASINO_RES_BUFFER ((void *)0x01000000)
#define SLOT_1_TEX_BUFFER ((void *)0x01008000)
#define SLOT_2_TEX_BUFFER ((void *)0x01048800)
#define BASE_TEX_BUFFER ((void *)0x01089000)
#define SLOT_W1_TEX_BUFFER ((void *)0x010C9800)
#define SLOT_HELP_TEX_BUFFER ((void *)0x0110A000)
#define MENU_BG_TEX_BUFFER ((void *)0x0114A800)
#define MENU_WINDOW_TEX_BUFFER ((void *)0x0118B000)
#define COIN_EXCHANGE_TEX_BUFFER ((void *)0x011CB800)
#define SHOP_EXCHANGE_TEX_BUFFER ((void *)0x0120C000)
#define VW_DATAVIEWER_TEX_BUFFER ((void *)0x0124C800)
#define POKER_SPADE_TEX_BUFFER ((void *)0x0128D000)
#define POKER_CLOVER_TEX_BUFFER ((void *)0x012CD800)
#define POKER_HEART_TEX_BUFFER ((void *)0x0130E000)
#define POKER_DIA_TEX_BUFFER ((void *)0x0134E800)
#define POKER_TABLE1_TEX_BUFFER ((void *)0x0138F000)
#define POKER_TABLE2_TEX_BUFFER ((void *)0x013CF800)
#define VW_SAM_TEX_BUFFER ((void *)0x01410000)
#define VW_IMAGE_1_BUFFER ((void *)0x01491000)
#define VW_IMAGE_2_BUFFER ((void *)0x014D1800)
#define VW_IMAGE_3_BUFFER ((void *)0x01512000)
#define VW_IMAGE_4_BUFFER ((void *)0x01552800)
#define TEST_MODE_BASE_TEX_BUFFER ((void *)0x01593000)

/* Buffers of MUSIC_CALL: the sequence (SMD) and wave (SWD) data of the
 * requested track, and the two files of the sound effect bank that it
 * loads first (read in this order). */
#define CASINO_MUSIC_SMD_BUFFER ((void *)0x015A3000)
#define CASINO_MUSIC_SWD_BUFFER ((void *)0x017A3000)
#define CASINO_EFFECT_1_BUFFER ((void *)0x017A7000)
#define CASINO_EFFECT_2_BUFFER ((void *)0x017B7000)

/* The casino's part of the persistent save block (SaveData + 0x15190). */
struct CasinoSaveWork {
    /* One flag per shop prize: set when the prize has been bought
     * (shop_main), and for prizes 6..35 also the enable flag of the viewer's
     * sampler slot of the same index (VW_SamlistInit). */
    char prize_bought[0x50];
    int coin_balance;
};

/* Casino settings, prizes and coin-purchase offers of the CASINO.res asset.
 * RES_* and ResData are original ELF symbols; the historical RES expansion
 * is not attested. The member names describe the observed CASINO.res. */
typedef struct CasinoPrize {
    int coin_cost;
    char name[40];
    char description[80];
} CasinoPrize;

typedef struct CasinoCoinOffer {
    int money_cost;
    char description[28];
} CasinoCoinOffer;

/* The proven part of the asset: the table of sound effect ids starts right
 * after the coin offers, and the word after it is the debug setting. The
 * table's own extent (256 entries up to the debug word) is inferred. */
typedef struct CasinoResourcePrefix {
    int bonus_duration; /* Countdown in eligible updates, not seconds. */
    int reel_step;      /* Spin-position increment per eligible update. */
    CasinoPrize prizes[51];
    CasinoCoinOffer coin_offers[4];
    int sound_effect_id[256];
    int debug_mode;
} CasinoResourcePrefix;

#endif /* SRC_OV11_MINI_G_PRIVATE_H */
