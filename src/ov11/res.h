#ifndef XENO_RES_H
#define XENO_RES_H

/* Casino settings, prizes and coin-purchase asset accessors.
 * RES_* and ResData are original ELF symbols; the historical RES expansion
 * is not attested. New type/member names describe the observed CASINO.res.
 * Provenance: slus-20469-412d448de315 / ov11, config/res-context.json and
 * reports/readability-semantics.md (binary/asset hashes and consumer VAs).
 * These are compatibility declarations, not recovered original C types.
 * The selected EE compiler has int/pointer/long widths 4/4/8.
 */

/* Arrays describe fixed asset storage slots, not maximum string lengths. */
typedef struct CasinoPrize {
    int coin_cost;
    char name[40];
    char description[80];
} CasinoPrize;

typedef struct CasinoCoinOffer {
    int money_cost;
    char description[28];
} CasinoCoinOffer;

/* Only the proven prefix: the following sound-ID region is not fully modeled. */
typedef struct CasinoResourcePrefix {
    int bonus_duration; /* Countdown in eligible updates, not seconds. */
    int reel_step;      /* Spin-position increment per eligible update. */
    CasinoPrize prizes[51];
    CasinoCoinOffer coin_offers[4];
} CasinoResourcePrefix;

/* Original ov11:0x00a0dfa0 NOTYPE/size-zero symbol; lw/sw prove a 4-byte slot.
 * RES_Load assigns the asset storage at 0x01000000; no slot is defined here.
 */
extern CasinoResourcePrefix *ResData;

/* Separated word after the incompletely modeled sound-ID region. */
#define CASINO_DEBUG_SETTING_OFFSET 0x1d3c

/* The eight RES_* accessors are not declared here. Every one of them is a
 * LOCAL symbol in OV11.OVL, so they have internal linkage and are defined
 * `static` in src/casino/res.c; a shared header cannot declare them without
 * making every other translation unit emit its own copy. Their signatures are
 * in that file. Signed int and mutable char * preserve the tested register
 * ABI; original source signedness and prototypes are not claimed.
 */

#endif
