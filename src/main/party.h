#ifndef MAIN_GAME_177_PARTY_H
#define MAIN_GAME_177_PARTY_H

void dataWpnBoxInc(int weapon_id);

#include "shared.h"

typedef struct PartyAttackPosition {
    unsigned short party_id;
    signed char attack_position;
} PartyAttackPosition;

/*
 * Converts a calendar record to the number of seconds elapsed since
 * 2000-01-01 00:00:00.  monthday[12..23] holds the cumulative day offset of
 * each month; the leap day of the current year counts from March onwards,
 * and ((year - 1997) >> 2) counts the leap days of the whole years before it.
 */
unsigned int xglClockDayTime2UInt(XglClock *clock_time);

extern void PartyTimePauseEnd(void);

/* canon: config/header-canon.json chose src/core/main-00245af8/GameModeCfEvent.c over 2 other accepted spellings */
extern PadPrefix PadData;

void PartyDataInit2(void);

void PartyFriendOn(int party_id);

void PartyFriendOff(int id);

int PartyFriendCheck(int id);

void PartyLockPartyOn(int id);

void PartyLockPartyOff(int id);

int PartyLockPartyCheck(int id);

void PartyOutFriendOn(int id);

void PartyOutFriendOff(int id);

int PartyOutFriendCheck(int id);

void PartyTakeAgwsOn(int id);

void PartyTakeAgwsOff(int id);

int PartyTakeAgwsCheck(int id);

/*
 * Puts `party_id` into attack slot `slot_index`, with `attack_position` when it
 * is positive. A zero party id empties the slot. Returns -1 when the member is
 * not a recruited friend, 0 otherwise.
 */
int PartyAttackerSet(int slot_index, int party_id, int attack_position);

/*
 * Swaps an attacking member out of battle: the attack slot that holds
 * `party_id` is given to the first reserve member, who also becomes the
 * party leader when the member leaving was the leader.
 */
void PartyBattleChange(int party_id);

/*
 * Makes an attacking party member the party leader. The low halfword of
 * `leader` is the member's party id, the high halfword an optional costume
 * number (1 or 2). Returns 0 when the member is not in the attack slots.
 */
int PartyLeaderSet(int leader);

int PartyLeaderCheck(int chara_id);

void PartyRadarDispSet(int value);

int PartyFriendLockCheck(int id, int flag);

/* Collects party member ids into out[] and returns how many were stored.
 * mode 0: the attack line followed by the reserve line,
 * mode 1: every friend that is not locked out (at most six),
 * mode 2: every friend that has joined (at most six). */
int PartyAllPartyGet(int *out, int mode);

/*
 * Stores the menu ids of the occupied attack slots, stopping at the first
 * empty slot, and returns how many were stored.
 *
 * The original loop is not rotated: both exits test at the top of every
 * iteration, including the first one. GCC 2.96 rotates for(;;)/while loops
 * whose body breaks out (and then folds the first slot_index < 3 test); the
 * do { } while (1) spelling keeps the tests in place. The slot's party id is
 * read again for the call, as in the original, which keeps slot++ after the
 * empty-slot test instead of in the halfword load's delay.
 */
int PartyAttackerGet(int *out);

/* Stores the ids of available friends that are not in the attack line, up
 * to a combined party of six, and returns how many were stored. */
int PartyReserveGet(int *reserve_ids);

void *MenuWorkEndGet(void);

void SsdSetOutputMode(int mode);

void UmnMailBoxSet(int box_id);

unsigned char *UmnMailDataGet(int box_id);

/* Inventory quantity tables: 255 u16 counters each (nm size 0x1fe),
 * the tables dataBoxPtrGet() hands to dataBoxInc/dataBoxDec. */
extern unsigned short itmBox[255];

extern unsigned short wpnBox[255];

extern unsigned short bltBox[255];

extern unsigned short accBox[255];

extern unsigned short evtBox[255];

/* moneyBox: 8-byte .sdata counter at 0x004dbb68 (sd -16392(gp)). */
extern long long moneyBox;

/* UmnSimulationNo: 4-byte .sdata value at 0x004da814 (sw -21340(gp)). */
extern int UmnSimulationNo;

int dataCidGet(int chara_id);

int MenuMaryIdChange(int id);

int PartyAgwsGet(int *party_ids);

int PartyCharNumGet(void);

int PartyAgwsNumGet(void);

int PartyAttackerCheck(int party_id);

int PartyAttackPosCheck(int attack_position);

int PartyAttackPosGet(int party_id);

void PartyAttackPosSet(int party_id, int attack_position);

void PartyAttackPosChange(int first_party_id, int second_party_id);

/*
 * PartyTimeLimitCheck (main 0x002b6868, 88 B): clamps an accumulated
 * party-time record shaped like XglClock (the same day/hour/minute/second
 * breakdown xglClockUInt2DayTime writes) to a maximum of 5 days, 3 hours,
 * 59 minutes and 59 seconds.
 *
 * Evidence: PartyTimeUpDate (0x002b6954) refreshes the static party-time
 * record at 0x00491818 with xglClockUInt2DayTime/xglClockDayTime2UInt (an
 * 8-byte ldl/ldr -> sdl/sdr copy) and then calls this function; SeisanTimeEx
 * (0x0029fc44) builds a matching stack record with
 * xglClockUInt2DayTime(sp, quotient) and passes the same record on to
 * PartyTimeDispChange. Both callers discard the returned flag.
 *
 * Only day/hour/minute/second are read or written; status, month and year
 * are never accessed by this function.
 */
int PartyTimeLimitCheck(XglClock *elapsed);

void PartyTimeInit(void);

extern XglClock _CountTime;

/* canon: config/header-canon.json chose src/core/main-002b68c0/PartyTimeInit.c over 1 other accepted spelling */
void xglClockUInt2DayTime(XglClock *clock, unsigned int seconds);

/*
 * Refreshes the persistent play-time record (SaveData+0x68, the same
 * offset PartyTimeInit uses to seed it) with the elapsed clock ticks
 * since the last call, mirrors the fresh reading into _CountTime for the
 * next call, clamps the record at the 99-day limit via
 * PartyTimeLimitCheck, and returns the record for the caller
 * (PartyTimePauseStart forwards it unchanged).
 */
XglClock *PartyTimeUpDate(void);

XglClock *PartyTimePauseStart(void);

/*
 * TU-local declarations of game/177_party (main#177): the private header of
 * the accepted unit core-main-002b56c0 (src/core/main-002b56c0/private.h),
 * copied verbatim below. core-main-002b68e8 (PartyTimeUpDate) has a private.h
 * that only includes xeno/core/types.h, which the TU prelude includes.
 */

/* TU-local declarations for the merged party translation unit
 * (main 0x002b56c0..0x002b6494, one original file: gcc2_compiled. marks
 * 0x002b56c0 and the next file starts at 0x002b69e8).
 *
 * This header is the union of the accepted units' private headers, copied
 * unchanged:
 *   - src/core/main-002b56d0/private.h (PartyDataInit): SaveData region
 *     offsets, party-state offsets and TecInitEntry;
 *   - src/core/main-002b5e58/private.h (PartyAttackerSet, PartyBattleChange,
 *     PartyLeaderSet): the PartyDataPrefix view.
 * Shared prototypes and types come from xeno/core/{types,functions}.h.
 */

/* SaveData regions touched here. Each offset is the one its accessor uses:
 *   PARTY_STATE      0x10078  PartyDataGet(), a 0x188-byte party state
 *   MENU_SAVE_DATA   0x10254  MenuSaveDataGet()
 *   UMN_WORK         0x1025c  UmnMailBoxSet() scans its first 128 signed
 *                             bytes for an empty (-1) mail-box slot
 *   UMN_MAIL_DATA    0x10304  UmnMailDataGet(id) returns entry id (2 bytes)
 *   INIT_BLOCKS      0x15090  three 256-byte blocks only cleared here
 */
#define SAVE_PARTY_STATE      0x10078
#define SAVE_MENU_SAVE_DATA   0x10254
#define SAVE_UMN_WORK         0x1025C
#define SAVE_UMN_MAIL_DATA    0x10304
#define SAVE_INIT_BLOCKS      0x15090

#define UMN_MAIL_BOX_SLOTS    128
#define UMN_MAIL_DATA_COUNT   128
#define UMN_WORK_SIZE         168
/* Halfword flag inside the UMN work (tyaUmlDispLoad reads SaveData+0x102e2). */
#define UMN_WORK_FLAGS        0x86

/* Party state offsets, as used by the accepted readers:
 * +0x3c  eight technique records (getPartyDataOfs() type 2),
 * +0x140 skill level per character, +0x148 points to the next skill level
 *        per character (SkillCharSkillLvGet(), SkillCharPointPlus()).
 * Skill data +0x17e is the u16 next-level table SkillNextLvGet(level)
 * indexes (lhu 382(level * 2 + table)). */
#define PARTY_STATE_SIZE      0x188
#define PARTY_TEC_RECORDS     0x3C
#define PARTY_SKILL_LEVELS    0x140
#define PARTY_SKILL_POINTS    0x148
#define SKILL_NEXT_LEVEL_TABLE 0x17E

/* One 32-byte record of data\endou\tec\tecinit.bin; eight of them are copied
 * into the party state. Only the size is evidenced (ldl/sdl x4). */
typedef struct TecInitEntry {
    unsigned char bytes[32];
} TecInitEntry;

/*
 * PARTIAL ACCESSED PREFIX (explicit limitation):
 * PartyData is the 392-byte (0x188) party state that PartyDataGet returns:
 * SaveData + 0x10078, that is the object at 0x004a1828, copied whole by
 * PartyAllPartyGet2 with one struct assignment.  This view models its first
 * 0x3c bytes and claims a member only where the original's own load/store
 * width and its callers give the field a role.  Every accessor of main/tu177
 * reads and writes the state through this type; the widths below are the
 * widths the original bytes use.
 *
 *  +0x24 lock_party_mask   u16.  lhu/sh 0x24(base) in PartyLockPartyOn
 *        (0x002b5ba0/0x002b5bac), PartyLockPartyOff (0x002b5bf0/0x002b5c00),
 *        PartyLockPartyCheck (0x002b5c4c) and PartyFriendLockCheck
 *        (0x002b60e4).  Bit = id - 1, guarded by `sltiu ..., 0xC` plus an
 *        explicit `id != 4`, so it covers party ids 1..12 with 4 unused.
 *        PartyFriendLockCheck(flag & 2) subtracts it from friend_mask:
 *        a set bit locks that member out.
 *  +0x26 out_friend_mask   u16.  Same shape, lhu/sh 0x26(base) in
 *        PartyOutFriendOn (0x002b5ca0/0x002b5cac), PartyOutFriendOff
 *        (0x002b5cf0/0x002b5d00), PartyOutFriendCheck (0x002b5d4c) and
 *        PartyFriendLockCheck (0x002b60cc), which subtracts it under
 *        flag & 1: a set bit means the member is out of the selectable party.
 *  +0x28 friend_mask       u16.  lhu/sh 0x28(base) in PartyFriendOn
 *        (0x002b5aa0/0x002b5aac), PartyFriendOff (0x002b5af0/0x002b5b00),
 *        PartyFriendCheck
 *        (0x002b5b4c) and PartyFriendLockCheck (0x002b60b4).  Bit = id - 1,
 *        guarded by `sltiu ..., 0x7` plus `id != 4`: the recruited members,
 *        party ids 1..7 with 4 unused.  It is the set PartyFriendLockCheck
 *        starts from and PartyAllPartyGet(mode 2) walks.
 *  +0x2a take_agws_mask    u16.  lhu/sh 0x2A(base) in PartyTakeAgwsOn
 *        (0x002b5da0/0x002b5da8), PartyTakeAgwsOff (0x002b5de8/0x002b5df0),
 *        PartyTakeAgwsCheck
 *        (0x002b5e38) and PartyAgwsGet (0x002b64ac).  Bit = id - 17 under
 *        `sltiu ..., 0x10`, and PartyAgwsGet turns bit i back into party id
 *        i + 17, so the sixteen bits are the AGWS units of ids 17..32.
 *  +0x2c leader_battle_id  u16.  sh 0x2C(base) with 1 in PartyDataInit
 *        (0x002b584c) and with $0 in PartyDataInit2 (0x002b5a64);
 *        PartyLeaderSet stores the battle id dataCidGet returns, or its
 *        costume substitute, with sh 0x2C(base) (0x002b5ff0), and
 *        PartyLeaderCheck reads it back with lhu 0x2C(base) (0x002b6034)
 *        and compares it with dataCidGet, so it holds a battle id and not
 *        a party id.
 *  +0x2e radar_disp        u8.  The only byte-wide access: sb 0x2E(base) in
 *        PartyRadarDispSet (0x002b606c), which PartyDataInit calls with 1.
 *  +0x30 attack_slots[3]   The three attack slots.  getPartyDataOfs
 *        (0x002f7be0) hands +0x30 out as its own table (Java category 1) and
 *        PartyAttackerSet indexes it as a member: the original adds the
 *        +0x30 member offset to slot_index * 4 before adding the object base.
 *        PartyAttackerGet/Check and PartyAttackPosCheck/Get/Set/Change walk
 *        the same three entries.
 *
 * Not claimed, on purpose:
 *  - the leading 0x24 bytes.  They are not untouched: PartyCharNumGet
 *    (0x002b6518) reads +0x00 and +0x01 with lbu and counts all sixteen of
 *    their bits, while PartyAgwsNumGet (0x002b6588) reads +0x00..+0x03 with
 *    lbu and counts only the low four bits of each.  The two disagree about
 *    the shape of the region, so no member width there is evidenced and both
 *    functions keep reading the state as bytes.
 *  - the byte at +0x2f, which nothing in the program reads or writes.  It is
 *    the alignment the u16 of PartyAttackPosition forces after radar_disp,
 *    not a field, so it is left to the compiler instead of being declared.
 *  - everything past the attack slots.  PartyDataInit reaches +0x3c (technique
 *    records), +0x140 and +0x148 through the PARTY_* offsets above, and their
 *    element types are only partly recovered; they stay outside this view.
 */
typedef struct PartyDataPrefix {
    u8 _unmodeled_00[0x24];
    u16 lock_party_mask;
    u16 out_friend_mask;
    u16 friend_mask;
    u16 take_agws_mask;
    u16 leader_battle_id;
    u8 radar_disp;
    PartyAttackPosition attack_slots[3];
} PartyDataPrefix;

#endif /* MAIN_GAME_177_PARTY_H */
